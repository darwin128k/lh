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

LH_COMPILER_EXTERN_C_BEGIN

/** @brief First register wanted. */
lh_u16_t
lh_mb_want_get_address(const lh_mb_want_t *self);

/** @brief Store: first register wanted. Level-1 setter. */
lh_void
lh_mb_want_set_address(lh_mb_want_t *self, lh_u16_t address);

/** @brief How many registers are wanted from @c address on. */
lh_u16_t
lh_mb_want_get_registers(const lh_mb_want_t *self);

/** @brief Store: how many registers are wanted from @c address on. Level-1 setter. */
lh_void
lh_mb_want_set_registers(lh_mb_want_t *self, lh_u16_t registers);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_MODBUS_WANT_H */
