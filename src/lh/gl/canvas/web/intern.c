/**
 * @file intern.c
 * @brief Implementation of `lh/gl/canvas/web.h`: the process-wide cover atlas, and interning a
 * cover into it.
 */

#include <lh/gl/canvas/web.h>

#include <lh/core/hash/fnv.h>
#include <lh/core/math.h>

/** Cover identity is process-wide so `m` does not reshuffle when the tree
 * changes. `self->glyphs` is only which ids the reader already holds. */
static lh_gl_canvas_web_atlas_t g_web_atlas;

lh_sint_t
lh_gl_canvas_web_intern(lh_gl_canvas_web_t *self, lh_byte_cptr px, lh_s32_t w, lh_s32_t h,
                        lh_bool_t *is_new)
{
    const lh_uint_t n = lh_cast_static(lh_uint_t, w) * lh_cast_static(lh_uint_t, h);
    /* The size seeds the hash: two covers of the same bytes in another shape differ. */
    const lh_u32_t seed = lh_bit_xor(
        LH_HASH_FNV1A_32_OFFSET,
        lh_math_add(lh_math_mul(lh_cast_static(lh_u32_t, w), 131U), lh_cast_static(lh_u32_t, h)));
    const lh_u32_t hash = lh_hash_fnv1a_32_update(seed, px, n);
    lh_gl_canvas_web_atlas_t *atlas = lh_addr_of(g_web_atlas);
    lh_gl_canvas_web_glyphs_t *glyphs;
    lh_uint_t slot = 0;

    lh_ptr_deref(is_new) = lh_bool_true;
    lh_sint_t id = lh_gl_canvas_web_atlas_find(atlas, px, n, w, h, hash, lh_addr_of(slot));
    if (id < 0)
    {
        id = lh_gl_canvas_web_atlas_add(atlas, px, n, w, h, hash, slot);
        if (id < 0)
        {
            return -1;
        }
    }
    glyphs = lh_gl_canvas_web_get_glyphs_v(self);
    if (glyphs == lh_null)
    {
        return id;
    }
    if (lh_gl_canvas_web_glyphs_get_sent(glyphs)[id] != 0u)
    {
        lh_ptr_deref(is_new) = lh_bool_false;
    }
    lh_gl_canvas_web_glyphs_get_sent_mut(glyphs)[id] = 1u;
    return id;
}
