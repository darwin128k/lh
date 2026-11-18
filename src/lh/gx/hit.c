/**
 * @file hit.c
 * @brief Implementation of `lh/gx/hit.h`.
 */

#include <lh/gx/hit.h>

#include <lh/core/cast/const.h>
#include <lh/core/util/return.h>
#include <lh/gx/class.h>
#include <lh/gx/walk.h>

lh_bool_t
lh_gx_hit_is_on(const lh_gx_entity_t *entity, const lh_gx_rect_t *place, const lh_gx_point_t *at)
{
    lh_return_if(!lh_gx_rect_contains_point(place, lh_ptr_deref(at)), lh_bool_false);
    const lh_gx_class_t *klass = lh_gx_entity_get_class(entity);

    lh_return_if(lh_null_eq(klass), lh_bool_true);
    lh_gx_class_hit_fn *hit = lh_gx_class_get_hit(klass);

    lh_return_if(lh_null_eq(lh_ptr_rcast(lh_void, hit)), lh_bool_true);
    return hit(entity, place, at);
}

lh_gx_entity_t *
lh_gx_hit_find(lh_gx_entity_t *top, const lh_gx_point_t *at)
{
    const lh_gx_entity_t *found = lh_null;
    lh_gx_walk_t walk;

    lh_gx_walk_init(lh_addr_of(walk), top);
    lh_bool_t descend;

    do
    {
        const lh_gx_entity_t *entity = lh_gx_walk_get_entity(lh_addr_of(walk));
        const lh_gx_rect_t *place = lh_gx_walk_get_place(lh_addr_of(walk));

        descend = lh_cast_static(lh_bool_t, lh_gx_entity_is_visible(entity) &&
                                                lh_gx_rect_contains_point(place, lh_ptr_deref(at)));
        if (descend && lh_gx_entity_can_take_input(entity) && lh_gx_hit_is_on(entity, place, at))
        {
            /* Drawn later is in front: the last match wins. */
            found = entity;
        }
    } while (lh_gx_walk_next(lh_addr_of(walk), descend) != lh_gx_walk_step_end);
    /* Every entity of the subtree is the caller's, as top is. */
    return lh_cast_const(lh_gx_entity_t *, found);
}
