/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_glyph_t.
 */

#ifndef LH_GX_GLYPH_FIELDS_H
#define LH_GX_GLYPH_FIELDS_H

/**
 * @def lh_gx_glyph_fields(mask_type, length_type)
 * @brief The ink of one character and where it goes.
 *
 * `mask` is the coverage of the ink, cropped to it (its bytes are the font's, not
 * owned); `advance` is how far the pen moves after it; `top` is where the top of
 * the mask is, from the baseline — up is negative.
 *
 * @param mask_type   Type of the mask (::lh_pixel_mask_t).
 * @param length_type Type of a length in pixels.
 */
#define lh_gx_glyph_fields(mask_type, length_type)                                                 \
    mask_type mask;                                                                                \
    length_type advance;                                                                           \
    length_type top

#endif /* LH_GX_GLYPH_FIELDS_H */
