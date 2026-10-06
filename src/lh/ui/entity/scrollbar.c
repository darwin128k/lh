/**
 * @file scrollbar.c
 * @brief Implementation of `lh/ui/entity/scrollbar.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/entity/scrollbar.h>
#include <lh/ui/range.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

/* ── Class ───────────────────────────────────────────────────────────────── */

const lh_ui_entity_class_t lh_ui_entity_scrollbar_class = {
    lh_ui_entity_scrollbar_event, lh_addr_of(lh_ui_entity_class)};

lh_void
lh_ui_entity_scrollbar_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event)
{
    /* The entity is the first field: self is the scrollbar. */
    const lh_ui_entity_scrollbar_t *scrollbar = lh_ptr_rcast(const lh_ui_entity_scrollbar_t, self);

    lh_ui_entity_class_event_base(lh_addr_of(lh_ui_entity_scrollbar_class), self, event);
    lh_ui_entity_scrollbar_on_draw(scrollbar, event);
    lh_ui_entity_scrollbar_on_click(scrollbar, event);
    lh_ui_entity_scrollbar_on_visible(scrollbar, event);
}

lh_void
lh_ui_entity_scrollbar_on_draw(const lh_ui_entity_scrollbar_t *self, const lh_ui_entity_event_t *event)
{
    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_draw);
    lh_ui_entity_scrollbar_draw_thumb(self, lh_ui_entity_event_get_canvas(event));
}

lh_void
lh_ui_entity_scrollbar_on_click(const lh_ui_entity_scrollbar_t *self, const lh_ui_entity_event_t *event)
{
    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_click);
    lh_ui_entity_scrollbar_page_toward(self, lh_ui_entity_event_get_point(event));
}

lh_void
lh_ui_entity_scrollbar_on_visible(const lh_ui_entity_scrollbar_t *self, const lh_ui_entity_event_t *event)
{
    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_visible);
    lh_return_if(lh_ui_entity_scrollbar_is_visible(self));
    *lh_ui_entity_event_get_visible(event) = lh_bool_false;
}

/* ── Lifetime and fields ─────────────────────────────────────────────────── */

lh_void
lh_ui_entity_scrollbar_init(lh_ui_entity_scrollbar_t *self, lh_ui_rect_t rect, lh_ui_axis_t axis,
                            lh_ui_entity_container_t *container)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(container);
    lh_ui_entity_init(lh_addr_of(self->entity), rect);
    lh_ui_entity_set_class(lh_addr_of(self->entity), lh_addr_of(lh_ui_entity_scrollbar_class));
    self->axis = axis;
    self->mode = lh_ui_entity_scrollbar_mode_auto;
    self->container = container;
    self->thumb_style = lh_null;
}

