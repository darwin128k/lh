/**
 * @file math.h
 * @brief Math primitives: arithmetic, comparison, and bitwise macros, plus
 *        integer 2D geometry types (point, size, rectangle).
 *
 * `lh_math_*` covers two things in the same module:
 *
 * - **Arithmetic / comparison / bitwise macros** (`<lh/math/ops.h>`):
 *   fully parenthesized expression-level wrappers for common operations.
 * - **Integer 2D geometry** (`point`, `size`, `rect`): screen / window
 *   value types on ::lh_math_scalar_t (`int`) — construct, query
 *   (empty / contains-point / intersects / equal), and combine
 *   (offset, inset, intersection, union).
 * - **Float geometry / 3D** (`fpoint`, `fsize`, `frect`, `vec` / `quat` /
 *   `mat4`) use ::lh_math_fscalar_t (`float`) and are built when
 *   ::LH_LIBRARY_OPTION_MATH_FPU is ON. UI picks which scalar (and which
 *   geometry) through <lh/ui/scalar.h> and friends.
 *
 * Requires nothing from `lh/os`. Safe in STM/embedded.
 *
 * The type-specific headers (`<lh/math/scalar.h>`, `<lh/math/point.h>`,
 * …) declare one thing each — include this file when you need several.
 */

#ifndef LH_MATH_H
#define LH_MATH_H

#include <lh/config.h>

#include <lh/math/rect.h>
#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/math/fscalar.h>
#    include <lh/math/fpoint.h>
#    include <lh/math/fsize.h>
#    include <lh/math/frect.h>
#    include <lh/math/mat4.h>
#    include <lh/math/pi.h>
#    include <lh/math/quat.h>
#    include <lh/math/vec2.h>
#    include <lh/math/vec3.h>
#    include <lh/math/vec4.h>
#endif

#include <lh/math/floor.h>
#include <lh/math/isqrt.h>
#include <lh/math/rescale.h>

#endif /* LH_MATH_H */
