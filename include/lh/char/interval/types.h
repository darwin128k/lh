/**
 * @file types.h
 * @brief Interval types for character types.
 *
 * Provides interval structures for the character types defined in
 * @ref char.h (::lh_uchar_t, ::lh_schar_t). Each struct holds a @c bounds
 * member (endpoint values in @c first / @c second) and a @c flags member
 * (boundary openness), declared via ::lh_interval_fields.
 *
 * @see <lh/char/interval/bounds/types.h> for the bounds struct types.
 * @see <lh/numeric/interval/types.h> for non-character intervals.
 */

#ifndef LH_CHAR_INTERVAL_TYPES_H
#define LH_CHAR_INTERVAL_TYPES_H

#include <lh/char/interval/bounds/types.h>
#include <lh/interval/fields.h>

/* ── unsigned ──────────────────────────────────────────────────────────── */

/**
 * @struct lh_uchar_interval
 * @brief Interval with 8-bit unsigned endpoints.
 *
 * Stores a numeric interval of ::lh_uchar_t values along with
 * boundary flags indicating whether each endpoint is open or closed.
 *
 * @see lh_uchar_interval_bounds
 * @see lh_interval_flags_t
 * @see lh_interval_fields
 */
struct lh_uchar_interval
{
    /** @c bounds — struct ::lh_uchar_interval_bounds. */
    lh_interval_fields(struct lh_uchar_interval_bounds);
};

/* ── signed (explicit) ─────────────────────────────────────────────────── */

/**
 * @struct lh_schar_interval
 * @brief Interval with 8-bit signed endpoints.
 *
 * Stores a numeric interval of ::lh_schar_t values along with
 * boundary flags indicating whether each endpoint is open or closed.
 *
 * @see lh_schar_interval_bounds
 * @see lh_interval_flags_t
 * @see lh_interval_fields
 */
struct lh_schar_interval
{
    /** @c bounds — struct ::lh_schar_interval_bounds. */
    lh_interval_fields(struct lh_schar_interval_bounds);
};

/* ── typedefs ──────────────────────────────────────────────────────────── */

/** @typedef lh_uchar_interval_t
 *  @brief Convenience alias for struct ::lh_uchar_interval. */
typedef struct lh_uchar_interval lh_uchar_interval_t;

/** @typedef lh_schar_interval_t
 *  @brief Convenience alias for struct ::lh_schar_interval. */
typedef struct lh_schar_interval lh_schar_interval_t;

#endif /* LH_CHAR_INTERVAL_TYPES_H */