#include <helix-c/helix.h>
#include <getopt.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <uv.h>

static const char *program;

FILE* output;

struct config {
	const char *symbol;
	const char *multicast_proto;
	const char *multicast_addr;
	int multicast_port;
	const char *format;
	const char *output;
};

struct trace_fmt_ops {
	void (*fmt_header)(void);
	void (*fmt_ob)(helix_order_book_t ob);
	void (*fmt_trade)(helix_trade_t trade);
};

static uv_buf_t alloc_packet(uv_handle_t* handle, size_t suggested_size)
{
	static char rx_buffer[65536];

	return uv_buf_init(rx_buffer, sizeof(rx_buffer));
}

static void fmt_pretty_header(void)
{
}

static void fmt_pretty_ob(helix_order_book_t ob)
{
	uint64_t timestamp = helix_order_book_timestamp(ob);
	uint64_t timestamp_in_sec = timestamp / 1000;
	uint64_t hours   = timestamp_in_sec / 60 / 60;
	uint64_t minutes = (timestamp_in_sec - (hours * 60 * 60)) / 60;
	uint64_t seconds = (timestamp_in_sec - (hours * 60 * 60) - (minutes * 60));

	if (helix_order_book_state(ob) == HELIX_TRADING_STATE_TRADING) {
		fprintf(output, "%s | %02lu:%02lu:%02lu %lu | %6lu  %.3f  %.3f  %-6lu |\n",
			helix_order_book_symbol(ob),
			hours, minutes, seconds, timestamp,
			helix_order_book_bid_size(ob, 0),
			(double)helix_order_book_bid_price(ob, 0)/10000.0,
			(double)helix_order_book_ask_price(ob, 0)/10000.0,
			helix_order_book_ask_size(ob, 0)
			);
	}
}

const char trade_sign(helix_trade_sign_t sign)
{
    switch (sign) {
    case HELIX_TRADE_SIGN_BUYER_INITIATED:    return 'B';
    case HELIX_TRADE_SIGN_SELLER_INITIATED:   return 'S';
    case HELIX_TRADE_SIGN_CROSSING:           return 'C';
    case HELIX_TRADE_SIGN_NON_DISPLAYABLE:    return 'N';
    default:
        return '?';
    }
}

static void fmt_pretty_trade(helix_trade_t trade)
{
	fprintf(output, "%s | %.3f | %c | \n", helix_trade_symbol(trade), helix_trade_price(trade)/10000.0, trade_sign(helix_trade_sign(trade)));
}

struct trace_fmt_ops fmt_pretty_ops = {
	.fmt_header	= fmt_pretty_header,
	.fmt_ob		= fmt_pretty_ob,
	.fmt_trade	= fmt_pretty_trade,
};

static void fmt_csv_header(void)
{
	fprintf(output, "Symbol,Timestamp,BidPrice,BidSize,AskPrice,AskSize,LastPrice,LastSign\n");
	fflush(output);
}

static void fmt_csv_ob(helix_order_book_t ob)
{
	if (helix_order_book_state(ob) == HELIX_TRADING_STATE_TRADING) {
		fprintf(output, "%s,%lu,%f,%lu,%f,%lu,,\n",
			helix_order_book_symbol(ob),
			helix_order_book_timestamp(ob),
			(double)helix_order_book_bid_price(ob, 0)/10000.0,
			helix_order_book_bid_size(ob, 0),
			(double)helix_order_book_ask_price(ob, 0)/10000.0,
			helix_order_book_ask_size(ob, 0)
			);
		fflush(output);
	}
}

static void fmt_csv_trade(helix_trade_t trade)
{
	fprintf(output, "%s,%lu,,,,,%f,%c\n",
		helix_trade_symbol(trade), helix_trade_timestamp(trade), helix_trade_price(trade)/10000.0, trade_sign(helix_trade_sign(trade)));
	fflush(output);
}

struct trace_fmt_ops fmt_csv_ops = {
	.fmt_header	= fmt_csv_header,
	.fmt_ob		= fmt_csv_ob,
	.fmt_trade	= fmt_csv_trade,
};

struct trace_fmt_ops *fmt_ops;

static void process_ob_event(helix_order_book_t ob)
{
	fmt_ops->fmt_ob(ob);
}

