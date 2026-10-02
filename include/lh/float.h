/**
 * @file float.h
 * @brief The floating-point type of lh's math.
 *
 * ::lh_float_t is single precision (`float`) and lh's math stays in it: no
 * `double`. On microcontrollers with a single-precision FPU (Cortex-M4F) a
 * `double` is emulated in software and many times slower, and a stray `1.0`
 * literal or `sqrt` call is enough to pull it in — so lh writes `1.0f` and
 * uses the single-precision builtins throughout.
 */

#ifndef LH_FLOAT_H
#define LH_FLOAT_H

/**
 * @typedef lh_float_t
 * @brief Single-precision floating point (`float`).
 *
 * Alias for: `float`
 */
typedef float lh_float_t;

#endif /* LH_FLOAT_H */
