/**
 * @file result.h
 * @brief ::lh_mb_server_result_t — what one ::lh_mb_server_poll did.
 */

#ifndef LH_NET_MODBUS_SERVER_RESULT_H
#define LH_NET_MODBUS_SERVER_RESULT_H

/** @brief What one poll of a server did. */
typedef enum lh_mb_server_result
{
    lh_mb_server_idle = 0,  /**< no whole request yet */
    lh_mb_server_answered,  /**< a request for this unit was done and answered */
    lh_mb_server_refused,   /**< a request for this unit was answered with an exception */
    lh_mb_server_broadcast, /**< a write to every unit was done; nobody answers one */
    lh_mb_server_ignored,   /**< a whole frame for another unit, stepped over */
    /** @brief Bytes that are no request: a bad CRC, a code that cannot be sized
     *  with no silence to end it, noise. They are dropped, and the next request
     *  is looked for after them. */
    lh_mb_server_broken,
    /** @brief The stream is gone or out of step: it ended, failed, took half an
     *  answer, or (Modbus TCP) carried a header that is no Modbus header. The
     *  application closes it and serves a new one (::lh_mb_server_set_stream). */
    lh_mb_server_lost
} lh_mb_server_result_t;

#endif /* LH_NET_MODBUS_SERVER_RESULT_H */
