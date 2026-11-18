/**
 * @file font.h
 * @brief Where a text takes its glyphs from: ::lh_gx_font_t.
 *
 * A font is a contract: a code point in, a glyph out (::lh_gx_glyph_t) — and the
 * two metrics a line needs, the ascent and the line height. Who fills it is not
 * gx's business: a baked bitmap font (`lh/gx/font/baked.h`, over the core's
 * ::lh_font_t, the way Roboto is shipped), a rasterizer of TTF files on a
 * desktop, a host's own. A style points at one font (::lh_gx_style_get_font),
 * and the text class asks it for each character it draws.
 *
 * The font carries its own data beside its slots (`context`), so one pointer is
 * the whole font. All three slots are required.
 *
 * Ownership: the application's; neither the font nor its data is owned by gx.
 */

#ifndef LH_GX_FONT_H
#define LH_GX_FONT_H

#include <lh/gx/font/fields.h>
#include <lh/gx/font/get_ascent/fn.h>
#include <lh/gx/font/get_glyph/fn.h>
#include <lh/gx/font/get_line_height/fn.h>

/**
 * @struct lh_gx_font
 * @brief The slots of a font and its own data.
 */
struct lh_gx_font
{
    /** Members: ::lh_gx_font_fields. */
    lh_gx_font_fields(lh_gx_font_get_glyph_fn, lh_gx_font_get_ascent_fn,
                      lh_gx_font_get_line_height_fn);
};
/** @copybrief lh_gx_font */
typedef struct lh_gx_font lh_gx_font_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The `get_glyph` slot of @p self (level 1).
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 */
lh_gx_font_get_glyph_fn *
lh_gx_font_get_get_glyph_fn(const lh_gx_font_t *self);

/**
 * @brief Store the `get_glyph` slot of @p self (level 1).
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 * @param fn   The call.
 */
lh_void
lh_gx_font_set_get_glyph_fn(lh_gx_font_t *self, lh_gx_font_get_glyph_fn *fn);

/**
 * @brief The `get_ascent` slot of @p self (level 1).
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 */
lh_gx_font_get_ascent_fn *
lh_gx_font_get_get_ascent_fn(const lh_gx_font_t *self);

/**
 * @brief Store the `get_ascent` slot of @p self (level 1).
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 * @param fn   The call.
 */
lh_void
lh_gx_font_set_get_ascent_fn(lh_gx_font_t *self, lh_gx_font_get_ascent_fn *fn);

/**
 * @brief The `get_line_height` slot of @p self (level 1).
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 */
lh_gx_font_get_line_height_fn *
lh_gx_font_get_get_line_height_fn(const lh_gx_font_t *self);

/**
 * @brief Store the `get_line_height` slot of @p self (level 1).
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 * @param fn   The call.
 */
lh_void
lh_gx_font_set_get_line_height_fn(lh_gx_font_t *self, lh_gx_font_get_line_height_fn *fn);

/**
 * @brief The font's own data, every slot is called with (level 1).
 *
 * @param self The font; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_gx_font_get_context(const lh_gx_font_t *self);

/**
 * @brief Store the font's own data (level 1).
 *
 * @param self    The font; checked by ::lh_assert_runtime_ref.
 * @param context The data; not owned.
 */
lh_void
lh_gx_font_set_context(lh_gx_font_t *self, lh_ptr context);

/**
 * @brief The glyph of @p code, into @p out, through the `get_glyph` slot.
 *
 * @param self The font; must not be null.
 * @param code The code point.
 * @param out  Receives the glyph; must not be null.
 * @return True when the font has a glyph for @p code.
 */
lh_bool_t
lh_gx_font_get_glyph(const lh_gx_font_t *self, lh_u32_t code, lh_gx_glyph_t *out);

/**
 * @brief How far the baseline is below the top of a line, through `get_ascent`.
 *
 * @param self The font; must not be null.
 * @return Pixels.
 */
lh_s32_t
lh_gx_font_get_ascent(const lh_gx_font_t *self);

/**
 * @brief The height of one line, through `get_line_height`.
 *
 * @param self The font; must not be null.
 * @return Pixels.
 */
lh_s32_t
lh_gx_font_get_line_height(const lh_gx_font_t *self);

/**
 * @brief A font of the three slots and @p context.
 *
 * @param self            The font; must not be null.
 * @param get_glyph       The `get_glyph` slot.
 * @param get_ascent      The `get_ascent` slot.
 * @param get_line_height The `get_line_height` slot.
 * @param context         The font's own data.
 */
lh_void
lh_gx_font_init(lh_gx_font_t *self, lh_gx_font_get_glyph_fn *get_glyph,
                lh_gx_font_get_ascent_fn *get_ascent,
                lh_gx_font_get_line_height_fn *get_line_height, lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_FONT_H */
