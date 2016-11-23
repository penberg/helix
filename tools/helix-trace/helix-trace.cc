#include <helix-c/helix.h>
#include <sys/mman.h>
#define __STDC_FORMAT_MACROS 1
#include <inttypes.h>
#include <stdbool.h>
#include <getopt.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <uv.h>

#include <stdexcept>
#include <string>
#include <vector>

static const char *program;

FILE* output;
bool flush;

size_t max_price_levels	= 0;
size_t max_order_count	= 0;
uint64_t quotes			= 0;
uint64_t trades			= 0;
uint64_t volume_shs		= 0;
double volume_ccy		= 0.0;
double high				= -INFINITY;
double low				= +INFINITY;

struct socket_address {
	std::string addr;
	int port;
	socket_address()
	{}
	socket_address(std::string addr, int port)
		: addr{addr}, port{port}
	{}
};

struct config {
	std::vector<std::string> symbols;
	size_t max_orders;
	const char *proto;
	const char *multicast_addr;
	int multicast_port;
	const char *request_server;
	const char *format;
	const char *input;
	const char *output;
};

struct trace_session {
       socket_address addr;
       uv_udp_t request_socket;
       uint64_t	bid_price = 0;
       uint64_t	bid_size  = 0;
       uint64_t	ask_price = UINT64_MAX;
       uint64_t	ask_size  = 0;
};

struct trace_fmt_ops {
	void (*fmt_header)(void);
	void (*fmt_event)(helix_session_t session, helix_event_t event);
};

socket_address parse_socket_address(std::string raw_addr)
{
	size_t pos = raw_addr.find_first_of(':');
	if (pos == std::string::npos) {
		throw std::invalid_argument(raw_addr + " is not a valid socket address");
	}
	try {
		auto addr = raw_addr.substr(0, pos);
		auto port = raw_addr.substr(pos + 1);
		return socket_address{addr, std::stoi(port)};
	} catch (...) {
		throw std::invalid_argument(raw_addr + " is not a valid socket address");
	}
}

static void alloc_packet(uv_handle_t* handle, size_t size, uv_buf_t* buf)
{
	static char rx_buffer[65536];

	buf->base = rx_buffer;
	buf->len  = sizeof(rx_buffer);
}

const char trade_sign(helix_trade_sign_t sign)
{
	switch (sign) {
	case HELIX_TRADE_SIGN_BUYER_INITIATED:	return 'B';
	case HELIX_TRADE_SIGN_SELLER_INITIATED:	return 'S';
	case HELIX_TRADE_SIGN_CROSSING:		return 'C';
	case HELIX_TRADE_SIGN_NON_DISPLAYABLE:	return 'N';
	default:				return '?';
	}
}

static bool is_order_book_changed(helix_session_t session, helix_event_t event)
{
	auto event_mask = helix_event_mask(event);
	if (event_mask & HELIX_EVENT_TRADE) {
		return true;
	}
	if (event_mask & HELIX_EVENT_ORDER_BOOK_UPDATE) {
		auto* ts = reinterpret_cast<trace_session*>(helix_session_data(session));
		auto ob = helix_event_order_book(event);
		auto bid_price = helix_order_book_bid_price(ob, 0);
		auto bid_size = helix_order_book_bid_size(ob, 0);
		auto ask_price = helix_order_book_ask_price(ob, 0);
		auto ask_size = helix_order_book_ask_size(ob, 0);
		if (!bid_price || !ask_size) {
			return false;
		}
		return bid_price != ts->bid_price || bid_size != ts->bid_size || ask_price != ts->ask_price || ask_size != ts->ask_size;
	}
	return false;
}

static void fmt_pretty_header(void)
{
}

