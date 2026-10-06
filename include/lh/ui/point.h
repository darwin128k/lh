/**
 * @file point.h
 * @brief UI point: aliases math integer or float point by FPU option.
 *
 * OFF → ::lh_math_ipoint_t (::lh_math_iscalar_t).
 * ON  → ::lh_math_point_t (::lh_math_scalar_t).
 * All `lh_ui_point_*` names forward to the chosen math API.
 */

#ifndef LH_UI_POINT_H
#define LH_UI_POINT_H

#include <lh/config.h>
#include <lh/ui/scalar.h>

#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/math/point.h>

typedef lh_math_point_t lh_ui_point_t;

#    define lh_ui_point_make lh_math_point_make
#    define lh_ui_point_make_empty lh_math_point_make_empty
#    define lh_ui_point_get_x lh_math_point_get_x
#    define lh_ui_point_get_y lh_math_point_get_y
#    define lh_ui_point_set_x lh_math_point_set_x
#    define lh_ui_point_set_y lh_math_point_set_y
#    define lh_ui_point_to_ipoint lh_math_point_to_ipoint
#    define lh_ui_ipoint_to_point lh_math_ipoint_to_point
#    define lh_ui_point_eq lh_math_point_eq
#else
#    include <lh/math/ipoint.h>

typedef lh_math_ipoint_t lh_ui_point_t;

#    define lh_ui_point_make lh_math_ipoint_make
#    define lh_ui_point_make_empty lh_math_ipoint_make_empty
#    define lh_ui_point_get_x lh_math_ipoint_get_x
#    define lh_ui_point_get_y lh_math_ipoint_get_y
#    define lh_ui_point_set_x lh_math_ipoint_set_x
#    define lh_ui_point_set_y lh_math_ipoint_set_y
#    define lh_ui_point_to_ipoint(self) (self)
#    define lh_ui_ipoint_to_point(self) (self)
#    define lh_ui_point_eq lh_math_ipoint_eq
#endif

#endif /* LH_UI_POINT_H */
