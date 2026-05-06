/**
 * @file interval.h
 * @brief Interval predicates and arithmetic bound-check helpers.
 *
 * Provides macro predicates for interval validity, value/range containment,
 * overlap checks, and arithmetic result checks against interval bounds
 * (add/sub/mul/div) for closed, left-open, right-open, and open forms.
 */

#ifndef LH_UTIL_INTERVAL_H
#define LH_UTIL_INTERVAL_H

#include <lh/util/math.h>
#include <lh/bool.h>

/**
 * @def lh_interval_sub_below_min_exclusive(a, b, min, max, add_overflow_macro)
 * @brief Non-zero if @p a - @p b goes below an exclusive lower bound.
 *
 * Used by subtraction overflow checks for interval kinds where the lower bound
 * is open (strictly greater than @p min). The helper also asks
 * @p add_overflow_macro to guard intermediate @c min + b bound reconstruction.
 */
#define lh_interval_sub_below_min_exclusive(a, b, min, max, add_overflow_macro)                    \
    (lh_math_positive(b) &&                                                                        \
     ((add_overflow_macro(min, b, min, max)) || lh_math_lt(a, lh_math_add(min, b))))

/**
 * @def lh_interval_sub_below_min_inclusive(a, b, min, max, add_overflow_macro)
 * @brief Non-zero if @p a - @p b goes below an inclusive lower bound.
 *
 * Used by subtraction overflow checks for interval kinds where the lower bound
 * is closed (allowing equality with @p min). Includes intermediate
 * reconstruction safety via @p add_overflow_macro.
 */
#define lh_interval_sub_below_min_inclusive(a, b, min, max, add_overflow_macro)                    \
    (!lh_math_negative(b) &&                                                                       \
     ((add_overflow_macro(min, b, min, max)) || lh_math_le(a, lh_math_add(min, b))))

/**
 * @def lh_interval_sub_over_max_exclusive(a, b, min, max, add_overflow_macro)
 * @brief Non-zero if @p a - @p b exceeds an exclusive upper bound.
 *
 * Used by subtraction overflow checks for interval kinds where the upper bound
 * is open (strictly less than @p max). Includes @p add_overflow_macro for
 * safe @c max + b reconstruction.
 */
#define lh_interval_sub_over_max_exclusive(a, b, min, max, add_overflow_macro)                     \
    (lh_math_negative(b) &&                                                                        \
     ((add_overflow_macro(max, b, min, max)) || lh_math_gt(a, lh_math_add(max, b))))
/**
 * @def lh_interval_sub_over_max_inclusive(a, b, min, max, add_overflow_macro)
 * @brief Non-zero if @p a - @p b exceeds an inclusive upper bound.
 *
 * Used by subtraction overflow checks for interval kinds where the upper bound
 * is closed (allowing equality with @p max). Includes @p add_overflow_macro
 * for safe intermediate bound reconstruction.
 */
#define lh_interval_sub_over_max_inclusive(a, b, min, max, add_overflow_macro)                     \
    (!lh_math_positive(b) &&                                                                       \
     ((add_overflow_macro(max, b, min, max)) || lh_math_ge(a, lh_math_add(max, b))))

/**
 * @def lh_interval_zero_product_outside(contains_macro, min, max)
 * @brief Non-zero if multiplicative zero result lies outside interval bounds.
 *
 * Used by multiplication checks when either factor is zero.
 */
#define lh_interval_zero_product_outside(contains_macro, min, max) (!(contains_macro(min, max, 0)))

/**
 * @def lh_interval_mul_pos_b_outside(min_cmp_macro, max_cmp_macro, a, b, min, max)
 * @brief Non-zero if @p a * @p b is outside bounds for positive @p b.
 *
 * @p min_cmp_macro and @p max_cmp_macro encode interval openness/closedness for
 * lower and upper checks respectively.
 */
#define lh_interval_mul_pos_b_outside(min_cmp_macro, max_cmp_macro, a, b, min, max)                \
    ((min_cmp_macro(a, lh_math_div(min, b))) || (max_cmp_macro(a, lh_math_div(max, b))))

