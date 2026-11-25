/**
 * @file hit.c
 * @brief Implementation of `lh/gl/view.h`: the hit test: the entity under a point.
 */

#include <lh/gl/view.h>

#include <lh/core/util/return.h>

lh_gl_entity_t *
lh_gl_view_hit_test(lh_gl_view_t *self, lh_gl_point_t point)
{
    lh_return_if(lh_null_eq(lh_gl_view_get_root(self)), lh_null);
    return lh_gl_entity_find_at(lh_gl_view_get_root(self), point);
}
