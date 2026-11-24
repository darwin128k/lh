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
 * @brief Eight coverages of a four-bit row: @p bytes is eight packed bytes,
 *        lowest first, and the result is eight coverages, pixel `i` in byte `i`
 *        (each sample times 17). The first pixel is the high nibble of the lowest
 *        byte, or its low nibble when @p odd; five bytes are used at most.
 *
 * Plain integer code, for every target -- the text of a frame on a microcontroller
 * is this call. ::lh_pixel_mask_unpack_4_sse2 gives the same eight from the same
 * bytes.
 *
 * @param bytes The packed bytes, the first in the lowest byte.
 * @param odd   True when the first pixel is the low nibble of its byte.
 * @return Eight coverages, pixel `i` in byte `i`.
 */
lh_u64_t
lh_pixel_mask_unpack_4_words(lh_u64_t bytes, lh_bool_t odd);

/**
 * @brief ::lh_pixel_mask_unpack_4_words in SSE2: one AND, one shift and one
 *        interleave put the eight samples in pixel order, and one multiply by 17
 *        makes them coverages.
 *
 * Only defined when `LH_LIBRARY_OPTION_SIMD_HAVE_SSE2` is on.
 *
 * @param bytes The packed bytes, the first in the lowest byte.
 * @param odd   True when the first pixel is the low nibble of its byte.
 * @return Eight coverages, pixel `i` in byte `i`.
 */
lh_u64_t
lh_pixel_mask_unpack_4_sse2(lh_u64_t bytes, lh_bool_t odd);

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
 * @brief The coverage of @p count pixels of row @p y from column @p x, one sample
 *        at a time, for any depth: what ::lh_pixel_mask_get_row_coverage does with
 *        what its eight-pixel step does not take.
 *
 * A pixel outside the mask is `0`.
 *
 * @param self  The mask; must not be null.
 * @param y     The row.
 * @param x     The first column.
 * @param count How many pixels.
 * @param out   Room for @p count bytes; must not be null.
 */
lh_void
lh_pixel_mask_get_row_coverage_by_samples(const lh_pixel_mask_t *self, lh_s32_t y, lh_s32_t x,
                                          lh_s32_t count, lh_byte_t *out);

/**
 * @brief Coverage at (@p x, @p y) in `0..255` (the sample rescaled); `0` outside.
 */
lh_byte_t
lh_pixel_mask_get_coverage(const lh_pixel_mask_t *self, lh_s32_t x, lh_s32_t y);

/**
 * @brief The coverage of pixels `x .. x + 7` of row @p y in one register, pixel
 *        `x + i` in byte `i`, `0` outside the mask.
 *
 * A four-bit row from a column inside the mask is read as one word where the row
 * has eight bytes left, its end gathered byte by byte otherwise -- never read past
 * -- and unpacked by ::lh_pixel_mask_unpack_4_sse2, or
 * ::lh_pixel_mask_unpack_4_words without SSE2. Every other depth and a start
 * before the mask go through ::lh_pixel_mask_get_row_coverage_by_samples.
 *
 * @param self The mask; must not be null.
 * @param x    The first column.
 * @param y    The row.
 * @return Eight coverages.
 */
lh_u64_t
lh_pixel_mask_get_coverage_8(const lh_pixel_mask_t *self, lh_s32_t x, lh_s32_t y);

/**
 * @brief Unpack @p count pixels of row @p y, from column @p x, as coverage
 *        `0..255` into @p out: what ::lh_pixel_mask_get_coverage answers for each,
 *        a row at a time.
 *
 * A four-bit mask -- what a glyph is -- goes eight pixels a step through
 * ::lh_pixel_mask_get_coverage_8, every other depth and the columns before the
 * mask through ::lh_pixel_mask_get_row_coverage_by_samples. Read pixel by pixel,
 * the glyphs of the `text` scene of `bench/lvgl/` (154 a paragraph, i7-3770K)
 * took 35 microseconds a paragraph to unpack.
 *
 * The depths a mask has (1, 2, 4 and 8 bits) all divide 255, so a sample is
 * scaled by one multiplication, exactly as the per-pixel read rounds it. A
 * pixel outside the mask — a row past its height, a column past its width or
 * before it — is `0`. What a glyph is drawn from: unpacking a glyph a row at a
 * time, and handing the canvas the whole glyph instead of a row, took the
 * `cards` scene of `bench/lvgl/` (24 captions, i7-3770K) from 257 to 185
 * microseconds a frame, the same picture to the byte (`bench/lvgl/RESULTS.md`).
 *
 * @param self  The mask; must not be null.
 * @param y     The row.
 * @param x     The first column.
 * @param count How many pixels.
 * @param out   Room for @p count bytes; must not be null.
 */
lh_void
lh_pixel_mask_get_row_coverage(const lh_pixel_mask_t *self, lh_s32_t y, lh_s32_t x, lh_s32_t count,
                               lh_byte_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_PIXEL_MASK_H */