/**
 * @def lh_interval_mul_neg_b_outside(min_cmp_macro, max_cmp_macro, a, b, min, max)
 * @brief Non-zero if @p a * @p b is outside bounds for negative @p b.
 *
 * For negative multiplier bounds are swapped before division checks.
 */
#define lh_interval_mul_neg_b_outside(min_cmp_macro, max_cmp_macro, a, b, min, max)                \
    ((min_cmp_macro(a, lh_math_div(max, b))) || (max_cmp_macro(a, lh_math_div(min, b))))

/**
 * @def lh_interval_div_invalid(a, b, min)
 * @brief Non-zero for invalid division preconditions.
 *
 * Detects division by zero and the signed overflow corner case @c min / -1.
 */
#define lh_interval_div_invalid(a, b, min) (lh_math_is_zero(b) || ((a == min) && (b == -1)))

/**
 * @def lh_interval_div_outside(contains_macro, a, b, min, max)
 * @brief Non-zero if division result lies outside interval bounds.
 *
 * Callers should first ensure divisor validity (see ::lh_interval_div_invalid).
 */
#define lh_interval_div_outside(contains_macro, a, b, min, max)                                    \
    (!(contains_macro(min, max, lh_math_div(a, b))))

/**
 * @def lh_interval_closed_is_add_overflow(a, b, min, max)
 * @brief Non-zero if @p a + @p b cannot be represented inside closed interval [@p min, @p max].
 */
#define lh_interval_closed_is_add_overflow(a, b, min, max)                                         \
    (lh_math_add_over_max_exclusive(a, b, max) || lh_math_add_below_min_exclusive(a, b, min))

/**
 * @def lh_interval_lopen_is_add_overflow(a, b, min, max)
 * @brief Non-zero if @p a + @p b cannot be represented inside left-open interval (@p min, @p max].
 */
#define lh_interval_lopen_is_add_overflow(a, b, min, max)                                          \
    (lh_math_add_over_max_exclusive(a, b, max) || lh_math_add_below_min_inclusive(a, b, min))

/**
 * @def lh_interval_ropen_is_add_overflow(a, b, min, max)
 * @brief Non-zero if @p a + @p b cannot be represented inside right-open interval [@p min, @p max).
 */
#define lh_interval_ropen_is_add_overflow(a, b, min, max)                                          \
    (lh_math_add_over_max_inclusive(a, b, max) || lh_math_add_below_min_exclusive(a, b, min))

/**
 * @def lh_interval_open_is_add_overflow(a, b, min, max)
 * @brief Non-zero if @p a + @p b cannot be represented inside open interval (@p min, @p max).
 */
#define lh_interval_open_is_add_overflow(a, b, min, max)                                           \
    (lh_math_add_over_max_inclusive(a, b, max) || lh_math_add_below_min_inclusive(a, b, min))

/**
 * @def lh_interval_closed_is_sub_overflow(a, b, min, max)
 * @brief Non-zero if @p a - @p b cannot be represented inside closed interval [@p min, @p max].
 */
#define lh_interval_closed_is_sub_overflow(a, b, min, max)                                         \
    (lh_interval_sub_below_min_exclusive(a, b, min, max, lh_interval_closed_is_add_overflow) ||    \
     lh_interval_sub_over_max_exclusive(a, b, min, max, lh_interval_closed_is_add_overflow))

/**
 * @def lh_interval_lopen_is_sub_overflow(a, b, min, max)
 * @brief Non-zero if @p a - @p b cannot be represented inside left-open interval (@p min, @p max].
 */
#define lh_interval_lopen_is_sub_overflow(a, b, min, max)                                          \
    (lh_interval_sub_below_min_inclusive(a, b, min, max, lh_interval_lopen_is_add_overflow) ||     \
     lh_interval_sub_over_max_exclusive(a, b, min, max, lh_interval_lopen_is_add_overflow))

