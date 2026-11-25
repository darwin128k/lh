/**
 * @file container.c
 * @brief Implementation of `lh/gl/container.h`.
 */
#include <lh/core/assert.h>

#include <lh/core/math.h>
#include <lh/gl/container.h>
#include <lh/core/util/return.h>

const lh_gl_entity_class_t lh_gl_container_class = {lh_gl_container_event,
                                                    lh_addr_of(lh_gl_entity_class)};

lh_void
lh_gl_container_on_focusable(const lh_gl_entity_event_t *event)
{
    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_focusable);
    lh_ptr_deref(lh_gl_entity_event_get_focusable(event)) = lh_bool_true;
}

const lh_gl_layout_t *
lh_gl_container_get_layout(const lh_gl_container_t *self)
{
    lh_assert_runtime_ref(self);
    return self->layout;
}

lh_void
lh_gl_container_set_layout(lh_gl_container_t *self, const lh_gl_layout_t *layout)
{
    lh_assert_runtime_ref(self);
    self->layout = layout;
}

lh_void
lh_gl_container_init(lh_gl_container_t *self, lh_gl_rect_t rect)
{
    lh_gl_entity_init(lh_gl_container_get_entity_mut(self), rect);
    lh_gl_entity_set_class_v(lh_gl_container_get_entity_mut(self),
                             lh_addr_of(lh_gl_container_class));
    lh_gl_point_init(lh_gl_container_get_scroll_mut(self), lh_gl_scalar(0), lh_gl_scalar(0));
    lh_gl_container_set_layout(self, lh_null);
    lh_gl_container_set_placing(self, lh_bool_false);
    lh_gl_container_set_scroll_enabled(self, lh_bool_true);
}

lh_gl_entity_t *
lh_gl_container_as_entity(lh_gl_container_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

lh_gl_container_t *
lh_gl_entity_as_container(lh_gl_entity_t *entity)
{
    lh_return_if(lh_null_eq(entity), lh_null);
    lh_return_if(
        !lh_gl_entity_class_is(lh_gl_entity_get_class(entity), lh_addr_of(lh_gl_container_class)),
        lh_null);
    return lh_ptr_rcast(lh_gl_container_t, entity);
}

lh_gl_container_t *
lh_gl_entity_find_container(lh_gl_entity_t *entity)
{
    lh_gl_container_t *container;

    for (; lh_null_ne(entity); entity = lh_gl_entity_get_parent(entity))
    {
        container = lh_gl_entity_as_container(entity);
        lh_return_if(lh_null_ne(container), container);
    }
    return lh_null;
}

lh_void
lh_gl_container_get_viewport_size(const lh_gl_container_t *self, lh_gl_size_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) =
        lh_ptr_deref(lh_gl_rect_get_size(lh_gl_entity_get_rect(lh_gl_container_get_entity(self))));
}

lh_void
lh_gl_container_pad_far(const lh_gl_container_t *self, lh_gl_point_t far, lh_gl_point_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_insets_t padding;
    lh_gl_entity_get_padding(lh_gl_container_get_entity(self), lh_addr_of(padding));

    lh_gl_point_offset(lh_addr_of(far), lh_gl_insets_get_right(lh_addr_of(padding)),
                       lh_gl_insets_get_bottom(lh_addr_of(padding)), out);
}

lh_void
lh_gl_container_get_content_far(const lh_gl_container_t *self, lh_gl_point_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_rect_t bounds;

    lh_gl_entity_get_content_bounds(lh_gl_container_get_entity(self), lh_addr_of(bounds));
    if (lh_gl_rect_is_empty(lh_addr_of(bounds)))
    {
        lh_ptr_deref(out) = lh_ptr_deref(
            lh_gl_rect_get_origin(lh_gl_entity_get_rect(lh_gl_container_get_entity(self))));
        return;
    }
    {
        lh_gl_point_t far;
        lh_gl_rect_far(lh_addr_of(bounds), lh_addr_of(far));
        lh_gl_container_pad_far(self, far, out);
    }
}

lh_void
lh_gl_container_get_content_size(const lh_gl_container_t *self, lh_gl_size_t *out)
{
    lh_assert_runtime_ref(out);

    const lh_gl_point_t *origin =
        lh_gl_rect_get_origin(lh_gl_entity_get_rect(lh_gl_container_get_entity(self)));
    lh_gl_point_t corner;
    lh_gl_container_get_content_far(self, lh_addr_of(corner));
    lh_gl_point_max(lh_addr_of(corner), origin, lh_addr_of(corner));
    lh_gl_size_from_extent(origin, lh_addr_of(corner), out);
}

lh_void
lh_gl_container_get_scroll_max(const lh_gl_container_t *self, lh_gl_point_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_size_t content;

    lh_gl_container_get_content_size(self, lh_addr_of(content));
    lh_gl_size_t viewport;
    lh_gl_container_get_viewport_size(self, lh_addr_of(viewport));
    lh_gl_point_init(out,
                     lh_gl_scalar_slack(lh_gl_size_get_width(lh_addr_of(content)),
                                        lh_gl_size_get_width(lh_addr_of(viewport))),
                     lh_gl_scalar_slack(lh_gl_size_get_height(lh_addr_of(content)),
                                        lh_gl_size_get_height(lh_addr_of(viewport))));
}

