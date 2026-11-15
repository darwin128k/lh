/**
 * @file scrollbar.c
 * @brief Implementation of `lh/gl/scrollbar.h`.
 */
#include <lh/core/assert.h>

#include <lh/gl/scrollbar.h>
#include <lh/gl/range.h>
#include <lh/core/util/return.h>

/* -- Class ----------------------------------------------------------------- */

const lh_gl_entity_class_t lh_gl_scrollbar_class = {lh_gl_scrollbar_event,
                                                    lh_addr_of(lh_gl_entity_class)};

lh_void
lh_gl_scrollbar_init(lh_gl_scrollbar_t *self, lh_gl_rect_t rect, lh_gl_axis_t axis,
                     lh_gl_container_t *container)
{
    lh_assert_runtime_ref(container);
    lh_gl_entity_init(lh_gl_scrollbar_get_entity_mut(self), rect);
    lh_gl_entity_set_class_v(lh_gl_scrollbar_get_entity_mut(self),
                             lh_addr_of(lh_gl_scrollbar_class));
    lh_gl_scrollbar_set_axis(self, axis);
    lh_gl_scrollbar_set_mode(self, lh_gl_scrollbar_mode_auto);
    lh_gl_scrollbar_set_container(self, container);
    lh_gl_scrollbar_set_thumb_style(self, lh_null);
}

