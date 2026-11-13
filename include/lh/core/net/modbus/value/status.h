/**
 * @file status.h
 * @brief ::lh_mb_value_status_t — how much a value held in an ::lh_mb_image_t can be
 *        trusted.
 */

#ifndef LH_NET_MODBUS_VALUE_STATUS_H
#define LH_NET_MODBUS_VALUE_STATUS_H

/**
 * @brief How much a value held in an image can be trusted.
 *
 * A failed read does not throw a value away: the register still holds what the device
 * said last, and a panel that blanks a temperature because one request waited out a
 * busy controller shows less than it knows. The status says what kind of value it is,
 * and the time of the last update (::lh_mb_image_get_updated_us) says how old.
 */
typedef enum lh_mb_value_status
{
    lh_mb_value_unread = 0, /**< never read: there is no value yet */
    lh_mb_value_fresh,      /**< the last read of it succeeded */
    lh_mb_value_stale,      /**< a later read failed; the value is the one read before */
    lh_mb_value_refused     /**< the device refused the read (an exception); no value */
} lh_mb_value_status_t;

#endif /* LH_NET_MODBUS_VALUE_STATUS_H */