lh_void
lh_gl_container_clamp_scroll_to(lh_gl_point_t scroll, lh_gl_point_t max, lh_gl_point_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_point_t zero;

    lh_gl_point_init(lh_addr_of(zero), lh_gl_scalar(0), lh_gl_scalar(0));
    lh_gl_point_max(lh_addr_of(scroll), lh_addr_of(zero), lh_addr_of(scroll));
    lh_gl_point_min(lh_addr_of(scroll), lh_addr_of(max), out);
}

lh_void
lh_gl_container_clamp_scroll(const lh_gl_container_t *self, lh_gl_point_t scroll,
                             lh_gl_point_t *out)
{
    lh_assert_runtime_ref(out);
    {
        lh_gl_point_t max;
        lh_gl_container_get_scroll_max(self, lh_addr_of(max));
        lh_gl_container_clamp_scroll_to(scroll, max, out);
    }
}

lh_void
lh_gl_container_get_scroll_within(const lh_gl_container_t *self, lh_gl_point_t max,
                                  lh_gl_point_t *out)
{
    lh_gl_container_clamp_scroll_to(lh_ptr_deref(lh_gl_container_get_scroll(self)), max, out);
}

lh_void
lh_gl_container_get_clamped_scroll(const lh_gl_container_t *self, lh_gl_point_t *out)
{
    lh_assert_runtime_ref(self);
    lh_gl_container_clamp_scroll(self, lh_ptr_deref(lh_gl_container_get_scroll(self)), out);
}

lh_gl_scalar_t
lh_gl_container_get_key_step(const lh_gl_container_t *self, lh_key_t key)
{
    lh_gl_size_t page;
    lh_gl_container_get_viewport_size(self, lh_addr_of(page));
    lh_gl_point_t scroll;
    lh_gl_container_get_clamped_scroll(self, lh_addr_of(scroll));
    lh_gl_point_t max;
    lh_gl_container_get_scroll_max(self, lh_addr_of(max));

    lh_return_if(key == lh_key_up || key == lh_key_left, -LH_GL_CONTAINER_KEY_STEP);
    lh_return_if(key == lh_key_down || key == lh_key_right, LH_GL_CONTAINER_KEY_STEP);
    lh_return_if(key == lh_key_page_up, lh_math_neg(lh_gl_size_get_height(lh_addr_of(page))));
    lh_return_if(key == lh_key_page_down, lh_gl_size_get_height(lh_addr_of(page)));
    lh_return_if(key == lh_key_home, lh_math_neg(lh_gl_point_get_y(lh_addr_of(scroll))));
    lh_return_if(key == lh_key_end,
                 lh_gl_point_get_y(lh_addr_of(max)) - lh_gl_point_get_y(lh_addr_of(scroll)));
    return lh_gl_scalar(0);
}

lh_void
lh_gl_container_get_key_delta(const lh_gl_container_t *self, const lh_gl_key_input_t *input,
                              lh_gl_point_t *out)
{
    lh_assert_runtime_ref(out);
    const lh_key_t key = lh_gl_key_input_get_key(input);
    const lh_bool_t across =
        key == lh_key_left || key == lh_key_right ? lh_bool_true : lh_bool_false;
    const lh_gl_scalar_t step =
        lh_gl_key_input_get_code(input) == 0U && lh_gl_key_input_is_pressed(input)
            ? lh_gl_container_get_key_step(self, key)
            : lh_gl_scalar(0);
    lh_gl_point_t delta;

    lh_gl_point_init(lh_addr_of(delta), across ? step : lh_gl_scalar(0),
                     across ? lh_gl_scalar(0) : step);
    lh_ptr_deref(out) = delta;
}

lh_void
lh_gl_container_place_children(const lh_gl_container_t *self, lh_gl_entity_transform_t *transform)
{
    /* First, because everything below is about where the content ended up: the
       flow is the container's own job and this is the only place in a frame that
       can do it. The event carries a const self because events do; the children
       it places are not read-only. */
    if (lh_null_ne(lh_gl_container_get_layout(self)) && !lh_gl_container_is_placing(self))
    {
        lh_gl_container_t *placing = lh_ptr_rcast(lh_gl_container_t, self);

        lh_gl_container_set_placing(placing, lh_bool_true);
        lh_gl_layout_apply(lh_gl_container_get_layout(self), lh_ptr_rcast(lh_gl_entity_t, self));
        lh_gl_container_set_placing(placing, lh_bool_false);
    }
    lh_gl_point_t scroll;
    lh_gl_container_get_clamped_scroll(self, lh_addr_of(scroll));
    lh_gl_point_t offset;
    lh_gl_point_init(lh_addr_of(offset), lh_math_neg(lh_gl_point_get_x(lh_addr_of(scroll))),
                     lh_math_neg(lh_gl_point_get_y(lh_addr_of(scroll))));
    lh_gl_entity_transform_set_offset(transform, offset);
    lh_gl_entity_transform_set_clip(transform, lh_bool_true);
}

