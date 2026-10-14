/**
 * @file point.h
 * @brief UI point: aliases math integer or float point by FPU option.
 *
 * OFF → ::lh_math_point_t (::lh_math_scalar_t).
 * ON  → ::lh_math_fpoint_t (::lh_math_fscalar_t).
 * All `lh_ui_point_*` names forward to the chosen math API.
 */

#ifndef LH_UI_POINT_H
#define LH_UI_POINT_H

#include <lh/config.h>
#include <lh/ui/scalar.h>

#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/math/fpoint.h>

/** @brief A UI point: ::lh_math_fpoint_t when ::LH_LIBRARY_OPTION_MATH_FPU is on. */
typedef lh_math_fpoint_t lh_ui_point_t;

/** @brief Name of the float math function @p name for ::lh_ui_point_t. */
#    define LH_UI_POINT_FN(name) lh_math_fpoint_##name
/** @brief ::lh_ui_point_t to ::lh_math_point_t (::lh_math_fpoint_to_point). */
#    define lh_ui_point_to_point lh_math_fpoint_to_point
/** @brief ::lh_math_point_t to ::lh_ui_point_t (::lh_math_point_to_fpoint). */
#    define lh_ui_point_from_point lh_math_point_to_fpoint
#else
#    include <lh/math/point.h>

/** @brief A UI point: ::lh_math_point_t when ::LH_LIBRARY_OPTION_MATH_FPU is off. */
typedef lh_math_point_t lh_ui_point_t;

/** @brief Name of the integer math function @p name for ::lh_ui_point_t. */
#    define LH_UI_POINT_FN(name) lh_math_point_##name
/** @brief ::lh_ui_point_t to ::lh_math_point_t: the same type here, so @p self as is. */
#    define lh_ui_point_to_point(self) (self)
/** @brief ::lh_math_point_t to ::lh_ui_point_t: the same type here, so @p self as is. */
#    define lh_ui_point_from_point(self) (self)
#endif

/* One list for both paths: a math function missing on either side fails to
 * compile on that side instead of silently drifting. */
/**
 * @name Point functions
 * Each `lh_ui_point_<op>` is `lh_math_point_<op>` (or `lh_math_fpoint_<op>` with
 * ::LH_LIBRARY_OPTION_MATH_FPU): see that function for the contract.
 * @{
 */
/** @brief The `lh_math_point_*` / `lh_math_fpoint_*` function of the same suffix. */
#define lh_ui_point_init LH_UI_POINT_FN(init)
#define lh_ui_point_init_empty LH_UI_POINT_FN(init_empty)
#define lh_ui_point_get_x LH_UI_POINT_FN(get_x)
#define lh_ui_point_get_y LH_UI_POINT_FN(get_y)
#define lh_ui_point_set_x LH_UI_POINT_FN(set_x)
#define lh_ui_point_set_y LH_UI_POINT_FN(set_y)
#define lh_ui_point_eq LH_UI_POINT_FN(eq)
#define lh_ui_point_equals LH_UI_POINT_FN(equals)
#define lh_ui_point_is_at_least LH_UI_POINT_FN(is_at_least)
#define lh_ui_point_is_less LH_UI_POINT_FN(is_less)
#define lh_ui_point_is_greater LH_UI_POINT_FN(is_greater)
#define lh_ui_point_min LH_UI_POINT_FN(min)
#define lh_ui_point_max LH_UI_POINT_FN(max)
#define lh_ui_point_in_extent LH_UI_POINT_FN(in_extent)
#define lh_ui_point_offset LH_UI_POINT_FN(offset)
#define lh_ui_point_offset_size LH_UI_POINT_FN(offset_size)
/** @} */

#endif /* LH_UI_POINT_H */
