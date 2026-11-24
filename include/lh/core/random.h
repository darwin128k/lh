/**
 * @file random.h
 * @brief Deterministic pseudo-random numbers: ::lh_random_t.
 *
 * xorshift32 (Marsaglia, 2003): one 32-bit state, no heap and no global data.
 * The caller owns the generator and seeds it explicitly (::lh_random_init), so
 * the same seed and the same calls always give the same numbers — simulations
 * and their tests replay exactly. Nothing reads the clock or the OS: the
 * seed is the caller's choice. Not for cryptography: the sequence is
 * predictable from one output.
 *
 * Built bottom-up: the state accessors, one step (::lh_random_next), and a
 * percent chance on top of it (::lh_random_is_hit).
 */

#ifndef LH_RANDOM_H
#define LH_RANDOM_H

#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/random/fields.h>
#include <lh/core/void.h>

/**
 * @def LH_RANDOM_ZERO_SEED_STATE
 * @brief The state ::lh_random_init stores for the seed 0: zero is the one
 *        fixed point of xorshift, which would return 0 forever.
 *
 * 0x9E3779B9 is the 32-bit golden-ratio constant: any nonzero value works,
 * this one is a well-known "nothing up my sleeve" number.
 */
#define LH_RANDOM_ZERO_SEED_STATE 0x9E3779B9U

/**
 * @struct lh_random
 * @brief A deterministic pseudo-random generator.
 */
struct lh_random
{
    /** Members: ::lh_random_fields. */
    lh_random_fields(lh_u32_t);
};
/** @copybrief lh_random */
typedef struct lh_random lh_random_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The generator state of @p self.
 *
 * @param self Not ::lh_null; checked by ::lh_assert_runtime_ref.
 * @return The value of `state`.
 */
lh_u32_t
lh_random_get_state(const lh_random_t *self);

/**
 * @brief Store the generator state of @p self. Level-1 setter: no validation.
 *
 * Saving a state with ::lh_random_get_state and storing it back replays the
 * sequence from that point. A state of 0 makes ::lh_random_next return 0
 * forever; seed with ::lh_random_init instead.
 *
 * @param self Not ::lh_null; checked by ::lh_assert_runtime_ref.
 * @param state The new state.
 */
lh_void
lh_random_set_state(lh_random_t *self, lh_u32_t state);

/**
 * @brief Advance @p self one step and return the new state
 *        (xorshift32, shifts 13, 17, 5).
 *
 * @param self Not ::lh_null; checked by the accessors.
 * @return 1 … 2^32 - 1 for a nonzero state (period 2^32 - 1); 0 only when the
 *         state was set to 0 by hand.
 */
lh_u32_t
lh_random_next(lh_random_t *self);

/**
 * @brief Whether a chance of @p percent percent hits: `next % 100 < percent`.
 *
 * Always advances @p self by exactly one ::lh_random_next, even for 0 and 100,
 * so the rest of the sequence does not depend on the chance asked. The modulo
 * bias of 2^32 - 1 over 100 is below 10^-7 per value.
 *
 * @param self Not ::lh_null; checked by the accessors.
 * @param percent The chance, 0–100: 0 never hits, 100 and above always hit.
 * @return ::lh_bool_true when the chance hit; ::lh_bool_false otherwise.
 */
lh_bool_t
lh_random_is_hit(lh_random_t *self, lh_u8_t percent);

/**
 * @brief Seed @p self: the state becomes @p seed, or
 *        ::LH_RANDOM_ZERO_SEED_STATE when @p seed is 0.
 *
 * @param self Not ::lh_null; checked by ::lh_random_set_state. Its previous
 *             contents are ignored.
 * @param seed Any value; the caller picks it (a constant for a replayable run).
 */
lh_void
lh_random_init(lh_random_t *self, lh_u32_t seed);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RANDOM_H */
