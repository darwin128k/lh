/**
 * @file sine.h
 * @brief Sine of a whole number of degrees, times 4096.
 *
 * A linear gradient is laid along this. Whole degrees are the angles a sheet
 * writes, and the table is the same on a build with no FPU.
 */

#ifndef LH_GL_GRADIENT_SINE_H
#define LH_GL_GRADIENT_SINE_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/numeric/fixed/types.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief sin(@p degrees) times 4096.
 *
 * @p degrees may be any whole number. The result is negative in the third and
 * fourth quadrants.
 */
lh_s32_t
lh_gl_gradient_sine(lh_s32_t degrees);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_GRADIENT_SINE_H */