static void fmt_pretty_event(helix_session_t session, helix_event_t event)
{
	auto* ts = reinterpret_cast<trace_session*>(helix_session_data(session));
	auto timestamp = helix_event_timestamp(event);
	if (!helix_session_is_rth_timestamp(session, timestamp) || !is_order_book_changed(session, event)) {
		return;
	}
	uint64_t timestamp_in_sec = timestamp / 1000;
	uint64_t hours   = timestamp_in_sec / 60 / 60;
	uint64_t minutes = (timestamp_in_sec - (hours * 60 * 60)) / 60;
	uint64_t seconds = (timestamp_in_sec - (hours * 60 * 60) - (minutes * 60));
	uint64_t milliseconds = timestamp % 1000;
	fprintf(output, "%s | %02" PRIu64":%02" PRIu64":%02" PRIu64".%03" PRIu64 " |",
		helix_event_symbol(event),
		hours, minutes, seconds, milliseconds);
	auto event_mask = helix_event_mask(event);
	if (event_mask & HELIX_EVENT_ORDER_BOOK_UPDATE) {
		auto ob = helix_event_order_book(event);

		auto bid_price = helix_order_book_bid_price(ob, 0);
		auto bid_size = helix_order_book_bid_size(ob, 0);
		auto ask_price = helix_order_book_ask_price(ob, 0);
		auto ask_size = helix_order_book_ask_size(ob, 0);

		fprintf(output, "%6" PRIu64"  %6.3f  %6.3f  %-6" PRIu64" |",
			bid_size,
			(double)bid_price/10000.0,
			(double)ask_price/10000.0,
			ask_size
			);

		ts->bid_price = bid_price;
		ts->bid_size = bid_size;
		ts->ask_price = ask_price;
		ts->ask_size = ask_size;
	} else {
		fprintf(output, "                               |");
	}
	if (event_mask & HELIX_EVENT_TRADE) {
		auto trade = helix_event_trade(event);
		fprintf(output, " %6.3f %6" PRIu64 " | %c | %6.3f |",
			helix_trade_price(trade)/10000.0,
			helix_trade_size(trade),
			trade_sign(helix_trade_sign(trade)),
			volume_ccy / (double)volume_shs
			);
	} else {
		fprintf(output, "               |   |        |");
	}
	std::string sweep_event;
	if (event_mask & HELIX_EVENT_SWEEP) {
		sweep_event = " Y |";
	} else {
		sweep_event = "   |";
        }
	fprintf(output, "%s", sweep_event.c_str());
	fprintf(output, "\n");
	if (flush) {
		fflush(output);
	}
}

struct trace_fmt_ops fmt_pretty_ops = {
	.fmt_header	= fmt_pretty_header,
	.fmt_event	= fmt_pretty_event,
};

static void fmt_csv_header(void)
{
	fprintf(output, "Symbol,Timestamp,BidPrice,BidSize,AskPrice,AskSize,LastPrice,LastSize,LastSign,VWAP,SweepEvent\n");
	if (flush) fflush(output);
}

static void fmt_csv_event(helix_session_t session, helix_event_t event)
{
	auto* ts = reinterpret_cast<trace_session*>(helix_session_data(session));
	auto timestamp = helix_event_timestamp(event);
	if (!helix_session_is_rth_timestamp(session, timestamp) || !is_order_book_changed(session, event)) {
		return;
	}
	auto symbol = helix_event_symbol(event);
	fprintf(output, "%s,%" PRIu64 ",", symbol, timestamp);
	auto event_mask = helix_event_mask(event);
	if (event_mask & HELIX_EVENT_ORDER_BOOK_UPDATE) {
		auto ob = helix_event_order_book(event);

		auto bid_price = helix_order_book_bid_price(ob, 0);
		auto bid_size = helix_order_book_bid_size(ob, 0);
		auto ask_price = helix_order_book_ask_price(ob, 0);
		auto ask_size = helix_order_book_ask_size(ob, 0);

		fprintf(output, "%f,%" PRIu64",%f,%" PRIu64",",
			(double)bid_price/10000.0,
			bid_size,
			(double)ask_price/10000.0,
			ask_size
			);

		ts->bid_price = bid_price;
		ts->bid_size = bid_size;
		ts->ask_price = ask_price;
		ts->ask_size = ask_size;
	} else {
		fprintf(output, ",,,,");
	}
	if (event_mask & HELIX_EVENT_TRADE) {
		auto trade = helix_event_trade(event);
		fprintf(output, "%f,%" PRIu64 ",%c,%f,",
			helix_trade_price(trade)/10000.0,
			helix_trade_size(trade),
			trade_sign(helix_trade_sign(trade)),
			volume_ccy / (double)volume_shs
			);
	} else {
		fprintf(output, ",,,,");
	}
	std::string sweep_event;
	if (event_mask & HELIX_EVENT_SWEEP) {
		sweep_event = "Y";
	}
	fprintf(output, "%s", sweep_event.c_str());
	fprintf(output, "\n");
	if (flush) {
		fflush(output);
	}
}

