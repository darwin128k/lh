#include <benchmark/benchmark.h>

#include <lh/os/fs/path.h>
#include <lh/str/view.h>
#include <lh/util/addr.h>

static const char k_path[] = "C:/Users/Vega/Projects/raspad-authum/lib/lh/include/lh/os/fs/path.h";

static void
BM_os_fs_path_is_sep(benchmark::State &state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_os_fs_path_is_sep('\\'));
        benchmark::DoNotOptimize(lh_os_fs_path_is_sep('a'));
    }
}
BENCHMARK(BM_os_fs_path_is_sep);

static void
BM_os_fs_path_set(benchmark::State &state)
{
    lh_os_fs_path_t path;
    lh_os_fs_path_init(lh_addr_of(path));
    const lh_str_view_t text = lh_str_view_make(k_path);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_os_fs_path_set(lh_addr_of(path), text));
    }
    lh_os_fs_path_deinit(lh_addr_of(path));
}
BENCHMARK(BM_os_fs_path_set);

static void
BM_os_fs_path_join(benchmark::State &state)
{
    lh_os_fs_path_t dir;
    lh_os_fs_path_t name;
    lh_os_fs_path_t out;
    lh_os_fs_path_init(lh_addr_of(dir));
    lh_os_fs_path_init(lh_addr_of(name));
    lh_os_fs_path_init(lh_addr_of(out));
    lh_os_fs_path_set(lh_addr_of(dir), lh_str_view_make("C:/Users/Vega/Projects"));
    lh_os_fs_path_set(lh_addr_of(name), lh_str_view_make("file.txt"));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(
            lh_os_fs_path_join(lh_addr_of(out), lh_addr_of(dir), lh_addr_of(name)));
    }
    lh_os_fs_path_deinit(lh_addr_of(out));
    lh_os_fs_path_deinit(lh_addr_of(name));
    lh_os_fs_path_deinit(lh_addr_of(dir));
}
BENCHMARK(BM_os_fs_path_join);

static void
BM_os_fs_path_dir(benchmark::State &state)
{
    lh_os_fs_path_t path;
    lh_os_fs_path_t out;
    lh_os_fs_path_init(lh_addr_of(path));
    lh_os_fs_path_init(lh_addr_of(out));
    lh_os_fs_path_set(lh_addr_of(path), lh_str_view_make(k_path));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_os_fs_path_dir(lh_addr_of(path), lh_addr_of(out)));
    }
    lh_os_fs_path_deinit(lh_addr_of(out));
    lh_os_fs_path_deinit(lh_addr_of(path));
}
BENCHMARK(BM_os_fs_path_dir);

static void
BM_os_fs_path_is_root(benchmark::State &state)
{
    lh_os_fs_path_t path;
    lh_os_fs_path_init(lh_addr_of(path));
    lh_os_fs_path_set(lh_addr_of(path), lh_str_view_make("C:/"));
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_os_fs_path_is_root(lh_addr_of(path)));
    }
    lh_os_fs_path_deinit(lh_addr_of(path));
}
BENCHMARK(BM_os_fs_path_is_root);

static void
BM_os_fs_path_get_part(benchmark::State &state)
{
    lh_os_fs_path_t path;
    lh_os_fs_path_init(lh_addr_of(path));
    lh_os_fs_path_set(lh_addr_of(path), lh_str_view_make(k_path));
    const lh_usize_t n = lh_os_fs_path_get_part_count(lh_addr_of(path));
    for (auto _ : state)
    {
        lh_str_view_t last = lh_os_fs_path_get_part(lh_addr_of(path), n - 1U);
        benchmark::DoNotOptimize(last);
    }
    lh_os_fs_path_deinit(lh_addr_of(path));
}
BENCHMARK(BM_os_fs_path_get_part);
