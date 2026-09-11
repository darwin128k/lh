#include <bench/bench.h>

#include <lh/net/ip.h>

BENCH(net_ip4_parse)
{
    const char text[] = "192.168.0.1";
    lh_net_ip4_t out;
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_net_ip4_parse(text, sizeof(text) - 1, &out));
    }
}

BENCH(net_ip4_format)
{
    lh_net_ip4_t addr = lh_net_ip4_make(192, 168, 0, 1);
    char buf[LH_NET_IP4_TEXT_MAX];
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_net_ip4_format(&addr, buf, sizeof(buf)));
    }
}

BENCH(net_ip4_equals)
{
    lh_net_ip4_t a = lh_net_ip4_make(192, 168, 0, 1);
    lh_net_ip4_t b = lh_net_ip4_make(192, 168, 0, 2);
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_net_ip4_equals(&a, &b));
    }
}