lh_ui_entity_t *
lh_ui_entity_scrollbar_as_entity(lh_ui_entity_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

lh_ui_axis_t
lh_ui_entity_scrollbar_get_axis(const lh_ui_entity_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return self->axis;
}

lh_ui_entity_scrollbar_mode_t
lh_ui_entity_scrollbar_get_mode(const lh_ui_entity_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mode;
}

lh_void
lh_ui_entity_scrollbar_set_mode(lh_ui_entity_scrollbar_t *self, lh_ui_entity_scrollbar_mode_t mode)
{
    lh_assert_runtime_ref(self);
    self->mode = mode;
}

const lh_ui_style_t *
lh_ui_entity_scrollbar_get_thumb_style(const lh_ui_entity_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return self->thumb_style;
}

lh_void
lh_ui_entity_scrollbar_set_thumb_style(lh_ui_entity_scrollbar_t *self, const lh_ui_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->thumb_style = style;
}

/* ── The container on this axis ──────────────────────────────────────────── */

lh_ui_scalar_t
lh_ui_entity_scrollbar_get_scroll(const lh_ui_entity_scrollbar_t *self)
{
    const lh_ui_point_t scroll = lh_ui_entity_container_get_scroll(self->container);

    return lh_ui_point_get_along(lh_addr_of(scroll), self->axis);
}

lh_ui_scalar_t
lh_ui_entity_scrollbar_get_scroll_max(const lh_ui_entity_scrollbar_t *self)
{
    const lh_ui_point_t max = lh_ui_entity_container_get_scroll_max(self->container);

    return lh_ui_point_get_along(lh_addr_of(max), self->axis);
}

lh_ui_scalar_t
lh_ui_entity_scrollbar_get_viewport_length(const lh_ui_entity_scrollbar_t *self)
{
    const lh_ui_size_t viewport = lh_ui_entity_container_get_viewport_size(self->container);

    return lh_ui_size_get_along(lh_addr_of(viewport), self->axis);
}

lh_ui_scalar_t
lh_ui_entity_scrollbar_get_content_length(const lh_ui_entity_scrollbar_t *self)
{
    const lh_ui_size_t content = lh_ui_entity_container_get_content_size(self->container);

    return lh_ui_size_get_along(lh_addr_of(content), self->axis);
}

lh_bool_t
lh_ui_entity_scrollbar_is_needed(const lh_ui_entity_scrollbar_t *self)
{
    return lh_ui_entity_scrollbar_get_scroll_max(self) > lh_ui_scalar(0) ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_ui_entity_scrollbar_is_visible(const lh_ui_entity_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mode == lh_ui_entity_scrollbar_mode_always ||
                   (self->mode == lh_ui_entity_scrollbar_mode_auto && lh_ui_entity_scrollbar_is_needed(self))
               ? lh_bool_true
               : lh_bool_false;
}

/* ── Thumb ───────────────────────────────────────────────────────────────── */

lh_ui_scalar_t
lh_ui_entity_scrollbar_get_track_length(const lh_ui_entity_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_size_get_along(lh_ui_rect_get_size_as_const(lh_addr_of(self->entity.rect)), self->axis);
}

lh_ui_scalar_t
lh_ui_entity_scrollbar_get_thumb_length(const lh_ui_entity_scrollbar_t *self)
{
    return lh_ui_range_window_length(lh_ui_entity_scrollbar_get_track_length(self),
                                     lh_ui_entity_scrollbar_get_viewport_length(self),
                                     lh_ui_entity_scrollbar_get_content_length(self),
                                     LH_UI_ENTITY_SCROLLBAR_THUMB_MIN);
}

lh_ui_scalar_t
lh_ui_entity_scrollbar_get_thumb_start(const lh_ui_entity_scrollbar_t *self)
{
    return lh_ui_range_window_start(lh_ui_entity_scrollbar_get_track_length(self),
                                    lh_ui_entity_scrollbar_get_thumb_length(self),
                                    lh_ui_entity_scrollbar_get_scroll(self),
                                    lh_ui_entity_scrollbar_get_scroll_max(self));
}

lh_ui_rect_t
lh_ui_entity_scrollbar_get_thumb_rect(const lh_ui_entity_scrollbar_t *self)
{
    lh_ui_rect_t thumb;

    lh_ui_rect_init_along(lh_addr_of(thumb), lh_addr_of(self->entity.rect), self->axis,
                          lh_ui_entity_scrollbar_get_thumb_start(self),
                          lh_ui_entity_scrollbar_get_thumb_length(self));
    return thumb;
}

lh_void
lh_ui_entity_scrollbar_draw_thumb(const lh_ui_entity_scrollbar_t *self, lh_ui_canvas_t *canvas)
{
    const lh_ui_color_t *color;
    lh_ui_rect_t thumb;

    lh_return_if(lh_null_eq(canvas) || lh_null_eq(self->thumb_style));
    color = lh_ui_style_get_fill_color(self->thumb_style);
    lh_return_if(lh_null_eq(color));
    thumb = lh_ui_entity_scrollbar_get_thumb_rect(self);
    lh_ui_canvas_fill_round_rect(canvas, lh_addr_of(thumb), lh_ui_style_get_radius(self->thumb_style), color);
}

/* ── Paging ──────────────────────────────────────────────────────────────── */

lh_ui_scalar_t
lh_ui_entity_scrollbar_get_page_toward(const lh_ui_entity_scrollbar_t *self, lh_ui_point_t point)
{
    const lh_ui_rect_t thumb = lh_ui_entity_scrollbar_get_thumb_rect(self);
    const lh_ui_scalar_t page = lh_ui_entity_scrollbar_get_viewport_length(self);

    lh_return_if(lh_ui_rect_contains_point(lh_addr_of(thumb), point), lh_ui_scalar(0));
    return lh_ui_point_get_along(lh_addr_of(point), self->axis) <
                   lh_ui_point_get_along(lh_ui_rect_get_origin_as_const(lh_addr_of(thumb)), self->axis)
               ? -page
               : page;
}

lh_void
lh_ui_entity_scrollbar_page_toward(const lh_ui_entity_scrollbar_t *self, lh_ui_point_t point)
{
    lh_ui_point_t delta;

    lh_ui_point_init_along(lh_addr_of(delta), self->axis, lh_ui_entity_scrollbar_get_page_toward(self, point),
                           lh_ui_scalar(0));
    lh_ui_entity_container_scroll_by(self->container, lh_ui_point_get_x(lh_addr_of(delta)),
                                     lh_ui_point_get_y(lh_addr_of(delta)));
}
