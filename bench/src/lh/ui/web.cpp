/*
 * The web side: a frame recorded as JSON instead of drawn, the WebSocket framing
 * that carries it, and the player page the host serves.
 */

#include <benchmark/benchmark.h>

#include "screen.h"

#include <lh/ui/canvas/web.h>
#include <lh/ui/web/page.h>
#include <lh/ui/web/ws.h>

#include <algorithm>
#include <memory>
#include <vector>

using namespace lh_bench_ui;

/** A whole screen recorded as one JSON frame, glyph covers sent inline every time. */
static void
BM_ui_web_frame(benchmark::State &state)
{
    std::unique_ptr<screen> s(new screen(static_cast<int>(state.range(0)), 0));
    std::vector<char> json(4u << 20);
    lh_ui_canvas_web_t web;
    lh_ui_canvas_t canvas;
    lh_usize_t bytes = 0;
    lh_bool_t agree = lh_bool_true;

    for (auto _ : state)
    {
        lh_ui_canvas_web_open_mem(&web, json.data(), json.size(), width, height);
        lh_ui_canvas_init(&canvas, &lh_ui_canvas_backend_web, &web);
        lh_ui_view_set_canvas(&s->view, &canvas);
        lh_ui_view_draw(&s->view, nullptr);
        lh_ui_canvas_web_close(&web);
        bytes = lh_ui_canvas_web_length(&web);
        benchmark::DoNotOptimize(bytes);
        /* GCC 16.1 with LTO was seen returning a stale length here (clang and a
           non-LTO GCC build agree with the field). A wrong length is a broken
           frame, so it fails the run instead of printing a byte rate. */
        agree = agree && bytes == web.len;
    }
    lh_ui_view_set_canvas(&s->view, &s->surf.canvas);
    if (!agree)
    {
        state.SkipWithError("lh_ui_canvas_web_length disagrees with the frame it closed");
    }
    state.SetBytesProcessed(state.iterations() * static_cast<std::int64_t>(bytes));
    state.counters["frame_bytes"] = static_cast<double>(bytes);
    state.SetLabel(screen_name(state.range(0)));
}
BENCHMARK(BM_ui_web_frame)->Arg(screen_cards)->Arg(screen_flex)->Arg(screen_text)->Unit(benchmark::kMicrosecond);

/** The same frame for a reader that already holds every glyph cover: what a WS push costs. */
static void
BM_ui_web_frame_glyphs_cached(benchmark::State &state)
{
    std::unique_ptr<screen> s(new screen(static_cast<int>(state.range(0)), 0));
    std::vector<char> json(4u << 20);
    std::unique_ptr<lh_ui_canvas_web_glyphs_t> glyphs(new lh_ui_canvas_web_glyphs_t);
    lh_ui_canvas_web_t web;
    lh_ui_canvas_t canvas;
    lh_usize_t bytes = 0;
    lh_bool_t agree = lh_bool_true;

    lh_ui_canvas_web_glyphs_reset(glyphs.get());
    for (auto _ : state)
    {
        lh_ui_canvas_web_open_mem(&web, json.data(), json.size(), width, height);
        lh_ui_canvas_web_set_glyphs_v(&web, glyphs.get());
        lh_ui_canvas_init(&canvas, &lh_ui_canvas_backend_web, &web);
        lh_ui_view_set_canvas(&s->view, &canvas);
        lh_ui_view_draw(&s->view, nullptr);
        lh_ui_canvas_web_close(&web);
        bytes = lh_ui_canvas_web_length(&web);
        benchmark::DoNotOptimize(bytes);
        agree = agree && bytes == web.len;
    }
    lh_ui_view_set_canvas(&s->view, &s->surf.canvas);
    if (!agree)
    {
        state.SkipWithError("lh_ui_canvas_web_length disagrees with the frame it closed");
    }
    state.SetBytesProcessed(state.iterations() * static_cast<std::int64_t>(bytes));
    state.counters["frame_bytes"] = static_cast<double>(bytes);
    state.SetLabel(screen_name(state.range(0)));
}
BENCHMARK(BM_ui_web_frame_glyphs_cached)->Arg(screen_cards)->Arg(screen_text)->Unit(benchmark::kMicrosecond);

/** One unmasked server frame of the argument's bytes written. */
static void
BM_ui_web_ws_write(benchmark::State &state)
{
    const lh_usize_t n = static_cast<lh_usize_t>(state.range(0));
    std::vector<lh_byte_t> payload(n, 0x5Au);
    std::vector<lh_byte_t> out(lh_ui_web_ws_frame_size(n));

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_ui_web_ws_write(out.data(), out.size(), LH_UI_WEB_WS_TEXT, lh_bool_true,
                                                    payload.data(), n));
        benchmark::ClobberMemory();
    }
    state.SetBytesProcessed(state.iterations() * static_cast<std::int64_t>(n));
}
BENCHMARK(BM_ui_web_ws_write)->Arg(64)->Arg(4096)->Arg(65536);

/** One masked client frame parsed and unmasked: an input line from the player. */
static void
BM_ui_web_ws_read_unmask(benchmark::State &state)
{
    const lh_usize_t n = static_cast<lh_usize_t>(state.range(0));
    std::vector<lh_byte_t> frame(n + 14u);
    std::vector<lh_byte_t> work(frame.size());
    const lh_byte_t mask[4] = {0x12, 0x34, 0x56, 0x78};
    lh_usize_t head = 2u;

    frame[0] = 0x81u;
    if (n < 126u)
    {
        frame[1] = static_cast<lh_byte_t>(0x80u | n);
    }
    else
    {
        frame[1] = 0x80u | 126u;
        frame[2] = static_cast<lh_byte_t>(n >> 8);
        frame[3] = static_cast<lh_byte_t>(n & 0xFFu);
        head = 4u;
    }
    for (int i = 0; i < 4; ++i)
    {
        frame[head + static_cast<lh_usize_t>(i)] = mask[i];
    }
    for (lh_usize_t i = 0; i < n; ++i)
    {
        frame[head + 4u + i] = static_cast<lh_byte_t>('a' + (i % 26u));
    }
    const lh_usize_t total = head + 4u + n;

    for (auto _ : state)
    {
        unsigned opcode = 0;
        lh_bool_t fin = lh_bool_false;
        const lh_byte_t *payload = nullptr;
        lh_usize_t plen = 0;
        const lh_byte_t *m = nullptr;

        std::copy(frame.begin(), frame.begin() + static_cast<std::ptrdiff_t>(total), work.begin());
        benchmark::DoNotOptimize(lh_ui_web_ws_read(work.data(), total, &opcode, &fin, &payload, &plen, &m));
        if (m != nullptr)
        {
            lh_ui_web_ws_unmask(const_cast<lh_byte_t *>(payload), plen, m);
        }
        benchmark::DoNotOptimize(payload);
    }
    state.SetBytesProcessed(state.iterations() * static_cast<std::int64_t>(n));
}
BENCHMARK(BM_ui_web_ws_read_unmask)->Arg(16)->Arg(1024)->Arg(16384);

/** The Canvas2D player page, built from its markup and script entities. */
static void
BM_ui_web_page_build(benchmark::State &state)
{
    std::vector<lh_byte_t> mem(64u * 1024u);
    lh_usize_t len = 0;

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(lh_ui_web_page_build(mem.data(), mem.size(), &len));
        benchmark::ClobberMemory();
    }
    state.SetBytesProcessed(state.iterations() * static_cast<std::int64_t>(len));
}
BENCHMARK(BM_ui_web_page_build);