/**
 * @def lh_interval_ropen_is_sub_overflow(a, b, min, max)
 * @brief Non-zero if @p a - @p b cannot be represented inside right-open interval [@p min, @p max).
 */
#define lh_interval_ropen_is_sub_overflow(a, b, min, max)                                          \
    (lh_interval_sub_below_min_exclusive(a, b, min, max, lh_interval_ropen_is_add_overflow) ||     \
     lh_interval_sub_over_max_inclusive(a, b, min, max, lh_interval_ropen_is_add_overflow))

/**
 * @def lh_interval_open_is_sub_overflow(a, b, min, max)
 * @brief Non-zero if @p a - @p b cannot be represented inside open interval (@p min, @p max).
 */
#define lh_interval_open_is_sub_overflow(a, b, min, max)                                           \
    (lh_interval_sub_below_min_inclusive(a, b, min, max, lh_interval_open_is_add_overflow) ||      \
     lh_interval_sub_over_max_inclusive(a, b, min, max, lh_interval_open_is_add_overflow))

/**
 * @def lh_interval_closed_is_mul_overflow(a, b, min, max)
 * @brief Non-zero if @p a * @p b cannot be represented inside closed interval [@p min, @p max].
 */
#define lh_interval_closed_is_mul_overflow(a, b, min, max)                                         \
    ((lh_math_is_zero(a) || lh_math_is_zero(b))                                                    \
         ? lh_interval_zero_product_outside(lh_interval_closed_contains_value, min, max)           \
         : (lh_math_positive(b)                                                                    \
                ? lh_interval_mul_pos_b_outside(lh_math_lt, lh_math_gt, a, b, min, max)            \
                : lh_interval_mul_neg_b_outside(lh_math_lt, lh_math_gt, a, b, min, max)))

/**
 * @def lh_interval_lopen_is_mul_overflow(a, b, min, max)
 * @brief Non-zero if @p a * @p b cannot be represented inside left-open interval (@p min, @p max].
 */
#define lh_interval_lopen_is_mul_overflow(a, b, min, max)                                          \
    ((lh_math_is_zero(a) || lh_math_is_zero(b))                                                    \
         ? lh_interval_zero_product_outside(lh_interval_lopen_contains_value, min, max)            \
         : (lh_math_positive(b)                                                                    \
                ? lh_interval_mul_pos_b_outside(lh_math_le, lh_math_gt, a, b, min, max)            \
                : lh_interval_mul_neg_b_outside(lh_math_le, lh_math_gt, a, b, min, max)))

/**
 * @def lh_interval_ropen_is_mul_overflow(a, b, min, max)
 * @brief Non-zero if @p a * @p b cannot be represented inside right-open interval [@p min, @p max).
 */
#define lh_interval_ropen_is_mul_overflow(a, b, min, max)                                          \
    ((lh_math_is_zero(a) || lh_math_is_zero(b))                                                    \
         ? lh_interval_zero_product_outside(lh_interval_ropen_contains_value, min, max)            \
         : (lh_math_positive(b)                                                                    \
                ? lh_interval_mul_pos_b_outside(lh_math_lt, lh_math_gt, a, b, min,                 \
                                                lh_math_sub_one(max))                              \
                : lh_interval_mul_neg_b_outside(lh_math_lt, lh_math_ge, a, b, min, max)))

/**
 * @def lh_interval_open_is_mul_overflow(a, b, min, max)
 * @brief Non-zero if @p a * @p b cannot be represented inside open interval (@p min, @p max).
 */
#define lh_interval_open_is_mul_overflow(a, b, min, max)                                           \
    ((lh_math_is_zero(a) || lh_math_is_zero(b))                                                    \
         ? lh_interval_zero_product_outside(lh_interval_open_contains_value, min, max)             \
         : (lh_math_positive(b)                                                                    \
                ? lh_interval_mul_pos_b_outside(lh_math_le, lh_math_gt, a, b, min,                 \
                                                lh_math_sub_one(max))                              \
                : lh_interval_mul_neg_b_outside(lh_math_le, lh_math_ge, a, b, min, max)))