struct trace_fmt_ops fmt_csv_ops = {
	.fmt_header	= fmt_csv_header,
	.fmt_event	= fmt_csv_event,
};

struct trace_fmt_ops *fmt_ops;

static void process_ob_event(helix_session_t session, helix_order_book_t ob, helix_event_mask_t event_mask)
{
	size_t bid_levels = helix_order_book_bid_levels(ob);
	size_t ask_levels = helix_order_book_ask_levels(ob);
	size_t order_count = helix_order_book_order_count(ob);

	max_price_levels = bid_levels > max_price_levels ? bid_levels : max_price_levels;
	max_price_levels = ask_levels > max_price_levels ? ask_levels : max_price_levels;
	max_order_count = order_count > max_order_count ? order_count : max_order_count;
	quotes++;
}

static void process_trade_event(helix_session_t session, helix_trade_t trade, helix_event_mask_t event_mask)
{
	double trade_price = helix_trade_price(trade)/10000.0;
	uint64_t trade_size = helix_trade_size(trade);
	volume_shs += trade_size;
	volume_ccy += (double)trade_size * trade_price;
	high = trade_price > high ? trade_price : high;
	low = trade_price < low ? trade_price : low;
	trades++;
}

static void process_event(helix_session_t session, helix_event_t event)
{
	helix_event_mask_t mask = helix_event_mask(event);

	if (mask & HELIX_EVENT_ORDER_BOOK_UPDATE) {
		helix_order_book_t ob = helix_event_order_book(event);
		process_ob_event(session, ob, mask);
	}
	if (mask & HELIX_EVENT_TRADE) {
		helix_trade_t trade = helix_event_trade(event);
		process_trade_event(session, trade, mask);
	}
	fmt_ops->fmt_event(session, event);
}

static void recv_packet(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags)
{
	if (nread > 0) {
		helix_session_process_packet(reinterpret_cast<helix_session_t>(handle->data), buf->base, nread);
	}
}

static void udp_send_done(uv_udp_send_t* req, int status)
{
	delete req;
}

static void process_send(helix_session_t session, char* base, size_t len)
{
	auto* ts = reinterpret_cast<trace_session*>(helix_session_data(session));

	static char tx_buf[1024];
	memcpy(tx_buf, base, len);

	uv_udp_send_t *send_req = new uv_udp_send_t;
	uv_buf_t msg = uv_buf_init(tx_buf, len);

	struct sockaddr_in saddr;
	uv_ip4_addr(ts->addr.addr.c_str(), ts->addr.port, &saddr);
	uv_udp_send(send_req, &ts->request_socket, &msg, 1, (const struct sockaddr *)&saddr, udp_send_done);
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
		"    -s, --symbol symbol            Ticker symbol to listen to.\n"
		"    -m, --max-orders number        Maximum number of orders per symbol (for pre-allocation).\n"
		"    -P, --proto proto              Market data protocol to listen to\n"
		"          or read from. Supported values:\n"
		"              nasdaq-nordic-moldudp-itch\n"
		"              nasdaq-nordic-soupfile-itch\n"
		"              nasdaq-binaryfile-itch50\n"
		"              parity-moldudp64-pmd\n"
		"    -a, --multicast-addr addr      UDP multicast address to listen to.\n"
		"    -p, --multicast-port port      UDP multicast port to listen to.\n"
		"    -r, --request-server addr:port UDP request server to connect to.\n"
		"    -i, --input filename           Input filename.\n"
		"    -o, --output filename          Output filename.\n"
		"    -f, --format format            Output format (pretty, csv).\n"
		"    -h, --help                     display this help and exit\n",
		program);
	exit(1);
}

static struct option trace_options[] = {
	{"symbol",          required_argument, 0, 's'},
	{"max-orders",      required_argument, 0, 'm'},
	{"proto",           required_argument, 0, 'P'},
	{"multicast-addr",  required_argument, 0, 'a'},
	{"multicast-port",  required_argument, 0, 'p'},
	{"request-server",  required_argument, 0, 'r'},
	{"input",           required_argument, 0, 'i'},
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

		c = getopt_long(argc, argv, "s:m:P:a:r:i:o:p:f:h", trace_options, &opt_idx);
		if (c == -1)
			break;

