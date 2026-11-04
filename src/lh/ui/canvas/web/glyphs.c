#include <lh/ui/canvas/web/glyphs.h>
#include <lh/ui/canvas/web.h>
#include <lh/core/cast/const.h>
#include <lh/core/hash/fnv.h>
#include <lh/core/math.h>
#include <lh/core/memory.h>
#include <lh/core/runtime/allocator.h>
#include <lh/core/str/format/bytes.h>
#include <lh/core/str/format/sint.h>
#include <lh/core/assert.h>

lh_void
lh_ui_canvas_web_glyphs_reset(lh_ui_canvas_web_glyphs_t *self)
{
    if (self != lh_null)
    {
        lh_memory_set(lh_ui_canvas_web_glyphs_get_sent_mut(self), sizeof(self->sent), 0);
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
