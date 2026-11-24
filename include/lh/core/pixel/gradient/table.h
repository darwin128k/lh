/**
 * @file table.h
 * @brief The 256 colours of a gradient, worked out once: ::lh_pixel_gradient_table_t.
 *
 * A position on a gradient is a byte, so a gradient has 256 colours at most.
 * The table works them all out once (::lh_pixel_gradient_table_fill), with
 * additions only (`lh/core/pixel/gradient/lerp.h`), and a box is then coloured
 * by looking positions up: the sampler (`lh/core/pixel/gradient/sampler.h`)
 * gives the positions of a row, the table turns them into colours
 * (::lh_pixel_gradient_table_map). Each entry is what
 * ::lh_pixel_gradient_get_color_at gives for its position: a table instead of a
 * computation.
 *
 * Carrying the current span until the position crosses its end, instead of
 * finding the span and dividing for every entry, took preparing a two-stop
 * table on an i.MX RT105x (ARMv7-M, GCC 13, Release) from 87–98 to 35–39
 * microseconds, and 24 gradient cards from 27.55 to 25.10 ms a full frame of
 * 480 by 320 (measured in gl, 2026-10-11, before the table moved here).
 *
 * 1 KB, a value: whoever draws a box keeps one on its stack for the fill, or
 * keeps one per gradient that does not change.
 */

#ifndef LH_CORE_PIXEL_GRADIENT_TABLE_H
#define LH_CORE_PIXEL_GRADIENT_TABLE_H

#include <lh/core/pixel/gradient/sampler.h>
#include <lh/core/pixel/gradient/table/fields.h>

/**
 * @def LH_PIXEL_GRADIENT_TABLE_RUN
 * @brief The positions of a run worked out at a time on the stack by
 *        ::lh_pixel_gradient_table_get_row and ::lh_pixel_gradient_table_get_column
 *        before they are looked up; a longer run is done in pieces this long.
 */
#define LH_PIXEL_GRADIENT_TABLE_RUN 128

/**
 * @struct lh_pixel_gradient_table
 * @brief A colour per position. Members: ::lh_pixel_gradient_table_fields.
 */
struct lh_pixel_gradient_table
{
    /** Members: ::lh_pixel_gradient_table_fields. */
    lh_pixel_gradient_table_fields(lh_u32_t);
};
/** @copybrief lh_pixel_gradient_table */
typedef struct lh_pixel_gradient_table lh_pixel_gradient_table_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The 256 colours, `0xAARRGGBB` (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The table's own colours.
 */
const lh_u32_t *
lh_pixel_gradient_table_get_colors(const lh_pixel_gradient_table_t *self);

/**
 * @brief The 256 colours, to write them (level 1).
 *
 * @param self The table; checked by ::lh_assert_runtime_ref.
 * @return The table's own colours.
 */
lh_u32_t *
lh_pixel_gradient_table_get_colors_mut(lh_pixel_gradient_table_t *self);

/**
 * @brief Work out the colour of every position of @p gradient: entry `frac` is
 *        ::lh_pixel_gradient_get_color_at of `frac`. Within a span between two
 *        stops in order the colour is carried a step at a time
 *        (::lh_pixel_gradient_lerp_t); anywhere else — before the first stop,
 *        after the last, between stops out of order — it is asked of
 *        ::lh_pixel_gradient_get_color_at itself.
 *
 * @param self     The table; checked by ::lh_pixel_gradient_table_get_colors_mut.
 * @param gradient The gradient; must not be null. Read, not kept.
 */
lh_void
lh_pixel_gradient_table_fill(lh_pixel_gradient_table_t *self, const lh_pixel_gradient_t *gradient);

/**
 * @brief Turn @p count positions into colours: `out[i]` is the table's entry of
 *        `levels[i]`. @p levels and @p out may not overlap.
 *
 * @param self   The table; checked by ::lh_pixel_gradient_table_get_colors.
 * @param levels The positions, from a sampler; @p count of them.
 * @param count  How many.
 * @param out    Room for @p count colours.
 */
lh_void
lh_pixel_gradient_table_map(const lh_pixel_gradient_table_t *self, const lh_byte_t *levels,
                            lh_s32_t count, lh_u32_t *out);

/**
 * @brief The colours of row @p y from column @p x0 to @p x1 of the box @p sampler
 *        is laid over: its positions (::lh_pixel_gradient_sampler_row) looked up
 *        (::lh_pixel_gradient_table_map), ::LH_PIXEL_GRADIENT_TABLE_RUN at a time.
 *
 * @param self    The table; checked by ::lh_pixel_gradient_table_get_colors.
 * @param sampler The gradient over the box; must not be null.
 * @param x0      The first column.
 * @param x1      One past the last, at least @p x0.
 * @param y       The row.
 * @param out     Room for `x1 - x0` colours, `0xAARRGGBB`.
 */
lh_void
lh_pixel_gradient_table_get_row(const lh_pixel_gradient_table_t *self,
                                const lh_pixel_gradient_sampler_t *sampler, lh_s32_t x0,
                                lh_s32_t x1, lh_s32_t y, lh_u32_t *out);

/**
 * @brief The colours of column @p x from row @p y0 to @p y1
 *        (::lh_pixel_gradient_sampler_column, then looked up), a run at a time.
 *
 * @param self    The table; checked by ::lh_pixel_gradient_table_get_colors.
 * @param sampler The gradient over the box; must not be null.
 * @param x       The column.
 * @param y0      The first row.
 * @param y1      One past the last, at least @p y0.
 * @param out     Room for `y1 - y0` colours.
 */
lh_void
lh_pixel_gradient_table_get_column(const lh_pixel_gradient_table_t *self,
                                   const lh_pixel_gradient_sampler_t *sampler, lh_s32_t x,
                                   lh_s32_t y0, lh_s32_t y1, lh_u32_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CORE_PIXEL_GRADIENT_TABLE_H */
