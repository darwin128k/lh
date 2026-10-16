#include <benchmark/benchmark.h>

#include <lh/math/mat4.h>
#include <lh/util/addr.h>

static void
BM_math_mat4_mul(benchmark::State &state)
{
    lh_math_vec3_t t, s;
    lh_math_vec3_init(lh_addr_of(t), 1.0f, 2.0f, 3.0f);
    lh_math_vec3_init(lh_addr_of(s), 2.0f, 3.0f, -1.0f);
    lh_math_mat4_t a = lh_math_mat4_from_translation(t);
    lh_math_mat4_t b = lh_math_mat4_from_scale(s);
    for (auto _ : state)
    {
        a = lh_math_mat4_mul(a, b);
        benchmark::DoNotOptimize(a);
    }
}
BENCHMARK(BM_math_mat4_mul);

static void
BM_math_mat4_transform_point(benchmark::State &state)
{
    lh_math_vec3_t t, p;
    lh_math_vec3_init(lh_addr_of(t), 1.0f, 2.0f, 3.0f);
    lh_math_vec3_init(lh_addr_of(p), 10.0f, 20.0f, 30.0f);
    const lh_math_mat4_t m = lh_math_mat4_from_translation(t);
    for (auto _ : state)
    {
        p = lh_math_mat4_transform_point(m, p);
        benchmark::DoNotOptimize(p);
    }
}
BENCHMARK(BM_math_mat4_transform_point);

static void
BM_math_mat4_inverse(benchmark::State &state)
{
    lh_math_vec3_t t;
    lh_math_vec3_init(lh_addr_of(t), 1.0f, 2.0f, 3.0f);
    lh_math_mat4_t m = lh_math_mat4_from_translation(t);
    lh_math_mat4_t out;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_math_mat4_inverse(m, lh_addr_of(out)));
        benchmark::DoNotOptimize(out);
        m = out;
    }
}
BENCHMARK(BM_math_mat4_inverse);

static void
BM_math_mat4_transpose(benchmark::State &state)
{
    lh_math_vec3_t t;
    lh_math_vec3_init(lh_addr_of(t), 1.0f, 2.0f, 3.0f);
    lh_math_mat4_t m = lh_math_mat4_from_translation(t);
    for (auto _ : state)
    {
        m = lh_math_mat4_transpose(m);
        benchmark::DoNotOptimize(m);
    }
}
BENCHMARK(BM_math_mat4_transpose);

static void
BM_math_mat4_from_quat(benchmark::State &state)
{
    lh_math_vec3_t axis;
    lh_math_vec3_init(lh_addr_of(axis), -1.0f, 2.0f, 0.5f);
    axis = lh_math_vec3_normalize(axis);
    const lh_math_quat_t q = lh_math_quat_from_axis_angle(axis, 1.3f);
    for (auto _ : state)
    {
        lh_math_mat4_t m = lh_math_mat4_from_quat(q);
        benchmark::DoNotOptimize(m);
    }
}
BENCHMARK(BM_math_mat4_from_quat);
