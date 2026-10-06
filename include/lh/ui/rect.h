/**
 * @file rect.h
 * @brief UI rect: aliases math integer or float rect by FPU option.
 *
 * OFF → ::lh_math_rect_t (::lh_math_scalar_t).
 * ON  → ::lh_math_frect_t (::lh_math_fscalar_t).
 * All `lh_ui_rect_*` names forward to the chosen math API.
 */

#ifndef LH_UI_RECT_H
#define LH_UI_RECT_H

#include <lh/config.h>
#include <lh/ui/point.h>
#include <lh/ui/size.h>

#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/math/frect.h>

typedef lh_math_frect_t lh_ui_rect_t;

#    define lh_ui_rect_make lh_math_frect_make
#    define lh_ui_rect_make_origin_size lh_math_frect_make_origin_size
#    define lh_ui_rect_from_extent lh_math_frect_from_extent
#    define lh_ui_rect_make_empty lh_math_frect_make_empty
#    define lh_ui_rect_init lh_math_frect_init
#    define lh_ui_rect_init_origin_size lh_math_frect_init_origin_size
#    define lh_ui_rect_get_origin lh_math_frect_get_origin
#    define lh_ui_rect_get_origin_as_const lh_math_frect_get_origin_as_const
#    define lh_ui_rect_get_size lh_math_frect_get_size
#    define lh_ui_rect_get_size_as_const lh_math_frect_get_size_as_const
#    define lh_ui_rect_set_origin lh_math_frect_set_origin
#    define lh_ui_rect_set_size lh_math_frect_set_size
#    define lh_ui_rect_far lh_math_frect_far
#    define lh_ui_rect_to_rect lh_math_frect_to_rect
#    define lh_ui_rect_from_rect lh_math_rect_to_frect
#    define lh_ui_rect_is_empty lh_math_frect_is_empty
#    define lh_ui_rect_contains_point lh_math_frect_contains_point
#    define lh_ui_rect_intersects lh_math_frect_intersects
#    define lh_ui_rect_eq lh_math_frect_eq
#    define lh_ui_rect_intersection lh_math_frect_intersection
#    define lh_ui_rect_union lh_math_frect_union
#    define lh_ui_rect_offset lh_math_frect_offset
#    define lh_ui_rect_inset lh_math_frect_inset
#else
#    include <lh/math/rect.h>

typedef lh_math_rect_t lh_ui_rect_t;

#    define lh_ui_rect_make lh_math_rect_make
#    define lh_ui_rect_make_origin_size lh_math_rect_make_origin_size
#    define lh_ui_rect_from_extent lh_math_rect_from_extent
#    define lh_ui_rect_make_empty lh_math_rect_make_empty
#    define lh_ui_rect_init lh_math_rect_init
#    define lh_ui_rect_init_origin_size lh_math_rect_init_origin_size
#    define lh_ui_rect_get_origin lh_math_rect_get_origin
#    define lh_ui_rect_get_origin_as_const lh_math_rect_get_origin_as_const
#    define lh_ui_rect_get_size lh_math_rect_get_size
#    define lh_ui_rect_get_size_as_const lh_math_rect_get_size_as_const
#    define lh_ui_rect_set_origin lh_math_rect_set_origin
#    define lh_ui_rect_set_size lh_math_rect_set_size
#    define lh_ui_rect_far lh_math_rect_far
#    define lh_ui_rect_to_rect(self) (self)
#    define lh_ui_rect_from_rect(self) (self)
#    define lh_ui_rect_is_empty lh_math_rect_is_empty
#    define lh_ui_rect_contains_point lh_math_rect_contains_point
#    define lh_ui_rect_intersects lh_math_rect_intersects
#    define lh_ui_rect_eq lh_math_rect_eq
#    define lh_ui_rect_intersection lh_math_rect_intersection
#    define lh_ui_rect_union lh_math_rect_union
#    define lh_ui_rect_offset lh_math_rect_offset
#    define lh_ui_rect_inset lh_math_rect_inset
#endif

#endif /* LH_UI_RECT_H */
