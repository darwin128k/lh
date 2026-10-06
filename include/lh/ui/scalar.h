/**
 * @file scalar.h
 * @brief UI component ::lh_ui_scalar_t — picks a math scalar by FPU option.
 *
 * ::LH_LIBRARY_OPTION_MATH_FPU selects which math scalar UI uses:
 * - OFF → ::lh_math_scalar_t (discrete int geometry)
 * - ON  → ::lh_math_fscalar_t (continuous float geometry)
 *
 * Matching geometry aliases live in <lh/ui/point.h>,
 * <lh/ui/size.h>, <lh/ui/rect.h>.
 *
 * ::lh_ui_scalar writes one number through the chosen math literal macro.
 */

#ifndef LH_UI_SCALAR_H
#define LH_UI_SCALAR_H

#include <lh/config.h>

#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/math/fscalar.h>

typedef lh_math_fscalar_t lh_ui_scalar_t;

/**
 * @brief One ::lh_ui_scalar_t from a numeric literal (float path).
 */
#    define lh_ui_scalar(n) lh_math_fscalar(n)

#    include <lh/math/fscalar/round.h>

/**
 * @brief Largest whole ::lh_s32_t not above an ::lh_ui_scalar_t.
 */
#    define lh_ui_scalar_floor_s32 lh_math_fscalar_floor_s32

/**
 * @brief Smallest whole ::lh_s32_t not below an ::lh_ui_scalar_t.
 */
#    define lh_ui_scalar_ceil_s32 lh_math_fscalar_ceil_s32
#else
#    include <lh/math/scalar.h>

typedef lh_math_scalar_t lh_ui_scalar_t;

/**
 * @brief One ::lh_ui_scalar_t from a numeric literal (int path).
 */
#    define lh_ui_scalar(n) lh_math_scalar(n)

#    include <lh/cast/static.h>
#    include <lh/numeric/fixed/types.h>

/**
 * @brief The integer scalar is already whole: a cast.
 */
#    define lh_ui_scalar_floor_s32(v) lh_cast_static(lh_s32_t, (v))

/**
 * @brief The integer scalar is already whole: a cast.
 */
#    define lh_ui_scalar_ceil_s32(v) lh_cast_static(lh_s32_t, (v))
#endif

#endif /* LH_UI_SCALAR_H */
