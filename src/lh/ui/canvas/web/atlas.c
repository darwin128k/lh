/**
 * @file atlas.c
 * @brief Implementation of `lh/ui/canvas/web/atlas.h`.
 */

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
        const lh_sint_t id = self->slot[s] - 1;

        if (id < 0)
        {
            lh_ptr_deref(slot) = s;
            return -1;
        }
        if (self->hash[id] == hash && self->w[id] == w && self->h[id] == h && self->len[id] == n &&
            lh_memory_compare(self->px[id], n, lh_cast_const(lh_ptr, px), n) == lh_null)
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
    const lh_uint_t id = self->n;

    if (id >= LH_UI_CANVAS_WEB_GLYPHS_MAX)
    {
        return -1;
    }
    self->px[id] = lh_ptr_rcast(lh_byte_t, lh_runtime_allocator_alloc(n));
    if (self->px[id] == lh_null)
    {
        return -1;
    }
    lh_memory_copy(self->px[id], n, lh_cast_const(lh_ptr, px), n);
    self->w[id] = w;
    self->h[id] = h;
    self->len[id] = n;
    self->hash[id] = hash;
    self->slot[slot] = lh_cast_static(lh_sint_t, id) + 1;
    ++self->n;
    return lh_cast_static(lh_sint_t, id);
}
