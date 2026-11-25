/**
 * @file canvas.c
 * @brief Implementation of `lh/gl/canvas.h`: the canvas itself: its fields (level 1), init, deinit,
 * the backend, the size, the scratch and the null backend.
 */

#include <lh/gl/canvas.h>

#include <lh/core/assert.h>

const lh_gl_canvas_backend_t lh_gl_canvas_backend_null = {lh_null, lh_null, lh_null, lh_null,
                                                          lh_null, lh_null, lh_null, lh_null,
                                                          lh_null, lh_null, lh_null, lh_null};

lh_u8_t *
lh_gl_canvas_get_scratch(const lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->scratch;
}

lh_usize_t
lh_gl_canvas_get_scratch_bytes(const lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->scratch_bytes;
}

lh_void
lh_gl_canvas_set_backend(lh_gl_canvas_t *self, const lh_gl_canvas_backend_t *backend)
{
    lh_assert_runtime_ref(self);
    self->backend = backend;
}

const lh_gl_canvas_backend_t *
lh_gl_canvas_get_backend(const lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->backend;
}

lh_void
lh_gl_canvas_set_context(lh_gl_canvas_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

lh_ptr
lh_gl_canvas_get_context(const lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_void
lh_gl_canvas_set_size(lh_gl_canvas_t *self, lh_gl_size_t size)
{
    lh_assert_runtime_ref(self);
    self->size = size;
}

const lh_gl_canvas_state_t *
lh_gl_canvas_get_state(const lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->state);
}

lh_gl_canvas_state_t *
lh_gl_canvas_get_state_mut(lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->state);
}

lh_void
lh_gl_canvas_set_state(lh_gl_canvas_t *self, lh_gl_canvas_state_t state)
{
    lh_assert_runtime_ref(self);
    self->state = state;
}

const lh_gl_canvas_state_t *
lh_gl_canvas_get_saved(const lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->saved;
}

lh_gl_canvas_state_t *
lh_gl_canvas_get_saved_mut(lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->saved;
}

lh_u8_t
lh_gl_canvas_get_depth(const lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->depth;
}

lh_void
lh_gl_canvas_set_depth(lh_gl_canvas_t *self, lh_u8_t depth)
{
    lh_assert_runtime_ref(self);
    self->depth = depth;
}

lh_gl_size_t *
lh_gl_canvas_get_size_mut(lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->size);
}

const lh_gl_point_t *
lh_gl_canvas_get_frame_at(const lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->frame_at);
}

lh_gl_point_t *
lh_gl_canvas_get_frame_at_mut(lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->frame_at);
}

lh_void
lh_gl_canvas_set_frame_at(lh_gl_canvas_t *self, lh_gl_point_t frame_at)
{
    lh_assert_runtime_ref(self);
    self->frame_at = frame_at;
}

lh_gl_rect_t *
lh_gl_canvas_get_damage_mut(lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->damage);
}

lh_void
lh_gl_canvas_set_damage(lh_gl_canvas_t *self, lh_gl_rect_t damage)
{
    lh_assert_runtime_ref(self);
    self->damage = damage;
}

lh_bool_t
lh_gl_canvas_is_has_damage(const lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->has_damage;
}

lh_void
lh_gl_canvas_set_has_damage(lh_gl_canvas_t *self, lh_bool_t has_damage)
{
    lh_assert_runtime_ref(self);
    self->has_damage = has_damage;
}

const lh_gl_canvas_clip_round_t *
lh_gl_canvas_get_rounds(const lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rounds;
}

lh_gl_canvas_clip_round_t *
lh_gl_canvas_get_rounds_mut(lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rounds;
}

const lh_gl_rects_t *
lh_gl_canvas_get_drawn(const lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->drawn);
}

lh_gl_rects_t *
lh_gl_canvas_get_drawn_mut(lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->drawn);
}

lh_void
lh_gl_canvas_set_drawn(lh_gl_canvas_t *self, lh_gl_rects_t drawn)
{
    lh_assert_runtime_ref(self);
    self->drawn = drawn;
}

lh_void
lh_gl_canvas_set_scratch_bytes(lh_gl_canvas_t *self, lh_usize_t scratch_bytes)
{
    lh_assert_runtime_ref(self);
    self->scratch_bytes = scratch_bytes;
}

const lh_gl_size_t *
lh_gl_canvas_get_size(const lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->size);
}

lh_void
lh_gl_canvas_set_scratch(lh_gl_canvas_t *self, lh_u8_t *scratch)
{
    lh_assert_runtime_ref(self);
    self->scratch = scratch;
}

const lh_gl_rect_t *
lh_gl_canvas_get_damage(const lh_gl_canvas_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->damage);
}

lh_void
lh_gl_canvas_init(lh_gl_canvas_t *self, const lh_gl_canvas_backend_t *backend, lh_ptr context)
{
    lh_gl_canvas_set_backend(self, backend);
    lh_gl_canvas_set_context(self, context);
    lh_gl_canvas_state_init(lh_gl_canvas_get_state_mut(self));
    lh_gl_canvas_set_depth(self, 0U);
    lh_gl_size_init(lh_gl_canvas_get_size_mut(self), lh_gl_scalar(0), lh_gl_scalar(0));
    lh_gl_point_init(lh_gl_canvas_get_frame_at_mut(self), lh_gl_scalar(0), lh_gl_scalar(0));
    lh_gl_rect_init_empty(lh_gl_canvas_get_damage_mut(self));
    lh_gl_canvas_set_has_damage(self, lh_bool_false);
    lh_gl_rects_init(lh_gl_canvas_get_drawn_mut(self));
    lh_gl_canvas_set_scratch(self, lh_null);
    lh_gl_canvas_set_scratch_bytes(self, 0U);
}

lh_void
lh_gl_canvas_use_scratch(lh_gl_canvas_t *self, lh_u8_t *scratch, lh_usize_t bytes)
{
    lh_gl_canvas_set_scratch(self, scratch);
    lh_gl_canvas_set_scratch_bytes(self, lh_null_eq(scratch) ? 0U : bytes);
}

lh_void
lh_gl_canvas_copy_size(const lh_gl_canvas_t *self, lh_gl_size_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_ptr_deref(lh_gl_canvas_get_size(self));
}

lh_void
lh_gl_canvas_deinit(lh_gl_canvas_t *self)
{
    lh_gl_canvas_init(self, lh_null, lh_null);
}
