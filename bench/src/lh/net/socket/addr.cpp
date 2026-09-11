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
    lh_net_ip4_t ip = lh_net_ip4_make(192, 168, 0, 1);
    lh_net_ip4_socket_addr_t addr = lh_net_ip4_socket_addr_make(&ip, 27015);
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
    lh_net_ip4_t ip = lh_net_ip4_make(192, 168, 0, 1);
    lh_net_ip4_socket_addr_t ip4_addr = lh_net_ip4_socket_addr_make(&ip, 27015);
    lh_net_socket_addr_t addr = lh_net_socket_addr_make_ip4(&ip4_addr);
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
    lh_net_ip4_t ip = lh_net_ip4_make(192, 168, 0, 1);
    lh_net_ip4_socket_addr_t a = lh_net_ip4_socket_addr_make(&ip, 27015);
    lh_net_ip4_socket_addr_t b = lh_net_ip4_socket_addr_make(&ip, 27016);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_net_ip4_socket_addr_equals(&a, &b));
    }
}
BENCHMARK(BM_net_ip4_socket_addr_equals);
