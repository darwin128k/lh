#include <benchmark/benchmark.h>

#include <lh/util/str/ptr.h>

#include <cstring>

static void
BM_str_ptr_len(benchmark::State &state)
{
    const char text[] = "the quick brown fox jumps over the lazy dog";
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_ptr_len(const_cast<lh_str_ptr>(text)));
    }
}
BENCHMARK(BM_str_ptr_len);

static void
BM_str_ptr_find(benchmark::State &state)
{
    const char text[] = "the quick brown fox jumps over the lazy dog";
    const char needle[] = "lazy";
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_ptr_find(const_cast<lh_str_ptr>(text), std::strlen(text),
                                                 const_cast<lh_str_ptr>(needle),
                                                 std::strlen(needle), lh_bool_false));
    }
}
BENCHMARK(BM_str_ptr_find);

static void
BM_str_ptr_equals(benchmark::State &state)
{
    const char a[] = "the quick brown fox jumps over the lazy dog";
    const char b[] = "the quick brown fox jumps over the lazy dog";
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(
            lh_str_ptr_equals(const_cast<lh_str_ptr>(a), const_cast<lh_str_ptr>(b), lh_bool_false));
    }
}
BENCHMARK(BM_str_ptr_equals);

static void
BM_str_ptr_to_lower_45B(benchmark::State &state)
{
    char buf[] = "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG";
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_ptr_to_lower(buf, sizeof(buf) - 1));
    }
}
BENCHMARK(BM_str_ptr_to_lower_45B);

static void
BM_str_ptr_trim(benchmark::State &state)
{
    char text[] = "   the quick brown fox   ";
    for (auto _ : state)
    {
        char buf[sizeof(text)];
        std::memcpy(buf, text, sizeof(text));
        benchmark::DoNotOptimize(lh_str_ptr_trim(buf));
    }
}
BENCHMARK(BM_str_ptr_trim);