/**
 * @def lh_interval_closed_is_div_overflow(a, b, min, max)
 * @brief Non-zero if @p a / @p b is undefined or cannot be represented in [@p min, @p max].
 */
#define lh_interval_closed_is_div_overflow(a, b, min, max)                                         \
    (lh_interval_div_invalid(a, b, min) ||                                                         \
     (!lh_math_is_zero(b) &&                                                                       \
      lh_interval_div_outside(lh_interval_closed_contains_value, a, b, min, max)))

/**
 * @def lh_interval_lopen_is_div_overflow(a, b, min, max)
 * @brief Non-zero if @p a / @p b is undefined or cannot be represented in (@p min, @p max].
 */
#define lh_interval_lopen_is_div_overflow(a, b, min, max)                                          \
    (lh_interval_div_invalid(a, b, min) ||                                                         \
     (!lh_math_is_zero(b) &&                                                                       \
      lh_interval_div_outside(lh_interval_lopen_contains_value, a, b, min, max)))

/**
 * @def lh_interval_ropen_is_div_overflow(a, b, min, max)
 * @brief Non-zero if @p a / @p b is undefined or cannot be represented in [@p min, @p max).
 */
#define lh_interval_ropen_is_div_overflow(a, b, min, max)                                          \
    (lh_interval_div_invalid(a, b, min) ||                                                         \
     (!lh_math_is_zero(b) &&                                                                       \
      lh_interval_div_outside(lh_interval_ropen_contains_value, a, b, min, max)))

/**
 * @def lh_interval_open_is_div_overflow(a, b, min, max)
 * @brief Non-zero if @p a / @p b is undefined or cannot be represented in (@p min, @p max).
 */
#define lh_interval_open_is_div_overflow(a, b, min, max)                                           \
    (lh_interval_div_invalid(a, b, min) ||                                                         \
     (!lh_math_is_zero(b) &&                                                                       \
      lh_interval_div_outside(lh_interval_open_contains_value, a, b, min, max)))

/**
 * @def lh_interval_closed_is_valid(lower, upper)
 * @brief Non-zero if closed interval [lower, upper] is valid.
 */
#define lh_interval_closed_is_valid(lower, upper) lh_math_le(lower, upper)

/**
 * @def lh_interval_lopen_is_valid(lower, upper)
 * @brief Non-zero if left-open interval (lower, upper] is valid.
 */
#define lh_interval_lopen_is_valid(lower, upper) lh_math_lt(lower, upper)

/**
 * @def lh_interval_ropen_is_valid(lower, upper)
 * @brief Non-zero if right-open interval [lower, upper) is valid.
 */
#define lh_interval_ropen_is_valid(lower, upper) lh_math_lt(lower, upper)

/**
 * @def lh_interval_open_is_valid(lower, upper)
 * @brief Non-zero if open interval (lower, upper) is valid.
 */
#define lh_interval_open_is_valid(lower, upper) lh_math_lt(lower, upper)

/**
 * @def lh_interval_half_open_midpoint(lo, hi)
 * @brief Integer `lo + (hi - lo) / 2` via ::lh_math_add / ::lh_math_sub / ::lh_math_div
 *        (safer than `(lo + hi) / 2` when bounds are large).
 *
 * Interprets a half-open index range `[lo, hi)` with @p hi exclusive. Building block for
 * ::lh_interval_closed_midpoint and ::lh_interval_*_midpoint.
 *
 * @param lo  Inclusive low index; must be less than or equal to @p hi.
 * @param hi  Exclusive high index.
 */
#define lh_interval_half_open_midpoint(lo, hi)                                                     \
    lh_math_add((lo), lh_math_div(lh_math_sub((hi), (lo)), (2u)))

/**
 * @def lh_interval_closed_midpoint(lower, upper)
 * @brief Integer midpoint for splitting a closed range `[lower, upper]` (both bounds inclusive).
 *
 * Same arithmetic as ::lh_interval_half_open_midpoint; @p upper is the inclusive high index.
 *
 * @see lh_interval_ropen_midpoint()
 */