lh_gl_entity_t *
lh_gl_scrollbar_as_entity(lh_gl_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

lh_gl_scrollbar_t *
lh_gl_entity_as_scrollbar(lh_gl_entity_t *entity)
{
    lh_return_if(lh_null_eq(entity), lh_null);
    lh_return_if(
        !lh_gl_entity_class_is(lh_gl_entity_get_class(entity), lh_addr_of(lh_gl_scrollbar_class)),
        lh_null);
    return lh_ptr_rcast(lh_gl_scrollbar_t, entity);
}

lh_gl_container_t *
lh_gl_scrollbar_get_container(const lh_gl_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return self->container;
}

/* -- Lifetime and fields --------------------------------------------------- */

lh_gl_axis_t
lh_gl_scrollbar_get_axis(const lh_gl_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return self->axis;
}

lh_gl_scrollbar_mode_t
lh_gl_scrollbar_get_mode(const lh_gl_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mode;
}

lh_void
lh_gl_scrollbar_set_mode(lh_gl_scrollbar_t *self, lh_gl_scrollbar_mode_t mode)
{
    lh_assert_runtime_ref(self);
    self->mode = mode;
}

const lh_gl_style_t *
lh_gl_scrollbar_get_thumb_style(const lh_gl_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return self->thumb_style;
}

lh_void
lh_gl_scrollbar_set_thumb_style(lh_gl_scrollbar_t *self, const lh_gl_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->thumb_style = style;
}

lh_gl_scalar_t
lh_gl_scrollbar_get_scroll(const lh_gl_scrollbar_t *self)
{
    lh_gl_point_t scroll;
    lh_gl_container_get_clamped_scroll(lh_gl_scrollbar_get_container(self), lh_addr_of(scroll));

    return lh_gl_point_get_along(lh_addr_of(scroll), lh_gl_scrollbar_get_axis(self));
}

lh_gl_scalar_t
lh_gl_scrollbar_get_scroll_max(const lh_gl_scrollbar_t *self)
{
    lh_gl_point_t max;
    lh_gl_container_get_scroll_max(lh_gl_scrollbar_get_container(self), lh_addr_of(max));

    return lh_gl_point_get_along(lh_addr_of(max), lh_gl_scrollbar_get_axis(self));
}

lh_gl_scalar_t
lh_gl_scrollbar_get_viewport_length(const lh_gl_scrollbar_t *self)
{
    lh_gl_size_t viewport;
    lh_gl_container_get_viewport_size(lh_gl_scrollbar_get_container(self), lh_addr_of(viewport));

    return lh_gl_size_get_along(lh_addr_of(viewport), lh_gl_scrollbar_get_axis(self));
}

lh_gl_scalar_t
lh_gl_scrollbar_get_content_length(const lh_gl_scrollbar_t *self)
{
    lh_gl_size_t content;
    lh_gl_container_get_content_size(lh_gl_scrollbar_get_container(self), lh_addr_of(content));

    return lh_gl_size_get_along(lh_addr_of(content), lh_gl_scrollbar_get_axis(self));
}

/* -- The container on this axis -------------------------------------------- */

lh_bool_t
lh_gl_scrollbar_is_needed(const lh_gl_scrollbar_t *self)
{
    return lh_gl_scrollbar_get_scroll_max(self) > lh_gl_scalar(0) ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_gl_scrollbar_is_visible(const lh_gl_scrollbar_t *self)
{
    return lh_gl_scrollbar_get_mode(self) == lh_gl_scrollbar_mode_always ||
                   (lh_gl_scrollbar_get_mode(self) == lh_gl_scrollbar_mode_auto &&
                    lh_gl_scrollbar_is_needed(self))
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_gl_scrollbar_on_visible(const lh_gl_scrollbar_t *self, const lh_gl_entity_event_t *event)
{
    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_visible);
    lh_return_if(lh_gl_scrollbar_is_visible(self));
    lh_ptr_deref(lh_gl_entity_event_get_visible(event)) = lh_bool_false;
}

lh_gl_scalar_t
lh_gl_scrollbar_get_track_length(const lh_gl_scrollbar_t *self)
{
    return lh_gl_size_get_along(
        lh_gl_rect_get_size(lh_gl_entity_get_rect(lh_gl_scrollbar_get_entity(self))),
        lh_gl_scrollbar_get_axis(self));
}

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
lh_gl_scrollbar_get_thumb_start_within(const lh_gl_scrollbar_t *self, lh_gl_point_t max)
{
    lh_gl_point_t scroll;
    lh_gl_container_get_scroll_within(lh_gl_scrollbar_get_container(self), max, lh_addr_of(scroll));

    return lh_gl_range_window_start(
        lh_gl_scrollbar_get_track_length(self), lh_gl_scrollbar_get_thumb_length_within(self, max),
        lh_gl_point_get_along(lh_addr_of(scroll), lh_gl_scrollbar_get_axis(self)),
        lh_gl_point_get_along(lh_addr_of(max), lh_gl_scrollbar_get_axis(self)));
}

/* -- Thumb ----------------------------------------------------------------- */

lh_gl_scalar_t
lh_gl_scrollbar_get_thumb_length(const lh_gl_scrollbar_t *self)
{
    lh_gl_point_t point;
    lh_gl_container_get_scroll_max(lh_gl_scrollbar_get_container(self), lh_addr_of(point));
    return lh_gl_scrollbar_get_thumb_length_within(self, point);
}

lh_gl_scalar_t
lh_gl_scrollbar_get_thumb_start(const lh_gl_scrollbar_t *self)
{
    lh_gl_point_t point;
    lh_gl_container_get_scroll_max(lh_gl_scrollbar_get_container(self), lh_addr_of(point));
    return lh_gl_scrollbar_get_thumb_start_within(self, point);
}

lh_gl_scalar_t
lh_gl_scrollbar_get_thumb_start_at(const lh_gl_scrollbar_t *self, lh_gl_point_t point)
{
    const lh_gl_point_t *origin;

    origin = lh_gl_rect_get_origin(lh_gl_entity_get_rect(lh_gl_scrollbar_get_entity(self)));
    return lh_gl_point_get_along(lh_addr_of(point), lh_gl_scrollbar_get_axis(self)) -
           lh_gl_point_get_along(origin, lh_gl_scrollbar_get_axis(self));
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

lh_void
lh_gl_scrollbar_on_draw(const lh_gl_scrollbar_t *self, const lh_gl_entity_event_t *event)
{
    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_draw);
    lh_gl_scrollbar_draw_thumb(self, lh_gl_entity_event_get_canvas(event));
}

lh_bool_t
lh_gl_scrollbar_contains_thumb(const lh_gl_scrollbar_t *self, lh_gl_point_t point)
{
    lh_gl_rect_t thumb;
    lh_gl_scrollbar_get_thumb_rect(self, lh_addr_of(thumb));

    return lh_gl_rect_contains_point(lh_addr_of(thumb), point);
}

lh_void
lh_gl_scrollbar_set_scroll_at(lh_gl_scrollbar_t *self, lh_gl_point_t point)
{
    lh_gl_scrollbar_set_thumb_start(self, lh_gl_scrollbar_get_thumb_start_at(self, point));
}

lh_gl_scalar_t
lh_gl_scrollbar_get_page_toward(const lh_gl_scrollbar_t *self, lh_gl_point_t point)
{
    lh_gl_rect_t thumb;
    lh_gl_scrollbar_get_thumb_rect(self, lh_addr_of(thumb));
    const lh_gl_scalar_t page = lh_gl_scrollbar_get_viewport_length(self);

    lh_return_if(lh_gl_rect_contains_point(lh_addr_of(thumb), point), lh_gl_scalar(0));
    return lh_gl_point_get_along(lh_addr_of(point), lh_gl_scrollbar_get_axis(self)) <
                   lh_gl_point_get_along(lh_gl_rect_get_origin(lh_addr_of(thumb)),
                                         lh_gl_scrollbar_get_axis(self))
               ? lh_math_neg(page)
               : page;
}

lh_void
lh_gl_scrollbar_page_toward(const lh_gl_scrollbar_t *self, lh_gl_point_t point)
{
    lh_gl_point_t delta;

    lh_gl_point_init_along(lh_addr_of(delta), lh_gl_scrollbar_get_axis(self),
                           lh_gl_scrollbar_get_page_toward(self, point), lh_gl_scalar(0));
    lh_gl_container_scroll_by(lh_gl_scrollbar_get_container(self),
                              lh_gl_point_get_x(lh_addr_of(delta)),
                              lh_gl_point_get_y(lh_addr_of(delta)));
}

/* -- Paging ---------------------------------------------------------------- */

lh_void
lh_gl_scrollbar_on_click(const lh_gl_scrollbar_t *self, const lh_gl_entity_event_t *event)
{
    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_click);
    lh_gl_container_t *box = lh_gl_scrollbar_get_container(self);
    lh_return_if(lh_null_eq(box) || !lh_gl_container_is_scroll_enabled(box));
    lh_gl_point_t point;
    lh_gl_entity_event_get_point(event, lh_addr_of(point));
    lh_gl_scrollbar_page_toward(self, point);
}

lh_void
lh_gl_scrollbar_event(const struct lh_gl_entity *self, const lh_gl_entity_event_t *event)
{
    /* The entity is the first field: self is the scrollbar. */
    const lh_gl_scrollbar_t *scrollbar = lh_ptr_rcast(const lh_gl_scrollbar_t, self);

    lh_gl_entity_class_event_base(lh_addr_of(lh_gl_scrollbar_class), self, event);
    lh_gl_scrollbar_on_draw(scrollbar, event);
    if (lh_gl_entity_event_get_code(event) == lh_gl_entity_event_clickable)
    {
        /* The thumb is drawn by this class and not by a child of its own, but the
           answer is the same one a button gives: the pointer over a scrollbar is the
           scrollbar's. */
        lh_ptr_deref(lh_gl_entity_event_get_clickable(event)) = lh_bool_true;
    }
    lh_gl_scrollbar_on_click(scrollbar, event);
    lh_gl_scrollbar_on_visible(scrollbar, event);
}

/* -- Finding and damage ---------------------------------------------------- */

lh_gl_container_t *
lh_gl_scrollbar_get_driven(lh_gl_entity_t *entity)
{
    const lh_gl_scrollbar_t *bar = lh_gl_entity_as_scrollbar(entity);

    return lh_null_eq(bar) ? lh_null : lh_gl_scrollbar_get_container(bar);
}

lh_gl_container_t *
lh_gl_scrollbar_find_scrolled(lh_gl_entity_t *entity)
{
    lh_gl_container_t *container = lh_gl_scrollbar_get_driven(entity);

    return lh_null_ne(container) ? container : lh_gl_entity_find_scrollable(entity);
}

lh_gl_scrollbar_t *
lh_gl_scrollbar_get_bound(lh_gl_entity_t *entity, const lh_gl_container_t *container)
{
    lh_gl_scrollbar_t *bar = lh_gl_entity_as_scrollbar(entity);

    lh_return_if(lh_null_eq(bar) || lh_gl_scrollbar_get_container(bar) != container, lh_null);
    return bar;
}

lh_void
lh_gl_scrollbar_add_bound_damage(lh_gl_entity_t *entity, const lh_gl_container_t *container,
                                 lh_gl_canvas_t *canvas)
{
    lh_gl_scrollbar_t *bar = lh_gl_scrollbar_get_bound(entity, container);

    lh_return_if(lh_null_eq(bar));
    lh_gl_entity_add_damage(lh_gl_scrollbar_as_entity(bar), canvas);
}

lh_void
lh_gl_scrollbar_add_scroll_damage(lh_gl_container_t *container, lh_gl_canvas_t *canvas)
{
    lh_gl_entity_t *entity = lh_gl_container_as_entity(container);
    lh_gl_entity_t *parent = lh_gl_entity_get_parent(entity);
    lh_gl_entity_t *child;

    lh_gl_entity_add_damage(entity, canvas);
    lh_return_if(lh_null_eq(parent));
    for (child = lh_gl_entity_get_first_child(parent); lh_null_ne(child);
         child = lh_gl_entity_get_next_child(parent, child))
    {
        lh_gl_scrollbar_add_bound_damage(child, container, canvas);
    }
}

const lh_gl_entity_t *
lh_gl_scrollbar_get_entity(const lh_gl_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

lh_gl_entity_t *
lh_gl_scrollbar_get_entity_mut(lh_gl_scrollbar_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

lh_void
lh_gl_scrollbar_set_entity(lh_gl_scrollbar_t *self, lh_gl_entity_t entity)
{
    lh_assert_runtime_ref(self);
    self->entity = entity;
}

lh_void
lh_gl_scrollbar_set_axis(lh_gl_scrollbar_t *self, lh_gl_axis_t axis)
{
    lh_assert_runtime_ref(self);
    self->axis = axis;
}

lh_void
lh_gl_scrollbar_set_container(lh_gl_scrollbar_t *self, lh_gl_container_t *container)
{
    lh_assert_runtime_ref(self);
    self->container = container;
}
