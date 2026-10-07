/**
 * @file partial_probe.h
 * @brief Test helper: a backend with `begin_area`, drawing each strip where it
 *        belongs on the whole target.
 *
 * ::lh_test::partial_probe is the one piece a partial test needs twice: the
 * strip lands on a real pixel buffer, so a frame drawn strip by strip can be
 * compared with the same frame drawn in one go, pixel for pixel. It records
 * every area it was given, and whether any primitive ever left the buffer.
 *
 * The buffer here is the whole target, which is what a real partial backend
 * avoids by keeping only the strip. That does not change where the pixels go:
 * the canvas hands the backend buffer-space coordinates, so the probe puts them
 * back at the area corner and the picture is the same either way.
 */

#ifndef LH_TEST_UI_PARTIAL_PROBE_H
#define LH_TEST_UI_PARTIAL_PROBE_H

#include <lh/ptr.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/sw.h>
#include <lh/ui/color.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace lh_test
{

struct partial_probe
{
    static const int width = 160;
    static const int height = 120;
    static const int capacity = 32;
    static const lh_u32_t sentinel = 0x00123456u; /* transparent, so blends over it see no dst */

    lh_u32_t words[width * height];
    lh_ui_pixmap_t pixmap;
    lh_ui_canvas_sw_t sw;
    lh_ui_canvas_t canvas;
    lh_ui_rect_t area; /* the strip being drawn, or empty outside a frame */
    lh_ui_rect_t areas[capacity];
    int area_count;
    int begin_count;
    int end_count;
    bool escaped; /* a primitive reached past the buffer of its own frame */
};

inline bool
partial_probe_is_inside(const partial_probe *probe, const lh_ui_rect_t *buffer)
{
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(lh_addr_of(probe->area));
    lh_ui_rect_t window;
    lh_ui_rect_t part;

    lh_ui_rect_init(lh_addr_of(window), lh_ui_scalar(0), lh_ui_scalar(0), lh_ui_size_get_width(lh_addr_of(*size)),
                    lh_ui_size_get_height(lh_addr_of(*size)));
    part = lh_ui_rect_intersection(lh_addr_of(window), buffer);
    return lh_ui_rect_equals(lh_addr_of(part), buffer);
}

inline void
partial_probe_place(lh_ui_rect_t *placed, const lh_ui_rect_t *buffer, partial_probe *probe)
{
    const lh_ui_point_t *at = lh_ui_rect_get_origin_as_const(lh_addr_of(probe->area));

    if (!partial_probe_is_inside(probe, buffer))
    {
        probe->escaped = true;
    }
    *placed = lh_ui_rect_offset(buffer, lh_ui_point_get_x(at), lh_ui_point_get_y(at));
}

inline void
partial_probe_begin_area(lh_ptr context, const lh_ui_rect_t *area)
{
    partial_probe *probe = lh_ptr_rcast(partial_probe, context);
    if (probe->area_count < partial_probe::capacity)
    {
        probe->areas[probe->area_count] = *area;
    }
    ++probe->area_count;
    probe->area = *area;
}

inline void
partial_probe_begin(lh_ptr context)
{
    partial_probe *probe = lh_ptr_rcast(partial_probe, context);
    ++probe->begin_count;
    lh_ui_rect_init_empty(lh_addr_of(probe->area));
}

inline void
partial_probe_end(lh_ptr context)
{
    partial_probe *probe = lh_ptr_rcast(partial_probe, context);
    ++probe->end_count;
}

inline lh_void
partial_probe_fill_rect(lh_ptr context, const lh_ui_rect_t *rect, const lh_ui_color_t *color)
{
    partial_probe *probe = lh_ptr_rcast(partial_probe, context);
    lh_ui_rect_t placed;

    partial_probe_place(lh_addr_of(placed), rect, probe);
    lh_ui_canvas_sw_fill_rect(lh_addr_of(probe->sw), lh_addr_of(placed), color);
}

inline lh_bool_t
partial_probe_fill_round_rect(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                              const lh_ui_color_t *color)
{
    partial_probe *probe = lh_ptr_rcast(partial_probe, context);
    lh_ui_rect_t placed;

    partial_probe_place(lh_addr_of(placed), rect, probe);
    return lh_ui_canvas_sw_fill_round_rect(lh_addr_of(probe->sw), lh_addr_of(placed), radius, color);
}

inline lh_void
partial_probe_clear(lh_ptr context, const lh_ui_color_t *color)
{
    partial_probe *probe = lh_ptr_rcast(partial_probe, context);
    partial_probe_fill_rect(context, lh_addr_of(probe->area), color);
}

/** The backend: `begin_area`, software drawing, no `set_clip` so the canvas
 *  cuts every primitive to the strip itself and the probe only places it. */
inline const lh_ui_canvas_backend_t *
partial_probe_backend()
{
    static const lh_ui_canvas_backend_t backend = {
        partial_probe_begin,   partial_probe_begin_area, partial_probe_end,   partial_probe_clear,
        partial_probe_fill_rect, partial_probe_fill_round_rect, nullptr,     nullptr};
    return &backend;
}

inline void
partial_probe_init(partial_probe *probe)
{
    lh_ui_size_t size;

    *probe = partial_probe{};
    for (lh_u32_t &w : probe->words)
    {
        w = partial_probe::sentinel;
    }
    lh_ui_rect_init_empty(lh_addr_of(probe->area));
    lh_ui_pixmap_init(lh_addr_of(probe->pixmap), lh_ptr_rcast(lh_byte_t, probe->words),
                      partial_probe::width, partial_probe::height, partial_probe::width * 4,
                      lh_ui_pixmap_format_argb8888);
    lh_ui_canvas_sw_init(lh_addr_of(probe->sw));
    lh_ui_canvas_sw_set_pixmap(lh_addr_of(probe->sw), lh_addr_of(probe->pixmap));
    lh_ui_canvas_init(lh_addr_of(probe->canvas), partial_probe_backend(), probe);
    lh_ui_size_init(lh_addr_of(size), lh_ui_scalar(partial_probe::width), lh_ui_scalar(partial_probe::height));
    lh_ui_canvas_set_size(lh_addr_of(probe->canvas), size);
}

inline lh_u32_t
partial_probe_at(const partial_probe &probe, int x, int y)
{
    return lh_ui_pixmap_read_word(lh_addr_of(probe.pixmap), x, y);
}

/** Pixels of @p a and @p b that differ over the whole target. */
inline int
partial_probe_diff(const partial_probe &a, const partial_probe &b)
{
    int different = 0;
    for (int i = 0; i < partial_probe::width * partial_probe::height; ++i)
    {
        if (a.words[i] != b.words[i])
        {
            ++different;
        }
    }
    return different;
}

} // namespace lh_test

#endif /* LH_TEST_UI_PARTIAL_PROBE_H */
