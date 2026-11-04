/**
 * @file math.h
 * @brief Math primitives: arithmetic, comparison, and bitwise macros, plus
 *        integer 2D geometry types (point, size, rectangle).
 *
 * `lh_math_*` covers two things in the same module:
 *
 * - **Arithmetic / comparison / bitwise macros** (`<lh/core/math/ops.h>`):
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
 * The type-specific headers (`<lh/core/math/scalar.h>`, `<lh/core/math/point.h>`,
 * …) declare one thing each — include this file when you need several.
 */

#ifndef LH_MATH_H
#define LH_MATH_H

#include <lh/config.h>

#include <lh/core/math/rect.h>
#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/core/math/fscalar.h>
#    include <lh/core/math/fpoint.h>
#    include <lh/core/math/fsize.h>
#    include <lh/core/math/frect.h>
#    include <lh/core/math/mat4.h>
#    include <lh/core/math/pi.h>
#    include <lh/core/math/quat.h>
#    include <lh/core/math/vec2.h>
#    include <lh/core/math/vec3.h>
#    include <lh/core/math/vec4.h>
#endif

#include <lh/core/math/floor.h>
#include <lh/core/math/isqrt.h>
#include <lh/core/math/rescale.h>

#endif /* LH_MATH_H */