#define lh_interval_closed_midpoint(lower, upper) lh_interval_half_open_midpoint((lower), (upper))

/**
 * @def lh_interval_ropen_midpoint(lower, upper)
 * @brief Integer midpoint for splitting a right-open range `[lower, upper)` (@p upper exclusive).
 *
 * Typical binary-search slice over indices `lower .. upper - 1`.
 *
 * @see lh_interval_closed_midpoint(), lh_interval_half_open_midpoint()
 */
#define lh_interval_ropen_midpoint(lower, upper) lh_interval_half_open_midpoint((lower), (upper))

/**
 * @def lh_interval_lopen_midpoint(lower, upper)
 * @brief Integer midpoint for splitting a left-open range `(lower, upper]` (@p lower exclusive,
 *        @p upper inclusive on the integer line).
 *
 * Implemented as half-open `[lower, upper + 1)` over valid indices
 * (first interior index is @p lower).
 *
 * @see lh_interval_open_midpoint(), lh_interval_ropen_midpoint()
 */
#define lh_interval_lopen_midpoint(lower, upper)                                                   \
    lh_interval_half_open_midpoint((lower), lh_math_add_one(upper))

/**
 * @def lh_interval_open_midpoint(lower, upper)
 * @brief Integer midpoint for splitting an open range `(lower, upper)` (both bounds exclusive).
 *
 * Implemented as half-open `[lower, upper)` over interior indices (@p upper is exclusive).
 *
 * @see lh_interval_lopen_midpoint(), lh_interval_ropen_midpoint()
 */
#define lh_interval_open_midpoint(lower, upper) lh_interval_half_open_midpoint((lower), (upper))

/**
 * @def lh_interval_closed_get_size(lower, upper)
 * @brief Computes the size of closed interval [lower, upper] as (upper - lower + 1).
 */
#define lh_interval_closed_get_size(lower, upper) lh_math_add_one(lh_math_sub((upper), (lower)))

/**
 * @def lh_interval_lopen_get_size(lower, upper)
 * @brief Computes the size of left-open interval (lower, upper] as (upper - lower).
 */
#define lh_interval_lopen_get_size(lower, upper) lh_math_sub((upper), (lower))

/**
 * @def lh_interval_ropen_get_size(lower, upper)
 * @brief Computes the size of right-open interval [lower, upper) as (upper - lower).
 */
#define lh_interval_ropen_get_size(lower, upper) lh_math_sub((upper), (lower))

/**
 * @def lh_interval_open_get_size(lower, upper)
 * @brief Computes the size of open interval (lower, upper) as (upper - lower - 1).
 */
#define lh_interval_open_get_size(lower, upper) lh_math_sub_one(lh_math_sub((upper), (lower)))

/**
 * @def lh_interval_closed_contains_value(lower, upper, value)
 * @brief Non-zero if @p value belongs to closed interval [lower, upper].
 */
#define lh_interval_closed_contains_value(lower, upper, value)                                     \
    (lh_math_ge(value, lower) && lh_math_le(value, upper))

/**
 * @def lh_interval_lopen_contains_value(lower, upper, value)
 * @brief Non-zero if @p value belongs to left-open interval (lower, upper].
 */
#define lh_interval_lopen_contains_value(lower, upper, value)                                      \
    (lh_math_gt(value, lower) && lh_math_le(value, upper))

/**
 * @def lh_interval_ropen_contains_value(lower, upper, value)
 * @brief Non-zero if @p value belongs to right-open interval [lower, upper).
 */
#define lh_interval_ropen_contains_value(lower, upper, value)                                      \
    (lh_math_ge(value, lower) && lh_math_lt(value, upper))

/**
 * @def lh_interval_open_contains_value(lower, upper, value)
 * @brief Non-zero if @p value belongs to open interval (lower, upper).
 */
