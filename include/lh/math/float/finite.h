/**
 * @file finite.h
 * @brief Whether a single-precision value is finite: ::lh_math_float_is_finite.
 *
 * Neither NaN nor an infinity. GCC and Clang get `__builtin_isfinite`, which
 * tests the exponent bits without a call; other compilers use `isfinite` from
 * `<math.h>`, a macro there too. Every finiteness test in the library goes
 * through this brick instead of including `<math.h>` itself.
 *
 * Requires ::LH_LIBRARY_OPTION_MATH_FPU.
 */

#ifndef LH_MATH_FLOAT_FINITE_H
#define LH_MATH_FLOAT_FINITE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/float.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Whether @p x is a finite number.
 *
 * @param x Value to test.
 * @return ::lh_bool_true for zero, subnormal and normal values;
 *         ::lh_bool_false for NaN and both infinities.
 */
lh_bool_t
lh_math_float_is_finite(lh_float_t x);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_FLOAT_FINITE_H */
