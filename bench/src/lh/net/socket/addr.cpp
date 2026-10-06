#include <benchmark/benchmark.h>

#include <lh/net/socket/addr.h>

static void
BM_net_ip4_socket_addr_parse(benchmark::State &state)
{
    const char text[] = "192.168.0.1:27015";
    lh_net_ip4_socket_addr_t out;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_net_ip4_socket_addr_parse(text, sizeof(text) - 1, &out));
    }
}
BENCHMARK(BM_net_ip4_socket_addr_parse);

static void
BM_net_ip4_socket_addr_format(benchmark::State &state)
{
    lh_net_ip4_t ip;

    lh_net_ip4_init(lh_addr_of(ip), 192, 168, 0, 1);
    lh_net_ip4_socket_addr_t addr;

    lh_net_ip4_socket_addr_init(lh_addr_of(addr), &ip, 27015);
    char buf[LH_NET_IP4_SOCKET_ADDR_TEXT_MAX];
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_net_ip4_socket_addr_format(&addr, buf, sizeof(buf)));
    }
}
BENCHMARK(BM_net_ip4_socket_addr_format);

static void
BM_net_socket_addr_format_dispatch(benchmark::State &state)
{
    lh_net_ip4_t ip;

    lh_net_ip4_init(lh_addr_of(ip), 192, 168, 0, 1);
    lh_net_ip4_socket_addr_t ip4_addr;

    lh_net_ip4_socket_addr_init(lh_addr_of(ip4_addr), &ip, 27015);
    lh_net_socket_addr_t addr;

    lh_net_socket_addr_init_ip4(lh_addr_of(addr), &ip4_addr);
    char buf[LH_NET_IP4_SOCKET_ADDR_TEXT_MAX];
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_net_socket_addr_format(&addr, buf, sizeof(buf)));
    }
}
BENCHMARK(BM_net_socket_addr_format_dispatch);

static void
BM_net_ip4_socket_addr_equals(benchmark::State &state)
{
    lh_net_ip4_t ip;

    lh_net_ip4_init(lh_addr_of(ip), 192, 168, 0, 1);
    lh_net_ip4_socket_addr_t a;

    lh_net_ip4_socket_addr_init(lh_addr_of(a), &ip, 27015);
    lh_net_ip4_socket_addr_t b;

    lh_net_ip4_socket_addr_init(lh_addr_of(b), &ip, 27016);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(a);
        benchmark::DoNotOptimize(b);
        benchmark::DoNotOptimize(lh_net_ip4_socket_addr_equals(&a, &b));
    }
}
BENCHMARK(BM_net_ip4_socket_addr_equals);
