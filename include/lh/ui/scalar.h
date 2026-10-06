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
#else
#    include <lh/math/scalar.h>

typedef lh_math_scalar_t lh_ui_scalar_t;

/**
 * @brief One ::lh_ui_scalar_t from a numeric literal (int path).
 */
#    define lh_ui_scalar(n) lh_math_scalar(n)
#endif

#endif /* LH_UI_SCALAR_H */
