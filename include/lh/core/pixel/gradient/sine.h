/**
 * @file sine.h
 * @brief ::lh_pixel_gradient_sine: the sine a linear gradient is laid along, in
 *        whole numbers.
 *
 * A linear gradient runs along `(sin a, -cos a)`: up at 0 degrees, right at 90.
 * Both the colour of one pixel (::lh_pixel_gradient_get_frac_at) and the walk
 * along a row (::lh_pixel_gradient_sampler_t) take the sine from here, so they
 * cannot disagree, and neither needs a floating-point unit.
 */

#ifndef LH_CORE_PIXEL_GRADIENT_SINE_H
#define LH_CORE_PIXEL_GRADIENT_SINE_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/numeric/fixed/types.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief sin(@p degrees) times 4096, for any whole number of degrees.
 *
 * Whole degrees are all the angles a sheet writes in practice, and a table of
 * the first quadrant is the same on a build with no FPU; any other angle is
 * folded onto it. sin(90) is exactly 4096, sin(0) and sin(180) exactly 0.
 *
 * @param degrees The angle, any whole number: negative and past 360 fold.
 * @return The sine times 4096, `-4096 .. 4096`.
 */
lh_s32_t
lh_pixel_gradient_sine(lh_s32_t degrees);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CORE_PIXEL_GRADIENT_SINE_H */
