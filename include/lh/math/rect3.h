/**
 * @file rect3.h
 * @brief An axis-aligned box: ::lh_math_rect3_t of a 2D ::lh_math_rect_t
 *        (an `origin` plus a `size`) plus a `z` (origin z position) and a
 *        `z_depth` (depth extent).
 *
 * Half-open in all three axes: `width = 10` covers columns
 * `[origin.x, origin.x + 10)`, and `z_depth = 5` covers planes
 * `[z, z + 5)`.
 *
 * The 2D rect (`origin` with `x, y` plus `size`) is the core; the 3D box is
 * that plus `z` (origin z position) and `z_depth`. All 2D fields and
 * functions are inherited: read `x, y, width, height` through the
 * ::lh_math_rect3 accessors that delegate to the inner rect; `z` and
 * `z_depth` have their own ::lh_math_rect3_get_z / _set_z / _get_z_depth / _set_z_depth.
 *
 * Empty boxes are represented as `size.width <= 0 || size.height <= 0 ||
 * z_depth <= 0`: ::lh_math_rect3_is_empty, ::lh_math_rect3_intersection, and
 * ::lh_math_rect3_width / _height / _z_depth all share that definition.
 *
 * Fields are not part of the public API: read and mutate them through the
 * `lh_math_rect3_get_*` / `lh_math_rect3_set_*` accessors.
 */

#ifndef LH_MATH_RECT3_H
#define LH_MATH_RECT3_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/coord.h>
#include <lh/math/rect.h>
#include <lh/void.h>

/**
 * @struct lh_math_rect3
 * @typedef lh_math_rect3_t
 * @brief A 3D box: `rect` (the 2D core) plus `z` and `z_depth`.
 */
struct lh_math_rect3
{
    lh_math_rect_t rect;
    lh_math_coord_t z;
    lh_math_coord_t z_depth;
};
typedef struct lh_math_rect3 lh_math_rect3_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── Constructors ────────────────────────────────────────────────────────── */

/**
 * @brief Make a `::lh_math_rect3_t` from origin `x, y, z`, extents, and depth.
 */
lh_math_rect3_t
lh_math_rect3_make(lh_math_coord_t x, lh_math_coord_t y, lh_math_coord_t z,
                  lh_math_coord_t width, lh_math_coord_t height, lh_math_coord_t z_depth);

/**
 * @brief The "no box" sentinel: origin `(0, 0, 0)`, size `(0, 0)`, depth `0`.
 *        ::lh_math_rect3_is_empty returns ::lh_bool_true for this value.
 */
lh_math_rect3_t
lh_math_rect3_make_empty(void);

/* ── Accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief X coordinate of the origin of @p self.
 */
lh_math_coord_t
lh_math_rect3_get_x(const lh_math_rect3_t *self);

/**
 * @brief Y coordinate of the origin of @p self.
 */
lh_math_coord_t
lh_math_rect3_get_y(const lh_math_rect3_t *self);

/**
 * @brief Z coordinate of the origin of @p self.
 */
lh_math_coord_t
lh_math_rect3_get_z(const lh_math_rect3_t *self);

/**
 * @brief Width component of the size of @p self. Zero for empty boxes.
 */
lh_math_coord_t
lh_math_rect3_get_width(const lh_math_rect3_t *self);

/**
 * @brief Height component of the size of @p self. Zero for empty boxes.
 */
lh_math_coord_t
lh_math_rect3_get_height(const lh_math_rect3_t *self);

/**
 * @brief Depth component of @p self. Zero for empty boxes.
 */
lh_math_coord_t
lh_math_rect3_get_z_depth(const lh_math_rect3_t *self);

/**
 * @brief Set the origin `z` of @p self (origin's x, y, width, height, z_depth unchanged).
 */
lh_void
lh_math_rect3_set_z(lh_math_rect3_t *self, lh_math_coord_t z);

/**
 * @brief Set the depth of @p self (everything else unchanged).
 */
lh_void
lh_math_rect3_set_z_depth(lh_math_rect3_t *self, lh_math_coord_t z_depth);

/* ── Queries ────────────────────────────────────────────────────────────── */

/**
 * @brief Width of @p self as a ::lh_math_coord_t. Zero for empty boxes.
 */
lh_math_coord_t
lh_math_rect3_width(const lh_math_rect3_t *self);

/**
 * @brief Height of @p self as a ::lh_math_coord_t. Zero for empty boxes.
 */
lh_math_coord_t
lh_math_rect3_height(const lh_math_rect3_t *self);

/**
 * @brief Depth of @p self as a ::lh_math_coord_t. Zero for empty boxes.
 */
lh_math_coord_t
lh_math_rect3_z_depth(const lh_math_rect3_t *self);

/**
 * @brief Test whether @p self is "empty" (any extent zero or negative).
 */
lh_bool_t
lh_math_rect3_is_empty(const lh_math_rect3_t *self);

/**
 * @brief Element-wise equality.
 */
lh_bool_t
lh_math_rect3_eq(const lh_math_rect3_t *a, const lh_math_rect3_t *b);

/* ── Set ops ────────────────────────────────────────────────────────────── */

/**
 * @brief Intersection of @p a and @p b. Returns ::lh_math_rect3_make_empty if they do
 *        not overlap in all three axes.
 */
lh_math_rect3_t
lh_math_rect3_intersection(const lh_math_rect3_t *a, const lh_math_rect3_t *b);

/**
 * @brief Translate @p self by `(@p dx, @p dy, @p dz)`.
 */
lh_math_rect3_t
lh_math_rect3_offset(const lh_math_rect3_t *self, lh_math_coord_t dx,
                     lh_math_coord_t dy, lh_math_coord_t dz);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_RECT3_H */