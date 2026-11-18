/**
 * @file walk.c
 * @brief Implementation of `lh/gx/walk.h`.
 */

#include <lh/gx/walk.h>

const lh_gx_entity_t *
lh_gx_walk_get_top(const lh_gx_walk_t *self)
{
    lh_assert_runtime_ref(self);
    return self->top;
}

lh_void
lh_gx_walk_set_top(lh_gx_walk_t *self, const lh_gx_entity_t *top)
{
    lh_assert_runtime_ref(self);
    self->top = top;
}

const lh_gx_entity_t *
lh_gx_walk_get_entity(const lh_gx_walk_t *self)
{
    lh_assert_runtime_ref(self);
    return self->entity;
}

lh_void
lh_gx_walk_set_entity(lh_gx_walk_t *self, const lh_gx_entity_t *entity)
{
    lh_assert_runtime_ref(self);
    self->entity = entity;
}

const lh_gx_point_t *
lh_gx_walk_get_origin(const lh_gx_walk_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->origin);
}

lh_void
lh_gx_walk_set_origin(lh_gx_walk_t *self, lh_gx_point_t origin)
{
    lh_assert_runtime_ref(self);
    self->origin = origin;
}

const lh_gx_rect_t *
lh_gx_walk_get_place(const lh_gx_walk_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->place);
}

lh_void
lh_gx_walk_set_place(lh_gx_walk_t *self, lh_gx_rect_t place)
{
    lh_assert_runtime_ref(self);
    self->place = place;
}

lh_void
lh_gx_walk_enter(lh_gx_walk_t *self, const lh_gx_entity_t *entity)
{
    const lh_gx_point_t *origin = lh_gx_walk_get_origin(self);
    lh_gx_rect_t place;

    lh_gx_rect_offset(lh_gx_entity_get_place(entity), lh_gx_point_get_x(origin),
                      lh_gx_point_get_y(origin), lh_addr_of(place));
    lh_gx_walk_set_entity(self, entity);
    lh_gx_walk_set_place(self, place);
}

lh_void
lh_gx_walk_init(lh_gx_walk_t *self, const lh_gx_entity_t *top)
{
    lh_gx_point_t origin;

    lh_gx_entity_get_owner_origin(top, lh_addr_of(origin));
    lh_gx_walk_set_top(self, top);
    lh_gx_walk_set_origin(self, origin);
    lh_gx_walk_enter(self, top);
}

lh_gx_walk_step_t
lh_gx_walk_next(lh_gx_walk_t *self, lh_bool_t descend)
{
    const lh_gx_entity_t *at = lh_gx_walk_get_entity(self);
    const lh_gx_entity_t *next = descend ? lh_gx_entity_get_first_child(at) : lh_null;

    if (lh_null_eq(next))
    {
        next = lh_gx_entity_get_next_skip(at, lh_gx_walk_get_top(self));
    }
    if (lh_null_eq(next))
    {
        lh_gx_walk_set_entity(self, lh_null);
        return lh_gx_walk_step_end;
    }
    lh_gx_walk_step_t step = lh_gx_walk_step_across;

    if (lh_ptr_eq(lh_gx_entity_get_owner(next), at))
    {
        /* Down: the children are counted from this entity's place. */
        lh_gx_walk_set_origin(self,
                              lh_ptr_deref(lh_gx_rect_get_origin(lh_gx_walk_get_place(self))));
        step = lh_gx_walk_step_down;
    }
    else if (lh_ptr_ne(lh_gx_entity_get_owner(next), lh_gx_entity_get_owner(at)))
    {
        /* Up: what was carried down is found again from the owners. */
        lh_gx_point_t origin;

        lh_gx_entity_get_owner_origin(next, lh_addr_of(origin));
        lh_gx_walk_set_origin(self, origin);
        step = lh_gx_walk_step_up;
    }
    lh_gx_walk_enter(self, next);
    return step;
}
