/**
 * @file font.c
 * @brief Implementation of `lh/ui/font.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/config.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/font.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

#if LH_LIBRARY_OPTION_UI_FONT_ROBOTO
#    include <lh/ui/font/roboto.h>
#endif

const lh_ui_font_t *
lh_ui_font_get_default(lh_void)
{
#if LH_LIBRARY_OPTION_UI_FONT_ROBOTO
    return lh_addr_of(lh_ui_font_roboto);
#else
    return lh_null;
#endif
}

lh_void
lh_ui_font_init(lh_ui_font_t *self, const lh_ui_mask_t *glyphs, const lh_byte_t *advances,
                const lh_s32_t *tops, lh_s32_t line_height, lh_s32_t ascent, lh_byte_t first,
                lh_u32_t count)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(glyphs);
    lh_assert_runtime_ref(advances);
    lh_assert_runtime_ref(tops);
    lh_assert_runtime_ifn(count > 0U && first + count <= 256U && line_height >= 0 && ascent >= 0 &&
                              ascent <= line_height,
                          lh_runtime_error_code_invalid_argument);
    self->glyphs = glyphs;
    self->advances = advances;
    self->tops = tops;
    self->line_height = line_height;
    self->ascent = ascent;
    self->first = first;
    self->count = count;
}

lh_s32_t
lh_ui_font_get_ascent(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ascent;
}

lh_s32_t
lh_ui_font_get_line_height(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->line_height;
}

lh_u32_t
lh_ui_font_get_first(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->first;
}

lh_u32_t
lh_ui_font_get_count(const lh_ui_font_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_bool_t
lh_ui_font_has_code(const lh_ui_font_t *self, lh_u32_t code)
{
    return code >= lh_ui_font_get_first(self) && code - lh_ui_font_get_first(self) < lh_ui_font_get_count(self)
               ? lh_bool_true
               : lh_bool_false;
}

lh_u32_t
lh_ui_font_get_index(const lh_ui_font_t *self, lh_u32_t code)
{
    lh_assert_runtime_ifn(lh_ui_font_has_code(self, code), lh_runtime_error_code_out_of_range);
    return code - lh_ui_font_get_first(self);
}

lh_s32_t
lh_ui_font_get_advance(const lh_ui_font_t *self, lh_u32_t code)
{
    lh_return_if(!lh_ui_font_has_code(self, code), 0);
    return self->advances[lh_ui_font_get_index(self, code)];
}

lh_s32_t
lh_ui_font_get_top(const lh_ui_font_t *self, lh_u32_t code)
{
    lh_return_if(!lh_ui_font_has_code(self, code), 0);
    return self->tops[lh_ui_font_get_index(self, code)];
}

lh_bool_t
lh_ui_font_get_glyph(const lh_ui_font_t *self, lh_u32_t code, lh_ui_mask_t *mask)
{
    lh_return_if(!lh_ui_font_has_code(self, code), lh_bool_false);
    lh_assert_runtime_ref(mask);
    *mask = self->glyphs[lh_ui_font_get_index(self, code)];
    return lh_bool_true;
}
