/**
 * @file status.h
 * @brief ::lh_mb_status_t — what came back for a request: an answer, or why not.
 */

#ifndef LH_NET_MODBUS_STATUS_H
#define LH_NET_MODBUS_STATUS_H

/** @brief What came back. */
typedef enum lh_mb_status
{
    lh_mb_status_ok = 0,     /**< the frame is an answer to what was asked */
    lh_mb_status_partial,    /**< a good frame, but with fewer words than were asked for */
    lh_mb_status_short,      /**< not a whole frame yet: ask for the rest */
    lh_mb_status_crc,        /**< a whole frame with the wrong CRC: the line is noisy */
    lh_mb_status_exception,  /**< the device understood and refused; `exception` says how */
    lh_mb_status_wrong_unit, /**< somebody else's answer on a shared line */
    lh_mb_status_wrong_code, /**< a different question than the one that was sent */
    lh_mb_status_garbage,    /**< a whole frame that is not an answer at all */
    /** @brief The stream is gone: the peer closed it, it failed, or half a request
     *  went out. Nothing more can be read on it in step with the frames, and the
     *  only way back is a new one (::lh_mb_link_set_stream). */
    lh_mb_status_lost
} lh_mb_status_t;

#endif /* LH_NET_MODBUS_STATUS_H */
