/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_backend_sw_shadow_t.
 */

#ifndef LH_GX_BACKEND_SW_SHADOW_FIELDS_H
#define LH_GX_BACKEND_SW_SHADOW_FIELDS_H

/**
 * @def lh_gx_backend_sw_shadow_fields(shadow_type)
 * @brief The alphas of the last shadow drawn, and which shadow they are.
 *
 * `bits` is the caller's memory, `capacity` bytes of it; `width` by `height` of
 * it hold the alphas of the shadow `key` — the core's shadow of a box laid at
 * `0, 0`, so that the same shadow of a box anywhere is the same key — over its
 * bounds, row by row; `filled` says whether they do yet.
 *
 * @param shadow_type Type of the key (::lh_pixel_shadow_t).
 */
#define lh_gx_backend_sw_shadow_fields(shadow_type)                                                \
    lh_byte_t *bits;                                                                               \
    lh_usize_t capacity;                                                                           \
    shadow_type key;                                                                               \
    lh_s32_t width;                                                                                \
    lh_s32_t height;                                                                               \
    lh_bool_t filled

#endif /* LH_GX_BACKEND_SW_SHADOW_FIELDS_H */
