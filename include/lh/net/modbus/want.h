/**
 * @file want.h
 * @brief ::lh_mb_want_t — one register somebody wants.
 */

#ifndef LH_NET_MODBUS_WANT_H
#define LH_NET_MODBUS_WANT_H

#include <lh/bool.h>
#include <lh/byte/ptr.h>
#include <lh/compiler/extern/c.h>
#include <lh/net/modbus/status.h>

/**
 * @brief One register somebody wants.
 *
 * Address-ordered, which is what makes coalescing a single pass and what
 * ::lh_cfg_map_t already gives: a device map is sorted by address because a poll
 * reads it in order.
 */
typedef struct lh_mb_want
{
    lh_u16_t address;   /**< first register wanted */
    lh_u16_t registers; /**< how many from there */
} lh_mb_want_t;

#endif /* LH_NET_MODBUS_WANT_H */
