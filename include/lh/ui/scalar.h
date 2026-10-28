/**
 * @file scalar.h
 * @brief UI component ::lh_ui_scalar_t — picks a math scalar by FPU option.
 *
 * ::LH_LIBRARY_OPTION_MATH_FPU selects which math scalar UI uses:
 * - OFF → ::lh_math_scalar_t (discrete int geometry)
 * - ON  → ::lh_math_fscalar_t (continuous float geometry)
 *
 * Matching geometry aliases live in <lh/ui/point.h>,
 * <lh/ui/size.h>, <lh/ui/rect.h>.
 *
 * ::lh_ui_scalar writes one number through the chosen math literal macro;
 * ::lh_ui_scalar_of converts any arithmetic expression (a variable, a call).
 */

#ifndef LH_UI_SCALAR_H
#define LH_UI_SCALAR_H

#include <lh/assert/runtime.h>
#include <lh/attribute/force_inline.h>
#include <lh/config.h>

#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/math/fscalar.h>

/** @brief A UI coordinate: ::lh_math_fscalar_t when ::LH_LIBRARY_OPTION_MATH_FPU is on. */
typedef lh_math_fscalar_t lh_ui_scalar_t;

/**
 * @brief One ::lh_ui_scalar_t from a numeric literal (float path).
 */
#    define lh_ui_scalar(n) lh_math_fscalar(n)

#    include <lh/math/fscalar/round.h>

/**
 * @brief Largest whole ::lh_s32_t not above an ::lh_ui_scalar_t.
 */
#    define lh_ui_scalar_floor_s32 lh_math_fscalar_floor_s32

/**
 * @brief Smallest whole ::lh_s32_t not below an ::lh_ui_scalar_t.
 */
#    define lh_ui_scalar_ceil_s32 lh_math_fscalar_ceil_s32
#else
#    include <lh/math/scalar.h>

/** @brief A UI coordinate: ::lh_math_scalar_t when ::LH_LIBRARY_OPTION_MATH_FPU is off. */
typedef lh_math_scalar_t lh_ui_scalar_t;

/**
 * @brief One ::lh_ui_scalar_t from a numeric literal (int path).
 */
#    define lh_ui_scalar(n) lh_math_scalar(n)

#    include <lh/numeric/fixed/types.h>

/**
 * @brief The integer scalar is already whole: a cast.
 */
#    define lh_ui_scalar_floor_s32(v) lh_cast_static(lh_s32_t, (v))

/**
 * @brief The integer scalar is already whole: a cast.
 */
#    define lh_ui_scalar_ceil_s32(v) lh_cast_static(lh_s32_t, (v))
#endif

/**
 * @def lh_ui_scalar_of(v)
 * @brief One ::lh_ui_scalar_t from an arithmetic expression `v` (a cast).
 *
 * ::lh_ui_scalar takes a numeric literal only (the float path pastes a suffix
 * onto it); use this for variables and calls. The float path converts exactly
 * for whole values up to 2^24; the int path truncates toward zero.
 */
#define lh_ui_scalar_of(v) lh_cast_static(lh_ui_scalar_t, (v))

#if LH_LIBRARY_OPTION_MATH_FPU

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p v with its fraction dropped, toward zero, into @p out.
 *
 * Alignment halves leftover room; on the int path that division already
 * truncates toward zero, and this makes the float path land on the same
 * pixels (negative room included).
 *
 * Float path: an exported function. @p out may point at the variable
 * @p v was read from. @p out must not be null (::lh_assert_runtime_ref).
 */
lh_void
lh_ui_scalar_trunc(lh_ui_scalar_t v, lh_ui_scalar_t *out);

LH_COMPILER_EXTERN_C_END

#else

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p v with its fraction dropped, toward zero, into @p out.
 *
 * Int path: the scalar is already whole, so this copies @p v. It is
 * ::LH_ATTRIBUTE_FORCE_INLINE, so call sites pay no call even in Debug or
 * without LTO; same signature and same null check as the float path, and
 * its address can still be taken. @p out must not be null
 * (::lh_assert_runtime_ref).
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_void
lh_ui_scalar_trunc(lh_ui_scalar_t v, lh_ui_scalar_t *out)
{
    lh_assert_runtime_ref(out);
    *out = v;
}

LH_COMPILER_EXTERN_C_END

#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Non-negative leftover room: `max(0, room - own)`.
 *
 * Text wider than its box, or a child taller than the cross slot, has nowhere
 * to go: every alignment starts at the near edge.
 */
lh_ui_scalar_t
lh_ui_scalar_slack(lh_ui_scalar_t room, lh_ui_scalar_t own);

/**
 * @brief Offset that centres @p own in @p room, half-up, same pixels on both
 *        FPU paths: ::lh_ui_scalar_trunc of `(room - own + 1) / 2`, then not
 *        below zero.
 *
 * Matches ::lh_math_scalar_centre on the int path. The leftover half-pixel of
 * an odd slack goes to the far side (right / bottom).
 */
lh_ui_scalar_t
lh_ui_scalar_centre(lh_ui_scalar_t room, lh_ui_scalar_t own);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_SCALAR_H */
