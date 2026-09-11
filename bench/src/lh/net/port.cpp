#include <benchmark/benchmark.h>

#include <lh/net/port.h>

static void
BM_net_port_parse(benchmark::State &state)
{
    const char text[] = "27015";
    lh_net_port_t out;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_net_port_parse(text, sizeof(text) - 1, &out));
    }
}
BENCHMARK(BM_net_port_parse);
