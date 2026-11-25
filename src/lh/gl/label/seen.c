/**
 * @file seen.c
 * @brief Implementation of `lh/gl/label/seen.h`.
 */

#include <lh/gl/label/seen.h>

#include <lh/core/assert.h>

const lh_gl_rect_t *
lh_gl_label_seen_get_noted(const lh_gl_label_seen_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->noted);
}

lh_gl_rect_t *
lh_gl_label_seen_get_noted_mut(lh_gl_label_seen_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->noted);
}

lh_void
lh_gl_label_seen_set_noted(lh_gl_label_seen_t *self, lh_gl_rect_t noted)
{
    lh_assert_runtime_ref(self);
    self->noted = noted;
}

lh_u32_t
lh_gl_label_seen_get_hash(const lh_gl_label_seen_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hash;
}

lh_void
lh_gl_label_seen_set_hash(lh_gl_label_seen_t *self, lh_u32_t hash)
{
    lh_assert_runtime_ref(self);
    self->hash = hash;
}

lh_u32_t
lh_gl_label_seen_get_length(const lh_gl_label_seen_t *self)
{
    lh_assert_runtime_ref(self);
    return self->length;
}

lh_void
lh_gl_label_seen_set_length(lh_gl_label_seen_t *self, lh_u32_t length)
{
    lh_assert_runtime_ref(self);
    self->length = length;
}

lh_bool_t
lh_gl_label_seen_is_hashed(const lh_gl_label_seen_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hashed;
}

lh_void
lh_gl_label_seen_set_hashed(lh_gl_label_seen_t *self, lh_bool_t hashed)
{
    lh_assert_runtime_ref(self);
    self->hashed = hashed;
}
