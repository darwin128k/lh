/**
 * @file size.h
 * @brief UI size: aliases math integer or float size by FPU option.
 *
 * OFF → ::lh_math_isize_t (::lh_math_iscalar_t).
 * ON  → ::lh_math_size_t (::lh_math_scalar_t).
 * All `lh_ui_size_*` names forward to the chosen math API.
 */

#ifndef LH_UI_SIZE_H
#define LH_UI_SIZE_H

#include <lh/config.h>
#include <lh/ui/scalar.h>

#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/math/size.h>

typedef lh_math_size_t lh_ui_size_t;

#    define lh_ui_size_make lh_math_size_make
#    define lh_ui_size_make_empty lh_math_size_make_empty
#    define lh_ui_size_get_width lh_math_size_get_width
#    define lh_ui_size_get_height lh_math_size_get_height
#    define lh_ui_size_set_width lh_math_size_set_width
#    define lh_ui_size_set_height lh_math_size_set_height
#    define lh_ui_size_to_isize lh_math_size_to_isize
#    define lh_ui_isize_to_size lh_math_isize_to_size
#    define lh_ui_size_eq lh_math_size_eq
#else
#    include <lh/math/isize.h>

typedef lh_math_isize_t lh_ui_size_t;

#    define lh_ui_size_make lh_math_isize_make
#    define lh_ui_size_make_empty lh_math_isize_make_empty
#    define lh_ui_size_get_width lh_math_isize_get_width
#    define lh_ui_size_get_height lh_math_isize_get_height
#    define lh_ui_size_set_width lh_math_isize_set_width
#    define lh_ui_size_set_height lh_math_isize_set_height
#    define lh_ui_size_to_isize(self) (self)
#    define lh_ui_isize_to_size(self) (self)
#    define lh_ui_size_eq lh_math_isize_eq
#endif

#endif /* LH_UI_SIZE_H */
