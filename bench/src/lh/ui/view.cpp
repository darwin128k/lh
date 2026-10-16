/*
 * A whole tree on the software canvas: what a frame of a real screen costs, and what
 * input costs on the same tree.
 *
 * Three screens, the ones bench/lvgl compares against LVGL:
 *   cards -- 24 rounded cards with a caption each, absolutely placed;
 *   flex  -- 32 cells in 8 rows, placed by flex layouts;
 *   text  -- four paragraphs wrapped at their boxes.
 * The pixel format is the last argument (0 ARGB8888, 1 RGB565).
 */

#include <benchmark/benchmark.h>

#include "screen.h"

#include <memory>

using namespace lh_bench_ui;

/** The whole screen, every frame. */
static void
BM_ui_view_paint(benchmark::State &state)
{
    std::unique_ptr<screen> s(new screen(static_cast<int>(state.range(0)), static_cast<int>(state.range(1))));

    for (auto _ : state)
    {
        lh_ui_view_paint(&s->view);
        benchmark::ClobberMemory();
    }
    screen_label(state);
}
BENCHMARK(BM_ui_view_paint)->ArgsProduct({{screen_cards, screen_flex, screen_text}, {0, 1}})->Unit(benchmark::kMicrosecond);

/** One card's area redrawn: what a hover or a changed caption costs. */
static void
BM_ui_view_draw_partial(benchmark::State &state)
{
    std::unique_ptr<screen> s(new screen(static_cast<int>(state.range(0)), static_cast<int>(state.range(1))));

    for (auto _ : state)
    {
        lh_ui_view_draw(&s->view, &s->partial);
        benchmark::ClobberMemory();
    }
    screen_label(state);
}
BENCHMARK(BM_ui_view_draw_partial)->ArgsProduct({{screen_cards, screen_flex, screen_text}, {0, 1}})->Unit(benchmark::kMicrosecond);

/** The flex screen laid out again from its root, without drawing. */
static void
BM_ui_layout_apply_flex(benchmark::State &state)
{
    std::unique_ptr<screen> s(new screen(screen_flex, 0));

    for (auto _ : state)
    {
        lh_ui_layout_apply(&s->down, lh_ui_container_as_entity(&s->root));
        for (int i = 0; i < 8; ++i)
        {
            lh_ui_layout_apply(&s->across, lh_ui_container_as_entity(&s->rows[i]));
        }
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_ui_layout_apply_flex);

/** Which entity is under a point, walking the cards screen. */
static void
BM_ui_view_hit_test(benchmark::State &state)
{
    std::unique_ptr<screen> s(new screen(static_cast<int>(state.range(0)), 0));
    lh_ui_point_t p;
    int i = 0;

    for (auto _ : state)
    {
        lh_ui_point_init(&p, lh_ui_scalar((i * 37) % width), lh_ui_scalar((i * 23) % height));
        benchmark::DoNotOptimize(lh_ui_view_hit_test(&s->view, p));
        ++i;
    }
    state.SetLabel(screen_name(state.range(0)));
}
BENCHMARK(BM_ui_view_hit_test)->Arg(screen_cards)->Arg(screen_flex);

/** A press and a release on the same card: focus, pressed state, synthesized click. */
static void
BM_ui_view_press_release(benchmark::State &state)
{
    std::unique_ptr<screen> s(new screen(screen_cards, 0));
    lh_ui_point_t p;

    lh_ui_point_init(&p, lh_ui_scalar(40), lh_ui_scalar(40));
    for (auto _ : state)
    {
        lh_ui_view_press(&s->view, p);
        benchmark::DoNotOptimize(lh_ui_view_release(&s->view, p));
    }
}
BENCHMARK(BM_ui_view_press_release);

/** The 60 Hz tick with nothing gliding and no transition in flight. */
static void
BM_ui_view_tick_idle(benchmark::State &state)
{
    std::unique_ptr<screen> s(new screen(screen_cards, 0));

    for (auto _ : state)
    {
        lh_ui_view_tick(&s->view);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_ui_view_tick_idle);
