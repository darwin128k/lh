/**
 * @file container.c
 * @brief Implementation of `lh/ui/entity/container.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/math.h>
#include <lh/null.h>
#include <lh/ui/entity/class.h>
#include <lh/ui/entity/container.h>
#include <lh/ui/entity/transform.h>
#include <lh/ui/key.h>
#include <lh/ui/rect.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

const lh_ui_entity_class_t lh_ui_entity_container_class = {
    lh_ui_entity_container_event, lh_addr_of(lh_ui_entity_class)};

lh_void
lh_ui_entity_container_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event)
{
    /* The entity is the first field: self is the container. Keys move its
       scroll, so the event's const self is the container it was sent to. */
    lh_ui_entity_container_t *container = lh_ptr_rcast(lh_ui_entity_container_t, self);

    lh_ui_entity_container_on_children(container, event);
    lh_ui_entity_container_on_focusable(event);
    lh_ui_entity_container_on_key(container, event);
    lh_ui_entity_class_event_base(lh_addr_of(lh_ui_entity_container_class), self, event);
}

lh_void
lh_ui_entity_container_on_children(const lh_ui_entity_container_t *self, const lh_ui_entity_event_t *event)
{
    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_children);
    lh_ui_entity_container_place_children(self, lh_ui_entity_event_get_transform(event));
}

lh_void
lh_ui_entity_container_on_focusable(const lh_ui_entity_event_t *event)
{
    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_focusable);
    *lh_ui_entity_event_get_focusable(event) = lh_bool_true;
}

lh_void
lh_ui_entity_container_on_key(lh_ui_entity_container_t *self, const lh_ui_entity_event_t *event)
{
    lh_ui_point_t delta;

    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_key);
    delta = lh_ui_entity_container_get_key_delta(self, lh_ui_entity_event_get_key(event));
    lh_ui_entity_container_scroll_by(self, lh_ui_point_get_x(lh_addr_of(delta)), lh_ui_point_get_y(lh_addr_of(delta)));
}

lh_ui_scalar_t
lh_ui_entity_container_get_key_step(const lh_ui_entity_container_t *self, lh_key_t key)
{
    const lh_ui_size_t page = lh_ui_entity_container_get_viewport_size(self);
    const lh_ui_point_t scroll = lh_ui_entity_container_get_scroll(self);
    const lh_ui_point_t max = lh_ui_entity_container_get_scroll_max(self);

    lh_return_if(key == lh_key_up || key == lh_key_left, -LH_UI_ENTITY_CONTAINER_KEY_STEP);
    lh_return_if(key == lh_key_down || key == lh_key_right, LH_UI_ENTITY_CONTAINER_KEY_STEP);
    lh_return_if(key == lh_key_page_up, -lh_ui_size_get_height(lh_addr_of(page)));
    lh_return_if(key == lh_key_page_down, lh_ui_size_get_height(lh_addr_of(page)));
    lh_return_if(key == lh_key_home, -lh_ui_point_get_y(lh_addr_of(scroll)));
    lh_return_if(key == lh_key_end, lh_ui_point_get_y(lh_addr_of(max)) - lh_ui_point_get_y(lh_addr_of(scroll)));
    return lh_ui_scalar(0);
}

lh_ui_point_t
lh_ui_entity_container_get_key_delta(const lh_ui_entity_container_t *self, const lh_ui_key_input_t *input)
{
    const lh_key_t key = lh_ui_key_input_get_key(input);
    const lh_bool_t across = key == lh_key_left || key == lh_key_right ? lh_bool_true : lh_bool_false;
    const lh_ui_scalar_t step = lh_ui_key_input_get_code(input) == 0U && lh_ui_key_input_is_pressed(input)
                                    ? lh_ui_entity_container_get_key_step(self, key)
                                    : lh_ui_scalar(0);
    lh_ui_point_t delta;

    lh_ui_point_init(lh_addr_of(delta), across ? step : lh_ui_scalar(0), across ? lh_ui_scalar(0) : step);
    return delta;
}

lh_void
lh_ui_entity_container_place_children(const lh_ui_entity_container_t *self,
                                      lh_ui_entity_transform_t *transform)
{
    lh_ui_point_t scroll;
    lh_ui_point_t offset;

    /* First, because everything below is about where the content ended up: the
       flow is the container's own job and this is the only place in a frame that
       can do it. The event carries a const self because events do; the children
       it places are not read-only. */
    if (lh_null_ne(self->layout))
    {
        lh_ui_layout_apply(self->layout, lh_ptr_rcast(lh_ui_entity_t, self));
    }
    scroll = lh_ui_entity_container_get_scroll(self);
    lh_ui_point_init(lh_addr_of(offset), -lh_ui_point_get_x(lh_addr_of(scroll)),
                     -lh_ui_point_get_y(lh_addr_of(scroll)));
    lh_ui_entity_transform_set_offset(transform, offset);
    lh_ui_entity_transform_set_clip(transform, lh_bool_true);
}

const lh_ui_layout_t *
lh_ui_entity_container_get_layout(const lh_ui_entity_container_t *self)
{
    lh_assert_runtime_ref(self);
    return self->layout;
}

lh_void
lh_ui_entity_container_set_layout(lh_ui_entity_container_t *self, const lh_ui_layout_t *layout)
{
    lh_assert_runtime_ref(self);
    self->layout = layout;
}

lh_void
lh_ui_entity_container_init(lh_ui_entity_container_t *self, lh_ui_rect_t rect)
{
    lh_assert_runtime_ref(self);
    lh_ui_entity_init(lh_addr_of(self->entity), rect);
    lh_ui_entity_set_class(lh_addr_of(self->entity), lh_addr_of(lh_ui_entity_container_class));
    lh_ui_point_init(lh_addr_of(self->scroll), lh_ui_scalar(0), lh_ui_scalar(0));
    self->layout = lh_null;
}

