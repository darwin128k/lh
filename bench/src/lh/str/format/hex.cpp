#include <bench/bench.h>

#include <lh/str/format/hex.h>

BENCH(str_format_hex_1_digit)
{
    char buf[32];
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_str_ptr_format_hex(0xF, lh_bool_false, buf, sizeof(buf)));
    }
}

BENCH(str_format_hex_8_digits)
{
    char buf[32];
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_str_ptr_format_hex(0xFFFFFFFFU, lh_bool_false, buf, sizeof(buf)));
    }
}
