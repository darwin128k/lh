/**
 * @file scrollbar.c
 * @brief Implementation of `lh/ui/scrollbar.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/entity/class.h>
#include <lh/ui/scrollbar.h>
#include <lh/ui/range.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

/* ── Class ───────────────────────────────────────────────────────────────── */

const lh_ui_entity_class_t lh_ui_scrollbar_class = {
    lh_ui_scrollbar_event, lh_addr_of(lh_ui_entity_class)};

lh_void
lh_ui_scrollbar_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event)
{
    /* The entity is the first field: self is the scrollbar. */
    const lh_ui_scrollbar_t *scrollbar = lh_ptr_rcast(const lh_ui_scrollbar_t, self);

    lh_ui_entity_class_event_base(lh_addr_of(lh_ui_scrollbar_class), self, event);
    lh_ui_scrollbar_on_draw(scrollbar, event);
    lh_ui_scrollbar_on_click(scrollbar, event);
    lh_ui_scrollbar_on_visible(scrollbar, event);
}

lh_void
lh_ui_scrollbar_on_draw(const lh_ui_scrollbar_t *self, const lh_ui_entity_event_t *event)
{
    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_draw);
    lh_ui_scrollbar_draw_thumb(self, lh_ui_entity_event_get_canvas(event));
}

lh_void
lh_ui_scrollbar_on_click(const lh_ui_scrollbar_t *self, const lh_ui_entity_event_t *event)
{
    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_click);
    lh_ui_scrollbar_page_toward(self, lh_ui_entity_event_get_point(event));
}

lh_void
lh_ui_scrollbar_on_visible(const lh_ui_scrollbar_t *self, const lh_ui_entity_event_t *event)
{
    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_visible);
    lh_return_if(lh_ui_scrollbar_is_visible(self));
    *lh_ui_entity_event_get_visible(event) = lh_bool_false;
}

/* ── Lifetime and fields ─────────────────────────────────────────────────── */

lh_void
lh_ui_scrollbar_init(lh_ui_scrollbar_t *self, lh_ui_rect_t rect, lh_ui_axis_t axis,
                            lh_ui_container_t *container)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(container);
    lh_ui_entity_init(lh_addr_of(self->entity), rect);
    lh_ui_entity_set_class(lh_addr_of(self->entity), lh_addr_of(lh_ui_scrollbar_class));
    self->axis = axis;
    self->mode = lh_ui_scrollbar_mode_auto;
    self->container = container;
    self->thumb_style = lh_null;
}

