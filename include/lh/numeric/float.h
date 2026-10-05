/**
 * @file float.h
 * @brief Single-precision floating point: ::lh_float_t.
 *
 * A numeric type, next to the integers in `lh/numeric/types.h`. ::lh_float_t
 * is `float`. There is no `double` alias: on a single-precision FPU
 * (Cortex-M4F) a `double` is emulated in software, and a stray `1.0` literal
 * or `sqrt` call is enough to pull that path in. lh writes `1.0f` and uses
 * the single-precision builtins throughout.
 */

#ifndef LH_NUMERIC_FLOAT_H
#define LH_NUMERIC_FLOAT_H

/**
 * @typedef lh_float_t
 * @brief Single-precision floating point (`float`).
 *
 * Alias for: `float`
 */
typedef float lh_float_t;

#endif /* LH_NUMERIC_FLOAT_H */
