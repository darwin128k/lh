/**
 * @file geometry.c
 * @brief Implementation of `lh/gl/entity.h`: the geometry: place, bounds, the children's space,
 * moving.
 */

#include <lh/gl/entity.h>

#include <lh/core/assert.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_entity_set_place_v(lh_gl_entity_t *self, const lh_gl_place_t *place)
{
    lh_return_if(lh_null_eq(place));
    lh_gl_entity_set_place(self, lh_ptr_deref(place));
}

lh_bool_t
lh_gl_entity_get_children_transform(const lh_gl_entity_t *self, lh_gl_point_t *offset)
{
    lh_assert_runtime_ref(offset);
    lh_gl_entity_transform_t transform;
    lh_gl_entity_ask_children(self, lh_addr_of(transform));
    lh_gl_entity_transform_copy_offset(lh_addr_of(transform), offset);
    return lh_gl_entity_transform_is_clip(lh_addr_of(transform));
}

lh_void
lh_gl_entity_extend_bounds(const lh_gl_entity_t *self, lh_gl_rect_t bounds, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    if (lh_gl_entity_is_hidden(self))
    {
        lh_ptr_deref(out) = bounds;
        return;
    }
    lh_gl_rect_union(lh_addr_of(bounds), lh_gl_entity_get_rect(self), out);
}

lh_gl_scalar_t
lh_gl_entity_get_baseline(const lh_gl_entity_t *self)
{
    lh_gl_scalar_t baseline = lh_gl_scalar(-1);

    /* No line to stand on, so a row that asks falls back to ::lh_gl_place_align_center.
       A base entity with a fill has nothing to be the baseline of. */
    lh_gl_entity_send(self, lh_gl_entity_event_baseline, lh_addr_of(baseline));
    return baseline;
}

lh_void
lh_gl_entity_get_measure_bounds(const lh_gl_entity_t *self, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_rect_t bounds;

    lh_gl_rect_init_empty(lh_addr_of(bounds));
    lh_gl_entity_send(self, lh_gl_entity_event_measure, lh_addr_of(bounds));
    lh_ptr_deref(out) = bounds;
}

lh_void
lh_gl_entity_set_rect_v(lh_gl_entity_t *self, lh_gl_rect_t rect)
{
    /* The same rect is what a layout writes on every poll. Recording it would
       paint the whole table to put each row back where it already is. */
    lh_return_if(lh_gl_rect_eq(lh_gl_entity_get_rect_mut(self), lh_addr_of(rect)));
    lh_gl_entity_note(self);
    lh_gl_entity_set_rect(self, rect);
    lh_gl_entity_note(self);
}

lh_void
lh_gl_entity_move_by(lh_gl_entity_t *self, lh_gl_scalar_t dx, lh_gl_scalar_t dy)
{
    lh_gl_entity_t *child;

    lh_return_if(dx == lh_gl_scalar(0) && dy == lh_gl_scalar(0));
    lh_gl_entity_note(self);
    lh_gl_rect_offset(lh_gl_entity_get_rect_mut(self), dx, dy, lh_gl_entity_get_rect_mut(self));
    for (child = lh_gl_entity_get_first_child(self); lh_null_ne(child);
         child = lh_gl_entity_get_next_child(self, child))
    {
        lh_gl_entity_move_by(child, dx, dy);
    }
    lh_gl_entity_note(self);
}

lh_void
lh_gl_entity_get_children_bounds(const lh_gl_entity_t *self, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_rect_t bounds;

    lh_gl_rect_init_empty(lh_addr_of(bounds));
    for (const lh_gl_entity_t *child = lh_gl_entity_get_first_child(self); lh_null_ne(child);
         child = lh_gl_entity_get_next_child(self, child))
    {
        lh_gl_entity_extend_bounds(child, bounds, lh_addr_of(bounds));
    }
    lh_ptr_deref(out) = bounds;
}

lh_void
lh_gl_entity_to_children_space(const lh_gl_entity_t *self, lh_gl_point_t point, lh_gl_point_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_point_t offset;

    LH_ATTRIBUTE_UNUSED(lh_gl_entity_get_children_transform(self, lh_addr_of(offset)));
    lh_gl_point_offset(lh_addr_of(point), lh_math_neg(lh_gl_point_get_x(lh_addr_of(offset))),
                       lh_math_neg(lh_gl_point_get_y(lh_addr_of(offset))), out);
}

lh_void
lh_gl_entity_add_children_offset(const lh_gl_entity_t *self, lh_gl_point_t point,
                                 lh_gl_point_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_point_t offset;

    LH_ATTRIBUTE_UNUSED(lh_gl_entity_get_children_transform(self, lh_addr_of(offset)));
    lh_gl_point_offset(lh_addr_of(point), lh_gl_point_get_x(lh_addr_of(offset)),
                       lh_gl_point_get_y(lh_addr_of(offset)), out);
}

lh_void
lh_gl_entity_move_to(lh_gl_entity_t *self, lh_gl_point_t origin)
{
    const lh_gl_point_t *from = lh_gl_rect_get_origin(lh_gl_entity_get_rect_mut(self));

    lh_gl_entity_move_by(self, lh_gl_point_get_x(lh_addr_of(origin)) - lh_gl_point_get_x(from),
                         lh_gl_point_get_y(lh_addr_of(origin)) - lh_gl_point_get_y(from));
}

lh_void
lh_gl_entity_get_content_bounds(const lh_gl_entity_t *self, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_rect_t measured;
    lh_gl_entity_get_measure_bounds(self, lh_addr_of(measured));
    lh_gl_rect_t children;
    lh_gl_entity_get_children_bounds(self, lh_addr_of(children));

    lh_gl_rect_union(lh_addr_of(measured), lh_addr_of(children), out);
}

lh_void
lh_gl_entity_get_root_offset(const lh_gl_entity_t *self, lh_gl_point_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_point_t total;

    lh_gl_point_init(lh_addr_of(total), lh_gl_scalar(0), lh_gl_scalar(0));
    for (self = lh_gl_entity_get_parent(self); lh_null_ne(self);
         self = lh_gl_entity_get_parent(self))
    {
        lh_gl_entity_add_children_offset(self, total, lh_addr_of(total));
    }
    lh_ptr_deref(out) = total;
}

lh_void
lh_gl_entity_to_local(const lh_gl_entity_t *self, lh_gl_point_t point, lh_gl_point_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_point_t offset;
    lh_gl_entity_get_root_offset(self, lh_addr_of(offset));

    lh_gl_point_offset(lh_addr_of(point), lh_math_neg(lh_gl_point_get_x(lh_addr_of(offset))),
                       lh_math_neg(lh_gl_point_get_y(lh_addr_of(offset))), out);
}

lh_void
lh_gl_entity_get_root_rect(const lh_gl_entity_t *self, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_point_t offset;
    lh_gl_entity_get_root_offset(self, lh_addr_of(offset));

    lh_gl_rect_offset(lh_gl_entity_get_rect(self), lh_gl_point_get_x(lh_addr_of(offset)),
                      lh_gl_point_get_y(lh_addr_of(offset)), out);
}
