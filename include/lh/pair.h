/**
 * @file pair.h
 * @brief Two values of any kind (::lh_pair_t): `first` and `second`.
 *
 * The general pair: both halves are ::lh_value_t, so a pair can hold a key
 * and its value, two ends of a range, a name and a pointer — whatever the
 * caller means by it. More specific pairs (a text key with a value, say) are
 * this same pair read with a fixed kind in `first`, not new types.
 *
 * Like ::lh_value_t it owns nothing: copied by plain assignment, no deinit,
 * and what its values point at must outlive it. For a pair of fixed C types
 * instead, embed ::lh_pair_fields directly (`lh/pair/fields.h`).
 */

#ifndef LH_PAIR_H
#define LH_PAIR_H

#include <lh/value.h>

/**
 * @struct lh_pair
 * @brief `first` and `second`, both ::lh_value_t. Fields via ::lh_pair_fields.
 */
struct lh_pair
{
    /** Members: ::lh_pair_fields. */
    lh_pair_fields(lh_value_t, lh_value_t);
};
/** @copybrief lh_pair */
typedef struct lh_pair lh_pair_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Both halves hold nothing (::lh_value_kind_none).
 */
void
lh_pair_init(lh_pair_t *self);

/**
 * @brief Copy @p other into @p self.
 */
void
lh_pair_assign(lh_pair_t *self, const lh_pair_t *other);

/**
 * @brief A pair whose halves both hold nothing.
 */
lh_pair_t
lh_pair_make_none(void);

/**
 * @brief A pair of @p first and @p second.
 */
lh_pair_t
lh_pair_make(lh_value_t first, lh_value_t second);

/**
 * @brief The first half.
 */
const lh_value_t *
lh_pair_get_first(const lh_pair_t *self);

/**
 * @brief The second half.
 */
const lh_value_t *
lh_pair_get_second(const lh_pair_t *self);

/**
 * @brief Replace the first half with @p value.
 */
void
lh_pair_set_first(lh_pair_t *self, lh_value_t value);

/**
 * @brief Replace the second half with @p value.
 */
void
lh_pair_set_second(lh_pair_t *self, lh_value_t value);

/**
 * @brief True when both halves are equal (::lh_value_equals).
 */
lh_bool_t
lh_pair_equals(const lh_pair_t *self, const lh_pair_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_PAIR_H */
