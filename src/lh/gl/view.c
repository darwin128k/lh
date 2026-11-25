/**
 * @file view.c
 * @brief Implementation of `lh/gl/view.h`: the view itself: its fields (level 1), init, deinit and
 * the root.
 */

#include <lh/gl/view.h>

#include <lh/core/assert.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_view_set_canvas(lh_gl_view_t *self, lh_gl_canvas_t *canvas)
{
    lh_assert_runtime_ref(self);
    self->canvas = canvas;
}

lh_gl_canvas_t *
lh_gl_view_get_canvas(const lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->canvas;
}

lh_gl_entity_t *
lh_gl_view_get_root(const lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->root;
}

lh_void
lh_gl_view_set_clear(lh_gl_view_t *self, const lh_gl_color_t *clear)
{
    lh_assert_runtime_ref(self);
    self->clear = clear;
}

const lh_gl_color_t *
lh_gl_view_get_clear(const lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->clear;
}

lh_void
lh_gl_view_set_strip_height(lh_gl_view_t *self, lh_gl_scalar_t height)
{
    lh_assert_runtime_ref(self);
    self->strip_height = height;
}

lh_gl_scalar_t
lh_gl_view_get_strip_height(const lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->strip_height;
}

lh_gl_entity_t *
lh_gl_view_get_focus(const lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->focus;
}

const lh_gl_view_pointer_t *
lh_gl_view_get_pointers(const lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pointers;
}

lh_gl_view_pointer_t *
lh_gl_view_get_pointers_mut(lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pointers;
}

const lh_gl_point_t *
lh_gl_view_get_velocity(const lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->velocity);
}

lh_gl_point_t *
lh_gl_view_get_velocity_mut(lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->velocity);
}

lh_void
lh_gl_view_set_velocity(lh_gl_view_t *self, lh_gl_point_t velocity)
{
    lh_assert_runtime_ref(self);
    self->velocity = velocity;
}

lh_gl_container_t *
lh_gl_view_get_scrolling(const lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->scrolling;
}

lh_void
lh_gl_view_set_scrolling(lh_gl_view_t *self, lh_gl_container_t *scrolling)
{
    lh_assert_runtime_ref(self);
    self->scrolling = scrolling;
}

lh_gl_container_t *
lh_gl_view_get_throwing(const lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->throwing;
}

lh_void
lh_gl_view_set_throwing(lh_gl_view_t *self, lh_gl_container_t *throwing)
{
    lh_assert_runtime_ref(self);
    self->throwing = throwing;
}

const lh_gl_rect_t *
lh_gl_view_get_whole(const lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->whole;
}

lh_gl_rect_t *
lh_gl_view_get_whole_mut(lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->whole;
}

lh_u32_t
lh_gl_view_get_whole_count(const lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->whole_count;
}

lh_void
lh_gl_view_set_whole_count(lh_gl_view_t *self, lh_u32_t whole_count)
{
    lh_assert_runtime_ref(self);
    self->whole_count = whole_count;
}

const lh_gl_rects_t *
lh_gl_view_get_pending(const lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pending);
}

lh_gl_rects_t *
lh_gl_view_get_pending_mut(lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pending);
}

lh_void
lh_gl_view_set_pending(lh_gl_view_t *self, lh_gl_rects_t pending)
{
    lh_assert_runtime_ref(self);
    self->pending = pending;
}

const lh_gl_transition_t *
lh_gl_view_get_transitions(const lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->transitions;
}

lh_gl_transition_t *
lh_gl_view_get_transitions_mut(lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->transitions;
}

lh_void
lh_gl_view_set_root(lh_gl_view_t *self, lh_gl_entity_t *root)
{
    lh_assert_runtime_ref(self);
    self->root = root;
}

lh_void
lh_gl_view_set_focus(lh_gl_view_t *self, lh_gl_entity_t *focus)
{
    lh_assert_runtime_ref(self);
    self->focus = focus;
}

lh_input_wheel_lines_fn *
lh_gl_view_get_wheel_lines(const lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wheel_lines;
}

lh_void
lh_gl_view_set_wheel_lines(lh_gl_view_t *self, lh_input_wheel_lines_fn *wheel_lines)
{
    lh_assert_runtime_ref(self);
    self->wheel_lines = wheel_lines;
}

lh_ptr
lh_gl_view_get_wheel_lines_context(const lh_gl_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wheel_lines_context;
}

lh_void
lh_gl_view_set_wheel_lines_context(lh_gl_view_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->wheel_lines_context = context;
}

lh_void
lh_gl_view_init(lh_gl_view_t *self)
{
    lh_gl_view_set_canvas(self, lh_null);
    lh_gl_view_set_root(self, lh_null);
    lh_gl_view_set_clear(self, lh_null);
    lh_gl_view_set_strip_height(self, lh_gl_scalar(0));
    lh_gl_view_set_scrolling(self, lh_null);
    lh_gl_view_set_throwing(self, lh_null);
    lh_gl_view_set_focus(self, lh_null);
    lh_gl_view_set_whole_count(self, 0);
    lh_gl_view_install_wheel_lines(self, lh_null, lh_null);
    lh_gl_rects_init(lh_gl_view_get_pending_mut(self));
    lh_gl_point_init(lh_gl_view_get_velocity_mut(self), lh_gl_scalar(0), lh_gl_scalar(0));
    for (lh_input_pointer_id_t id = 0U; id < LH_INPUT_POINTER_MAX; ++id)
    {
        lh_gl_view_pointer_init(lh_addr_of(lh_gl_view_get_pointers_mut(self)[id]));
    }
    for (int i = 0; i < LH_GL_VIEW_TRANSITION_MAX; ++i)
    {
        lh_gl_transition_init(lh_addr_of(lh_gl_view_get_transitions_mut(self)[i]));
    }
}

lh_void
lh_gl_view_set_root_v(lh_gl_view_t *self, lh_gl_entity_t *root)
{
    lh_gl_entity_t *top;

    /* The tree that leaves takes its looks with it, landed: its entities must not
       point into a view that no longer holds them. */
    if (lh_gl_view_get_root(self) != root)
    {
        lh_gl_view_finish_transitions(self, lh_null);
    }
    /* The pointer lives on the top of the tree, which is where a setter walks
       to find this view. A root that is itself parented still records on the
       top, or the walk would stop one short of it. */
    if (lh_null_ne(lh_gl_view_get_root(self)) &&
        lh_gl_entity_get_view(lh_gl_view_get_root(self)) == self)
    {
        lh_gl_entity_set_view(lh_gl_view_get_root(self), lh_null);
    }
    lh_gl_view_set_root(self, root);
    lh_return_if(lh_null_eq(root));
    top = root;
    while (lh_null_ne(lh_gl_entity_get_parent(top)))
    {
        top = lh_gl_entity_get_parent(top);
    }
    lh_gl_entity_set_view(top, self);
}

lh_void
lh_gl_view_deinit(lh_gl_view_t *self)
{
    /* Every entity in flight points into this view's slots: they land first. */
    lh_gl_view_finish_transitions(self, lh_null);
    lh_gl_view_init(self);
}
