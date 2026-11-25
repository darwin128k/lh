/**
 * @file thumb.c
 * @brief Implementation of `lh/gl/scrollbar.h`: the thumb: its length, where it starts, its rect,
 * dragging it and drawing it.
 */

#include <lh/gl/scrollbar.h>

#include <lh/core/assert.h>
#include <lh/gl/range.h>
#include <lh/core/util/return.h>

lh_gl_scalar_t
lh_gl_scrollbar_get_thumb_length_within(const lh_gl_scrollbar_t *self, lh_gl_point_t max)
{
    const lh_gl_scalar_t view = lh_gl_scrollbar_get_viewport_length(self);

    /* With overflow, content is viewport + max; without, the thumb is the track either way. */
    return lh_gl_range_window_length(
        lh_gl_scrollbar_get_track_length(self), view,
        view + lh_gl_point_get_along(lh_addr_of(max), lh_gl_scrollbar_get_axis(self)),
        LH_GL_SCROLLBAR_THUMB_MIN);
}

lh_gl_scalar_t
lh_gl_scrollbar_get_thumb_start_at(const lh_gl_scrollbar_t *self, lh_gl_point_t point)
{
    const lh_gl_point_t *origin;

    origin = lh_gl_rect_get_origin(lh_gl_entity_get_rect(lh_gl_scrollbar_get_entity(self)));
    return lh_gl_point_get_along(lh_addr_of(point), lh_gl_scrollbar_get_axis(self)) -
           lh_gl_point_get_along(origin, lh_gl_scrollbar_get_axis(self));
}

lh_gl_scalar_t
lh_gl_scrollbar_get_thumb_start_within(const lh_gl_scrollbar_t *self, lh_gl_point_t max)
{
    lh_gl_point_t scroll;
    lh_gl_container_get_scroll_within(lh_gl_scrollbar_get_container(self), max, lh_addr_of(scroll));

    return lh_gl_range_window_start(
        lh_gl_scrollbar_get_track_length(self), lh_gl_scrollbar_get_thumb_length_within(self, max),
        lh_gl_point_get_along(lh_addr_of(scroll), lh_gl_scrollbar_get_axis(self)),
        lh_gl_point_get_along(lh_addr_of(max), lh_gl_scrollbar_get_axis(self)));
}

lh_gl_scalar_t
lh_gl_scrollbar_get_thumb_length(const lh_gl_scrollbar_t *self)
{
    lh_gl_point_t point;
    lh_gl_container_get_scroll_max(lh_gl_scrollbar_get_container(self), lh_addr_of(point));
    return lh_gl_scrollbar_get_thumb_length_within(self, point);
}

lh_void
lh_gl_scrollbar_set_thumb_start(lh_gl_scrollbar_t *self, lh_gl_scalar_t start)
{
    lh_gl_point_t max;

    lh_gl_container_get_scroll_max(lh_gl_scrollbar_get_container(self), lh_addr_of(max));
    lh_gl_point_t scroll;
    lh_gl_container_get_scroll_within(lh_gl_scrollbar_get_container(self), max, lh_addr_of(scroll));
    lh_gl_point_set_along(
        lh_addr_of(scroll), lh_gl_scrollbar_get_axis(self),
        lh_gl_range_offset_from_window_start(
            lh_gl_scrollbar_get_track_length(self),
            lh_gl_scrollbar_get_thumb_length_within(self, max), start,
            lh_gl_point_get_along(lh_addr_of(max), lh_gl_scrollbar_get_axis(self))));
    lh_gl_container_set_scroll_v(lh_gl_scrollbar_get_container(self), scroll);
}

lh_gl_scalar_t
lh_gl_scrollbar_get_thumb_start(const lh_gl_scrollbar_t *self)
{
    lh_gl_point_t point;
    lh_gl_container_get_scroll_max(lh_gl_scrollbar_get_container(self), lh_addr_of(point));
    return lh_gl_scrollbar_get_thumb_start_within(self, point);
}

lh_void
lh_gl_scrollbar_get_thumb_rect(const lh_gl_scrollbar_t *self, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_point_t max;

    lh_gl_container_get_scroll_max(lh_gl_scrollbar_get_container(self), lh_addr_of(max));
    lh_gl_rect_t thumb;
    lh_gl_rect_init_along(
        lh_addr_of(thumb), lh_gl_entity_get_rect(lh_gl_scrollbar_get_entity(self)),
        lh_gl_scrollbar_get_axis(self), lh_gl_scrollbar_get_thumb_start_within(self, max),
        lh_gl_scrollbar_get_thumb_length_within(self, max));
    lh_ptr_deref(out) = thumb;
}

lh_void
lh_gl_scrollbar_set_scroll_at(lh_gl_scrollbar_t *self, lh_gl_point_t point)
{
    lh_gl_scrollbar_set_thumb_start(self, lh_gl_scrollbar_get_thumb_start_at(self, point));
}

lh_void
lh_gl_scrollbar_draw_thumb(const lh_gl_scrollbar_t *self, lh_gl_canvas_t *canvas)
{

    lh_return_if(lh_null_eq(canvas) || lh_null_eq(lh_gl_scrollbar_get_thumb_style(self)));
    const lh_gl_color_t *color = lh_gl_style_get_fill_color(lh_gl_scrollbar_get_thumb_style(self));
    lh_return_if(lh_null_eq(color));
    lh_gl_rect_t thumb;
    lh_gl_scrollbar_get_thumb_rect(self, lh_addr_of(thumb));
    lh_gl_canvas_fill_round_rect(canvas, lh_addr_of(thumb),
                                 lh_gl_style_get_radius(lh_gl_scrollbar_get_thumb_style(self)),
                                 color);
}

lh_bool_t
lh_gl_scrollbar_contains_thumb(const lh_gl_scrollbar_t *self, lh_gl_point_t point)
{
    lh_gl_rect_t thumb;
    lh_gl_scrollbar_get_thumb_rect(self, lh_addr_of(thumb));

    return lh_gl_rect_contains_point(lh_addr_of(thumb), point);
}
