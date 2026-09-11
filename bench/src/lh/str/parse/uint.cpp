#include <bench/bench.h>

#include <lh/numeric/limits.h>
#include <lh/str/parse/uint.h>

BENCH(str_parse_uint_1_digit)
{
    const char text[] = "7";
    lh_uint_t out;
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_str_ptr_parse_uint(text, sizeof(text) - 1, LH_UINT_T_MAX, &out));
    }
}

BENCH(str_parse_uint_10_digits)
{
    const char text[] = "4294967295";
    lh_uint_t out;
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_str_ptr_parse_uint(text, sizeof(text) - 1, LH_UINT_T_MAX, &out));
    }
}
