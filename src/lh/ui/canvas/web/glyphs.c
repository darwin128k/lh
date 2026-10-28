#include <lh/ui/canvas/web/glyphs.h>
#include <lh/ui/canvas/web.h>
#include <lh/cast/const.h>
#include <lh/hash/fnv.h>
#include <lh/math.h>
#include <lh/memory.h>
#include <lh/runtime/allocator.h>
#include <lh/str/format/bytes.h>
#include <lh/str/format/sint.h>

lh_void
lh_ui_canvas_web_glyphs_reset(lh_ui_canvas_web_glyphs_t *self)
{
    if (self != lh_null)
    {
        lh_memory_set(self->sent, sizeof(self->sent), 0);
    }
}
