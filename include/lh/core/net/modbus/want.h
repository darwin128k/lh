/**
 * @file want.h
 * @brief ::lh_mb_want_t — one register somebody wants.
 */

#ifndef LH_NET_MODBUS_WANT_H
#define LH_NET_MODBUS_WANT_H

#include <lh/core/bool.h>
#include <lh/core/byte/ptr.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/net/modbus/status.h>

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

/**
 * @brief One past the last register @p self wants (level 2): its address plus its
 *        register count, a count of 0 counting as 1.
 *
 * 32 bits, because the end of a want that takes the last register, 65535, is 65536,
 * and in 16 bits that is 0. The planner held it in 16 bits once, and a want for
 * register 65535 behind another want folded into a request that ended before it:
 * the register was planned, counted, and never read.
 *
 * @param self The want; checked by ::lh_assert_runtime_ref.
 *
 * @return 1..65536.
 */
lh_u32_t
lh_mb_want_get_end(const lh_mb_want_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_MODBUS_WANT_H */
