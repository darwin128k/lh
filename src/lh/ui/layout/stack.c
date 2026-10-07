/**
 * @file stack.c
 * @brief Implementation of `lh/ui/layout/stack.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/insets.h>
#include <lh/ui/layout/stack.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

lh_void
lh_ui_layout_stack_init(lh_ui_layout_stack_t *self, lh_ui_axis_t axis, lh_ui_scalar_t gap)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(gap < lh_ui_scalar(0), lh_runtime_error_code_invalid_argument);
    self->axis = axis;
    self->gap = gap;
    self->stretch = lh_bool_false;
}

lh_ui_axis_t
lh_ui_layout_stack_get_axis(const lh_ui_layout_stack_t *self)
{
    lh_assert_runtime_ref(self);
    return self->axis;
}

lh_ui_scalar_t
lh_ui_layout_stack_get_gap(const lh_ui_layout_stack_t *self)
{
    lh_assert_runtime_ref(self);
    return self->gap;
}

lh_bool_t
lh_ui_layout_stack_is_stretch(const lh_ui_layout_stack_t *self)
{
    lh_assert_runtime_ref(self);
    return self->stretch;
}

lh_void
lh_ui_layout_stack_set_stretch(lh_ui_layout_stack_t *self, lh_bool_t stretch)
{
    lh_assert_runtime_ref(self);
    self->stretch = stretch;
}

lh_void
lh_ui_layout_stack_fit_cross(const lh_ui_layout_stack_t *self, lh_ui_entity_t *child, const lh_ui_rect_t *content)
{
    const lh_ui_axis_t cross = lh_ui_axis_get_cross(self->axis);
    lh_ui_rect_t rect = lh_ui_entity_get_rect(child);

    lh_return_if(!self->stretch);
    lh_ui_size_set_along(lh_ui_rect_get_size(lh_addr_of(rect)), cross,
                         lh_ui_size_get_along(lh_ui_rect_get_size_as_const(content), cross));
    lh_ui_entity_set_rect(child, rect);
}

lh_ui_point_t
lh_ui_layout_stack_place(const lh_ui_layout_stack_t *self, lh_ui_entity_t *child, lh_ui_point_t cursor,
                         const lh_ui_rect_t *content)
{
    lh_ui_rect_t rect;

    lh_assert_runtime_ref(self);
    lh_return_if(lh_ui_entity_is_hidden(child), cursor);
    lh_ui_entity_move_to(child, cursor);
    lh_ui_layout_stack_fit_cross(self, child, content);
    rect = lh_ui_entity_get_rect(child);
    lh_ui_point_set_along(lh_addr_of(cursor), self->axis,
                          lh_ui_point_get_along(lh_addr_of(cursor), self->axis) +
                              lh_ui_size_get_along(lh_ui_rect_get_size_as_const(lh_addr_of(rect)), self->axis) +
                              self->gap);
    return cursor;
}

lh_void
lh_ui_layout_stack_apply(const lh_ui_layout_stack_t *self, lh_ui_entity_t *parent)
{
    const lh_ui_insets_t padding = lh_ui_entity_get_padding(parent);
    const lh_ui_rect_t rect = lh_ui_entity_get_rect(parent);
    const lh_ui_rect_t content = lh_ui_insets_shrink(lh_addr_of(padding), lh_addr_of(rect));
    lh_ui_point_t cursor = *lh_ui_rect_get_origin_as_const(lh_addr_of(content));
    lh_ui_entity_t *child;

    for (child = lh_ui_entity_get_first_child(parent); lh_null_ne(child);
         child = lh_ui_entity_get_next_child(parent, child))
    {
        cursor = lh_ui_layout_stack_place(self, child, cursor, lh_addr_of(content));
    }
}