#define lh_interval_open_contains_value(lower, upper, value)                                       \
    (lh_math_gt(value, lower) && lh_math_lt(value, upper))

/**
 * @def lh_interval_closed_contains_range(outer_lower, outer_upper, inner_lower, inner_upper)
 * @brief Non-zero if closed interval [inner_lower, inner_upper] is fully inside
 *        [outer_lower, outer_upper].
 *
 * Assumes both ranges were validated by the caller.
 */
#define lh_interval_closed_contains_range(outer_lower, outer_upper, inner_lower, inner_upper)      \
    (lh_math_ge(inner_lower, outer_lower) && lh_math_le(inner_upper, outer_upper))

/**
 * @def lh_interval_lopen_contains_range(outer_lower, outer_upper, inner_lower, inner_upper)
 * @brief Non-zero if left-open interval (inner_lower, inner_upper] is fully inside
 *        (outer_lower, outer_upper].
 *
 * Assumes both ranges were validated by the caller.
 */
#define lh_interval_lopen_contains_range(outer_lower, outer_upper, inner_lower, inner_upper)       \
    (lh_math_ge(inner_lower, outer_lower) && lh_math_le(inner_upper, outer_upper))

/**
 * @def lh_interval_ropen_contains_range(outer_lower, outer_upper, inner_lower, inner_upper)
 * @brief Non-zero if right-open interval [inner_lower, inner_upper) is fully inside
 *        [outer_lower, outer_upper).
 *
 * Assumes both ranges were validated by the caller.
 */
#define lh_interval_ropen_contains_range(outer_lower, outer_upper, inner_lower, inner_upper)       \
    (lh_math_ge(inner_lower, outer_lower) && lh_math_le(inner_upper, outer_upper))

/**
 * @def lh_interval_open_contains_range(outer_lower, outer_upper, inner_lower, inner_upper)
 * @brief Non-zero if open interval (inner_lower, inner_upper) is fully inside
 *        (outer_lower, outer_upper).
 *
 * Assumes both ranges were validated by the caller.
 */
#define lh_interval_open_contains_range(outer_lower, outer_upper, inner_lower, inner_upper)        \
    (lh_math_ge(inner_lower, outer_lower) && lh_math_le(inner_upper, outer_upper))

/**
 * @def lh_interval_closed_overlaps_range(first_lower, first_upper, second_lower, second_upper)
 * @brief Non-zero if closed intervals [first_lower, first_upper] and
 *        [second_lower, second_upper] overlap.
 *
 * Assumes both ranges were validated by the caller.
 */
#define lh_interval_closed_overlaps_range(first_lower, first_upper, second_lower, second_upper)    \
    (lh_math_le(first_lower, second_upper) && lh_math_le(second_lower, first_upper))

/**
 * @def lh_interval_lopen_overlaps_range(first_lower, first_upper, second_lower, second_upper)
 * @brief Non-zero if left-open intervals (first_lower, first_upper] and
 *        (second_lower, second_upper] overlap.
 *
 * Assumes both ranges were validated by the caller.
 */
#define lh_interval_lopen_overlaps_range(first_lower, first_upper, second_lower, second_upper)     \
    (lh_math_lt(first_lower, second_upper) && lh_math_lt(second_lower, first_upper))

/**
 * @def lh_interval_ropen_overlaps_range(first_lower, first_upper, second_lower, second_upper)
 * @brief Non-zero if right-open intervals [first_lower, first_upper) and
 *        [second_lower, second_upper) overlap.
 *
 * Assumes both ranges were validated by the caller.
 */
#define lh_interval_ropen_overlaps_range(first_lower, first_upper, second_lower, second_upper)     \
    (lh_math_lt(first_lower, second_upper) && lh_math_lt(second_lower, first_upper))

/**
 * @def lh_interval_open_overlaps_range(first_lower, first_upper, second_lower, second_upper)
 * @brief Non-zero if open intervals (first_lower, first_upper) and
 *        (second_lower, second_upper) overlap.
 *
 * Assumes both ranges were validated by the caller.
 */