lh_ui_entity_t *
lh_ui_scrollbar_as_entity(lh_ui_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

lh_ui_scrollbar_t *
lh_ui_entity_as_scrollbar(lh_ui_entity_t *entity)
{
    lh_return_if(lh_null_eq(entity), lh_null);
    lh_return_if(!lh_ui_entity_class_is(lh_ui_entity_get_class(entity),
                                        lh_addr_of(lh_ui_scrollbar_class)),
                 lh_null);
    return lh_ptr_rcast(lh_ui_scrollbar_t, entity);
}

lh_ui_container_t *
lh_ui_scrollbar_get_container(const lh_ui_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return self->container;
}

lh_ui_axis_t
lh_ui_scrollbar_get_axis(const lh_ui_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return self->axis;
}

lh_ui_scrollbar_mode_t
lh_ui_scrollbar_get_mode(const lh_ui_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mode;
}

lh_void
lh_ui_scrollbar_set_mode(lh_ui_scrollbar_t *self, lh_ui_scrollbar_mode_t mode)
{
    lh_assert_runtime_ref(self);
    self->mode = mode;
}

const lh_ui_style_t *
lh_ui_scrollbar_get_thumb_style(const lh_ui_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return self->thumb_style;
}

lh_void
lh_ui_scrollbar_set_thumb_style(lh_ui_scrollbar_t *self, const lh_ui_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->thumb_style = style;
}

/* ── The container on this axis ──────────────────────────────────────────── */

lh_ui_scalar_t
lh_ui_scrollbar_get_scroll(const lh_ui_scrollbar_t *self)
{
    const lh_ui_point_t scroll = lh_ui_container_get_scroll(self->container);

    return lh_ui_point_get_along(lh_addr_of(scroll), self->axis);
}

lh_ui_scalar_t
lh_ui_scrollbar_get_scroll_max(const lh_ui_scrollbar_t *self)
{
    const lh_ui_point_t max = lh_ui_container_get_scroll_max(self->container);

    return lh_ui_point_get_along(lh_addr_of(max), self->axis);
}

lh_ui_scalar_t
lh_ui_scrollbar_get_viewport_length(const lh_ui_scrollbar_t *self)
{
    const lh_ui_size_t viewport = lh_ui_container_get_viewport_size(self->container);

    return lh_ui_size_get_along(lh_addr_of(viewport), self->axis);
}

lh_ui_scalar_t
lh_ui_scrollbar_get_content_length(const lh_ui_scrollbar_t *self)
{
    const lh_ui_size_t content = lh_ui_container_get_content_size(self->container);

    return lh_ui_size_get_along(lh_addr_of(content), self->axis);
}

lh_bool_t
lh_ui_scrollbar_is_needed(const lh_ui_scrollbar_t *self)
{
    return lh_ui_scrollbar_get_scroll_max(self) > lh_ui_scalar(0) ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_ui_scrollbar_is_visible(const lh_ui_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mode == lh_ui_scrollbar_mode_always ||
                   (self->mode == lh_ui_scrollbar_mode_auto && lh_ui_scrollbar_is_needed(self))
               ? lh_bool_true
               : lh_bool_false;
}

/* ── Thumb ───────────────────────────────────────────────────────────────── */

lh_ui_scalar_t
lh_ui_scrollbar_get_track_length(const lh_ui_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_size_get_along(lh_ui_rect_get_size_as_const(lh_addr_of(self->entity.rect)), self->axis);
}

lh_ui_scalar_t
lh_ui_scrollbar_get_thumb_length_within(const lh_ui_scrollbar_t *self, lh_ui_point_t max)
{
    const lh_ui_scalar_t view = lh_ui_scrollbar_get_viewport_length(self);

    /* With overflow, content is viewport + max; without, the thumb is the track either way. */
    return lh_ui_range_window_length(lh_ui_scrollbar_get_track_length(self), view,
                                     view + lh_ui_point_get_along(lh_addr_of(max), self->axis),
                                     LH_UI_SCROLLBAR_THUMB_MIN);
}

lh_ui_scalar_t
lh_ui_scrollbar_get_thumb_start_within(const lh_ui_scrollbar_t *self, lh_ui_point_t max)
{
    const lh_ui_point_t scroll = lh_ui_container_get_scroll_within(self->container, max);

    return lh_ui_range_window_start(lh_ui_scrollbar_get_track_length(self),
                                    lh_ui_scrollbar_get_thumb_length_within(self, max),
                                    lh_ui_point_get_along(lh_addr_of(scroll), self->axis),
                                    lh_ui_point_get_along(lh_addr_of(max), self->axis));
}

lh_ui_scalar_t
lh_ui_scrollbar_get_thumb_length(const lh_ui_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_scrollbar_get_thumb_length_within(
        self, lh_ui_container_get_scroll_max(self->container));
}

lh_ui_scalar_t
lh_ui_scrollbar_get_thumb_start(const lh_ui_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_scrollbar_get_thumb_start_within(
        self, lh_ui_container_get_scroll_max(self->container));
}

lh_ui_scalar_t
lh_ui_scrollbar_get_thumb_start_at(const lh_ui_scrollbar_t *self, lh_ui_point_t point)
{
    const lh_ui_point_t *origin;

    lh_assert_runtime_ref(self);
    origin = lh_ui_rect_get_origin_as_const(lh_addr_of(self->entity.rect));
    return lh_ui_point_get_along(lh_addr_of(point), self->axis) -
           lh_ui_point_get_along(origin, self->axis);
}

lh_void
lh_ui_scrollbar_set_thumb_start(lh_ui_scrollbar_t *self, lh_ui_scalar_t start)
{
    lh_ui_point_t max;
    lh_ui_point_t scroll;

    lh_assert_runtime_ref(self);
    max = lh_ui_container_get_scroll_max(self->container);
    scroll = lh_ui_container_get_scroll_within(self->container, max);
    lh_ui_point_set_along(lh_addr_of(scroll), self->axis,
                          lh_ui_range_offset_from_window_start(
                              lh_ui_scrollbar_get_track_length(self),
                              lh_ui_scrollbar_get_thumb_length_within(self, max), start,
                              lh_ui_point_get_along(lh_addr_of(max), self->axis)));
    lh_ui_container_set_scroll(self->container, scroll);
}

lh_ui_rect_t
lh_ui_scrollbar_get_thumb_rect(const lh_ui_scrollbar_t *self)
{
    lh_ui_point_t max;
    lh_ui_rect_t thumb;

    lh_assert_runtime_ref(self);
    max = lh_ui_container_get_scroll_max(self->container);
    lh_ui_rect_init_along(lh_addr_of(thumb), lh_addr_of(self->entity.rect), self->axis,
                          lh_ui_scrollbar_get_thumb_start_within(self, max),
                          lh_ui_scrollbar_get_thumb_length_within(self, max));
    return thumb;
}

lh_void
lh_ui_scrollbar_draw_thumb(const lh_ui_scrollbar_t *self, lh_ui_canvas_t *canvas)
{
    const lh_ui_color_t *color;
    lh_ui_rect_t thumb;

    lh_return_if(lh_null_eq(canvas) || lh_null_eq(self->thumb_style));
    color = lh_ui_style_get_fill_color(self->thumb_style);
    lh_return_if(lh_null_eq(color));
    thumb = lh_ui_scrollbar_get_thumb_rect(self);
    lh_ui_canvas_fill_round_rect(canvas, lh_addr_of(thumb), lh_ui_style_get_radius(self->thumb_style), color);
}

lh_bool_t
lh_ui_scrollbar_contains_thumb(const lh_ui_scrollbar_t *self, lh_ui_point_t point)
{
    const lh_ui_rect_t thumb = lh_ui_scrollbar_get_thumb_rect(self);

    lh_assert_runtime_ref(self);
    return lh_ui_rect_contains_point(lh_addr_of(thumb), point);
}

lh_void
lh_ui_scrollbar_set_scroll_at(lh_ui_scrollbar_t *self, lh_ui_point_t point)
{
    lh_ui_scrollbar_set_thumb_start(self, lh_ui_scrollbar_get_thumb_start_at(self, point));
}

/* ── Paging ──────────────────────────────────────────────────────────────── */

lh_ui_scalar_t
lh_ui_scrollbar_get_page_toward(const lh_ui_scrollbar_t *self, lh_ui_point_t point)
{
    const lh_ui_rect_t thumb = lh_ui_scrollbar_get_thumb_rect(self);
    const lh_ui_scalar_t page = lh_ui_scrollbar_get_viewport_length(self);

    lh_return_if(lh_ui_rect_contains_point(lh_addr_of(thumb), point), lh_ui_scalar(0));
    return lh_ui_point_get_along(lh_addr_of(point), self->axis) <
                   lh_ui_point_get_along(lh_ui_rect_get_origin_as_const(lh_addr_of(thumb)), self->axis)
               ? -page
               : page;
}

lh_void
lh_ui_scrollbar_page_toward(const lh_ui_scrollbar_t *self, lh_ui_point_t point)
{
    lh_ui_point_t delta;

    lh_ui_point_init_along(lh_addr_of(delta), self->axis, lh_ui_scrollbar_get_page_toward(self, point),
                           lh_ui_scalar(0));
    lh_ui_container_scroll_by(self->container, lh_ui_point_get_x(lh_addr_of(delta)),
                                     lh_ui_point_get_y(lh_addr_of(delta)));
}

/* ── Finding and damage ──────────────────────────────────────────────────── */

lh_ui_container_t *
lh_ui_scrollbar_get_driven(lh_ui_entity_t *entity)
{
    const lh_ui_scrollbar_t *bar = lh_ui_entity_as_scrollbar(entity);

    return lh_null_eq(bar) ? lh_null : bar->container;
}

lh_ui_container_t *
lh_ui_scrollbar_find_scrolled(lh_ui_entity_t *entity)
{
    lh_ui_container_t *container = lh_ui_scrollbar_get_driven(entity);

    return lh_null_ne(container) ? container : lh_ui_entity_find_container(entity);
}

lh_ui_scrollbar_t *
lh_ui_scrollbar_get_bound(lh_ui_entity_t *entity, const lh_ui_container_t *container)
{
    lh_ui_scrollbar_t *bar = lh_ui_entity_as_scrollbar(entity);

    lh_return_if(lh_null_eq(bar) || bar->container != container, lh_null);
    return bar;
}

lh_void
lh_ui_scrollbar_add_bound_damage(lh_ui_entity_t *entity, const lh_ui_container_t *container,
                                        lh_ui_canvas_t *canvas)
{
    lh_ui_scrollbar_t *bar = lh_ui_scrollbar_get_bound(entity, container);

    lh_return_if(lh_null_eq(bar));
    lh_ui_entity_add_damage(lh_ui_scrollbar_as_entity(bar), canvas);
}

lh_void
lh_ui_scrollbar_add_scroll_damage(lh_ui_container_t *container, lh_ui_canvas_t *canvas)
{
    lh_ui_entity_t *entity = lh_ui_container_as_entity(container);
    lh_ui_entity_t *parent = lh_ui_entity_get_parent(entity);
    lh_ui_entity_t *child;

    lh_ui_entity_add_damage(entity, canvas);
    lh_return_if(lh_null_eq(parent));
    for (child = lh_ui_entity_get_first_child(parent); lh_null_ne(child);
         child = lh_ui_entity_get_next_child(parent, child))
    {
        lh_ui_scrollbar_add_bound_damage(child, container, canvas);
    }
}
