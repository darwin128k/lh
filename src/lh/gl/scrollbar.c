/**
 * @file scrollbar.c
 * @brief Implementation of `lh/gl/scrollbar.h`: the bar itself: its fields (level 1), init and its
 * class.
 */

#include <lh/gl/scrollbar.h>

#include <lh/core/assert.h>
#include <lh/core/util/return.h>

const lh_gl_entity_class_t lh_gl_scrollbar_class = {lh_gl_scrollbar_event,
                                                    lh_addr_of(lh_gl_entity_class)};

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
