/**
 * @file color.h
 * @brief A colour of the gx plane: ::lh_gx_color_t.
 *
 * One 32-bit word `0xAARRGGBB`, straight alpha — not a struct of four channels
 * with a getter each. A colour is passed by value in every call of the canvas
 * and lives by the thousand in themes in flash, so it is the cheapest thing that
 * says it: one register, one word of a table. Its arithmetic is the core's
 * (`lh/core/pixel/argb.h`: ::lh_pixel_argb_make, ::lh_pixel_argb_over, …), shared
 * with every other part that draws.
 *
 * A pixel format is the backend's business: an RGB565 target narrows the colour
 * when it stores it.
 */

#ifndef LH_GX_COLOR_H
#define LH_GX_COLOR_H

#include <lh/core/numeric/fixed/types.h>

/**
 * @typedef lh_gx_color_t
 * @brief `0xAARRGGBB`, straight alpha.
 */
typedef lh_u32_t lh_gx_color_t;

#endif /* LH_GX_COLOR_H */
