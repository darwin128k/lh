/**
 * @file style.h
 * @brief How an entity looks: ::lh_gx_style_t.
 *
 * A style is flat data — colours and lengths, no pointers, no chain — that a
 * class's `paint` reads. Entities point at one (::lh_gx_entity_set_style) and
 * share it: a theme is the application's array of styles, usually constants in
 * flash made with ::lh_gx_style_initializer, so fifty buttons are one record,
 * not fifty copies of it, and changing the theme is changing one array.
 *
 * Ownership: the application's; an entity only points at its style.
 */

#ifndef LH_GX_STYLE_H
#define LH_GX_STYLE_H

#include <lh/gx/color.h>
#include <lh/gx/font.h>
#include <lh/gx/scalar.h>
#include <lh/gx/style/fields.h>

/**
 * @struct lh_gx_style
 * @brief The fill, the border, the corners, the ink and the font of an entity.
 */
struct lh_gx_style
{
    /** Members: ::lh_gx_style_fields. */
    lh_gx_style_fields(lh_gx_color_t, lh_gx_scalar_t, lh_gx_font_t);
};
/** @copybrief lh_gx_style */
typedef struct lh_gx_style lh_gx_style_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The colour inside (level 1).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 */
lh_gx_color_t
lh_gx_style_get_fill(const lh_gx_style_t *self);

/**
 * @brief Store the colour inside (level 1).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @param fill `0xAARRGGBB`.
 */
lh_void
lh_gx_style_set_fill(lh_gx_style_t *self, lh_gx_color_t fill);

/**
 * @brief The colour of the edge (level 1).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 */
lh_gx_color_t
lh_gx_style_get_border(const lh_gx_style_t *self);

/**
 * @brief Store the colour of the edge (level 1).
 *
 * @param self   The style; checked by ::lh_assert_runtime_ref.
 * @param border `0xAARRGGBB`.
 */
lh_void
lh_gx_style_set_border(lh_gx_style_t *self, lh_gx_color_t border);

/**
 * @brief How wide the edge is; `0` for none (level 1).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 */
lh_gx_scalar_t
lh_gx_style_get_border_width(const lh_gx_style_t *self);

/**
 * @brief Store how wide the edge is (level 1).
 *
 * @param self         The style; checked by ::lh_assert_runtime_ref.
 * @param border_width The width; `0` for none.
 */
lh_void
lh_gx_style_set_border_width(lh_gx_style_t *self, lh_gx_scalar_t border_width);

/**
 * @brief How round the corners are; `0` for square (level 1).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 */
lh_gx_scalar_t
lh_gx_style_get_radius(const lh_gx_style_t *self);

/**
 * @brief Store how round the corners are (level 1).
 *
 * @param self   The style; checked by ::lh_assert_runtime_ref.
 * @param radius The radius; `0` for square.
 */
lh_void
lh_gx_style_set_radius(lh_gx_style_t *self, lh_gx_scalar_t radius);

/**
 * @brief The colour a text is drawn in (level 1).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 */
lh_gx_color_t
lh_gx_style_get_ink(const lh_gx_style_t *self);

/**
 * @brief Store the colour a text is drawn in (level 1).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @param ink  `0xAARRGGBB`.
 */
lh_void
lh_gx_style_set_ink(lh_gx_style_t *self, lh_gx_color_t ink);

/**
 * @brief Where a text takes its glyphs from, or ::lh_null (level 1).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_font_t *
lh_gx_style_get_font(const lh_gx_style_t *self);

/**
 * @brief Store where a text takes its glyphs from (level 1).
 *
 * @param self The style; checked by ::lh_assert_runtime_ref.
 * @param font The font; not owned.
 */
lh_void
lh_gx_style_set_font(lh_gx_style_t *self, const lh_gx_font_t *font);

/**
 * @brief A style of @p fill, a @p border of @p border_width, corners of
 *        @p radius, text in @p ink and @p font — for a style built at run time; a theme in flash
 *        uses ::lh_gx_style_initializer.
 *
 * @param self         The style; must not be null.
 * @param fill         The colour inside.
 * @param border       The colour of the edge.
 * @param border_width How wide the edge is; `0` for none.
 * @param radius       How round the corners are; `0` for square.
 * @param ink          The colour of a text.
 * @param font         Where a text takes its glyphs; ::lh_null for none.
 */
lh_void
lh_gx_style_init(lh_gx_style_t *self, lh_gx_color_t fill, lh_gx_color_t border,
                 lh_gx_scalar_t border_width, lh_gx_scalar_t radius, lh_gx_color_t ink,
                 const lh_gx_font_t *font);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_STYLE_H */