#define lh_interval_open_overlaps_range(first_lower, first_upper, second_lower, second_upper)      \
    (lh_math_lt(first_lower, second_upper) && lh_math_lt(second_lower, first_upper))

/**
 * @def lh_interval_closed_binary_search(type, table, n, key, field_from, result_var, found_var)
 * @brief Binary search in closed interval [0, n] for sorted table by field_from.
 *
 * Searches @p table of @p n elements for @p key using binary search on @p field_from.
 * On match, sets @p result_var to the found element and @p found_var to 1.
 * On no match, @p found_var is set to 0.
 *
 * @param type       Element type (e.g., lh_u32_t for array index)
 * @param table      Array of elements with .field_from field
 * @param n          Number of elements in table
 * @param key        Value to search for
 * @param field_from Field name to compare (e.g., from)
 * @param result_var Variable to store matched element (only set if found)
 * @param found_var  Variable to store search result (1 if found, 0 if not)
 */
#define lh_interval_closed_binary_search(type, table, n, key, field_from, result_var, found_var)   \
    do {                                                                                           \
        type lh_bs_lo = 0U;                                                                        \
        type lh_bs_hi = (n);                                                                       \
        (found_var) = lh_bool_false;                                                               \
        while (lh_math_le(lh_bs_lo, lh_bs_hi)) {                                                   \
            const type lh_bs_mid = lh_interval_closed_midpoint(lh_bs_lo, lh_bs_hi);                \
            const type lh_bs_from = (table)[lh_bs_mid].field_from;                                 \
            if (lh_math_lt((key), lh_bs_from)) {                                                   \
                if (lh_math_is_zero(lh_bs_mid))                                                    \
                    break;                                                                         \
                lh_bs_hi = lh_math_sub_one(lh_bs_mid);                                             \
            } else if (lh_math_gt((key), lh_bs_from)) {                                            \
                lh_bs_lo = lh_math_add_one(lh_bs_mid);                                             \
            } else {                                                                               \
                (result_var) = (table)[lh_bs_mid];                                                 \
                (found_var) = lh_bool_true;                                                        \
                break;                                                                             \
            }                                                                                      \
        }                                                                                          \
    } while (0)

/**
 * @def lh_interval_lopen_binary_search(type, table, n, key, field_from, result_var, found_var)
 * @brief Binary search in left-open interval (0, n] for sorted table by field_from.
 *
 * Searches @p table of @p n elements for @p key using binary search on @p field_from.
 * On match, sets @p result_var to the found element and @p found_var to 1.
 * On no match, @p found_var is set to 0.
 *
 * @param type       Element type (e.g., lh_u32_t for array index)
 * @param table      Array of elements with .field_from field
 * @param n          Number of elements in table
 * @param key        Value to search for
 * @param field_from Field name to compare (e.g., from)
 * @param result_var Variable to store matched element (only set if found)
 * @param found_var  Variable to store search result (1 if found, 0 if not)
 */
#define lh_interval_lopen_binary_search(type, table, n, key, field_from, result_var, found_var)    \
    do {                                                                                           \
        type lh_bs_lo = 1U;                                                                        \
        type lh_bs_hi = (n);                                                                       \
        (found_var) = lh_bool_false;                                                               \
        while (lh_math_le(lh_bs_lo, lh_bs_hi)) {                                                   \
            const type lh_bs_mid = lh_interval_lopen_midpoint(lh_bs_lo, lh_bs_hi);                 \
            const type lh_bs_from = (table)[lh_bs_mid].field_from;                                 \
            if (lh_math_lt((key), lh_bs_from)) {                                                   \
                if (lh_math_le(lh_bs_mid, 1U))                                                     \
                    break;                                                                         \
                lh_bs_hi = lh_math_sub_one(lh_bs_mid);                                             \
            } else if (lh_math_gt((key), lh_bs_from)) {                                            \
                lh_bs_lo = lh_math_add_one(lh_bs_mid);                                             \
            } else {                                                                               \
                (result_var) = (table)[lh_bs_mid];                                                 \
                (found_var) = lh_bool_true;                                                        \
                break;                                                                             \
            }                                                                                      \
        }                                                                                          \
    } while (0)

