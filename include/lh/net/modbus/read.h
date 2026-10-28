/**
 * @file read.h
 * @brief ::lh_mb_read_t — one request that answers for a run of wants.
 */

#ifndef LH_NET_MODBUS_READ_H
#define LH_NET_MODBUS_READ_H

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

LH_COMPILER_EXTERN_C_BEGIN

/** @brief First register this read asks the device for. */
lh_u16_t
lh_mb_read_get_start(const lh_mb_read_t *self);

/** @brief Store: first register this read asks the device for. Level-1 setter. */
lh_void
lh_mb_read_set_start(lh_mb_read_t *self, lh_u16_t start);

/** @brief How many registers this read asks for. */
lh_u16_t
lh_mb_read_get_count(const lh_mb_read_t *self);

/** @brief Store: how many registers this read asks for. Level-1 setter. */
lh_void
lh_mb_read_set_count(lh_mb_read_t *self, lh_u16_t count);

/** @brief Index into the wants of the first one this read covers. */
lh_u16_t
lh_mb_read_get_first(const lh_mb_read_t *self);

/** @brief Store: index into the wants of the first one this read covers. Level-1 setter. */
lh_void
lh_mb_read_set_first(lh_mb_read_t *self, lh_u16_t first);

/** @brief Index of the last want this read covers. */
lh_u16_t
lh_mb_read_get_last(const lh_mb_read_t *self);

/** @brief Store: index of the last want this read covers. Level-1 setter. */
lh_void
lh_mb_read_set_last(lh_mb_read_t *self, lh_u16_t last);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_MODBUS_READ_H */
