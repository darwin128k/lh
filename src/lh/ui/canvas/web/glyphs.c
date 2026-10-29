#include <lh/ui/canvas/web/glyphs.h>
#include <lh/ui/canvas/web.h>
#include <lh/cast/const.h>
#include <lh/hash/fnv.h>
#include <lh/math.h>
#include <lh/memory.h>
#include <lh/runtime/allocator.h>
#include <lh/str/format/bytes.h>
#include <lh/str/format/sint.h>
#include <lh/assert.h>

lh_void
lh_ui_canvas_web_glyphs_reset(lh_ui_canvas_web_glyphs_t *self)
{
    if (self != lh_null)
    {
        lh_memory_set(lh_ui_canvas_web_glyphs_get_sent(self), sizeof(self->sent), 0);
    }
}

const lh_byte_t *
lh_ui_canvas_web_glyphs_get_sent(const lh_ui_canvas_web_glyphs_t *self)
{
    lh_assert_runtime_ref(self);
    return self->sent;
}

lh_byte_t *
lh_ui_canvas_web_glyphs_get_sent_mut(lh_ui_canvas_web_glyphs_t *self)
{
    lh_assert_runtime_ref(self);
    return self->sent;
}
