/**
 * @file lanes.h
 * @brief ::lh_lanes_t — which of several queues gets the next turn: by priority, but
 *        none waits for ever.
 *
 * One server, several kinds of work: urgent requests, interactive ones and background
 * work that may lag. Strict priority starves the lower lanes as soon as a higher one
 * is busy; taking turns ignores that an urgent request should not wait behind
 * background work. Lanes do both: the ready lane with the lowest
 * index goes first, **unless** a ready lane has let ::lh_lanes_get_patience other
 * turns go by since it last had one -- then it goes, however low it is.
 *
 * Patience 1 makes two lanes alternate while both are ready; patience 3 gives a lane
 * at least every fourth turn; patience 0 makes a lane wait until every lane above it
 * is idle. Lane 0 never waits for anybody, so its patience does not matter.
 */

#ifndef LH_LANES_H
#define LH_LANES_H

#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/void.h>

/** @brief The most lanes one ::lh_lanes_t picks among. */
#define LH_LANES_MAX 8U

/** @brief Which lane goes next. */
typedef struct lh_lanes
{
    lh_u8_t count;                  /**< how many lanes, up to ::LH_LANES_MAX */
    lh_u8_t patience[LH_LANES_MAX]; /**< turns a ready lane lets go by before it goes first */
    lh_u8_t waited[LH_LANES_MAX];   /**< turns it has let go by since it last had one */
} lh_lanes_t;                       /**< Which lane goes next. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief How many lanes (level 1).
 *
 * @param self The lanes; checked by ::lh_assert_runtime_ref.
 */
lh_u8_t
lh_lanes_get_count(const lh_lanes_t *self);

/**
 * @brief Store how many lanes (level 1).
 *
 * @param self  The lanes; checked by ::lh_assert_runtime_ref.
 * @param count The new value; more than ::LH_LANES_MAX is taken as ::LH_LANES_MAX.
 */
lh_void
lh_lanes_set_count(lh_lanes_t *self, lh_u8_t count);

/**
 * @brief Turns lane @p lane lets go by before it goes first; 0 for never (level 1).
 *
 * @param self The lanes; checked by ::lh_assert_runtime_ref.
 * @param lane Below ::lh_lanes_get_count; not checked.
 */
lh_u8_t
lh_lanes_get_patience(const lh_lanes_t *self, lh_u8_t lane);

/**
 * @brief Store the patience of lane @p lane (level 1).
 *
 * @param self     The lanes; checked by ::lh_assert_runtime_ref.
 * @param lane     Below ::lh_lanes_get_count; not checked.
 * @param patience The new value; 0 waits until the lanes above are idle.
 */
lh_void
lh_lanes_set_patience(lh_lanes_t *self, lh_u8_t lane, lh_u8_t patience);

/**
 * @brief Turns lane @p lane has let go by since it last had one (level 1).
 *
 * @param self The lanes; checked by ::lh_assert_runtime_ref.
 * @param lane Below ::lh_lanes_get_count; not checked.
 */
lh_u8_t
lh_lanes_get_waited(const lh_lanes_t *self, lh_u8_t lane);

/**
 * @brief Store the turns lane @p lane has let go by (level 1).
 *
 * @param self   The lanes; checked by ::lh_assert_runtime_ref.
 * @param lane   Below ::lh_lanes_get_count; not checked.
 * @param waited The new value.
 */
lh_void
lh_lanes_set_waited(lh_lanes_t *self, lh_u8_t lane, lh_u8_t waited);

/**
 * @brief Start @p count lanes, every one with patience 0: plain priority (level 2).
 *
 * @param self  The lanes; checked by ::lh_assert_runtime_ref.
 * @param count How many lanes; more than ::LH_LANES_MAX is taken as ::LH_LANES_MAX.
 */
lh_void
lh_lanes_init(lh_lanes_t *self, lh_u8_t count);

/**
 * @brief Whether bit @p lane of @p ready is set: the lane has work.
 *
 * @param ready Bit `i` set when lane `i` has work.
 * @param lane  Below ::LH_LANES_MAX.
 */
lh_bool_t
lh_lanes_is_ready(lh_u32_t ready, lh_u8_t lane);

/**
 * @brief Whether lane @p lane has run out of patience: it is ready and has let its
 *        patience of turns go by (level 2).
 *
 * @param self  The lanes; checked by ::lh_assert_runtime_ref.
 * @param lane  Below ::lh_lanes_get_count; not checked.
 * @param ready Bit @p lane set when the lane has work.
 */
lh_bool_t
lh_lanes_is_impatient(const lh_lanes_t *self, lh_u8_t lane, lh_u32_t ready);

/**
 * @brief The lane that gets the next turn among the ready ones, and count the turn
 *        (level 3).
 *
 * The lowest impatient lane (::lh_lanes_is_impatient) if there is one, else the
 * lowest ready lane. The lane picked starts waiting from 0; every other ready lane
 * has let one more turn go by.
 *
 * @param self  The lanes; checked by ::lh_assert_runtime_ref.
 * @param ready Bit `i` set when lane `i` has work.
 *
 * @return The lane, or ::lh_lanes_get_count when none is ready (nothing is counted).
 */
lh_u8_t
lh_lanes_pick(lh_lanes_t *self, lh_u32_t ready);

LH_COMPILER_EXTERN_C_END

#endif /* LH_LANES_H */
