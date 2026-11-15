/**
 * @file caption.c
 * @brief Static 8×12 glyphs for ::lh_gl_font_caption.
 */

#include <lh/gl/font/caption.h>

#include <lh/core/bool.h>
#include <lh/core/char/map.h>
#include <lh/core/index.h>
#include <lh/core/util/ptr.h>

enum
{
    LH_GL_FONT_CAPTION_WIDTH = 8,
    LH_GL_FONT_CAPTION_HEIGHT = 12,
    LH_GL_FONT_CAPTION_ADVANCE = 8,
    LH_GL_FONT_CAPTION_ASCENT = 12,
    LH_GL_FONT_CAPTION_LINE = 14,
    LH_GL_FONT_CAPTION_FIRST = lh_char_map_space,
    LH_GL_FONT_CAPTION_LAST = lh_char_map_small_r,
    LH_GL_FONT_CAPTION_COUNT = LH_GL_FONT_CAPTION_LAST - LH_GL_FONT_CAPTION_FIRST + 1,
    LH_GL_FONT_CAPTION_INK = 14
};

static lh_byte_t lh_gl_font_caption_bits[LH_GL_FONT_CAPTION_INK][LH_GL_FONT_CAPTION_HEIGHT *
                                                                 LH_GL_FONT_CAPTION_WIDTH];
static lh_gl_glyph_t lh_gl_font_caption_glyphs[LH_GL_FONT_CAPTION_COUNT];
static lh_gl_font_t lh_gl_font_caption_font;
static lh_bool_t lh_gl_font_caption_ready = lh_bool_false;

static const lh_byte_t lh_gl_font_caption_digit[][LH_GL_FONT_CAPTION_HEIGHT] = {
    {0x3C, 0x42, 0x46, 0x4A, 0x52, 0x62, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00},
    {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00},
    {0x3C, 0x42, 0x02, 0x0C, 0x10, 0x20, 0x40, 0x7E, 0x00, 0x00, 0x00, 0x00},
    {0x3C, 0x42, 0x02, 0x1C, 0x02, 0x02, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00},
    {0x04, 0x0C, 0x14, 0x24, 0x44, 0x7E, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00},
    {0x7E, 0x40, 0x7C, 0x02, 0x02, 0x02, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00},
    {0x1C, 0x20, 0x40, 0x7C, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00},
    {0x7E, 0x02, 0x04, 0x08, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00},
    {0x3C, 0x42, 0x42, 0x3C, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00},
    {0x3C, 0x42, 0x42, 0x3E, 0x02, 0x02, 0x04, 0x38, 0x00, 0x00, 0x00, 0x00},
};

static const lh_byte_t lh_gl_font_caption_letter_c[LH_GL_FONT_CAPTION_HEIGHT] = {
    0x3C, 0x42, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x3C, 0x00, 0x00};
static const lh_byte_t lh_gl_font_caption_letter_a[LH_GL_FONT_CAPTION_HEIGHT] = {
    0x00, 0x00, 0x3C, 0x02, 0x3E, 0x42, 0x42, 0x3E, 0x00, 0x00, 0x00, 0x00};
static const lh_byte_t lh_gl_font_caption_letter_d[LH_GL_FONT_CAPTION_HEIGHT] = {
    0x02, 0x02, 0x3A, 0x46, 0x42, 0x42, 0x46, 0x3A, 0x00, 0x00, 0x00, 0x00};
static const lh_byte_t lh_gl_font_caption_letter_r[LH_GL_FONT_CAPTION_HEIGHT] = {
    0x00, 0x00, 0x5C, 0x62, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00};

const lh_gl_font_t *
lh_gl_font_caption(void)
{
    if (lh_gl_font_caption_ready == lh_bool_true)
    {
        return lh_addr_of(lh_gl_font_caption_font);
    }
    lh_uindex_t ink = 0U;

    for (lh_u32_t code = LH_GL_FONT_CAPTION_FIRST; code <= LH_GL_FONT_CAPTION_LAST; ++code)
    {
        const lh_byte_t *rows = lh_null;

        if (code >= lh_char_map_digit_0 && code <= lh_char_map_digit_9)
        {
            rows = lh_gl_font_caption_digit[code - lh_char_map_digit_0];
        }
        else if (code == lh_char_map_capital_c)
        {
            rows = lh_gl_font_caption_letter_c;
        }
        else if (code == lh_char_map_small_a)
        {
            rows = lh_gl_font_caption_letter_a;
        }
        else if (code == lh_char_map_small_d)
        {
            rows = lh_gl_font_caption_letter_d;
        }
        else if (code == lh_char_map_small_r)
        {
            rows = lh_gl_font_caption_letter_r;
        }
        lh_uindex_t index = code - LH_GL_FONT_CAPTION_FIRST;

        if (lh_ptr_is_null(rows))
        {
            lh_gl_scalar_t advance = code == lh_char_map_space ? LH_GL_FONT_CAPTION_ADVANCE : 0;

            lh_gl_glyph_init(lh_addr_of(lh_gl_font_caption_glyphs[index]), 0, 0, advance, 0,
                             lh_null);
            continue;
        }
        lh_byte_t *dest = lh_gl_font_caption_bits[ink];

        for (lh_uindex_t row = 0U; row < LH_GL_FONT_CAPTION_HEIGHT; ++row)
        {
            lh_byte_t mask = rows[row];

            for (lh_uindex_t col = 0U; col < LH_GL_FONT_CAPTION_WIDTH; ++col)
            {
                lh_byte_t bit = lh_cast_static(lh_byte_t, 0x80U >> col);

                dest[row * LH_GL_FONT_CAPTION_WIDTH + col] = (mask & bit) == 0 ? 0 : 255;
            }
        }
        lh_gl_glyph_init(lh_addr_of(lh_gl_font_caption_glyphs[index]), LH_GL_FONT_CAPTION_WIDTH,
                         LH_GL_FONT_CAPTION_HEIGHT, LH_GL_FONT_CAPTION_ADVANCE,
                         -LH_GL_FONT_CAPTION_ASCENT, dest);
        ++ink;
    }
    lh_gl_font_init(lh_addr_of(lh_gl_font_caption_font), lh_gl_font_caption_glyphs,
                    LH_GL_FONT_CAPTION_FIRST, LH_GL_FONT_CAPTION_COUNT, LH_GL_FONT_CAPTION_LINE,
                    LH_GL_FONT_CAPTION_ASCENT);
    lh_gl_font_caption_ready = lh_bool_true;
    return lh_addr_of(lh_gl_font_caption_font);
}