static void process_trade_event(helix_trade_t trade)
{
	fmt_ops->fmt_trade(trade);
}

static void recv_packet(uv_udp_t* handle, ssize_t nread, uv_buf_t buf, struct sockaddr* addr, unsigned flags)
{
	if (nread > 0) {
		helix_session_process_packet(handle->data, buf.base, nread);
	}
}

static void libuv_error(const char *s)
{
	uv_err_t err;

	err = uv_last_error(uv_default_loop());

	fprintf(stderr, "error: %s: %s (%s)\n", s, uv_strerror(err), uv_err_name(err));
	exit(1);
}

static void usage(void)
{
	fprintf(stdout,
		"usage: %s [options]\n"
		"  options:\n"
		"    -s, --symbol symbol          Ticker symbol to listen to.\n"
		"    -c, --multicast-proto proto  UDP multicast protocol listen to.\n"
		"    -a, --multicast-addr addr    UDP multicast address to listen to.\n"
		"    -p, --multicast-port port    UDP multicast port to listen to.\n"
		"    -o, --output filename        Output filename.\n"
		"    -f, --format format          Output format (pretty, csv).\n"
		"    -h, --help                   display this help and exit\n",
		program);
	exit(1);
}

static struct option trace_options[] = {
	{"symbol",          required_argument, 0, 's'},
	{"multicast-proto", required_argument, 0, 'c'},
	{"multicast-addr",  required_argument, 0, 'a'},
	{"multicast-port",  required_argument, 0, 'p'},
	{"output",          required_argument, 0, 'o'},
	{"format",          required_argument, 0, 'f'},
	{"help",            no_argument,       0, 'h'},
	{0, 0, 0, 0}
};

static void parse_options(struct config *cfg, int argc, char *argv[])
{
	cfg->format = "pretty";

	for (;;) {
		int opt_idx = 0;
		int c;

		c = getopt_long(argc, argv, "s:c:a:o:p:f:h", trace_options, &opt_idx);
		if (c == -1)
			break;

		switch (c) {
		case 's':
			cfg->symbol = optarg;
			break;
		case 'c':
			cfg->multicast_proto = optarg;
			break;
		case 'a':
			cfg->multicast_addr = optarg;
			break;
		case 'o':
			cfg->output = optarg;
			break;
		case 'p':
			cfg->multicast_port = strtol(optarg, NULL, 10);
			break;
		case 'f':
			cfg->format = optarg;
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

	if (!cfg.multicast_proto) {
		fprintf(stderr, "error: multicast protocol is not specified. Use the '-c' option to specify it.\n");
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

	if (!strcmp(cfg.format, "pretty")) {
		fmt_ops = &fmt_pretty_ops;
	} else if (!strcmp(cfg.format, "csv")) {
		fmt_ops = &fmt_csv_ops;
	} else {
		fprintf(stderr, "error: %s: unsupported format\n", cfg.format);
		exit(1);
	}

	if (cfg.output) {
		output = fopen(cfg.output, "w");
		if (!output) {
			fprintf(stderr, "error: %s: %s\n", cfg.output, strerror(errno));
			exit(1);
		}
	} else {
		output = stdout;
	}

	proto = helix_protocol_lookup(cfg.multicast_proto);
	if (!proto) {
		fprintf(stderr, "error: protocol '%s' is not supported\n", cfg.multicast_proto);
		exit(1);
	}

	session = helix_session_create(proto, cfg.symbol, process_ob_event, process_trade_event, NULL);
	if (!session) {
		fprintf(stderr, "error: unable to create new session\n");
		exit(1);
	}

	err = uv_udp_init(uv_default_loop(), &socket);
	if (err) {
		libuv_error("uv_udp_init");
	}
	socket.data = session;

	addr = uv_ip4_addr("0.0.0.0", cfg.multicast_port);

	err = uv_udp_bind(&socket, addr, 0);
	if (err) {
		libuv_error("uv_udp_bind");
	}

	err = uv_udp_set_membership(&socket, cfg.multicast_addr, NULL, UV_JOIN_GROUP);
	if (err) {
		libuv_error("uv_udp_set_membership");
	}

	err = uv_udp_recv_start(&socket, alloc_packet, recv_packet);
	if (err) {
		libuv_error("uv_udp_recv_start");
	}

	fmt_ops->fmt_header();

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
