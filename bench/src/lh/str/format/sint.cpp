#include <bench/bench.h>

#include <lh/str/format/sint.h>

BENCH(str_format_sint_1_digit)
{
    char buf[32];
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_str_ptr_format_sint(-7, buf, sizeof(buf)));
    }
}

BENCH(str_format_sint_10_digits)
{
    char buf[32];
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_str_ptr_format_sint(-2147483647, buf, sizeof(buf)));
    }
}
