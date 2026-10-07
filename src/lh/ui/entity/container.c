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
#include <lh/ui/rect.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

const lh_ui_entity_class_t lh_ui_entity_container_class = {
    lh_ui_entity_container_event, lh_addr_of(lh_ui_entity_class)};

lh_void
lh_ui_entity_container_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event)
{
    lh_assert_runtime_ref(self);
    if (lh_ui_entity_event_get_code(event) == lh_ui_entity_event_children)
    {
        /* The entity is the first field: self is the container. */
        lh_ui_entity_container_place_children(lh_ptr_rcast(const lh_ui_entity_container_t, self),
                                              lh_ui_entity_event_get_transform(event));
        return;
    }
    lh_ui_entity_class_event_base(lh_addr_of(lh_ui_entity_container_class), self, event);
}

lh_void
lh_ui_entity_container_place_children(const lh_ui_entity_container_t *self,
                                      lh_ui_entity_transform_t *transform)
{
    lh_ui_point_t scroll;
    lh_ui_point_t offset;

    scroll = lh_ui_entity_container_get_scroll(self);
    lh_ui_point_init(lh_addr_of(offset), -lh_ui_point_get_x(lh_addr_of(scroll)),
                     -lh_ui_point_get_y(lh_addr_of(scroll)));
    lh_ui_entity_transform_set_offset(transform, offset);
    lh_ui_entity_transform_set_clip(transform, lh_bool_true);
}

lh_void
lh_ui_entity_container_init(lh_ui_entity_container_t *self, lh_ui_rect_t rect)
{
    lh_assert_runtime_ref(self);
    lh_ui_entity_init(lh_addr_of(self->entity), rect);
    lh_ui_entity_set_class(lh_addr_of(self->entity), lh_addr_of(lh_ui_entity_container_class));
    lh_ui_point_init(lh_addr_of(self->scroll), lh_ui_scalar(0), lh_ui_scalar(0));
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
    const lh_ui_scalar_t padding = lh_ui_entity_get_padding(lh_addr_of(self->entity));

    return lh_ui_point_offset(lh_addr_of(far), padding, padding);
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
