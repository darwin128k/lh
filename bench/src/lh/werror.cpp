#include <benchmark/benchmark.h>

#include <lh/util/addr.h>
#include <lh/util/wstr/ptr.h>
#include <lh/werror.h>
#include <lh/wstr/view.h>
#include <lh/wstr/view/initializer.h>

namespace
{

struct raw_ptr_werror
{
    lh_error_code_t code;
    const lh_wchar_t *desc;
};

struct view_werror
{
    lh_error_code_t code;
    lh_wstr_view_t desc;
};

const lh_wchar_t k_desc[] = L"invalid argument";

} // namespace

static void
BM_wstr_ptr_get_size_desc(benchmark::State &state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_wstr_ptr_get_size(k_desc));
    }
}
BENCHMARK(BM_wstr_ptr_get_size_desc);

static void
BM_wstr_ptr_len_desc(benchmark::State &state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_wstr_ptr_len(const_cast<lh_wstr_ptr>(k_desc)));
    }
}
BENCHMARK(BM_wstr_ptr_len_desc);

static void
BM_werror_make_lit(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_werror_t err = lh_werror_make(1, lh_wstr_view_lit(k_desc));
        benchmark::DoNotOptimize(err);
    }
}
BENCHMARK(BM_werror_make_lit);

static void
BM_werror_make_by_code(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_werror_t err = lh_werror_make_by_code(1);
        benchmark::DoNotOptimize(err);
    }
}
BENCHMARK(BM_werror_make_by_code);

static void
BM_werror_make_from_ptr_size(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_wstr_view_t desc;
        lh_wstr_view_init_by_size(lh_addr_of(desc), lh_wstr_ptr_get_begin(k_desc),
                                  lh_wstr_ptr_get_size(k_desc));
        lh_werror_t err = lh_werror_make(1, desc);
        benchmark::DoNotOptimize(err);
    }
}
BENCHMARK(BM_werror_make_from_ptr_size);

static void
BM_werror_make_from_ptr_len(benchmark::State &state)
{
    lh_wstr_cptr text = k_desc;
    for (auto _ : state)
    {
        lh_wstr_view_t desc;
        lh_wstr_view_init_by_size(lh_addr_of(desc), text,
                                  lh_wstr_ptr_len(const_cast<lh_wstr_ptr>(text)));
        lh_werror_t err = lh_werror_make(1, desc);
        benchmark::DoNotOptimize(err);
    }
}
BENCHMARK(BM_werror_make_from_ptr_len);

static void
BM_wstr_view_lit(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_wstr_view_t desc = lh_wstr_view_lit(k_desc);
        benchmark::DoNotOptimize(desc);
    }
}
BENCHMARK(BM_wstr_view_lit);

static void
BM_raw_ptr_werror_store(benchmark::State &state)
{
    for (auto _ : state)
    {
        raw_ptr_werror err;
        err.code = 1;
        err.desc = k_desc;
        benchmark::DoNotOptimize(err);
    }
}
BENCHMARK(BM_raw_ptr_werror_store);

static void
BM_view_werror_store_lit(benchmark::State &state)
{
    for (auto _ : state)
    {
        view_werror err;
        err.code = 1;
        err.desc = lh_wstr_view_lit(k_desc);
        benchmark::DoNotOptimize(err);
    }
}
BENCHMARK(BM_view_werror_store_lit);

static void
BM_werror_init_lit(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_werror_t err;
        lh_werror_init(lh_addr_of(err), 1, lh_wstr_view_lit(k_desc));
        benchmark::DoNotOptimize(err);
    }
}
BENCHMARK(BM_werror_init_lit);

static void
BM_werror_make_empty(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_werror_t err = lh_werror_make(1, lh_wstr_view_empty());
        benchmark::DoNotOptimize(err);
    }
}
BENCHMARK(BM_werror_make_empty);
