/**
 * @file color.h
 * @brief One pixel: ::lh_ui_color_t, 8-bit RGBA, straight alpha.
 *
 * The canvas paints this. Whoever shows the buffer converts it to the
 * device format. Requires nothing from `lh/os`.
 */

#ifndef LH_UI_COLOR_H
#define LH_UI_COLOR_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/ui/color/channels.h>
#include <lh/ui/color/fields.h>
#include <lh/void.h>

/**
 * @struct lh_ui_color
 * @typedef lh_ui_color_t
 * @brief 8-bit RGBA. Alpha at the top of the channel is opaque.
 */
struct lh_ui_color
{
    lh_ui_color_fields(lh_ui_color_channel_t);
};
typedef struct lh_ui_color lh_ui_color_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Constructors ────────────────────────────────────────────────────────── */

/**
 * @brief Fill @p self from channels in `0..255`.
 */
lh_void
lh_ui_color_init(lh_ui_color_t *self, lh_ui_color_channel_t r, lh_ui_color_channel_t g,
                 lh_ui_color_channel_t b, lh_ui_color_channel_t a);

/**
 * @brief Fill @p self from @p hex as `0xRRGGBBAA`.
 */
lh_void
lh_ui_color_init_hex(lh_ui_color_t *self, lh_uint_t hex);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Red of @p self.
 */
lh_ui_color_channel_t
lh_ui_color_get_r(const lh_ui_color_t *self);

/**
 * @brief Green of @p self.
 */
lh_ui_color_channel_t
lh_ui_color_get_g(const lh_ui_color_t *self);

/**
 * @brief Blue of @p self.
 */
lh_ui_color_channel_t
lh_ui_color_get_b(const lh_ui_color_t *self);

/**
 * @brief Alpha of @p self.
 */
lh_ui_color_channel_t
lh_ui_color_get_a(const lh_ui_color_t *self);

/**
 * @brief Set the red channel of @p self.
 */
lh_void
lh_ui_color_set_r(lh_ui_color_t *self, lh_ui_color_channel_t r);

/**
 * @brief Set the green channel of @p self.
 */
lh_void
lh_ui_color_set_g(lh_ui_color_t *self, lh_ui_color_channel_t g);

/**
 * @brief Set the blue channel of @p self.
 */
lh_void
lh_ui_color_set_b(lh_ui_color_t *self, lh_ui_color_channel_t b);

/**
 * @brief Set the alpha channel of @p self.
 */
lh_void
lh_ui_color_set_a(lh_ui_color_t *self, lh_ui_color_channel_t a);

/**
 * @brief True if @p self and @p other have the same RGBA channels.
 */
lh_bool_t
lh_ui_color_equals(const lh_ui_color_t *self, const lh_ui_color_t *other);

/* ── Blend ───────────────────────────────────────────────────────────────── */

/**
 * @brief Paint @p color over @p dst. Straight alpha.
 */
lh_ui_color_t
lh_ui_color_over(const lh_ui_color_t *dst, const lh_ui_color_t *color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_COLOR_H */
