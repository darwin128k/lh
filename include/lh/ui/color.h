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
#include <lh/byte.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
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
 *
 * @p hex is 32 bits wide on every target, so the alpha byte is never lost to
 * a 16-bit `unsigned int`.
 */
lh_void
lh_ui_color_init_hex(lh_ui_color_t *self, lh_u32_t hex);

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
 * @brief Paint @p color over @p dst (Porter-Duff "over"). Straight alpha.
 *
 * Both colors and the result are straight (not premultiplied). @p dst may be
 * translucent: the result alpha is `a + dst_a * (1 - a)` and each channel is
 * divided back by it. Two fully transparent inputs give `0, 0, 0, 0`.
 */
lh_ui_color_t
lh_ui_color_over(const lh_ui_color_t *dst, const lh_ui_color_t *color);

/**
 * @brief Weight of the painted color in ::lh_ui_color_over: `a * 255`.
 */
lh_u32_t
lh_ui_color_over_src_weight(const lh_ui_color_t *color);

/**
 * @brief Weight of what lies under it in ::lh_ui_color_over:
 *        `dst_a * (255 - a)`.
 */
lh_u32_t
lh_ui_color_over_dst_weight(const lh_ui_color_t *dst, const lh_ui_color_t *color);

/**
 * @brief One channel mixed by weight, rounded:
 *        `(src * src_w + dst * dst_w) / (src_w + dst_w)`. The weights must
 *        not both be `0`.
 */
lh_ui_color_channel_t
lh_ui_color_blend_channel(lh_u32_t src, lh_u32_t dst, lh_u32_t src_w, lh_u32_t dst_w);

/**
 * @brief @p color mixed into @p dst by the weights, alpha included
 *        (the alpha is `(src_w + dst_w) / 255`). The weights must not both
 *        be `0`.
 */
lh_ui_color_t
lh_ui_color_blend(const lh_ui_color_t *dst, const lh_ui_color_t *color, lh_u32_t src_w, lh_u32_t dst_w);

/**
 * @brief A summed ::lh_ui_color_over weight (alpha scaled by 255) back to an
 *        alpha channel, rounded.
 */
lh_ui_color_channel_t
lh_ui_color_weight_to_alpha(lh_u32_t weight);

/**
 * @brief @p color with its alpha scaled by @p coverage (`0..255`), rounded.
 *
 * What an anti-aliased edge pixel is painted with.
 */
lh_ui_color_t
lh_ui_color_with_coverage(const lh_ui_color_t *color, lh_byte_t coverage);

/**
 * @brief The byte of @p hex at bit @p shift as a channel.
 */
lh_ui_color_channel_t
lh_ui_color_hex_channel(lh_u32_t hex, lh_u32_t shift);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_COLOR_H */
