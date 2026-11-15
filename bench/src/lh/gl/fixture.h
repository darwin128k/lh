/*
 * fixture.h -- a 480 x 320 software canvas the UI benchmarks draw into.
 *
 * The same size as bench/lvgl/scene.h, so a number here and a scene there are the
 * same pixels. The pixel format is the benchmark's argument: 0 is ARGB8888 (a
 * desktop DIB), 1 is RGB565 (an MCU panel).
 */

#ifndef LH_BENCH_UI_FIXTURE_H
#define LH_BENCH_UI_FIXTURE_H

#include <lh/gl/canvas/sw.h>

#include <cstdint>
#include <vector>

namespace lh_bench_ui
{

constexpr int width = 480;
constexpr int height = 320;

/** A pixmap over its own pixels, a software context on it and a canvas on that. */
struct surface
{
    std::vector<std::uint32_t> pixels;
    std::vector<lh_u8_t> scratch;
    lh_gl_pixmap_t pixmap;
    lh_gl_canvas_sw_t sw;
    lh_gl_canvas_t canvas;

    explicit surface(int format)
        : pixels(static_cast<std::size_t>(width) * height, 0u), scratch(1u << 20, 0u)
    {
        const lh_gl_pixmap_format_t fmt =
            format == 0 ? lh_gl_pixmap_format_argb8888 : lh_gl_pixmap_format_rgb565;
        const int bytes = format == 0 ? 4 : 2;
        lh_gl_size_t size;

        lh_gl_pixmap_init(&pixmap, reinterpret_cast<lh_byte_t *>(pixels.data()), width, height, width * bytes, fmt);
        lh_gl_canvas_sw_init(&sw);
        lh_gl_canvas_sw_use_pixmap(&sw, &pixmap);
        lh_gl_canvas_init(&canvas, &lh_gl_canvas_backend_sw, &sw);
        lh_gl_size_init(&size, lh_gl_scalar(width), lh_gl_scalar(height));
        lh_gl_canvas_set_size(&canvas, size);
        lh_gl_canvas_use_scratch(&canvas, scratch.data(), scratch.size());
        clear_to_window();
    }

    /** What every frame starts on: an opaque background. A primitive over the
        zeroed buffer would blend into transparent ARGB pixels -- an off-screen
        layer, not a window, and a different (slower) path through the blend. */
    void
    clear_to_window()
    {
        lh_gl_color_t bg;

        lh_gl_color_init(&bg, 20, 24, 29, 255);
        lh_gl_pixmap_clear(&pixmap, &bg);
    }

    surface(const surface &) = delete;
    surface &operator=(const surface &) = delete;
};

inline lh_gl_color_t
rgb(int r, int g, int b, int a = 255)
{
    lh_gl_color_t c;
    lh_gl_color_init(&c, static_cast<lh_gl_color_channel_t>(r), static_cast<lh_gl_color_channel_t>(g),
                     static_cast<lh_gl_color_channel_t>(b), static_cast<lh_gl_color_channel_t>(a));
    return c;
}

inline lh_gl_rect_t
rect(int x, int y, int w, int h)
{
    lh_gl_rect_t r;
    lh_gl_rect_init(&r, lh_gl_scalar(x), lh_gl_scalar(y), lh_gl_scalar(w), lh_gl_scalar(h));
    return r;
}

inline const char *
format_name(int format)
{
    return format == 0 ? "argb8888" : "rgb565";
}

} // namespace lh_bench_ui

#endif /* LH_BENCH_UI_FIXTURE_H */
