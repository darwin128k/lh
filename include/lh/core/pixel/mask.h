/**
 * @file mask.h
 * @brief A packed coverage mask: ::lh_pixel_mask_t.
 *
 * `width` x `height` pixels of coverage, each row packed high bit first at 1, 2,
 * 4 or 8 bits per pixel (`lh/core/bit/packed.h`) and `row_bytes` long. The bytes
 * are not owned. A font glyph is a mask (`lh/core/font.h`); so is a baked icon
 * or cursor. Data and small queries only: a renderer reads the coverage and lays
 * a colour through it.
 */

#ifndef LH_PIXEL_MASK_H
#define LH_PIXEL_MASK_H

#include <lh/core/byte/ptr.h>
#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/pixel/mask/fields.h>

/**
 * @struct lh_pixel_mask
 * @brief A view of packed coverage pixels.
 */
struct lh_pixel_mask
{
    /** Members: ::lh_pixel_mask_fields. */
    lh_pixel_mask_fields(lh_byte_t, lh_s32_t, lh_byte_t);
};
/** @copybrief lh_pixel_mask */
typedef struct lh_pixel_mask lh_pixel_mask_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief True when @p bpp is a depth a mask can hold: 1, 2, 4 or 8.
 */
lh_bool_t
lh_pixel_mask_is_bpp(lh_u32_t bpp);

/**
 * @brief Width of @p self, in pixels.
 */
lh_s32_t
lh_pixel_mask_get_width(const lh_pixel_mask_t *self);

/**
 * @brief Height of @p self, in pixels.
 */
lh_s32_t
lh_pixel_mask_get_height(const lh_pixel_mask_t *self);

/**
 * @brief Bits per pixel of @p self.
 */
lh_byte_t
lh_pixel_mask_get_bpp(const lh_pixel_mask_t *self);

/**
 * @brief The coverage bits (level 1).
 *
 * @param self The mask; checked by ::lh_assert_runtime_ref.
 */
const lh_byte_t *
lh_pixel_mask_get_bits(const lh_pixel_mask_t *self);

/**
 * @brief Store the coverage bits (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The mask; checked by ::lh_assert_runtime_ref.
 * @param bits The new value.
 */
lh_void
lh_pixel_mask_set_bits(lh_pixel_mask_t *self, const lh_byte_t *bits);

/**
 * @brief Store the width of the mask in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The mask; checked by ::lh_assert_runtime_ref.
 * @param width The new value.
 */
lh_void
lh_pixel_mask_set_width(lh_pixel_mask_t *self, lh_s32_t width);

/**
 * @brief Store the height of the mask in pixels (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The mask; checked by ::lh_assert_runtime_ref.
 * @param height The new value.
 */
lh_void
lh_pixel_mask_set_height(lh_pixel_mask_t *self, lh_s32_t height);

/**
 * @brief How many bytes one row takes (level 1).
 *
 * @param self The mask; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_pixel_mask_get_row_bytes(const lh_pixel_mask_t *self);

/**
 * @brief Store how many bytes one row takes (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The mask; checked by ::lh_assert_runtime_ref.
 * @param row_bytes The new value.
 */
lh_void
lh_pixel_mask_set_row_bytes(lh_pixel_mask_t *self, lh_s32_t row_bytes);

/**
 * @brief Store how many bits one pixel of coverage takes (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The mask; checked by ::lh_assert_runtime_ref.
 * @param bpp The new value.
 */
lh_void
lh_pixel_mask_set_bpp(lh_pixel_mask_t *self, lh_byte_t bpp);

/**
 * @brief Fill @p self over @p bits: @p height rows of @p row_bytes, @p width
 *        pixels of @p bpp each. The bytes are not copied.
 *
 * @p row_bytes is at least ::lh_bit_packed_bytes(width, bpp).
 */
lh_void
lh_pixel_mask_init(lh_pixel_mask_t *self, const lh_byte_ptr bits, lh_s32_t width, lh_s32_t height,
                   lh_s32_t row_bytes, lh_byte_t bpp);

/**
 * @brief First byte of row @p y. @p y must be in `0 .. height - 1`.
 */
const lh_byte_ptr
lh_pixel_mask_get_row(const lh_pixel_mask_t *self, lh_s32_t y);

/**
 * @brief True when (@p x, @p y) lies inside @p self.
 */
lh_bool_t
lh_pixel_mask_has_pixel(const lh_pixel_mask_t *self, lh_s32_t x, lh_s32_t y);

/**
 * @brief Raw sample at (@p x, @p y), `0 .. 2^bpp - 1`; `0` outside.
 */
lh_u32_t
lh_pixel_mask_get_sample(const lh_pixel_mask_t *self, lh_s32_t x, lh_s32_t y);

/**
 * @brief Coverage at (@p x, @p y) in `0..255` (the sample rescaled); `0` outside.
 */
lh_byte_t
lh_pixel_mask_get_coverage(const lh_pixel_mask_t *self, lh_s32_t x, lh_s32_t y);

LH_COMPILER_EXTERN_C_END

#endif /* LH_PIXEL_MASK_H */
