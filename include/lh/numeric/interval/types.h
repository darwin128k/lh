/**
 * @file types.h
 * @brief Interval types for non-character numeric types.
 *
 * Provides interval structures for the integer types defined in
 * @ref numeric/types.h (`::lh_short_t`, `::lh_int_t`, `::lh_long_t`,
 * `::lh_llong_t` and their explicit signed/unsigned variants).
 *
 * Each struct holds a @c bounds member (endpoint values in @c first / @c second)
 * and a @c flags member (boundary openness), declared via ::lh_interval_fields.
 * The bounds struct types are defined in `bounds/types.h` (numeric interval).
 *
 * @note Character intervals (for ::lh_uchar_t, ::lh_schar_t) live in
 *       <lh/char/interval/types.h>.
 */

#ifndef LH_NUMERIC_INTERVAL_TYPES_H
#define LH_NUMERIC_INTERVAL_TYPES_H

#include <lh/interval/fields.h>
#include <lh/numeric/interval/bounds/types.h>

/* ── unsigned ──────────────────────────────────────────────────────────── */

/**
 * @struct lh_ushort_interval
 * @brief Interval with 16-bit unsigned endpoints.
 *
 * Stores a numeric interval of ::lh_ushort_t values along with
 * boundary flags indicating whether each endpoint is open or closed.
 *
 * @see lh_ushort_interval_bounds
 * @see lh_interval_flags_t
 * @see lh_interval_fields
 */
struct lh_ushort_interval
{
    /** @c bounds — struct ::lh_ushort_interval_bounds. */
    lh_interval_fields(struct lh_ushort_interval_bounds);
};

/**
 * @struct lh_uint_interval
 * @brief Interval with 32-bit unsigned endpoints.
 *
 * Stores a numeric interval of ::lh_uint_t values along with
 * boundary flags indicating whether each endpoint is open or closed.
 *
 * @see lh_uint_interval_bounds
 * @see lh_interval_flags_t
 * @see lh_interval_fields
 */
struct lh_uint_interval
{
    /** @c bounds — struct ::lh_uint_interval_bounds. */
    lh_interval_fields(struct lh_uint_interval_bounds);
};

/**
 * @struct lh_ulong_interval
 * @brief Interval with platform-width unsigned endpoints.
 *
 * Stores a numeric interval of ::lh_ulong_t values along with
 * boundary flags.
 * Width is architecture-dependent (32 or 64 bits).
 *
 * @note Prefer ::lh_uint_interval or ::lh_ullong_interval when a fixed-width
 *       type is required.
 *
 * @see lh_ulong_interval_bounds
 * @see lh_interval_flags_t
 * @see lh_interval_fields
 */
struct lh_ulong_interval
{
    /** @c bounds — struct ::lh_ulong_interval_bounds. */
    lh_interval_fields(struct lh_ulong_interval_bounds);
};

/**
 * @struct lh_ullong_interval
 * @brief Interval with 64-bit unsigned endpoints.
 *
 * Stores a numeric interval of ::lh_ullong_t values along with
 * boundary flags indicating whether each endpoint is open or closed.
 *
 * @see lh_ullong_interval_bounds
 * @see lh_interval_flags_t
 * @see lh_interval_fields
 */
struct lh_ullong_interval
{
    /** @c bounds — struct ::lh_ullong_interval_bounds. */
    lh_interval_fields(struct lh_ullong_interval_bounds);
};

/* ── signed (explicit) ─────────────────────────────────────────────────── */

/**
 * @struct lh_sshort_interval
 * @brief Interval with 16-bit signed endpoints.
 *
 * Stores a numeric interval of ::lh_sshort_t values along with
 * boundary flags indicating whether each endpoint is open or closed.
 *
 * @see lh_sshort_interval_bounds
 * @see lh_interval_flags_t
 * @see lh_interval_fields
 */
struct lh_sshort_interval
{
    /** @c bounds — struct ::lh_sshort_interval_bounds. */
    lh_interval_fields(struct lh_sshort_interval_bounds);
};

/**
 * @struct lh_sint_interval
 * @brief Interval with 32-bit signed endpoints.
 *
 * Stores a numeric interval of ::lh_sint_t values along with
 * boundary flags indicating whether each endpoint is open or closed.
 *
 * @see lh_sint_interval_bounds
 * @see lh_interval_flags_t
 * @see lh_interval_fields
 */
struct lh_sint_interval
{
    /** @c bounds — struct ::lh_sint_interval_bounds. */
    lh_interval_fields(struct lh_sint_interval_bounds);
};

/**
 * @struct lh_slong_interval
 * @brief Interval with platform-width signed endpoints.
 *
 * Stores a numeric interval of ::lh_slong_t values along with
 * boundary flags.
 * Width is architecture-dependent (32 or 64 bits).
 *
 * @note Prefer ::lh_sint_interval or ::lh_sllong_interval when a fixed-width
 *       type is required.
 *
 * @see lh_slong_interval_bounds
 * @see lh_interval_flags_t
 * @see lh_interval_fields
 */
struct lh_slong_interval
{
    /** @c bounds — struct ::lh_slong_interval_bounds. */
    lh_interval_fields(struct lh_slong_interval_bounds);
};

/**
 * @struct lh_sllong_interval
 * @brief Interval with 64-bit signed endpoints.
 *
 * Stores a numeric interval of ::lh_sllong_t values along with
 * boundary flags indicating whether each endpoint is open or closed.
 *
 * @see lh_sllong_interval_bounds
 * @see lh_interval_flags_t
 * @see lh_interval_fields
 */
struct lh_sllong_interval
{
    /** @c bounds — struct ::lh_sllong_interval_bounds. */
    lh_interval_fields(struct lh_sllong_interval_bounds);
};

/* ── typedefs ──────────────────────────────────────────────────────────── */

/** @typedef lh_ushort_interval_t
 *  @brief Convenience alias for struct ::lh_ushort_interval. */
typedef struct lh_ushort_interval lh_ushort_interval_t;

/** @typedef lh_uint_interval_t
 *  @brief Convenience alias for struct ::lh_uint_interval. */
typedef struct lh_uint_interval lh_uint_interval_t;

/** @typedef lh_ulong_interval_t
 *  @brief Convenience alias for struct ::lh_ulong_interval. */
typedef struct lh_ulong_interval lh_ulong_interval_t;

/** @typedef lh_ullong_interval_t
 *  @brief Convenience alias for struct ::lh_ullong_interval. */
typedef struct lh_ullong_interval lh_ullong_interval_t;

/** @typedef lh_sshort_interval_t
 *  @brief Convenience alias for struct ::lh_sshort_interval. */
typedef struct lh_sshort_interval lh_sshort_interval_t;

/** @typedef lh_sint_interval_t
 *  @brief Convenience alias for struct ::lh_sint_interval. */
typedef struct lh_sint_interval lh_sint_interval_t;

/** @typedef lh_slong_interval_t
 *  @brief Convenience alias for struct ::lh_slong_interval. */
typedef struct lh_slong_interval lh_slong_interval_t;

/** @typedef lh_sllong_interval_t
 *  @brief Convenience alias for struct ::lh_sllong_interval. */
typedef struct lh_sllong_interval lh_sllong_interval_t;

#endif /* LH_NUMERIC_INTERVAL_TYPES_H */