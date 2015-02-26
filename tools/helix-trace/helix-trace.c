#include <helix-c/Helix.h>
#include <getopt.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <uv.h>

static const char *program;

struct config {
	const char *multicast_proto;
	const char *multicast_addr;
	int multicast_port;
};

static uv_buf_t alloc_packet(uv_handle_t* handle, size_t suggested_size)
{
	static char rx_buffer[65536];

	return uv_buf_init(rx_buffer, sizeof(rx_buffer));
}

static void recv_packet(uv_udp_t* handle, ssize_t nread, uv_buf_t buf, struct sockaddr* addr, unsigned flags)
{
	HelixSessionProcessPacket(NULL, buf.base, nread);
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
		"    -c, --multicast-proto proto  UDP multicast protocol listen to.\n"
		"    -a, --multicast-addr addr    UDP multicast address to listen to.\n"
		"    -p, --multicast-port port    UDP multicast port to listen to.\n"
		"    -h, --help                   display this help and exit\n",
		program);
	exit(1);
}

static struct option trace_options[] = {
	{"multicast-proto", required_argument, 0, 'c'},
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

		c = getopt_long(argc, argv, "c:a:p:h", trace_options, &opt_idx);
		if (c == -1)
			break;

		switch (c) {
		case 'c':
			cfg->multicast_proto = optarg;
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
	struct config cfg = {};
	struct sockaddr_in addr;
	HelixProtocol proto;
	uv_udp_t socket;
	int err;

	program = basename(argv[0]);

	parse_options(&cfg, argc, argv);

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

	proto = HelixProtocolLookup(cfg.multicast_proto);
	if (!proto) {
		fprintf(stderr, "error: protocol '%s' is not supported\n", cfg.multicast_proto);
		exit(1);
	}

	err = uv_udp_init(uv_default_loop(), &socket);
	if (err) {
		libuv_error("uv_udp_init");
	}

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

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
