#include <benchmark/benchmark.h>

#include <lh/math/vec3.h>
#include <lh/util/addr.h>

static void
BM_math_vec3_add(benchmark::State &state)
{
    lh_math_vec3_t a, b;
    lh_math_vec3_init(lh_addr_of(a), 1.0f, 2.0f, 3.0f);
    lh_math_vec3_init(lh_addr_of(b), 4.0f, -5.0f, 0.5f);
    for (auto _ : state)
    {
        a = lh_math_vec3_add(a, b);
        benchmark::DoNotOptimize(a);
    }
}
BENCHMARK(BM_math_vec3_add);

static void
BM_math_vec3_dot(benchmark::State &state)
{
    lh_math_vec3_t a, b;
    lh_math_vec3_init(lh_addr_of(a), 1.0f, 2.0f, 3.0f);
    lh_math_vec3_init(lh_addr_of(b), 4.0f, -5.0f, 6.0f);
    lh_math_fscalar_t sum = 0.0f;
    for (auto _ : state)
    {
        sum += lh_math_vec3_dot(a, b);
        benchmark::DoNotOptimize(sum);
        a = lh_math_vec3_add(a, b);
    }
}
BENCHMARK(BM_math_vec3_dot);

static void
BM_math_vec3_cross(benchmark::State &state)
{
    lh_math_vec3_t a, b;
    lh_math_vec3_init(lh_addr_of(a), 1.0f, 2.0f, 3.0f);
    lh_math_vec3_init(lh_addr_of(b), 4.0f, -5.0f, 6.0f);
    for (auto _ : state)
    {
        a = lh_math_vec3_cross(a, b);
        benchmark::DoNotOptimize(a);
    }
}
BENCHMARK(BM_math_vec3_cross);

static void
BM_math_vec3_normalize(benchmark::State &state)
{
    lh_math_vec3_t v;
    lh_math_vec3_init(lh_addr_of(v), 2.0f, 3.0f, 6.0f);
    for (auto _ : state)
    {
        v = lh_math_vec3_normalize(v);
        v = lh_math_vec3_scale(v, 7.0f);
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(BM_math_vec3_normalize);

static void
BM_math_vec3_length(benchmark::State &state)
{
    lh_math_vec3_t v;
    lh_math_vec3_init(lh_addr_of(v), 2.0f, 3.0f, 6.0f);
    lh_math_fscalar_t sum = 0.0f;
    for (auto _ : state)
    {
        sum += lh_math_vec3_length(v);
        v = lh_math_vec3_add(v, v);
        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_math_vec3_length);

static void
BM_math_vec3_lerp(benchmark::State &state)
{
    lh_math_vec3_t a, b;
    lh_math_vec3_init(lh_addr_of(a), 0.0f, 0.0f, 0.0f);
    lh_math_vec3_init(lh_addr_of(b), 1.0f, 2.0f, 3.0f);
    lh_math_fscalar_t t = 0.0f;
    for (auto _ : state)
    {
        a = lh_math_vec3_lerp(a, b, t);
        t += 0.001f;
        if (t > 1.0f)
        {
            t = 0.0f;
        }
        benchmark::DoNotOptimize(a);
    }
}
BENCHMARK(BM_math_vec3_lerp);
