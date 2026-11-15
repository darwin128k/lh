/**
 * @file walk.c
 * @brief Level-1 accessors of ::lh_gl_view_focus_walk_t.
 */

#include <lh/gl/view/focus/walk.h>


const lh_gl_entity_t *
lh_gl_view_focus_walk_get_current(const lh_gl_view_focus_walk_t *self)
{
    lh_assert_runtime_ref(self);
    return self->current;
}

lh_void
lh_gl_view_focus_walk_set_current(lh_gl_view_focus_walk_t *self, const lh_gl_entity_t * current)
{
    lh_assert_runtime_ref(self);
    self->current = current;
}

lh_bool_t
lh_gl_view_focus_walk_is_passed(const lh_gl_view_focus_walk_t *self)
{
    lh_assert_runtime_ref(self);
    return self->passed;
}

lh_void
lh_gl_view_focus_walk_set_passed(lh_gl_view_focus_walk_t *self, lh_bool_t passed)
{
    lh_assert_runtime_ref(self);
    self->passed = passed;
}

const lh_gl_entity_t *
lh_gl_view_focus_walk_get_first(const lh_gl_view_focus_walk_t *self)
{
    lh_assert_runtime_ref(self);
    return self->first;
}

lh_void
lh_gl_view_focus_walk_set_first(lh_gl_view_focus_walk_t *self, const lh_gl_entity_t * first)
{
    lh_assert_runtime_ref(self);
    self->first = first;
}

const lh_gl_entity_t *
lh_gl_view_focus_walk_get_next(const lh_gl_view_focus_walk_t *self)
{
    lh_assert_runtime_ref(self);
    return self->next;
}

lh_void
lh_gl_view_focus_walk_set_next(lh_gl_view_focus_walk_t *self, const lh_gl_entity_t * next)
{
    lh_assert_runtime_ref(self);
    self->next = next;
}
