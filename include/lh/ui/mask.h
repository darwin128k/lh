/**
 * @file mask.h
 * @brief A packed alpha mask: ::lh_ui_mask_t.
 *
 * `width` x `height` pixels of coverage, each row packed high bit first at
 * 1, 2, 4 or 8 bits per pixel (`lh/bit/packed.h`) and `row_bytes` long. The
 * bytes are not owned. A font glyph is a mask; so is a baked icon or cursor.
 * ::lh_ui_canvas_fill_mask paints one in a color.
 */

#ifndef LH_UI_MASK_H
#define LH_UI_MASK_H

#include <lh/bool.h>
#include <lh/byte.h>
#include <lh/byte/ptr.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/mask/fields.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/void.h>

/**
 * @struct lh_ui_mask
 * @brief A view of packed coverage pixels.
 */
struct lh_ui_mask
{
    /** Members: ::lh_ui_mask_fields. */
    lh_ui_mask_fields(lh_byte_t, lh_s32_t, lh_byte_t);
};
/** @copybrief lh_ui_mask */
typedef struct lh_ui_mask lh_ui_mask_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief True when @p bpp is a depth a mask can hold: 1, 2, 4 or 8.
 */
lh_bool_t
lh_ui_mask_is_bpp(lh_u32_t bpp);

/**
 * @brief Fill @p self over @p bits: @p height rows of @p row_bytes, @p width
 *        pixels of @p bpp each. The bytes are not copied.
 *
 * @p row_bytes is at least ::lh_bit_packed_bytes(width, bpp).
 */
lh_void
lh_ui_mask_init(lh_ui_mask_t *self, const lh_byte_ptr bits, lh_s32_t width, lh_s32_t height, lh_s32_t row_bytes,
                lh_byte_t bpp);

/**
 * @brief Width of @p self, in pixels.
 */
lh_s32_t
lh_ui_mask_get_width(const lh_ui_mask_t *self);

/**
 * @brief Height of @p self, in pixels.
 */
lh_s32_t
lh_ui_mask_get_height(const lh_ui_mask_t *self);

/**
 * @brief Bits per pixel of @p self.
 */
lh_byte_t
lh_ui_mask_get_bpp(const lh_ui_mask_t *self);

/**
 * @brief First byte of row @p y. @p y must be in `0 .. height - 1`.
 */
const lh_byte_ptr
lh_ui_mask_get_row(const lh_ui_mask_t *self, lh_s32_t y);

/**
 * @brief True when (@p x, @p y) lies inside @p self.
 */
lh_bool_t
lh_ui_mask_has_pixel(const lh_ui_mask_t *self, lh_s32_t x, lh_s32_t y);

/**
 * @brief Raw sample at (@p x, @p y), `0 .. 2^bpp - 1`; `0` outside.
 */
lh_u32_t
lh_ui_mask_get_sample(const lh_ui_mask_t *self, lh_s32_t x, lh_s32_t y);

/**
 * @brief Coverage at (@p x, @p y) in `0..255` (the sample rescaled); `0` outside.
 */
lh_byte_t
lh_ui_mask_get_coverage(const lh_ui_mask_t *self, lh_s32_t x, lh_s32_t y);

/**
 * @brief Coverage of pixels `x0 .. x1 - 1` of row @p y, written to @p out
 *        `x1 - x0` bytes. The same values as calling
 *        ::lh_ui_mask_get_coverage per pixel, with the row pointer, the bits per
 *        pixel and the rescale taken once instead of once per pixel.
 *
 * @param self Mask to read.
 * @param x0  First column.
 * @param x1  One past the last column.
 * @param y   Row.
 * @param out Receives `x1 - x0` bytes.
 */
lh_void
lh_ui_mask_coverage_run(const lh_ui_mask_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_byte_ptr out);

/**
 * @brief Coverage of pixels `x .. x + 7` of row @p y in one register, pixel
 *        `x + i` in byte `i` (`0` outside the mask): eight of
 *        ::lh_ui_mask_coverage_run without a buffer, which is how
 *        ::lh_ui_pixmap_blend_cover_8 takes them. A four-bit row is read with
 *        ::lh_ui_mask_unpack_4_sse2.
 */
lh_u64_t
lh_ui_mask_get_coverage_8(const lh_ui_mask_t *self, lh_s32_t x, lh_s32_t y);

/**
 * @brief Eight coverages of a four-bit row: @p bytes is eight packed bytes,
 *        lowest first, and the result is eight coverages, pixel `i` in byte `i`
 *        (each sample times 17). The first pixel is the high nibble of the lowest
 *        byte, or its low nibble when @p odd; five bytes are used at most.
 *        ::lh_ui_mask_get_coverage_8 runs it on a row, a zero-padded end included.
 *
 * Only defined when `LH_LIBRARY_OPTION_SIMD_HAVE_SSE2` is on.
 */
lh_u64_t
lh_ui_mask_unpack_4_sse2(lh_u64_t bytes, lh_bool_t odd);

/**
 * @brief The rect @p self covers when its top-left pixel is at @p origin.
 */
lh_ui_rect_t
lh_ui_mask_get_rect(const lh_ui_mask_t *self, lh_ui_point_t origin);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_MASK_H */
