/**
 * @file color.h
 * @brief OS-portable 8-bit RGBA color.
 *
 * A single value type `lh_ui_color_t` with four `lh_byte_t` channels
 * (red, green, blue, alpha), straight (not premultiplied) alpha. Every
 * channel spans the full byte range, `lh_numeric_limit_umin(lh_byte_t)` to
 * `lh_numeric_limit_umax(lh_byte_t)`, so an alpha of the maximum is fully
 * opaque and the minimum is fully transparent.
 *
 * Part of the UI layer, next to ::lh_math_point_t. The `lh/os/system`
 * backends do not use it: each works in its own native color type, and
 * any mapping between the two belongs to the lh layer that needs it.
 *
 * Requires nothing from `lh/os`. Safe in STM/embedded.
 */

#ifndef LH_UI_COLOR_H
#define LH_UI_COLOR_H

#include <lh/byte.h>
#include <lh/ui/color/fields.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @struct lh_ui_color
 * @typedef lh_ui_color_t
 * @brief 8-bit RGBA color, straight alpha. One channel per ::lh_byte_t.
 */
struct lh_ui_color
{
    lh_ui_color_fields(lh_byte_t);
};
typedef struct lh_ui_color lh_ui_color_t;

/**
 * @brief Build a `::lh_ui_color_t` from explicit 0..255 channel values.
 */
lh_ui_color_t
lh_ui_color_make(lh_byte_t r, lh_byte_t g, lh_byte_t b, lh_byte_t a);

/**
 * @brief Build a `::lh_ui_color_t` from a packed `0xAARRGGBB` `lh_uint_t`.
 *        Alpha `0xFF` is fully opaque.
 */
lh_ui_color_t
lh_ui_color_from_argb(lh_uint_t argb);

/**
 * @brief Pack a `::lh_ui_color_t` as `0xAARRGGBB` `lh_uint_t`.
 */
lh_uint_t
lh_ui_color_to_argb(lh_ui_color_t self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_COLOR_H */