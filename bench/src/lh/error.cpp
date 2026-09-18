#include <benchmark/benchmark.h>

#include <lh/error.h>
#include <lh/exception/origin.h>
#include <lh/exception/origin/initializer.h>
#include <lh/runtime/error.h>
#include <lh/str/view.h>
#include <lh/str/view/initializer.h>
#include <lh/util/addr.h>
#include <lh/util/str/ptr.h>

namespace
{

struct raw_ptr_error
{
    lh_error_code_t code;
    const char *desc;
};

struct view_error
{
    lh_error_code_t code;
    lh_str_view_t desc;
};

struct raw_ptr_origin
{
    const char *timestamp;
    const char *filename;
    const char *function;
    lh_usize_t line;
};

const char k_desc[] = "invalid argument";

} // namespace

static void
BM_str_ptr_get_size_desc(benchmark::State &state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_ptr_get_size(k_desc));
    }
}
BENCHMARK(BM_str_ptr_get_size_desc);

static void
BM_str_ptr_len_desc(benchmark::State &state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_str_ptr_len(const_cast<lh_str_ptr>(k_desc)));
    }
}
BENCHMARK(BM_str_ptr_len_desc);

static void
BM_error_make_lit(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_error_t err = lh_error_make(1, lh_str_view_lit(k_desc));
        benchmark::DoNotOptimize(err);
    }
}
BENCHMARK(BM_error_make_lit);

static void
BM_error_make_by_code(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_error_t err = lh_error_make_by_code(1);
        benchmark::DoNotOptimize(err);
    }
}
BENCHMARK(BM_error_make_by_code);

static void
BM_error_make_from_ptr_size(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_str_view_t desc;
        lh_str_init_by_size(lh_addr_of(desc), lh_str_ptr_get_begin(k_desc),
                            lh_str_ptr_get_size(k_desc));
        lh_error_t err = lh_error_make(1, desc);
        benchmark::DoNotOptimize(err);
    }
}
BENCHMARK(BM_error_make_from_ptr_size);

static void
BM_error_make_from_ptr_len(benchmark::State &state)
{
    lh_str_cptr text = k_desc;
    for (auto _ : state)
    {
        lh_str_view_t desc;
        lh_str_init_by_size(lh_addr_of(desc), text, lh_str_ptr_len(const_cast<lh_str_ptr>(text)));
        lh_error_t err = lh_error_make(1, desc);
        benchmark::DoNotOptimize(err);
    }
}
BENCHMARK(BM_error_make_from_ptr_len);

static void
BM_runtime_error_make_lit(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_runtime_error_t err =
            lh_runtime_error_make(lh_runtime_error_code_null_pointer,
                                  lh_str_view_lit("invalid reference to null pointer"));
        benchmark::DoNotOptimize(err);
    }
}
BENCHMARK(BM_runtime_error_make_lit);

static void
BM_str_view_lit(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_str_view_t desc = lh_str_view_lit(k_desc);
        benchmark::DoNotOptimize(desc);
    }
}
BENCHMARK(BM_str_view_lit);

static void
BM_raw_ptr_error_store(benchmark::State &state)
{
    for (auto _ : state)
    {
        raw_ptr_error err;
        err.code = 1;
        err.desc = k_desc;
        benchmark::DoNotOptimize(err);
    }
}
BENCHMARK(BM_raw_ptr_error_store);

static void
BM_view_error_store_lit(benchmark::State &state)
{
    for (auto _ : state)
    {
        view_error err;
        err.code = 1;
        err.desc = lh_str_view_lit(k_desc);
        benchmark::DoNotOptimize(err);
    }
}
BENCHMARK(BM_view_error_store_lit);

static void
BM_error_init_lit(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_error_t err;
        lh_error_init(lh_addr_of(err), 1, lh_str_view_lit(k_desc));
        benchmark::DoNotOptimize(err);
    }
}
BENCHMARK(BM_error_init_lit);

static void
BM_error_make_empty(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_error_t err = lh_error_make(1, lh_str_view_empty());
        benchmark::DoNotOptimize(err);
    }
}
BENCHMARK(BM_error_make_empty);

static void
BM_exception_origin_now(benchmark::State &state)
{
    for (auto _ : state)
    {
        lh_exception_origin_t origin = lh_exception_origin_initializer_now();
        benchmark::DoNotOptimize(origin);
    }
}
BENCHMARK(BM_exception_origin_now);

static void
BM_raw_ptr_origin_store(benchmark::State &state)
{
    for (auto _ : state)
    {
        raw_ptr_origin origin;
        origin.timestamp = __TIMESTAMP__;
        origin.filename = __FILE__;
        origin.function = __FUNCTION__;
        origin.line = __LINE__;
        benchmark::DoNotOptimize(origin);
    }
}
BENCHMARK(BM_raw_ptr_origin_store);
