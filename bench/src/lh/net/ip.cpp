#include <benchmark/benchmark.h>

#include <lh/net/ip.h>

static void
BM_net_ip4_parse(benchmark::State &state)
{
    const char text[] = "192.168.0.1";
    lh_net_ip4_t out;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_net_ip4_parse(text, sizeof(text) - 1, &out));
    }
}
BENCHMARK(BM_net_ip4_parse);

static void
BM_net_ip4_format(benchmark::State &state)
{
    lh_net_ip4_t addr = lh_net_ip4_make(192, 168, 0, 1);
    char buf[LH_NET_IP4_TEXT_MAX];
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_net_ip4_format(&addr, buf, sizeof(buf)));
    }
}
BENCHMARK(BM_net_ip4_format);

static void
BM_net_ip4_equals(benchmark::State &state)
{
    lh_net_ip4_t a = lh_net_ip4_make(192, 168, 0, 1);
    lh_net_ip4_t b = lh_net_ip4_make(192, 168, 0, 2);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_net_ip4_equals(&a, &b));
    }
}
BENCHMARK(BM_net_ip4_equals);
