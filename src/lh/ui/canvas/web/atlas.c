/**
 * @file atlas.c
 * @brief Implementation of `lh/ui/canvas/web/atlas.h`.
 */
#include <lh/assert.h>

#include <lh/ui/canvas/web/atlas.h>

#include <lh/cast/const.h>
#include <lh/memory.h>
#include <lh/runtime/allocator.h>

lh_sint_t
lh_ui_canvas_web_atlas_find(const lh_ui_canvas_web_atlas_t *self, lh_byte_cptr px, lh_uint_t n,
                            lh_s32_t w, lh_s32_t h, lh_u32_t hash, lh_uint_t *slot)
{
    lh_uint_t s = hash & (LH_UI_CANVAS_WEB_ATLAS_SLOTS - 1u);

    for (;;)
    {
        const lh_sint_t id = lh_ui_canvas_web_atlas_get_slot(self)[s] - 1;

        if (id < 0)
        {
            lh_ptr_deref(slot) = s;
            return -1;
        }
        if (lh_ui_canvas_web_atlas_get_hash(self)[id] == hash && lh_ui_canvas_web_atlas_get_w(self)[id] == w && lh_ui_canvas_web_atlas_get_h(self)[id] == h && lh_ui_canvas_web_atlas_get_len(self)[id] == n &&
            lh_memory_compare(lh_ui_canvas_web_atlas_get_px(self)[id], n, lh_cast_const(lh_ptr, px), n) == lh_null)
        {
            return id;
        }
        s = (s + 1u) & (LH_UI_CANVAS_WEB_ATLAS_SLOTS - 1u);
    }
}

lh_sint_t
lh_ui_canvas_web_atlas_add(lh_ui_canvas_web_atlas_t *self, lh_byte_cptr px, lh_uint_t n, lh_s32_t w,
                           lh_s32_t h, lh_u32_t hash, lh_uint_t slot)
{
    const lh_uint_t id = lh_ui_canvas_web_atlas_get_n(self);

    if (id >= LH_UI_CANVAS_WEB_GLYPHS_MAX)
    {
        return -1;
    }
    lh_ui_canvas_web_atlas_get_px_mut(self)[id] = lh_ptr_rcast(lh_byte_t, lh_runtime_allocator_alloc(n));
    if (lh_ui_canvas_web_atlas_get_px(self)[id] == lh_null)
    {
        return -1;
    }
    lh_memory_copy(lh_ui_canvas_web_atlas_get_px(self)[id], n, lh_cast_const(lh_ptr, px), n);
    lh_ui_canvas_web_atlas_get_w_mut(self)[id] = w;
    lh_ui_canvas_web_atlas_get_h_mut(self)[id] = h;
    lh_ui_canvas_web_atlas_get_len_mut(self)[id] = n;
    lh_ui_canvas_web_atlas_get_hash_mut(self)[id] = hash;
    lh_ui_canvas_web_atlas_get_slot_mut(self)[slot] = lh_cast_static(lh_sint_t, id) + 1;
    lh_ui_canvas_web_atlas_set_n(self, lh_ui_canvas_web_atlas_get_n(self) + 1);
    return lh_cast_static(lh_sint_t, id);
}

lh_uint_t
lh_ui_canvas_web_atlas_get_n(const lh_ui_canvas_web_atlas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->n;
}

lh_void
lh_ui_canvas_web_atlas_set_n(lh_ui_canvas_web_atlas_t *self, lh_uint_t n)
{
    lh_assert_runtime_ref(self);
    self->n = n;
}

const lh_s32_t *
lh_ui_canvas_web_atlas_get_w(const lh_ui_canvas_web_atlas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->w;
}

lh_s32_t *
lh_ui_canvas_web_atlas_get_w_mut(lh_ui_canvas_web_atlas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->w;
}

const lh_s32_t *
lh_ui_canvas_web_atlas_get_h(const lh_ui_canvas_web_atlas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->h;
}

lh_s32_t *
lh_ui_canvas_web_atlas_get_h_mut(lh_ui_canvas_web_atlas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->h;
}

const lh_uint_t *
lh_ui_canvas_web_atlas_get_len(const lh_ui_canvas_web_atlas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->len;
}

lh_uint_t *
lh_ui_canvas_web_atlas_get_len_mut(lh_ui_canvas_web_atlas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->len;
}

const lh_u32_t *
lh_ui_canvas_web_atlas_get_hash(const lh_ui_canvas_web_atlas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hash;
}

lh_u32_t *
lh_ui_canvas_web_atlas_get_hash_mut(lh_ui_canvas_web_atlas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hash;
}

lh_byte_ptr const *
lh_ui_canvas_web_atlas_get_px(const lh_ui_canvas_web_atlas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->px;
}

lh_byte_ptr *
lh_ui_canvas_web_atlas_get_px_mut(lh_ui_canvas_web_atlas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->px;
}

const lh_sint_t *
lh_ui_canvas_web_atlas_get_slot(const lh_ui_canvas_web_atlas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->slot;
}

lh_sint_t *
lh_ui_canvas_web_atlas_get_slot_mut(lh_ui_canvas_web_atlas_t *self)
{
    lh_assert_runtime_ref(self);
    return self->slot;
}
