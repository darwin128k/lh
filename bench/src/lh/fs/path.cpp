#include <benchmark/benchmark.h>

#include <lh/fs/path.h>
#include <lh/str/view.h>

static void
BM_fs_path_set(benchmark::State &state)
{
    lh_fs_path_t path;
    lh_fs_path_init(&path);
    const lh_str_view_t view = lh_str_view_lit("/usr/local/bin/tool");
    for (auto _ : state)
    {
        lh_fs_path_set(&path, view, lh_fs_path_style_posix);
        benchmark::DoNotOptimize(lh_fs_path_get_segment_count(&path));
    }
    lh_fs_path_deinit(&path);
}
BENCHMARK(BM_fs_path_set);

static void
BM_fs_path_join(benchmark::State &state)
{
    lh_fs_path_t dir, name, out;
    lh_fs_path_init(&dir);
    lh_fs_path_init(&name);
    lh_fs_path_init(&out);
    lh_fs_path_set(&dir, lh_str_view_lit("/a/b/c"), lh_fs_path_style_posix);
    lh_fs_path_set(&name, lh_str_view_lit("d/e/f"), lh_fs_path_style_posix);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_fs_path_join(&out, &dir, &name));
        benchmark::DoNotOptimize(lh_fs_path_get_segment_count(&out));
    }
    lh_fs_path_deinit(&out);
    lh_fs_path_deinit(&name);
    lh_fs_path_deinit(&dir);
}
BENCHMARK(BM_fs_path_join);

static void
BM_fs_path_parent(benchmark::State &state)
{
    lh_fs_path_t path, parent;
    lh_fs_path_init(&path);
    lh_fs_path_init(&parent);
    lh_fs_path_set(&path, lh_str_view_lit("/usr/local/bin/tool"), lh_fs_path_style_posix);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_fs_path_parent(&parent, &path));
        benchmark::DoNotOptimize(lh_fs_path_get_segment_count(&parent));
    }
    lh_fs_path_deinit(&parent);
    lh_fs_path_deinit(&path);
}
BENCHMARK(BM_fs_path_parent);

static void
BM_fs_path_get_segment(benchmark::State &state)
{
    lh_fs_path_t path;
    lh_fs_path_init(&path);
    lh_fs_path_set(&path, lh_str_view_lit("/usr/local/bin/tool/extra"), lh_fs_path_style_posix);
    const lh_usize_t count = lh_fs_path_get_segment_count(&path);
    lh_usize_t i = 0;
    for (auto _ : state)
    {
        lh_str_view_t seg = lh_fs_path_get_segment(&path, i % count);
        benchmark::DoNotOptimize(lh_str_view_get_size(&seg));
        ++i;
    }
    lh_fs_path_deinit(&path);
}
BENCHMARK(BM_fs_path_get_segment);
