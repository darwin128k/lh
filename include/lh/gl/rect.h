/**
 * @file rect.h
 * @brief UI rect: aliases math integer or float rect by FPU option.
 *
 * OFF → ::lh_math_rect_t (::lh_math_scalar_t).
 * ON  → ::lh_math_frect_t (::lh_math_fscalar_t).
 * All `lh_gl_rect_*` names forward to the chosen math API.
 */

#ifndef LH_GL_RECT_H
#define LH_GL_RECT_H

#include <lh/config.h>
#include <lh/gl/point.h>
#include <lh/gl/size.h>

#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/core/math/frect.h>

/** @brief A UI rectangle: ::lh_math_frect_t when ::LH_LIBRARY_OPTION_MATH_FPU is on. */
typedef lh_math_frect_t lh_gl_rect_t;

/** @brief Name of the float math function @p name for ::lh_gl_rect_t. */
#    define LH_GL_RECT_FN(name) lh_math_frect_##name
/** @brief ::lh_gl_rect_t to ::lh_math_rect_t (::lh_math_frect_to_rect). */
#    define lh_gl_rect_to_rect lh_math_frect_to_rect
/** @brief ::lh_math_rect_t to ::lh_gl_rect_t (::lh_math_rect_to_frect). */
#    define lh_gl_rect_from_rect lh_math_rect_to_frect
#else
#    include <lh/core/math/rect.h>

/** @brief A UI rectangle: ::lh_math_rect_t when ::LH_LIBRARY_OPTION_MATH_FPU is off. */
typedef lh_math_rect_t lh_gl_rect_t;

/** @brief Name of the integer math function @p name for ::lh_gl_rect_t. */
#    define LH_GL_RECT_FN(name) lh_math_rect_##name
/** @brief ::lh_gl_rect_t to ::lh_math_rect_t: the same type here, so @p self copied to @p out. */
#    define lh_gl_rect_to_rect(self, out) (*(out) = (self))
/** @brief ::lh_math_rect_t to ::lh_gl_rect_t: the same type here, so @p self copied to @p out. */
#    define lh_gl_rect_from_rect(self, out) (*(out) = (self))
#endif

/* One list for both paths: a math function missing on either side fails to
 * compile on that side instead of silently drifting. */
/**
 * @name Rect functions
 * Each `lh_gl_rect_<op>` is `lh_math_rect_<op>` (or `lh_math_frect_<op>` with
 * ::LH_LIBRARY_OPTION_MATH_FPU): see that function for the contract.
 * @{
 */
/** @brief The `lh_math_rect_*` / `lh_math_frect_*` function of the same suffix. */
#define lh_gl_rect_from_extent LH_GL_RECT_FN(from_extent)
#define lh_gl_rect_init LH_GL_RECT_FN(init)
#define lh_gl_rect_init_origin_size LH_GL_RECT_FN(init_origin_size)
#define lh_gl_rect_init_empty LH_GL_RECT_FN(init_empty)
#define lh_gl_rect_get_origin_mut LH_GL_RECT_FN(get_origin_mut)
#define lh_gl_rect_get_origin LH_GL_RECT_FN(get_origin)
#define lh_gl_rect_get_size_mut LH_GL_RECT_FN(get_size_mut)
#define lh_gl_rect_get_size LH_GL_RECT_FN(get_size)
#define lh_gl_rect_set_origin LH_GL_RECT_FN(set_origin)
#define lh_gl_rect_set_size LH_GL_RECT_FN(set_size)
#define lh_gl_rect_far LH_GL_RECT_FN(far)
#define lh_gl_rect_origin_min LH_GL_RECT_FN(origin_min)
#define lh_gl_rect_origin_max LH_GL_RECT_FN(origin_max)
#define lh_gl_rect_far_min LH_GL_RECT_FN(far_min)
#define lh_gl_rect_far_max LH_GL_RECT_FN(far_max)
#define lh_gl_rect_is_empty LH_GL_RECT_FN(is_empty)
#define lh_gl_rect_contains_point LH_GL_RECT_FN(contains_point)
#define lh_gl_rect_intersects LH_GL_RECT_FN(intersects)
#define lh_gl_rect_eq LH_GL_RECT_FN(eq)
#define lh_gl_rect_equals LH_GL_RECT_FN(equals)
#define lh_gl_rect_is_at_least LH_GL_RECT_FN(is_at_least)
#define lh_gl_rect_is_less LH_GL_RECT_FN(is_less)
#define lh_gl_rect_is_greater LH_GL_RECT_FN(is_greater)
#define lh_gl_rect_intersection LH_GL_RECT_FN(intersection)
#define lh_gl_rect_union LH_GL_RECT_FN(union)
#define lh_gl_rect_offset LH_GL_RECT_FN(offset)
#define lh_gl_rect_inset LH_GL_RECT_FN(inset)
/** @} */

#endif /* LH_GL_RECT_H */
