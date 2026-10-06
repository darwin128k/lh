/**
 * @file rect.h
 * @brief UI rect: aliases math integer or float rect by FPU option.
 *
 * OFF → ::lh_math_irect_t (::lh_math_iscalar_t).
 * ON  → ::lh_math_rect_t (::lh_math_scalar_t).
 * All `lh_ui_rect_*` names forward to the chosen math API.
 */

#ifndef LH_UI_RECT_H
#define LH_UI_RECT_H

#include <lh/config.h>
#include <lh/ui/point.h>
#include <lh/ui/size.h>

#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/math/rect.h>

typedef lh_math_rect_t lh_ui_rect_t;

#    define lh_ui_rect_make lh_math_rect_make
#    define lh_ui_rect_make_empty lh_math_rect_make_empty
#    define lh_ui_rect_get_origin lh_math_rect_get_origin
#    define lh_ui_rect_get_size lh_math_rect_get_size
#    define lh_ui_rect_get_x lh_math_rect_get_x
#    define lh_ui_rect_get_y lh_math_rect_get_y
#    define lh_ui_rect_get_size_width lh_math_rect_get_size_width
#    define lh_ui_rect_get_size_height lh_math_rect_get_size_height
#    define lh_ui_rect_set_origin lh_math_rect_set_origin
#    define lh_ui_rect_set_size lh_math_rect_set_size
#    define lh_ui_rect_to_irect lh_math_rect_to_irect
#    define lh_ui_irect_to_rect lh_math_irect_to_rect
#    define lh_ui_rect_get_width lh_math_rect_get_width
#    define lh_ui_rect_get_height lh_math_rect_get_height
#    define lh_ui_rect_is_empty lh_math_rect_is_empty
#    define lh_ui_rect_eq lh_math_rect_eq
#else
#    include <lh/math/irect.h>

typedef lh_math_irect_t lh_ui_rect_t;

#    define lh_ui_rect_make lh_math_irect_make
#    define lh_ui_rect_make_empty lh_math_irect_make_empty
#    define lh_ui_rect_get_origin lh_math_irect_get_origin
#    define lh_ui_rect_get_size lh_math_irect_get_size
#    define lh_ui_rect_get_x lh_math_irect_get_x
#    define lh_ui_rect_get_y lh_math_irect_get_y
#    define lh_ui_rect_get_size_width lh_math_irect_get_size_width
#    define lh_ui_rect_get_size_height lh_math_irect_get_size_height
#    define lh_ui_rect_set_origin lh_math_irect_set_origin
#    define lh_ui_rect_set_size lh_math_irect_set_size
#    define lh_ui_rect_to_irect(self) (self)
#    define lh_ui_irect_to_rect(self) (self)
#    define lh_ui_rect_get_width lh_math_irect_get_width
#    define lh_ui_rect_get_height lh_math_irect_get_height
#    define lh_ui_rect_is_empty lh_math_irect_is_empty
#    define lh_ui_rect_eq lh_math_irect_eq
#endif

#endif /* LH_UI_RECT_H */
