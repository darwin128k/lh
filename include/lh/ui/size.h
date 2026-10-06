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

/** @brief Name of the float math function @p name for ::lh_ui_size_t. */
#    define LH_UI_SIZE_FN(name) lh_math_fsize_##name
#    define lh_ui_size_to_size lh_math_fsize_to_size
#    define lh_ui_size_from_size lh_math_size_to_fsize
#else
#    include <lh/math/size.h>

typedef lh_math_size_t lh_ui_size_t;

/** @brief Name of the integer math function @p name for ::lh_ui_size_t. */
#    define LH_UI_SIZE_FN(name) lh_math_size_##name
#    define lh_ui_size_to_size(self) (self)
#    define lh_ui_size_from_size(self) (self)
#endif

/* One list for both paths: a math function missing on either side fails to
 * compile on that side instead of silently drifting. */
#define lh_ui_size_init LH_UI_SIZE_FN(init)
#define lh_ui_size_init_empty LH_UI_SIZE_FN(init_empty)
#define lh_ui_size_from_extent LH_UI_SIZE_FN(from_extent)
#define lh_ui_size_get_width LH_UI_SIZE_FN(get_width)
#define lh_ui_size_get_height LH_UI_SIZE_FN(get_height)
#define lh_ui_size_set_width LH_UI_SIZE_FN(set_width)
#define lh_ui_size_set_height LH_UI_SIZE_FN(set_height)
#define lh_ui_size_eq LH_UI_SIZE_FN(eq)
#define lh_ui_size_equals LH_UI_SIZE_FN(equals)
#define lh_ui_size_is_at_least LH_UI_SIZE_FN(is_at_least)
#define lh_ui_size_is_less LH_UI_SIZE_FN(is_less)
#define lh_ui_size_is_greater LH_UI_SIZE_FN(is_greater)
#define lh_ui_size_is_empty LH_UI_SIZE_FN(is_empty)
#define lh_ui_size_inset LH_UI_SIZE_FN(inset)

#endif /* LH_UI_SIZE_H */
