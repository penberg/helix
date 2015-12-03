#include <helix-c/helix.h>
#include <ncurses.h>
#include <getopt.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <uv.h>

static const char *program;

struct config {
	const char *symbol;
	size_t max_orders;
	const char *proto;
	const char *multicast_addr;
	int multicast_port;
};

static void alloc_packet(uv_handle_t* handle, size_t size, uv_buf_t* buf)
{
	static char rx_buffer[65536];

	buf->base = rx_buffer;
	buf->len  = sizeof(rx_buffer);
}

static void print_top(helix_order_book_t ob)
{
	uint64_t timestamp = helix_order_book_timestamp(ob);
	uint64_t timestamp_in_sec = timestamp / 1000;
	uint64_t hours   = timestamp_in_sec / 60 / 60;
	uint64_t minutes = (timestamp_in_sec - (hours * 60 * 60)) / 60;
	uint64_t seconds = (timestamp_in_sec - (hours * 60 * 60) - (minutes * 60));
	uint64_t msecs   = timestamp - (timestamp_in_sec * 1000);

	if (helix_order_book_state(ob) == HELIX_TRADING_STATE_TRADING) {
		clear();
		move(0, 0);
		printw("%16s    %02lu:%02lu:%02lu.%03lu\n",
			helix_order_book_symbol(ob),
			hours, minutes, seconds, msecs
			);
		for (unsigned i = 0; i < 5; i++) {
			move(i+1, 0);
			printw("| %6lu  %.3f  %.3f  %-6lu |\n",
				helix_order_book_bid_size(ob, i),
				(double)helix_order_book_bid_price(ob, i)/10000.0,
				(double)helix_order_book_ask_price(ob, i)/10000.0,
				helix_order_book_ask_size(ob, i)
				);
		}
		refresh();
	}
}

static void process_ob_event(helix_session_t session, helix_order_book_t ob)
{
	print_top(ob);
}

static void process_event(helix_session_t session, helix_event_t event)
{
	helix_event_mask_t mask = helix_event_mask(event);

	if (mask & HELIX_EVENT_ORDER_BOOK_UPDATE) {
		helix_order_book_t ob = helix_event_order_book(event);
		process_ob_event(session, ob);
	}
}


static void recv_packet(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags)
{
	if (nread > 0) {
		helix_session_process_packet(handle->data, buf->base, nread);
	}
}

static void libuv_error(const char *s, int err)
{
	fprintf(stderr, "error: %s: %s (%s)\n", s, uv_strerror(err), uv_err_name(err));
	exit(1);
}

static void usage(void)
{
	fprintf(stdout,
		"usage: %s [options]\n"
		"  options:\n"
		"    -s, --symbol symbol          Ticker symbol to listen to.\n"
		"    -m, --max-orders number      Maximum number of orders per symbol (for pre-allocation).\n"
		"    -P, --proto proto            Market data protocol listen to.\n"
		"    -a, --multicast-addr addr    UDP multicast address to listen to.\n"
		"    -p, --multicast-port port    UDP multicast port to listen to.\n"
		"    -h, --help                   display this help and exit\n",
		program);
	exit(1);
}

static struct option trace_options[] = {
	{"symbol",          required_argument, 0, 's'},
	{"max-orders",      required_argument, 0, 'm'},
	{"proto",           required_argument, 0, 'P'},
	{"multicast-addr",  required_argument, 0, 'a'},
	{"multicast-port",  required_argument, 0, 'p'},
	{"help",            no_argument,       0, 'h'},
	{0, 0, 0, 0}
};

static void parse_options(struct config *cfg, int argc, char *argv[])
{
	for (;;) {
		int opt_idx = 0;
		int c;

		c = getopt_long(argc, argv, "s:m:P:a:p:h", trace_options, &opt_idx);
		if (c == -1)
			break;

		switch (c) {
		case 's':
			cfg->symbol = optarg;
			break;
		case 'm':
			cfg->max_orders = strtol(optarg, NULL, 10);
			break;
		case 'P':
			cfg->proto = optarg;
			break;
		case 'a':
			cfg->multicast_addr = optarg;
			break;
		case 'p':
			cfg->multicast_port = strtol(optarg, NULL, 10);
			break;
		case 'h':
			usage();
		default:
			usage();
		}
	}
}

int main(int argc, char *argv[])
{
	helix_session_t session;
	struct sockaddr_in addr;
	helix_protocol_t proto;
	struct config cfg = {};
	uv_udp_t socket;
	int err;

	program = basename(argv[0]);

	parse_options(&cfg, argc, argv);

	if (!cfg.symbol) {
		fprintf(stderr, "error: symbol is not specified. Use the '-s' option to specify it.\n");
		exit(1);
	}

	if (!cfg.proto) {
		fprintf(stderr, "error: multicast protocol is not specified. Use the '-P' option to specify it.\n");
		exit(1);
	}

	if (!cfg.multicast_addr) {
		fprintf(stderr, "error: multicast address is not specified. Use the '-a' option to specify it.\n");
		exit(1);
	}

	if (!cfg.multicast_port) {
		fprintf(stderr, "error: multicast port is not specified. Use the '-p' option to specify it.\n");
		exit(1);
	}

	proto = helix_protocol_lookup(cfg.proto);
	if (!proto) {
		fprintf(stderr, "error: protocol '%s' is not supported\n", cfg.proto);
		exit(1);
	}

	session = helix_session_create(proto, process_event, NULL);
	if (!session) {
		fprintf(stderr, "error: unable to create new session\n");
		exit(1);
	}

	helix_session_subscribe(session, cfg.symbol, cfg.max_orders);

	err = uv_udp_init(uv_default_loop(), &socket);
	if (err) {
		libuv_error("uv_udp_init", err);
	}
	socket.data = session;

	err = uv_ip4_addr("0.0.0.0", cfg.multicast_port, &addr);
	if (err) {
		libuv_error("uv_ip4_addr", err);
	}

	err = uv_udp_bind(&socket, (const struct sockaddr *)&addr, UV_UDP_REUSEADDR);
	if (err) {
		libuv_error("uv_udp_bind", err);
	}

	err = uv_udp_set_membership(&socket, cfg.multicast_addr, NULL, UV_JOIN_GROUP);
	if (err) {
		libuv_error("uv_udp_set_membership", err);
	}

	err = uv_udp_recv_start(&socket, alloc_packet, recv_packet);
	if (err) {
		libuv_error("uv_udp_recv_start", err);
	}

	initscr();

	clear();

	refresh();

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);

	endwin();

	return 0;
}
