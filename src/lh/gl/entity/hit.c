/**
 * @file hit.c
 * @brief Implementation of `lh/gl/entity.h`: the hit test: which entity is under a point.
 */

#include <lh/gl/entity.h>

#include <lh/core/assert.h>
#include <lh/core/util/return.h>

lh_bool_t
lh_gl_entity_contains_point(const lh_gl_entity_t *self, lh_gl_point_t point)
{
    return lh_gl_radius_contains(lh_gl_entity_get_rect(self), lh_gl_entity_get_hit_radius(self),
                                 point);
}

lh_bool_t
lh_gl_entity_is_hit(const lh_gl_entity_t *self, lh_gl_point_t point)
{
    lh_return_if(!lh_gl_entity_contains_point(self, point), lh_bool_false);
    return lh_gl_entity_is_shown(self);
}

lh_bool_t
lh_gl_entity_push_children(const lh_gl_entity_t *self, lh_gl_canvas_t *canvas)
{
    lh_return_if(lh_null_eq(canvas), lh_bool_false);
    lh_gl_entity_transform_t transform;
    lh_gl_entity_ask_children(self, lh_addr_of(transform));
    lh_return_if(lh_gl_entity_transform_is_identity(lh_addr_of(transform)), lh_bool_false);
    lh_gl_rect_t rect = lh_ptr_deref(lh_gl_entity_get_rect(self));
    lh_gl_point_t point;
    lh_gl_entity_transform_copy_offset(lh_addr_of(transform), lh_addr_of(point));
    lh_gl_canvas_push_round(canvas, point,
                            lh_gl_entity_transform_is_clip(lh_addr_of(transform)) ? lh_addr_of(rect)
                                                                                  : lh_null,
                            lh_gl_entity_get_radius_now(self));
    return lh_bool_true;
}

lh_bool_t
lh_gl_entity_is_clipping(const lh_gl_entity_t *self)
{
    lh_gl_point_t offset;

    return lh_gl_entity_get_children_transform(self, lh_addr_of(offset));
}

lh_bool_t
lh_gl_entity_searches_children_at(const lh_gl_entity_t *self, lh_gl_point_t point)
{
    /* The shape it is drawn and clipped with, not the shape it is hit with: a
       clipping parent must not hand its cut corner to a child, and a plain one
       must, because a plain parent draws children outside its rounded shape. */
    lh_return_if(lh_gl_radius_contains(lh_gl_entity_get_rect(self),
                                       lh_gl_entity_get_radius_now(self), point),
                 lh_bool_true);
    /* Outside: only children could be hit, and asking about the clip may measure content. */
    lh_return_if(lh_null_eq(lh_gl_entity_get_first_child(self)), lh_bool_false);
    return !lh_gl_entity_is_clipping(self) ? lh_bool_true : lh_bool_false;
}

lh_gl_entity_t *
lh_gl_entity_find_child_at(const lh_gl_entity_t *self, lh_gl_point_t point, lh_gl_point_t *local)
{
    lh_gl_entity_t *child;
    lh_gl_entity_t *hit = lh_null;

    lh_return_if(!lh_gl_entity_searches_children_at(self, point), lh_null);
    lh_return_if(lh_null_eq(lh_gl_entity_get_last_child(self)), lh_null);
    lh_gl_entity_to_children_space(self, point, lh_addr_of(point));
    for (child = lh_gl_entity_get_last_child(self); lh_null_eq(hit) && lh_null_ne(child);
         child = lh_gl_entity_get_prev_child(self, child))
    {
        hit = lh_gl_entity_find_at_local(child, point, local);
    }
    return hit;
}

lh_gl_entity_t *
lh_gl_entity_find_at_local(lh_gl_entity_t *self, lh_gl_point_t point, lh_gl_point_t *local)
{
    lh_gl_entity_t *hit;

    lh_assert_runtime_ref(local);
    lh_return_if(!lh_gl_entity_is_shown(self), lh_null);
    hit = lh_gl_entity_find_child_at(self, point, local);
    lh_return_if(lh_null_ne(hit), hit);
    lh_return_if(!lh_gl_entity_contains_point(self, point), lh_null);
    lh_ptr_deref(local) = point;
    return self;
}

lh_bool_t
lh_gl_entity_may_hit(const lh_gl_entity_t *self, lh_gl_point_t point)
{
    lh_return_if(!lh_gl_entity_is_shown(self), lh_bool_false);
    /* Its own target, or somewhere a child may be. Not one gate over both: the
       shape a clipping parent draws is what may hand out its children, while the
       entity itself is hit by its own target, which by default is the whole
       rect. Gating the entity on the drawn shape instead would make
       ::lh_gl_style_set_hit_radius unreachable — a rounded entity could never
       be pressed in the corner it does not paint. */
    return lh_gl_entity_contains_point(self, point) ||
           lh_gl_entity_searches_children_at(self, point);
}

lh_gl_entity_t *
lh_gl_entity_find_at(lh_gl_entity_t *self, lh_gl_point_t point)
{
    lh_gl_point_t local;

    return lh_gl_entity_find_at_local(self, point, lh_addr_of(local));
}
