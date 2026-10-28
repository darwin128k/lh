/**
 * @file read.h
 * @brief ::lh_mb_read_t — one request that answers for a run of wants.
 */

#ifndef LH_NET_MODBUS_READ_H
#define LH_NET_MODBUS_READ_H

#include <lh/bool.h>
#include <lh/byte/ptr.h>
#include <lh/compiler/extern/c.h>
#include <lh/util/ptr.h>
#include <lh/net/modbus/status.h>
#include <lh/net/modbus/want.h>

/**
 * @brief One request that answers for a run of wants.
 */
typedef struct lh_mb_read
{
    lh_u16_t start; /**< first register this request asks for */
    lh_u16_t count; /**< how many registers it asks for */
    lh_u16_t first; /**< index into the wants this one starts at */
    lh_u16_t last;  /**< index of the last want it covers */
} lh_mb_read_t;

#endif /* LH_NET_MODBUS_READ_H */
