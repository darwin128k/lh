/**
 * @file item.h
 * @brief ::lh_mb_batch_item_t — how far one value of a batch of writes has got.
 */

#ifndef LH_NET_MODBUS_BATCH_ITEM_H
#define LH_NET_MODBUS_BATCH_ITEM_H

/**
 * @brief How far one value of a batch of writes (::lh_mb_batch_t) has got.
 *
 * ::lh_mb_batch_item_pending and ::lh_mb_batch_item_written are on the way; the rest
 * are the final word on the value. A value is only ::lh_mb_batch_item_confirmed when a
 * read-back showed it on the device.
 */
typedef enum lh_mb_batch_item
{
    lh_mb_batch_item_pending = 0, /**< to be written: not yet, or once more after a mismatch */
    lh_mb_batch_item_written,     /**< the device answered the write; the read-back is next */
    lh_mb_batch_item_confirmed,   /**< written, and read back as written */
    lh_mb_batch_item_refused,     /**< the device answered the write with an exception */
    lh_mb_batch_item_failed,      /**< the write went unanswered: it may or may not be in */
    lh_mb_batch_item_mismatch,    /**< written, but read back as another value every time */
    lh_mb_batch_item_unverified   /**< written, but the read-back went unanswered */
} lh_mb_batch_item_t;

#endif /* LH_NET_MODBUS_BATCH_ITEM_H */