		switch (c) {
		case 's':
			cfg->symbols.emplace_back(optarg);
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
		case 'r':
			cfg->request_server = optarg;
			break;
		case 'i':
			cfg->input = optarg;
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
	struct stat input_st;
	trace_session ts;
	void *input_mmap;
	uv_udp_t socket;
	int input_fd;
	int err;

	program = basename(argv[0]);

	parse_options(&cfg, argc, argv);

	if (cfg.symbols.empty()) {
		fprintf(stderr, "error: no symbols are specified. Use the '-s' option to specify them.\n");
		exit(1);
	}

	if (!cfg.proto) {
		fprintf(stderr, "error: multicast protocol is not specified. Use the '-P' option to specify it.\n");
		exit(1);
	}

	if (!cfg.input) {
		if (!cfg.request_server) {
			fprintf(stderr, "error: no request server specified. Use the '-r' option to specify it.\n");
			exit(1);
		}
		ts.addr = parse_socket_address(cfg.request_server);
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
		flush = false;
		if (!output) {
			fprintf(stderr, "error: %s: %s\n", cfg.output, strerror(errno));
			exit(1);
		}
	} else {
		output = stdout;
		flush = true;
	}

	proto = helix_protocol_lookup(cfg.proto);
	if (!proto) {
		fprintf(stderr, "error: protocol '%s' is not supported\n", cfg.proto);
		exit(1);
	}

	session = helix_session_create(proto, process_event, &ts);
	if (!session) {
		fprintf(stderr, "error: unable to create new session\n");
		exit(1);
	}

	for (auto&& symbol : cfg.symbols) {
		helix_session_subscribe(session, symbol.c_str(), cfg.max_orders);
	}

	helix_session_set_send_callback(session, process_send);

	if (cfg.input) {
		const char* p;
		size_t size;

		input_fd = open(cfg.input, O_RDONLY);
		if (input_fd < 0) {
			fprintf(stderr, "error: %s: %s\n", cfg.input, strerror(errno));
			exit(1);
		}
		if (fstat(input_fd, &input_st) < 0) {
			fprintf(stderr, "error: %s: %s\n", cfg.input, strerror(errno));
			exit(1);
		}
		input_mmap = mmap(NULL, input_st.st_size, PROT_READ, MAP_SHARED, input_fd, 0);
		if (input_mmap == MAP_FAILED) {
			fprintf(stderr, "error: %s: %s\n", cfg.input, strerror(errno));
			exit(1);
		}

		fmt_ops->fmt_header();

		p = reinterpret_cast<char*>(input_mmap);
		size = input_st.st_size;
		while (size > 0) {
			size_t nr;

			nr = helix_session_process_packet(session, p, size);
			if (!nr)
				break;

			p += nr;
			size -= nr;
		}

		if (munmap(input_mmap, input_st.st_size) < 0) {
			fprintf(stderr, "error: %s: %s\n", cfg.input, strerror(errno));
			exit(1);
		}
	} else {
		if (!cfg.multicast_addr) {
			fprintf(stderr, "error: multicast address is not specified. Use the '-a' option to specify it.\n");
			exit(1);
		}

		if (!cfg.multicast_port) {
			fprintf(stderr, "error: multicast port is not specified. Use the '-p' option to specify it.\n");
			exit(1);
		}

		err = uv_udp_init(uv_default_loop(), &socket);
		if (err) {
			libuv_error("uv_udp_init", err);
		}
		socket.data = session;

		err = uv_udp_init(uv_default_loop(), &ts.request_socket);
		if (err) {
			libuv_error("uv_udp_init", err);
		}
		ts.request_socket.data = session;

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

		err = uv_udp_recv_start(&ts.request_socket, alloc_packet, recv_packet);
		if (err) {
			libuv_error("uv_udp_recv_start", err);
		}

		fmt_ops->fmt_header();

		uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	}

	if (cfg.output && output)
		fclose(output);

	fprintf(stderr, "quotes: %" PRId64  ", trades: %" PRId64 " , max levels: %zu, max orders: %zu\n", quotes, trades, max_price_levels, max_order_count);
	fprintf(stderr, "volume (mio): %.4lf, notional (mio): %.4lf, VWAP: %.3lf, high: %.3lf, low: %.3lf\n", (double)volume_shs * 1e-6, volume_ccy * 1e-6, (volume_ccy / (double)volume_shs), high, low);
}