lh_ui_entity_t *
lh_ui_entity_container_as_entity(lh_ui_entity_container_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

lh_ui_entity_container_t *
lh_ui_entity_as_container(lh_ui_entity_t *entity)
{
    lh_return_if(lh_null_eq(entity), lh_null);
    lh_return_if(!lh_ui_entity_class_is(lh_ui_entity_get_class(entity),
                                        lh_addr_of(lh_ui_entity_container_class)),
                 lh_null);
    return lh_ptr_rcast(lh_ui_entity_container_t, entity);
}

lh_ui_entity_container_t *
lh_ui_entity_find_container(lh_ui_entity_t *entity)
{
    lh_ui_entity_container_t *container;

    for (; lh_null_ne(entity); entity = lh_ui_entity_get_parent(entity))
    {
        container = lh_ui_entity_as_container(entity);
        lh_return_if(lh_null_ne(container), container);
    }
    return lh_null;
}

lh_ui_size_t
lh_ui_entity_container_get_viewport_size(const lh_ui_entity_container_t *self)
{
    lh_assert_runtime_ref(self);
    return *lh_ui_rect_get_size_as_const(lh_addr_of(self->entity.rect));
}

lh_ui_point_t
lh_ui_entity_container_pad_far(const lh_ui_entity_container_t *self, lh_ui_point_t far)
{
    const lh_ui_insets_t padding = lh_ui_entity_get_padding(lh_addr_of(self->entity));

    return lh_ui_point_offset(lh_addr_of(far), lh_ui_insets_get_right(lh_addr_of(padding)),
                              lh_ui_insets_get_bottom(lh_addr_of(padding)));
}

lh_ui_point_t
lh_ui_entity_container_get_content_far(const lh_ui_entity_container_t *self)
{
    lh_ui_rect_t bounds;

    lh_assert_runtime_ref(self);
    bounds = lh_ui_entity_get_content_bounds(lh_addr_of(self->entity));
    lh_return_if(lh_ui_rect_is_empty(lh_addr_of(bounds)),
                 *lh_ui_rect_get_origin_as_const(lh_addr_of(self->entity.rect)));
    return lh_ui_entity_container_pad_far(self, lh_ui_rect_far(lh_addr_of(bounds)));
}

lh_ui_size_t
lh_ui_entity_container_get_content_size(const lh_ui_entity_container_t *self)
{
    const lh_ui_point_t *origin;
    lh_ui_point_t corner;

    lh_assert_runtime_ref(self);
    origin = lh_ui_rect_get_origin_as_const(lh_addr_of(self->entity.rect));
    corner = lh_ui_entity_container_get_content_far(self);
    corner = lh_ui_point_max(lh_addr_of(corner), origin);
    return lh_ui_size_from_extent(origin, lh_addr_of(corner));
}

lh_ui_point_t
lh_ui_entity_container_get_scroll_max(const lh_ui_entity_container_t *self)
{
    lh_ui_size_t content;
    lh_ui_size_t viewport;
    lh_ui_scalar_t dx;
    lh_ui_scalar_t dy;
    lh_ui_point_t max;

    content = lh_ui_entity_container_get_content_size(self);
    viewport = lh_ui_entity_container_get_viewport_size(self);
    dx = lh_ui_size_get_width(lh_addr_of(content)) - lh_ui_size_get_width(lh_addr_of(viewport));
    dy = lh_ui_size_get_height(lh_addr_of(content)) - lh_ui_size_get_height(lh_addr_of(viewport));
    lh_ui_point_init(lh_addr_of(max), lh_math_max(dx, lh_ui_scalar(0)), lh_math_max(dy, lh_ui_scalar(0)));
    return max;
}

lh_ui_point_t
lh_ui_entity_container_clamp_scroll_to(lh_ui_point_t scroll, lh_ui_point_t max)
{
    lh_ui_point_t zero;

    lh_ui_point_init(lh_addr_of(zero), lh_ui_scalar(0), lh_ui_scalar(0));
    scroll = lh_ui_point_max(lh_addr_of(scroll), lh_addr_of(zero));
    return lh_ui_point_min(lh_addr_of(scroll), lh_addr_of(max));
}

lh_ui_point_t
lh_ui_entity_container_clamp_scroll(const lh_ui_entity_container_t *self, lh_ui_point_t scroll)
{
    return lh_ui_entity_container_clamp_scroll_to(scroll, lh_ui_entity_container_get_scroll_max(self));
}

lh_ui_point_t
lh_ui_entity_container_get_scroll_within(const lh_ui_entity_container_t *self, lh_ui_point_t max)
{
    lh_assert_runtime_ref(self);
    return lh_ui_entity_container_clamp_scroll_to(self->scroll, max);
}

lh_ui_point_t
lh_ui_entity_container_get_scroll(const lh_ui_entity_container_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_entity_container_clamp_scroll(self, self->scroll);
}

lh_void
lh_ui_entity_container_set_scroll(lh_ui_entity_container_t *self, lh_ui_point_t scroll)
{
    lh_assert_runtime_ref(self);
    self->scroll = lh_ui_entity_container_clamp_scroll(self, scroll);
}

lh_void
lh_ui_entity_container_scroll_by(lh_ui_entity_container_t *self, lh_ui_scalar_t dx, lh_ui_scalar_t dy)
{
    lh_ui_point_t scroll;

    scroll = lh_ui_entity_container_get_scroll(self);
    lh_ui_entity_container_set_scroll(self, lh_ui_point_offset(lh_addr_of(scroll), dx, dy));
}
