/**
 * @file size.h
 * @brief UI size: aliases math integer or float size by FPU option.
 *
 * OFF → ::lh_math_size_t (::lh_math_scalar_t).
 * ON  → ::lh_math_fsize_t (::lh_math_fscalar_t).
 * All `lh_ui_size_*` names forward to the chosen math API.
 */

#ifndef LH_UI_SIZE_H
#define LH_UI_SIZE_H

#include <lh/config.h>
#include <lh/ui/scalar.h>

#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/math/fsize.h>

typedef lh_math_fsize_t lh_ui_size_t;

#    define lh_ui_size_from_extent lh_math_fsize_from_extent
#    define lh_ui_size_get_width lh_math_fsize_get_width
#    define lh_ui_size_get_height lh_math_fsize_get_height
#    define lh_ui_size_set_width lh_math_fsize_set_width
#    define lh_ui_size_set_height lh_math_fsize_set_height
#    define lh_ui_size_to_size lh_math_fsize_to_size
#    define lh_ui_size_from_size lh_math_size_to_fsize
#    define lh_ui_size_eq lh_math_fsize_eq
#    define lh_ui_size_equals lh_math_fsize_equals
#    define lh_ui_size_is_at_least lh_math_fsize_is_at_least
#    define lh_ui_size_is_less lh_math_fsize_is_less
#    define lh_ui_size_is_greater lh_math_fsize_is_greater
#    define lh_ui_size_is_empty lh_math_fsize_is_empty
#    define lh_ui_size_inset lh_math_fsize_inset
#else
#    include <lh/math/size.h>

typedef lh_math_size_t lh_ui_size_t;

#    define lh_ui_size_from_extent lh_math_size_from_extent
#    define lh_ui_size_get_width lh_math_size_get_width
#    define lh_ui_size_get_height lh_math_size_get_height
#    define lh_ui_size_set_width lh_math_size_set_width
#    define lh_ui_size_set_height lh_math_size_set_height
#    define lh_ui_size_to_size(self) (self)
#    define lh_ui_size_from_size(self) (self)
#    define lh_ui_size_eq lh_math_size_eq
#    define lh_ui_size_equals lh_math_size_equals
#    define lh_ui_size_is_at_least lh_math_size_is_at_least
#    define lh_ui_size_is_less lh_math_size_is_less
#    define lh_ui_size_is_greater lh_math_size_is_greater
#    define lh_ui_size_is_empty lh_math_size_is_empty
#    define lh_ui_size_inset lh_math_size_inset
#endif

#endif /* LH_UI_SIZE_H */
