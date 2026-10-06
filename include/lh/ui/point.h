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

typedef lh_math_fpoint_t lh_ui_point_t;

#    define lh_ui_point_init lh_math_fpoint_init
#    define lh_ui_point_init_empty lh_math_fpoint_init_empty
#    define lh_ui_point_get_x lh_math_fpoint_get_x
#    define lh_ui_point_get_y lh_math_fpoint_get_y
#    define lh_ui_point_set_x lh_math_fpoint_set_x
#    define lh_ui_point_set_y lh_math_fpoint_set_y
#    define lh_ui_point_to_point lh_math_fpoint_to_point
#    define lh_ui_point_from_point lh_math_point_to_fpoint
#    define lh_ui_point_eq lh_math_fpoint_eq
#    define lh_ui_point_min lh_math_fpoint_min
#    define lh_ui_point_max lh_math_fpoint_max
#    define lh_ui_point_in_extent lh_math_fpoint_in_extent
#    define lh_ui_point_offset lh_math_fpoint_offset
#    define lh_ui_point_offset_size lh_math_fpoint_offset_size
#else
#    include <lh/math/point.h>

typedef lh_math_point_t lh_ui_point_t;

#    define lh_ui_point_init lh_math_point_init
#    define lh_ui_point_init_empty lh_math_point_init_empty
#    define lh_ui_point_get_x lh_math_point_get_x
#    define lh_ui_point_get_y lh_math_point_get_y
#    define lh_ui_point_set_x lh_math_point_set_x
#    define lh_ui_point_set_y lh_math_point_set_y
#    define lh_ui_point_to_point(self) (self)
#    define lh_ui_point_from_point(self) (self)
#    define lh_ui_point_eq lh_math_point_eq
#    define lh_ui_point_min lh_math_point_min
#    define lh_ui_point_max lh_math_point_max
#    define lh_ui_point_in_extent lh_math_point_in_extent
#    define lh_ui_point_offset lh_math_point_offset
#    define lh_ui_point_offset_size lh_math_point_offset_size
#endif

#endif /* LH_UI_POINT_H */
