/**
 * @file types.h
 * @brief Interval bounds struct types for character types.
 *
 * Provides interval bounds structures for the character types defined in
 * @ref char.h (::lh_uchar_t, ::lh_schar_t).
 *
 * Each struct stores a homogeneous pair of boundary values (members @c first
 * and @c second) via ::lh_interval_bounds_fields; for intervals read them as
 * the lower and upper endpoints in the usual order.
 *
 * These structs are intended to be used as the @c bounds
 * member inside full interval structs built with ::lh_interval_fields.
 *
 * @see lh_interval_bounds_fields
 * @see <lh/numeric/interval/bounds/types.h> for non-character bounds.
 */

#ifndef LH_CHAR_INTERVAL_BOUNDS_TYPES_H
#define LH_CHAR_INTERVAL_BOUNDS_TYPES_H

#include <lh/char.h>
#include <lh/interval/bounds/fields.h>

/* ── unsigned ──────────────────────────────────────────────────────────── */

/**
 * @struct lh_uchar_interval_bounds
 * @brief Interval bounds with 8-bit unsigned endpoints.
 *
 * Both @c first and @c second are ::lh_uchar_t values (unsigned char,
 * range 0..255).
 * Suitable for byte-range or color-channel intervals.
 *
 * @see lh_uchar_t
 * @see lh_interval_bounds_fields
 */
struct lh_uchar_interval_bounds
{
    /** Endpoints of type ::lh_uchar_t (`first`, `second`). */
    lh_interval_bounds_fields(lh_uchar_t);
};

/* ── signed (explicit) ─────────────────────────────────────────────────── */

/**
 * @struct lh_schar_interval_bounds
 * @brief Interval bounds with 8-bit signed endpoints.
 *
 * Both @c first and @c second are ::lh_schar_t values (signed char,
 * range -128..+127).
 * Suitable for small signed delta or offset ranges.
 *
 * @see lh_schar_t
 * @see lh_interval_bounds_fields
 */
struct lh_schar_interval_bounds
{
    /** Endpoints of type ::lh_schar_t (`first`, `second`). */
    lh_interval_bounds_fields(lh_schar_t);
};

/* ── typedefs ──────────────────────────────────────────────────────────── */

/** @typedef lh_uchar_interval_bounds_t
 *  @brief Convenience alias for struct ::lh_uchar_interval_bounds. */
typedef struct lh_uchar_interval_bounds lh_uchar_interval_bounds_t;

/** @typedef lh_schar_interval_bounds_t
 *  @brief Convenience alias for struct ::lh_schar_interval_bounds. */
typedef struct lh_schar_interval_bounds lh_schar_interval_bounds_t;

#endif /* LH_CHAR_INTERVAL_BOUNDS_TYPES_H */