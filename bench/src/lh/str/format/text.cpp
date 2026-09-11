#include <bench/bench.h>

#include <lh/str/format/text.h>

BENCH(str_format_text_literal)
{
    char buf[64];
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_str_ptr_format_text(buf, sizeof(buf), "hello world"));
    }
}

BENCH(str_format_text_mixed_conversions)
{
    char buf[64];
    for (std::uint64_t i = 0; i < iterations; ++i)
    {
        bench::DoNotOptimize(lh_str_ptr_format_text(buf, sizeof(buf), "id=%u name=%s x=%x", 7U, "abc", 255U));
    }
}
