/**
 * @file real.h
 * @brief The floating point type lh uses (::lh_real_t), chosen per target.
 *
 * Floating point is not free everywhere: a Cortex-M4F has only a
 * single-precision FPU (double is emulated in software), and a Cortex-M0,
 * M3 or AVR has none. ::LH_LIBRARY_OPTION_REAL picks:
 *
 *   ::LH_REAL_DOUBLE — ::lh_real_t is `double` (desktop).
 *   ::LH_REAL_FLOAT  — ::lh_real_t is `float` (single-precision FPU).
 *   ::LH_REAL_NONE   — there is no ::lh_real_t; code that would need one
 *                      is left out (test ::LH_REAL_ENABLED), so no soft-float
 *                      routines end up in the image.
 */

#ifndef LH_REAL_H
#define LH_REAL_H

#include <lh/config.h>

/** @brief ::LH_LIBRARY_OPTION_REAL: no floating point. */
#define LH_REAL_NONE 0
/** @brief ::LH_LIBRARY_OPTION_REAL: ::lh_real_t is `float`. */
#define LH_REAL_FLOAT 1
/** @brief ::LH_LIBRARY_OPTION_REAL: ::lh_real_t is `double`. */
#define LH_REAL_DOUBLE 2

/**
 * @def LH_REAL_ENABLED
 * @brief `1` when ::lh_real_t exists, `0` for ::LH_REAL_NONE.
 */
#if LH_LIBRARY_OPTION_REAL == LH_REAL_DOUBLE
#    define LH_REAL_ENABLED 1
/** @brief The floating point type of this build. */
typedef double lh_real_t;
#elif LH_LIBRARY_OPTION_REAL == LH_REAL_FLOAT
/**
 * @def LH_REAL_ENABLED
 * @brief `1` when ::lh_real_t exists, `0` for ::LH_REAL_NONE.
 */
#    define LH_REAL_ENABLED 1
/** @brief The floating point type of this build. */
typedef float lh_real_t;
#elif LH_LIBRARY_OPTION_REAL == LH_REAL_NONE
/**
 * @def LH_REAL_ENABLED
 * @brief `1` when ::lh_real_t exists, `0` for ::LH_REAL_NONE.
 */
#    define LH_REAL_ENABLED 0
#else
#    error "LH_LIBRARY_OPTION_REAL must be LH_REAL_NONE, LH_REAL_FLOAT or LH_REAL_DOUBLE"
#endif

#endif /* LH_REAL_H */
