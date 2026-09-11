#include <bench/bench.h>

#include <lh/str/format/uint.h>

BENCH(str_format_uint_1_digit)
{
    char buf[32];
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_str_ptr_format_uint(7, buf, sizeof(buf)));
    }
}

BENCH(str_format_uint_10_digits)
{
    char buf[32];
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_str_ptr_format_uint(4294967295U, buf, sizeof(buf)));
    }
}
