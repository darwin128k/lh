/**
 * @file sine.h
 * @brief ::lh_ui_gradient_sine: the sine a linear gradient is laid along, in whole
 *        numbers.
 */

#ifndef LH_UI_GRADIENT_SINE_H
#define LH_UI_GRADIENT_SINE_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief sin(@p degrees) times 4096, for any whole number of degrees.
 *
 * Whole degrees are all the angles a sheet writes in practice, and a table is the
 * same on a build with no FPU. ::lh_ui_gradient_frac_at and
 * ::lh_ui_gradient_sampler_t both lay a gradient along it, so they cannot disagree.
 */
lh_s32_t
lh_ui_gradient_sine(lh_s32_t degrees);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_GRADIENT_SINE_H */
