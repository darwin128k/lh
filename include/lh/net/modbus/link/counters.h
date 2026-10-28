/**
 * @file counters.h
 * @brief ::lh_mb_link_counters_t — the numbers a bench reads and a status bar shows.
 */

#ifndef LH_NET_MODBUS_LINK_COUNTERS_H
#define LH_NET_MODBUS_LINK_COUNTERS_H

#include <lh/numeric/fixed/types.h>
#include <lh/compiler/extern/c.h>
#include <lh/void.h>

/**
 * @brief The numbers a bench reads and a status bar shows.
 *
 * Counted because "it is fast" is a claim and these are the evidence for it: how
 * many questions went out, how many answers came back whole, how many devices
 * stopped answering, and how long the answers took once they were in.
 */
typedef struct lh_mb_link_counters
{
    lh_u32_t sent;   /**< requests written to the stream */
    lh_u32_t done;   /**< answers parsed in full */
    lh_u32_t failed; /**< timeouts and stream errors, and nothing else */
    /** @brief Answers the device understood and refused: a Modbus exception.
     *
     *  **This is an answer, and it is the most useful one a device can give.** It
     *  says the line is good, the framing is right, the function code was
     *  understood, and this address is not on the device.
     *
     *  It used to be counted in ::failed, and that was a lie the app could not see
     *  through: the counter's own words said "timeouts and transport errors", the
     *  code counted exceptions there too, and a card that asked for one register
     *  the device did not have looked exactly like a device that had stopped
     *  answering. That is not a small difference here — the client's rule for
     *  "this device is off line" is a count of failures in a row, so a **healthy**
     *  controller with a card slightly wider than its firmware would be taken off
     *  line by its own app while every other device kept being polled.
     *
     *  Against a real controller on a panel this is not a corner: a card is a
     *  union of everything the family ever had, and asking a JL211 for a JL204C7
     *  register is refused, politely, forever.
     */
    lh_u32_t refused; /**< answers that were Modbus exceptions: the device is talking */
    lh_u32_t bytes;   /**< request and answer bytes both ways */
    lh_u64_t us_busy; /**< microseconds a request spent in flight */
} lh_mb_link_counters_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The count of requests written to the stream (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_link_counters_get_sent(const lh_mb_link_counters_t *self);

/**
 * @brief Store the count of requests written to the stream (level 1).
 *
 * A level-1 setter: the value is stored as given. The link itself counts
 * through the count_ and add_ bricks below.
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 * @param sent The new count.
 */
lh_void
lh_mb_link_counters_set_sent(lh_mb_link_counters_t *self, lh_u32_t sent);

/**
 * @brief The count of answers parsed in full (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_link_counters_get_done(const lh_mb_link_counters_t *self);

/**
 * @brief Store the count of answers parsed in full (level 1).
 *
 * A level-1 setter: the value is stored as given. The link itself counts
 * through the count_ and add_ bricks below.
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 * @param done The new count.
 */
lh_void
lh_mb_link_counters_set_done(lh_mb_link_counters_t *self, lh_u32_t done);

/**
 * @brief The count of timeouts and stream errors, and nothing else (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_link_counters_get_failed(const lh_mb_link_counters_t *self);

/**
 * @brief Store the count of timeouts and stream errors, and nothing else (level 1).
 *
 * A level-1 setter: the value is stored as given. The link itself counts
 * through the count_ and add_ bricks below.
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 * @param failed The new count.
 */
lh_void
lh_mb_link_counters_set_failed(lh_mb_link_counters_t *self, lh_u32_t failed);

/**
 * @brief The count of answers that were Modbus exceptions: the device is talking (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_link_counters_get_refused(const lh_mb_link_counters_t *self);

/**
 * @brief Store the count of answers that were Modbus exceptions: the device is talking (level 1).
 *
 * A level-1 setter: the value is stored as given. The link itself counts
 * through the count_ and add_ bricks below.
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 * @param refused The new count.
 */
lh_void
lh_mb_link_counters_set_refused(lh_mb_link_counters_t *self, lh_u32_t refused);

/**
 * @brief The count of request and answer bytes, both ways (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_link_counters_get_bytes(const lh_mb_link_counters_t *self);

/**
 * @brief Store the count of request and answer bytes, both ways (level 1).
 *
 * A level-1 setter: the value is stored as given. The link itself counts
 * through the count_ and add_ bricks below.
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 * @param bytes The new count.
 */
lh_void
lh_mb_link_counters_set_bytes(lh_mb_link_counters_t *self, lh_u32_t bytes);

/**
 * @brief The count of microseconds requests spent in flight (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_u64_t
lh_mb_link_counters_get_us_busy(const lh_mb_link_counters_t *self);

/**
 * @brief Store the count of microseconds requests spent in flight (level 1).
 *
 * A level-1 setter: the value is stored as given. The link itself counts
 * through the count_ and add_ bricks below.
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 * @param us_busy The new count.
 */
lh_void
lh_mb_link_counters_set_us_busy(lh_mb_link_counters_t *self, lh_u64_t us_busy);

/**
 * @brief Level 2: one more request went out.
 *
 * Adds one to ::lh_mb_link_counters_get_sent. The count wraps at 2^32; that is
 * not checked.
 *
 * @param self The counters.
 */
lh_void
lh_mb_link_counters_count_sent(lh_mb_link_counters_t *self);

/**
 * @brief Level 2: one more answer came back whole.
 *
 * Adds one to ::lh_mb_link_counters_get_done. The count wraps at 2^32; that is
 * not checked.
 *
 * @param self The counters.
 */
lh_void
lh_mb_link_counters_count_done(lh_mb_link_counters_t *self);

/**
 * @brief Level 2: one more timeout or stream error.
 *
 * Adds one to ::lh_mb_link_counters_get_failed. The count wraps at 2^32; that is
 * not checked.
 *
 * @param self The counters.
 */
lh_void
lh_mb_link_counters_count_failed(lh_mb_link_counters_t *self);

/**
 * @brief Level 2: one more Modbus exception answer.
 *
 * Adds one to ::lh_mb_link_counters_get_refused. The count wraps at 2^32; that is
 * not checked.
 *
 * @param self The counters.
 */
lh_void
lh_mb_link_counters_count_refused(lh_mb_link_counters_t *self);

/**
 * @brief Level 2: add @p bytes to the bytes moved both ways.
 *
 * The sum wraps at 2^32; that is not checked.
 *
 * @param self  The counters.
 * @param bytes Bytes written or read.
 */
lh_void
lh_mb_link_counters_add_bytes(lh_mb_link_counters_t *self, lh_u32_t bytes);

/**
 * @brief Level 2: add @p us to the time requests spent in flight.
 *
 * @param self The counters.
 * @param us   Microseconds one request took.
 */
lh_void
lh_mb_link_counters_add_us_busy(lh_mb_link_counters_t *self, lh_u64_t us);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_MODBUS_LINK_COUNTERS_H */
