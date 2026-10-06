/**
 * @file float_trunc.c
 * @brief Cost of cutting a float down to an int.
 *
 * The cut is the cast to ::lh_int_t: truncation toward zero, the same
 * conversion ::lh_ui_point_to_ipoint uses when the scalar is
 * ::lh_float_t. Not part of lh_test. Build and run it on its own.
 */

#include <stdio.h>
#include <windows.h>

#include <lh/cast/static.h>
#include <lh/numeric/float.h>
#include <lh/numeric/types.h>

enum
{
    FLOAT_TRUNC_N = 4096,
    FLOAT_TRUNC_PASSES = 20000,
    FLOAT_TRUNC_TRIALS = 7
};

static lh_ullong_t
float_trunc_now(void)
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return lh_cast_static(lh_ullong_t, counter.QuadPart);
}

static lh_ullong_t
float_trunc_hz(void)
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return lh_cast_static(lh_ullong_t, frequency.QuadPart);
}

/* One cut per element. Values stay live so the cast cannot be deleted. */
static lh_int_t
float_trunc_cut(const lh_float_t *values, lh_int_t n, lh_int_t passes)
{
    lh_int_t sum = 0;
    lh_int_t pass;
    for (pass = 0; pass < passes; ++pass)
    {
        lh_int_t i;
        for (i = 0; i < n; ++i)
        {
            sum += lh_cast_static(lh_int_t, values[i]);
        }
    }
    return sum;
}

/* Same traffic, already integers. The difference is the cut. */
static lh_int_t
float_trunc_int_add(const lh_int_t *values, lh_int_t n, lh_int_t passes)
{
    lh_int_t sum = 0;
    lh_int_t pass;
    for (pass = 0; pass < passes; ++pass)
    {
        lh_int_t i;
        for (i = 0; i < n; ++i)
        {
            sum += values[i];
        }
    }
    return sum;
}

/* Each cut waits on the previous one: latency of one coordinate. */
static lh_int_t
float_trunc_chain(lh_float_t x, lh_int_t steps)
{
    lh_int_t step;
    for (step = 0; step < steps; ++step)
    {
        lh_int_t cut = lh_cast_static(lh_int_t, x);
        __asm__ __volatile__("" : "+r"(cut));
        x = lh_cast_static(lh_float_t, cut) + 0.25f;
        __asm__ __volatile__("" : "+x"(x));
    }
    return lh_cast_static(lh_int_t, x);
}

static double
float_trunc_ns(lh_ullong_t ticks, lh_ullong_t hz, lh_ullong_t ops)
{
    return (lh_cast_static(double, ticks) * 1000000000.0) / lh_cast_static(double, hz) / lh_cast_static(double, ops);
}

static lh_ullong_t
float_trunc_min_ticks(lh_ullong_t *samples, lh_int_t count)
{
    lh_ullong_t best = samples[0];
    lh_int_t i;
    for (i = 1; i < count; ++i)
    {
        if (samples[i] < best)
        {
            best = samples[i];
        }
    }
    return best;
}

int
main(void)
{
    lh_float_t floats[FLOAT_TRUNC_N];
    lh_int_t ints[FLOAT_TRUNC_N];
    lh_ullong_t cut_ticks[FLOAT_TRUNC_TRIALS];
    lh_ullong_t add_ticks[FLOAT_TRUNC_TRIALS];
    lh_ullong_t chain_ticks[FLOAT_TRUNC_TRIALS];
    const lh_ullong_t hz = float_trunc_hz();
    const lh_ullong_t bulk_ops = lh_cast_static(lh_ullong_t, FLOAT_TRUNC_N) * FLOAT_TRUNC_PASSES;
    const lh_ullong_t chain_ops = bulk_ops;
    lh_int_t i;
    lh_int_t trial;
    lh_int_t sink = 0;

    for (i = 0; i < FLOAT_TRUNC_N; ++i)
    {
        const lh_float_t x = lh_cast_static(lh_float_t, (i % 200) - 100) + 0.9f;
        floats[i] = x;
        ints[i] = lh_cast_static(lh_int_t, x);
    }

    sink += float_trunc_cut(floats, FLOAT_TRUNC_N, 1000);
    sink += float_trunc_int_add(ints, FLOAT_TRUNC_N, 1000);
    sink += float_trunc_chain(1.9f, 1000);

    for (trial = 0; trial < FLOAT_TRUNC_TRIALS; ++trial)
    {
        const lh_ullong_t t0 = float_trunc_now();
        sink += float_trunc_cut(floats, FLOAT_TRUNC_N, FLOAT_TRUNC_PASSES);
        cut_ticks[trial] = float_trunc_now() - t0;
    }
    for (trial = 0; trial < FLOAT_TRUNC_TRIALS; ++trial)
    {
        const lh_ullong_t t0 = float_trunc_now();
        sink += float_trunc_int_add(ints, FLOAT_TRUNC_N, FLOAT_TRUNC_PASSES);
        add_ticks[trial] = float_trunc_now() - t0;
    }
    for (trial = 0; trial < FLOAT_TRUNC_TRIALS; ++trial)
    {
        const lh_ullong_t t0 = float_trunc_now();
        sink += float_trunc_chain(1.9f, lh_cast_static(lh_int_t, chain_ops));
        chain_ticks[trial] = float_trunc_now() - t0;
    }

    printf("sink %d\n", sink);
    printf("cut_throughput_ns %.3f\n", float_trunc_ns(float_trunc_min_ticks(cut_ticks, FLOAT_TRUNC_TRIALS), hz, bulk_ops));
    printf("int_add_throughput_ns %.3f\n", float_trunc_ns(float_trunc_min_ticks(add_ticks, FLOAT_TRUNC_TRIALS), hz, bulk_ops));
    printf("cut_chain_ns %.3f\n", float_trunc_ns(float_trunc_min_ticks(chain_ticks, FLOAT_TRUNC_TRIALS), hz, chain_ops));
    return sink == 0 ? 1 : 0;
}