lh_void
lh_gl_container_on_children(const lh_gl_container_t *self, const lh_gl_entity_event_t *event)
{
    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_children);
    lh_gl_container_place_children(self, lh_gl_entity_event_get_transform(event));
}

lh_void
lh_gl_container_set_scroll_v(lh_gl_container_t *self, lh_gl_point_t scroll)
{
    lh_gl_container_clamp_scroll(self, scroll, lh_gl_container_get_scroll_mut(self));
}

lh_void
lh_gl_container_scroll_by(lh_gl_container_t *self, lh_gl_scalar_t dx, lh_gl_scalar_t dy)
{
    lh_gl_point_t scroll;

    lh_gl_container_get_clamped_scroll(self, lh_addr_of(scroll));
    lh_gl_point_t point;
    lh_gl_point_offset(lh_addr_of(scroll), dx, dy, lh_addr_of(point));
    lh_gl_container_set_scroll_v(self, point);
}

lh_void
lh_gl_container_on_key(lh_gl_container_t *self, const lh_gl_entity_event_t *event)
{
    lh_return_if(lh_gl_entity_event_get_code(event) != lh_gl_entity_event_key);
    lh_return_if(!lh_gl_container_is_scroll_enabled(self));
    lh_gl_point_t delta;
    lh_gl_container_get_key_delta(self, lh_gl_entity_event_get_key(event), lh_addr_of(delta));
    lh_gl_container_scroll_by(self, lh_gl_point_get_x(lh_addr_of(delta)),
                              lh_gl_point_get_y(lh_addr_of(delta)));
}

lh_void
lh_gl_container_event(const struct lh_gl_entity *self, const lh_gl_entity_event_t *event)
{
    /* The entity is the first field: self is the container. Keys move its
       scroll, so the event's const self is the container it was sent to. */
    lh_gl_container_t *container = lh_ptr_rcast(lh_gl_container_t, self);

    lh_gl_container_on_children(container, event);
    lh_gl_container_on_focusable(event);
    lh_gl_container_on_key(container, event);
    lh_gl_entity_class_event_base(lh_addr_of(lh_gl_container_class), self, event);
}

const lh_gl_entity_t *
lh_gl_container_get_entity(const lh_gl_container_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

lh_gl_entity_t *
lh_gl_container_get_entity_mut(lh_gl_container_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

lh_void
lh_gl_container_set_entity(lh_gl_container_t *self, lh_gl_entity_t entity)
{
    lh_assert_runtime_ref(self);
    self->entity = entity;
}

lh_gl_point_t *
lh_gl_container_get_scroll_mut(lh_gl_container_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->scroll);
}

lh_bool_t
lh_gl_container_is_placing(const lh_gl_container_t *self)
{
    lh_assert_runtime_ref(self);
    return self->placing;
}

lh_void
lh_gl_container_set_placing(lh_gl_container_t *self, lh_bool_t placing)
{
    lh_assert_runtime_ref(self);
    self->placing = placing;
}

const lh_gl_point_t *
lh_gl_container_get_scroll(const lh_gl_container_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->scroll);
}

lh_void
lh_gl_container_set_scroll(lh_gl_container_t *self, lh_gl_point_t scroll)
{
    lh_assert_runtime_ref(self);
    self->scroll = scroll;
}

lh_bool_t
lh_gl_container_can_scroll(const lh_gl_container_t *self)
{
    lh_return_if(!lh_gl_container_is_scroll_enabled(self), lh_bool_false);
    lh_gl_point_t max;
    lh_gl_container_get_scroll_max(self, lh_addr_of(max));
    return lh_gl_point_get_x(lh_addr_of(max)) > lh_gl_scalar(0) ||
           lh_gl_point_get_y(lh_addr_of(max)) > lh_gl_scalar(0);
}

lh_gl_container_t *
lh_gl_entity_find_scrollable(lh_gl_entity_t *entity)
{
    for (; lh_null_ne(entity); entity = lh_gl_entity_get_parent(entity))
    {
        lh_gl_container_t *box = lh_gl_entity_as_container(entity);
        lh_return_if(lh_null_ne(box) && lh_gl_container_can_scroll(box), box);
        lh_return_if(!lh_gl_entity_is_scroll_chain_enabled(entity), lh_null);
    }
    return lh_null;
}

lh_bool_t
lh_gl_container_is_scroll_enabled(const lh_gl_container_t *self)
{
    lh_assert_runtime_ref(self);
    return self->scroll_enabled;
}

lh_void
lh_gl_container_set_scroll_enabled(lh_gl_container_t *self, lh_bool_t enabled)
{
    lh_assert_runtime_ref(self);
    self->scroll_enabled = enabled;
}
