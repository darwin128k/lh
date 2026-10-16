#include <benchmark/benchmark.h>

#include <lh/math/rect.h>
#include <lh/util/addr.h>

static void
BM_math_rect_contains_point(benchmark::State &state)
{
    lh_math_rect_t r;
    lh_math_point_t p;
    lh_math_rect_init(lh_addr_of(r), 0, 0, 100, 80);
    lh_math_point_init(lh_addr_of(p), 40, 30);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_math_rect_contains_point(lh_addr_of(r), p));
        lh_math_point_set_x(lh_addr_of(p), static_cast<lh_math_scalar_t>(
                                               (lh_math_point_get_x(lh_addr_of(p)) + 1) % 120));
    }
}
BENCHMARK(BM_math_rect_contains_point);

static void
BM_math_rect_intersects(benchmark::State &state)
{
    lh_math_rect_t a, b;
    lh_math_rect_init(lh_addr_of(a), 0, 0, 100, 80);
    lh_math_rect_init(lh_addr_of(b), 50, 40, 100, 80);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_math_rect_intersects(lh_addr_of(a), lh_addr_of(b)));
        lh_math_point_t *origin = lh_math_rect_get_origin(lh_addr_of(b));
        lh_math_point_set_x(origin, static_cast<lh_math_scalar_t>((lh_math_point_get_x(origin) + 1) % 150));
    }
}
BENCHMARK(BM_math_rect_intersects);

static void
BM_math_rect_intersection(benchmark::State &state)
{
    lh_math_rect_t a, b;
    lh_math_rect_init(lh_addr_of(a), 0, 0, 100, 80);
    lh_math_rect_init(lh_addr_of(b), 50, 40, 100, 80);
    for (auto _ : state)
    {
        lh_math_rect_t out = lh_math_rect_intersection(lh_addr_of(a), lh_addr_of(b));
        benchmark::DoNotOptimize(out);
        lh_math_point_t *origin = lh_math_rect_get_origin(lh_addr_of(b));
        lh_math_point_set_x(origin, static_cast<lh_math_scalar_t>((lh_math_point_get_x(origin) + 1) % 150));
    }
}
BENCHMARK(BM_math_rect_intersection);

static void
BM_math_rect_union(benchmark::State &state)
{
    lh_math_rect_t a, b;
    lh_math_rect_init(lh_addr_of(a), 0, 0, 100, 80);
    lh_math_rect_init(lh_addr_of(b), 50, 40, 100, 80);
    for (auto _ : state)
    {
        lh_math_rect_t out = lh_math_rect_union(lh_addr_of(a), lh_addr_of(b));
        benchmark::DoNotOptimize(out);
        lh_math_point_t *origin = lh_math_rect_get_origin(lh_addr_of(b));
        lh_math_point_set_x(origin, static_cast<lh_math_scalar_t>((lh_math_point_get_x(origin) + 1) % 150));
    }
}
BENCHMARK(BM_math_rect_union);
