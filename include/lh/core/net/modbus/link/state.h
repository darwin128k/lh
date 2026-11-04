/**
 * @file state.h
 * @brief ::lh_mb_link_state_t — where a link is with the device behind it.
 */

#ifndef LH_NET_MODBUS_LINK_STATE_H
#define LH_NET_MODBUS_LINK_STATE_H

/**
 * @brief Where a link is with the device behind it.
 *
 * @note ::lh_mb_link_failed is a **result, not a resting place**. A question that
 *       timed out, came back with a bad CRC or came back as an exception puts the
 *       link straight back to ::lh_mb_link_idle and this poll reports the failure
 *       on its way. A link stayed in `failed` for the rest of the session, which
 *       meant a device that answers "illegal function" for one register bank was
 *       dropped for good, and a poll loop that keeps finding the link busy spins
 *       for ever instead of moving on to the next device. Whether a device is
 *       *off* rather than *slow* is the app's decision — only it knows how many
 *       failures in a row mean that — and ::lh_mb_link_counters_t.failed is what
 *       it counts them from.
 */
typedef enum lh_mb_link_state
{
    lh_mb_link_idle = 0, /**< nothing in flight */
    lh_mb_link_waiting,  /**< a question is out and no answer has come back yet */
    lh_mb_link_answered, /**< an answer arrived and was parsed; ::lh_mb_link_take takes it */
    lh_mb_link_failed    /**< the stream gave up, or the answer was not ours */
} lh_mb_link_state_t;

#endif /* LH_NET_MODBUS_LINK_STATE_H */
