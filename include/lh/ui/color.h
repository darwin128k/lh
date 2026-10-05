/**
 * @file color.h
 * @brief One pixel: ::lh_ui_color_t, 8-bit RGBA, straight alpha.
 *
 * The canvas paints this. Whoever shows the buffer converts it to the
 * device format. Requires nothing from `lh/os`.
 */

#ifndef LH_UI_COLOR_H
#define LH_UI_COLOR_H

#include <lh/byte.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/ui/color/fields.h>
#include <lh/void.h>

/**
 * @struct lh_ui_color
 * @typedef lh_ui_color_t
 * @brief 8-bit RGBA. Alpha at the top of the byte is opaque.
 */
struct lh_ui_color
{
    lh_ui_color_fields(lh_byte_t);
};
typedef struct lh_ui_color lh_ui_color_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self from channels in `0..255`.
 */
lh_void
lh_ui_color_init(lh_ui_color_t *self, lh_byte_t r, lh_byte_t g, lh_byte_t b, lh_byte_t a);

/**
 * @brief Red of @p self.
 */
lh_byte_t
lh_ui_color_get_r(const lh_ui_color_t *self);

/**
 * @brief Green of @p self.
 */
lh_byte_t
lh_ui_color_get_g(const lh_ui_color_t *self);

/**
 * @brief Blue of @p self.
 */
lh_byte_t
lh_ui_color_get_b(const lh_ui_color_t *self);

/**
 * @brief Alpha of @p self.
 */
lh_byte_t
lh_ui_color_get_a(const lh_ui_color_t *self);

/**
 * @brief Paint @p color over @p dst. Straight alpha.
 */
lh_ui_color_t
lh_ui_color_over(const lh_ui_color_t *dst, const lh_ui_color_t *color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_COLOR_H */
