/**
 * @file fill_probe.h
 * @brief Test helper: a canvas backend that counts one expected fill.
 *
 * ::lh_test::fill_probe holds the rect and color the base class owes. The
 * backend counts only calls that match both, so other draws (text, children)
 * do not disturb the count. Round-rect calls are counted separately and also
 * remember the radius the canvas passed down.
 */

#ifndef LH_TEST_UI_FILL_PROBE_H
#define LH_TEST_UI_FILL_PROBE_H

#include <lh/ptr.h>
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace lh_test
{

struct fill_probe
{
    lh_ui_rect_t rect;
    lh_ui_color_t color;
    int matches;
    int round_matches;
    lh_ui_scalar_t radius;
};

inline bool
fill_probe_matches(const fill_probe *probe, const lh_ui_rect_t *rect, const lh_ui_color_t *color)
{
    return lh_ui_rect_eq(rect, lh_addr_of(probe->rect)) &&
           lh_ui_color_equals(color, lh_addr_of(probe->color));
}

inline lh_void
fill_probe_fill_rect(lh_ptr context, const lh_ui_rect_t *rect, const lh_ui_color_t *color)
{
    fill_probe *probe = lh_ptr_rcast(fill_probe, context);
    if (fill_probe_matches(probe, rect, color))
    {
        ++probe->matches;
    }
}

inline lh_bool_t
fill_probe_fill_round_rect(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                           const lh_ui_color_t *color)
{
    fill_probe *probe = lh_ptr_rcast(fill_probe, context);
    if (fill_probe_matches(probe, rect, color))
    {
        ++probe->round_matches;
        probe->radius = radius;
    }
    return lh_bool_true;
}

/** Backend with only fill_rect: round rects go through the canvas fallback. */
inline const lh_ui_canvas_backend_t *
fill_probe_backend()
{
    static const lh_ui_canvas_backend_t backend = {nullptr, nullptr, nullptr, fill_probe_fill_rect,
                                                   nullptr, nullptr, nullptr};
    return &backend;
}

/** Backend with both slots: round rects reach fill_round_rect. */
inline const lh_ui_canvas_backend_t *
fill_probe_round_backend()
{
    static const lh_ui_canvas_backend_t backend = {nullptr, nullptr, nullptr, fill_probe_fill_rect,
                                                   fill_probe_fill_round_rect, nullptr, nullptr};
    return &backend;
}

/** Point @p canvas at a probe expecting @p rect filled with @p color. */
inline lh_void
fill_probe_init(fill_probe *probe, lh_ui_canvas_t *canvas, const lh_ui_canvas_backend_t *backend,
                lh_ui_rect_t rect, lh_ui_color_t color)
{
    *probe = fill_probe{};
    probe->rect = rect;
    probe->color = color;
    lh_ui_canvas_init(canvas, backend, probe);
}

} // namespace lh_test

#endif /* LH_TEST_UI_FILL_PROBE_H */
