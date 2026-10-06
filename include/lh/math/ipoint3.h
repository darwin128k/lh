/**
 * @file ipoint3.h
 * @brief A 3D point: ::lh_math_ipoint3_t with ::lh_math_iscalar_t `x, y, z`.
 *
 * `x, y` live in `::lh_math_ipoint_t point` — the 2D point is the core, this
 * just adds a `z` field. All 2D fields and functions are inherited: read `x, y`
 * through ::lh_math_ipoint3_get_x / _get_y (which delegate to the inner point),
 * read `z` through ::lh_math_ipoint3_get_z.
 *
 * Like ::lh_math_ipoint_t, this is screen / window coordinates, not
 * floating-point math.
 *
 * Fields are not part of the public API: read and mutate them through the
 * `lh_math_ipoint3_get_*` / `lh_math_ipoint3_set_*` accessors. The struct is
 * defined here only so `point` can be embedded by value.
 */

#ifndef LH_MATH_IPOINT3_H
#define LH_MATH_IPOINT3_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/math/iscalar.h>
#include <lh/math/ipoint.h>
#if LH_LIBRARY_OPTION_MATH_FPU
#    include <lh/math/vec3.h>
#endif
#include <lh/void.h>

/**
 * @struct lh_math_ipoint3
 * @typedef lh_math_ipoint3_t
 * @brief A 3D point: `point` (the 2D core) plus a `z` coordinate.
 */
struct lh_math_ipoint3
{
    lh_math_ipoint_t point;
    lh_math_iscalar_t z;
};
typedef struct lh_math_ipoint3 lh_math_ipoint3_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Constructors ────────────────────────────────────────────────────────── */

/**
 * @brief Make a `::lh_math_ipoint3_t` from explicit coordinates.
 */
lh_math_ipoint3_t
lh_math_ipoint3_make(lh_math_iscalar_t x, lh_math_iscalar_t y, lh_math_iscalar_t z);

/**
 * @brief The origin: `(0, 0, 0)`.
 */
lh_math_ipoint3_t
lh_math_ipoint3_make_empty(void);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief X coordinate of @p self (delegates to the inner ::lh_math_ipoint_t).
 */
lh_math_iscalar_t
lh_math_ipoint3_get_x(const lh_math_ipoint3_t *self);

/**
 * @brief Y coordinate of @p self (delegates to the inner ::lh_math_ipoint_t).
 */
lh_math_iscalar_t
lh_math_ipoint3_get_y(const lh_math_ipoint3_t *self);

/**
 * @brief Z coordinate of @p self.
 */
lh_math_iscalar_t
lh_math_ipoint3_get_z(const lh_math_ipoint3_t *self);

/**
 * @brief Set the X coordinate of @p self (delegates to the inner ::lh_math_ipoint_t).
 */
lh_void
lh_math_ipoint3_set_x(lh_math_ipoint3_t *self, lh_math_iscalar_t x);

/**
 * @brief Set the Y coordinate of @p self (delegates to the inner ::lh_math_ipoint_t).
 */
lh_void
lh_math_ipoint3_set_y(lh_math_ipoint3_t *self, lh_math_iscalar_t y);

/**
 * @brief Set the Z coordinate of @p self.
 */
lh_void
lh_math_ipoint3_set_z(lh_math_ipoint3_t *self, lh_math_iscalar_t z);

/* ── Conversions ─────────────────────────────────────────────────────────── */

#if LH_LIBRARY_OPTION_MATH_FPU
/**
 * @brief Widen @p self to continuous coordinates.
 *
 * Exact: grid coordinates are already integers, so no value is lost.
 *
 * @param self Point to widen.
 * @return `(self.x, self.y, self.z)`.
 */
lh_math_vec3_t
lh_math_ipoint3_to_vec3(lh_math_ipoint3_t self);

/**
 * @brief Narrow @p v to the grid cell it lands in.
 *
 * Rounds to the nearest integer by the same rule as ::lh_math_vec2_to_ipoint,
 * so a grid point narrowed from a transformed position lands on the cell that
 * snapping would select.
 *
 * @param v Continuous position.
 * @return The grid point containing @p v.
 */
lh_math_ipoint3_t
lh_math_vec3_to_ipoint3(lh_math_vec3_t v);
#endif

/* ── Queries ────────────────────────────────────────────────────────────── */

/**
 * @brief Element-wise equality.
 */
lh_bool_t
lh_math_ipoint3_eq(const lh_math_ipoint3_t *a, const lh_math_ipoint3_t *b);

/* ── Set ops ────────────────────────────────────────────────────────────── */

/**
 * @brief Translate @p self by `(@p dx, @p dy, @p dz)`.
 */
lh_math_ipoint3_t
lh_math_ipoint3_offset(const lh_math_ipoint3_t *self, lh_math_iscalar_t dx,
                     lh_math_iscalar_t dy, lh_math_iscalar_t dz);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_IPOINT3_H */