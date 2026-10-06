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

/** @brief Name of the float math function @p name for ::lh_ui_rect_t. */
#    define LH_UI_RECT_FN(name) lh_math_frect_##name
#    define lh_ui_rect_to_rect lh_math_frect_to_rect
#    define lh_ui_rect_from_rect lh_math_rect_to_frect
#else
#    include <lh/math/rect.h>

typedef lh_math_rect_t lh_ui_rect_t;

/** @brief Name of the integer math function @p name for ::lh_ui_rect_t. */
#    define LH_UI_RECT_FN(name) lh_math_rect_##name
#    define lh_ui_rect_to_rect(self) (self)
#    define lh_ui_rect_from_rect(self) (self)
#endif

/* One list for both paths: a math function missing on either side fails to
 * compile on that side instead of silently drifting. */
#define lh_ui_rect_from_extent LH_UI_RECT_FN(from_extent)
#define lh_ui_rect_init LH_UI_RECT_FN(init)
#define lh_ui_rect_init_origin_size LH_UI_RECT_FN(init_origin_size)
#define lh_ui_rect_init_empty LH_UI_RECT_FN(init_empty)
#define lh_ui_rect_get_origin LH_UI_RECT_FN(get_origin)
#define lh_ui_rect_get_origin_as_const LH_UI_RECT_FN(get_origin_as_const)
#define lh_ui_rect_get_size LH_UI_RECT_FN(get_size)
#define lh_ui_rect_get_size_as_const LH_UI_RECT_FN(get_size_as_const)
#define lh_ui_rect_set_origin LH_UI_RECT_FN(set_origin)
#define lh_ui_rect_set_size LH_UI_RECT_FN(set_size)
#define lh_ui_rect_far LH_UI_RECT_FN(far)
#define lh_ui_rect_origin_min LH_UI_RECT_FN(origin_min)
#define lh_ui_rect_origin_max LH_UI_RECT_FN(origin_max)
#define lh_ui_rect_far_min LH_UI_RECT_FN(far_min)
#define lh_ui_rect_far_max LH_UI_RECT_FN(far_max)
#define lh_ui_rect_is_empty LH_UI_RECT_FN(is_empty)
#define lh_ui_rect_contains_point LH_UI_RECT_FN(contains_point)
#define lh_ui_rect_intersects LH_UI_RECT_FN(intersects)
#define lh_ui_rect_eq LH_UI_RECT_FN(eq)
#define lh_ui_rect_equals LH_UI_RECT_FN(equals)
#define lh_ui_rect_is_at_least LH_UI_RECT_FN(is_at_least)
#define lh_ui_rect_is_less LH_UI_RECT_FN(is_less)
#define lh_ui_rect_is_greater LH_UI_RECT_FN(is_greater)
#define lh_ui_rect_intersection LH_UI_RECT_FN(intersection)
#define lh_ui_rect_union LH_UI_RECT_FN(union)
#define lh_ui_rect_offset LH_UI_RECT_FN(offset)
#define lh_ui_rect_inset LH_UI_RECT_FN(inset)

#endif /* LH_UI_RECT_H */
