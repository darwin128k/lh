/**
 * @file stop.h
 * @brief One colour of a gradient and where it stands: ::lh_pixel_gradient_stop_t.
 *
 * A gradient (`lh/core/pixel/gradient.h`) is a row of stops in order of their
 * position; between two stops the colour is mixed in proportion. A stop is a
 * value: it points at nothing, and the gradient holds its stops by value.
 */

#ifndef LH_CORE_PIXEL_GRADIENT_STOP_H
#define LH_CORE_PIXEL_GRADIENT_STOP_H

#include <lh/core/byte.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/void.h>
#include <lh/core/pixel/gradient/stop/fields.h>

/**
 * @struct lh_pixel_gradient_stop
 * @brief A colour and where along the gradient it stands. Members:
 *        ::lh_pixel_gradient_stop_fields.
 */
struct lh_pixel_gradient_stop
{
    /** Members: ::lh_pixel_gradient_stop_fields. */
    lh_pixel_gradient_stop_fields(lh_u32_t, lh_byte_t);
};
/** @copybrief lh_pixel_gradient_stop */
typedef struct lh_pixel_gradient_stop lh_pixel_gradient_stop_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The stop's colour, `0xAARRGGBB` (level 1).
 *
 * @param self The stop; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_pixel_gradient_stop_get_color(const lh_pixel_gradient_stop_t *self);

/**
 * @brief Store the stop's colour (level 1).
 *
 * @param self  The stop; checked by ::lh_assert_runtime_ref.
 * @param color `0xAARRGGBB`.
 */
lh_void
lh_pixel_gradient_stop_set_color(lh_pixel_gradient_stop_t *self, lh_u32_t color);

/**
 * @brief Where the stop stands: `0` at the start of the gradient, `255` at its
 *        end (level 1).
 *
 * @param self The stop; checked by ::lh_assert_runtime_ref.
 */
lh_byte_t
lh_pixel_gradient_stop_get_frac(const lh_pixel_gradient_stop_t *self);

/**
 * @brief Store where the stop stands (level 1).
 *
 * @param self The stop; checked by ::lh_assert_runtime_ref.
 * @param frac `0 .. 255`.
 */
lh_void
lh_pixel_gradient_stop_set_frac(lh_pixel_gradient_stop_t *self, lh_byte_t frac);

/**
 * @brief A stop of @p color at @p frac.
 *
 * @param self  The stop; checked by the setters.
 * @param color `0xAARRGGBB`.
 * @param frac  `0 .. 255`.
 */
lh_void
lh_pixel_gradient_stop_init(lh_pixel_gradient_stop_t *self, lh_u32_t color, lh_byte_t frac);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CORE_PIXEL_GRADIENT_STOP_H */
