#include <benchmark/benchmark.h>

#include <lh/str/split/next.h>
#include <lh/util/addr.h>

#include <cstring>

static const char k_path[] = "C:/Users/Vega/Projects/raspad-authum/lib/lh/include/lh/os/fs/path.h";
static const lh_char_t k_seps[] = {'/', '\\'};

static void
BM_str_ptr_split_next_slash(benchmark::State &state)
{
    const lh_usize_t n = static_cast<lh_usize_t>(std::strlen(k_path));
    for (auto _ : state)
    {
        lh_usize_t pos = 0;
        lh_str_cptr field;
        lh_usize_t field_size;
        lh_bool_t had_delim;
        lh_usize_t parts = 0;
        while (lh_str_ptr_split_next(k_path, n, '/', lh_addr_of(pos), lh_addr_of(field),
                                     lh_addr_of(field_size), lh_addr_of(had_delim)))
        {
            ++parts;
        }
        benchmark::DoNotOptimize(parts);
    }
}
BENCHMARK(BM_str_ptr_split_next_slash);

static void
BM_str_ptr_split_next_of_sep(benchmark::State &state)
{
    const lh_usize_t n = static_cast<lh_usize_t>(std::strlen(k_path));
    for (auto _ : state)
    {
        lh_usize_t pos = 0;
        lh_str_cptr field;
        lh_usize_t field_size;
        lh_bool_t had_delim;
        lh_usize_t parts = 0;
        while (lh_str_ptr_split_next_of(k_path, n, k_seps, 2, lh_addr_of(pos), lh_addr_of(field),
                                        lh_addr_of(field_size), lh_addr_of(had_delim)))
        {
            ++parts;
        }
        benchmark::DoNotOptimize(parts);
    }
}
BENCHMARK(BM_str_ptr_split_next_of_sep);

static void
BM_str_view_split_next_of_sep(benchmark::State &state)
{
    const lh_str_view_t text = lh_str_view_make(k_path);
    for (auto _ : state)
    {
        lh_usize_t pos = 0;
        lh_str_view_t field;
        lh_bool_t had_delim;
        lh_usize_t parts = 0;
        while (lh_str_view_split_next_of(lh_addr_of(text), k_seps, 2, lh_addr_of(pos),
                                         lh_addr_of(field), lh_addr_of(had_delim)))
        {
            ++parts;
        }
        benchmark::DoNotOptimize(parts);
    }
}
BENCHMARK(BM_str_view_split_next_of_sep);
