/**
 * @file state.h
 * @brief Bit flags describing stored @c first / @c second endpoints of ::lh_memory_bounds_t
 *        (::lh_memory_bounds_state_t).
 *
 * Classification follows the half-open interval model <tt>[first, second)</tt> used by
 * ::lh_memory_bounds_t and is based on null-endpoint patterns and pointer ordering when
 * both endpoints are non-null.
 *
 * Values are combined with bitwise OR into a single ::lh_byte_t-sized word,
 * except ::lh_memory_bounds_state_has_data and ::lh_memory_bounds_state_reversed,
 * which are classified ordinals (not OR-ed bit masks).
 *
 * @note Ordering flags (::lh_memory_bounds_state_has_data,
 *       ::lh_memory_bounds_state_reversed) are only set when neither
 *       ::lh_memory_bounds_state_null_begin nor ::lh_memory_bounds_state_null_end is set.
 *
 *       If both pointers are non-null and equal, the word matches
 *       ::lh_memory_bounds_state_empty.
 */

#ifndef LH_MEMORY_BOUNDS_STATE_H
#define LH_MEMORY_BOUNDS_STATE_H

#include <lh/byte.h>
#include <lh/util/bit.h>

/**
 * @def lh_memory_bounds_state_empty
 * @brief No flags set (`0`). Starting value before classification; after classification,
 *        both endpoints were non-null and equal (degenerate half-open span).
 */
#define lh_memory_bounds_state_empty 0

/**
 * @def lh_memory_bounds_state_null_begin
 * @brief Bit mask: stored @c first (begin) was null when inspected.
 */
#define lh_memory_bounds_state_null_begin lh_bit_mask(0)

/**
 * @def lh_memory_bounds_state_null_end
 * @brief Bit mask: stored @c second (end) was null when inspected.
 */
#define lh_memory_bounds_state_null_end lh_bit_mask(1)

/**
 * @def lh_memory_bounds_state_uninitialized
 * @brief Mask of both null-endpoint bits (::lh_memory_bounds_state_null_begin |
 *        ::lh_memory_bounds_state_null_end): classified state when both stored endpoints were null.
 */
#define lh_memory_bounds_state_uninitialized                                                        \
    (lh_memory_bounds_state_null_begin | lh_memory_bounds_state_null_end)

/**
 * @def lh_memory_bounds_state_has_data
 * @brief Classified value (not a null bitmask):
 *        both pointers non-null and @c first < @c second.
 */
#define lh_memory_bounds_state_has_data 4

/**
 * @def lh_memory_bounds_state_reversed
 * @brief Classified value (not a null bitmask):
 *        both pointers non-null and @c first > @c second.
 */
#define lh_memory_bounds_state_reversed 5

/**
 * @typedef lh_memory_bounds_state_t
 * @brief Bit-flag / classification word type for range endpoint inspection (::lh_byte_t).
 */
typedef lh_byte_t lh_memory_bounds_state_t;

#endif // LH_MEMORY_BOUNDS_STATE_H