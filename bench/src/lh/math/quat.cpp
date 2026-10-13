#include <benchmark/benchmark.h>

#include <lh/math/quat.h>
#include <lh/math/vec3.h>
#include <lh/util/addr.h>

static void
BM_math_quat_mul(benchmark::State &state)
{
    lh_math_vec3_t axis;
    lh_math_vec3_init(lh_addr_of(axis), 0.0f, 1.0f, 0.0f);
    lh_math_quat_t a = lh_math_quat_from_axis_angle(axis, 0.3f);
    lh_math_quat_t b = lh_math_quat_from_axis_angle(axis, 0.7f);
    for (auto _ : state)
    {
        a = lh_math_quat_mul(a, b);
        benchmark::DoNotOptimize(a);
    }
}
BENCHMARK(BM_math_quat_mul);

static void
BM_math_quat_rotate(benchmark::State &state)
{
    lh_math_vec3_t axis, v;
    lh_math_vec3_init(lh_addr_of(axis), 0.0f, 1.0f, 0.0f);
    lh_math_vec3_init(lh_addr_of(v), 0.4f, -2.0f, 5.0f);
    const lh_math_quat_t q = lh_math_quat_from_axis_angle(axis, 1.3f);
    for (auto _ : state)
    {
        v = lh_math_quat_rotate(q, v);
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(BM_math_quat_rotate);

static void
BM_math_quat_from_axis_angle(benchmark::State &state)
{
    lh_math_vec3_t axis;
    lh_math_vec3_init(lh_addr_of(axis), -1.0f, 2.0f, 0.5f);
    axis = lh_math_vec3_normalize(axis);
    lh_math_fscalar_t angle = 0.0f;
    for (auto _ : state)
    {
        lh_math_quat_t q = lh_math_quat_from_axis_angle(axis, angle);
        angle += 0.01f;
        benchmark::DoNotOptimize(q);
    }
}
BENCHMARK(BM_math_quat_from_axis_angle);

static void
BM_math_quat_nlerp(benchmark::State &state)
{
    lh_math_vec3_t axis;
    lh_math_vec3_init(lh_addr_of(axis), 0.0f, 1.0f, 0.0f);
    const lh_math_quat_t a = lh_math_quat_from_axis_angle(axis, 0.0f);
    const lh_math_quat_t b = lh_math_quat_from_axis_angle(axis, 1.5f);
    lh_math_fscalar_t t = 0.0f;
    for (auto _ : state)
    {
        lh_math_quat_t q = lh_math_quat_nlerp(a, b, t);
        t += 0.001f;
        if (t > 1.0f)
        {
            t = 0.0f;
        }
        benchmark::DoNotOptimize(q);
    }
}
BENCHMARK(BM_math_quat_nlerp);