/**
 * @def lh_interval_ropen_binary_search(type, table, n, key, field_from, result_var, found_var)
 * @brief Binary search in right-open interval [0, n) for sorted table by field_from.
 *
 * Searches @p table of @p n elements for @p key using binary search on @p field_from.
 * On match, sets @p result_var to the found element and @p found_var to 1.
 * On no match, @p found_var is set to 0.
 *
 * @param type       Element type (e.g., lh_u32_t for array index)
 * @param table      Array of elements with .field_from field
 * @param n          Number of elements in table
 * @param key        Value to search for
 * @param field_from Field name to compare (e.g., from)
 * @param result_var Variable to store matched element (only set if found)
 * @param found_var  Variable to store search result (1 if found, 0 if not)
 */
#define lh_interval_ropen_binary_search(type, table, n, key, field_from, result_var, found_var)    \
    do {                                                                                           \
        type lh_bs_lo = 0U;                                                                        \
        type lh_bs_hi = (n);                                                                       \
        (found_var) = lh_bool_false;                                                               \
        while (lh_math_lt(lh_bs_lo, lh_bs_hi)) {                                                   \
            const type lh_bs_mid = lh_interval_ropen_midpoint(lh_bs_lo, lh_bs_hi);                 \
            const type lh_bs_from = (table)[lh_bs_mid].field_from;                                 \
            if (lh_math_lt((key), lh_bs_from)) {                                                   \
                lh_bs_hi = lh_bs_mid;                                                              \
            } else if (lh_math_gt((key), lh_bs_from)) {                                            \
                lh_bs_lo = lh_math_add_one(lh_bs_mid);                                             \
            } else {                                                                               \
                (result_var) = (table)[lh_bs_mid];                                                 \
                (found_var) = lh_bool_true;                                                        \
                break;                                                                             \
            }                                                                                      \
        }                                                                                          \
    } while (0)

/**
 * @def lh_interval_open_binary_search(type, table, n, key, field_from, result_var, found_var)
 * @brief Binary search in open interval (0, n) for sorted table by field_from.
 *
 * Searches @p table of @p n elements for @p key using binary search on @p field_from.
 * On match, sets @p result_var to the found element and @p found_var to 1.
 * On no match, @p found_var is set to 0.
 *
 * @param type       Element type (e.g., lh_u32_t for array index)
 * @param table      Array of elements with .field_from field
 * @param n          Number of elements in table
 * @param key        Value to search for
 * @param field_from Field name to compare (e.g., from)
 * @param result_var Variable to store matched element (only set if found)
 * @param found_var  Variable to store search result (1 if found, 0 if not)
 */
#define lh_interval_open_binary_search(type, table, n, key, field_from, result_var, found_var)     \
    do {                                                                                           \
        type lh_bs_lo = 1U;                                                                        \
        type lh_bs_hi = (n);                                                                       \
        (found_var) = lh_bool_false;                                                               \
        while (lh_math_lt(lh_bs_lo, lh_bs_hi)) {                                                   \
            const type lh_bs_mid = lh_interval_open_midpoint(lh_bs_lo, lh_bs_hi);                  \
            const type lh_bs_from = (table)[lh_bs_mid].field_from;                                 \
            if (lh_math_lt((key), lh_bs_from)) {                                                   \
                lh_bs_hi = lh_bs_mid;                                                              \
            } else if (lh_math_gt((key), lh_bs_from)) {                                            \
                lh_bs_lo = lh_math_add_one(lh_bs_mid);                                             \
            } else {                                                                               \
                (result_var) = (table)[lh_bs_mid];                                                 \
                (found_var) = lh_bool_true;                                                        \
                break;                                                                             \
            }                                                                                      \
        }                                                                                          \
    } while (0)

#endif // LH_UTIL_INTERVAL_H