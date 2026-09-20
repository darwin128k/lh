#include <benchmark/benchmark.h>

#include <lh/memory.h>
#include <lh/util/str/ptr.h>

#include <cstring>
#include <string>

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
BM_str_ptr_get_size(benchmark::State &state)
{
    const char text[] = "the quick brown fox jumps over the lazy dog";
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_ptr_get_size(text));
    }
}
BENCHMARK(BM_str_ptr_get_size);

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

static std::string
haystack_ending_in_sep(lh_usize_t n)
{
    std::string s(n, 'a');
    s.back() = '\\';
    return s;
}

static void
BM_str_ptr_find_of_char(benchmark::State &state)
{
    const std::string s = haystack_ending_in_sep(static_cast<lh_usize_t>(state.range(0)));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_ptr_find_of_char(s.data(), s.size(), '\\'));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_str_ptr_find_of_char)->Arg(64)->Arg(256)->Arg(4096);

static void
BM_memory_find_byte(benchmark::State &state)
{
    const std::string s = haystack_ending_in_sep(static_cast<lh_usize_t>(state.range(0)));
    const char needle = '\\';
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_memory_find(s.data(), s.size(), &needle, 1));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_memory_find_byte)->Arg(64)->Arg(256)->Arg(4096);

static void
BM_crt_memchr(benchmark::State &state)
{
    const std::string s = haystack_ending_in_sep(static_cast<lh_usize_t>(state.range(0)));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(std::memchr(s.data(), '\\', s.size()));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_crt_memchr)->Arg(64)->Arg(256)->Arg(4096);

static void
BM_str_ptr_find_of_chars_sep(benchmark::State &state)
{
    const std::string s = haystack_ending_in_sep(static_cast<lh_usize_t>(state.range(0)));
    const char seps[] = {'/', '\\'};
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_ptr_find_of_chars(s.data(), s.size(), seps, 2));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * state.range(0));
}
BENCHMARK(BM_str_ptr_find_of_chars_sep)->Arg(64)->Arg(256)->Arg(4096);
