/**
 * @file health.h
 * @brief ::lh_mb_health_t — whether a device is still there: failures in a row against
 *        a limit.
 *
 * One request that fails says little: a controller busy for a second, a packet lost on
 * a VPN, a frame hit by noise. Declaring the device gone on the first failure is what
 * makes a master show "no link" every few seconds on a line that is working (a real
 * controller was measured holding requests for 1.05-1.2 s every 1.3-7 s). The device is
 * gone after
 * ::lh_mb_health_get_limit failures **in a row**; any answer, a refusal included,
 * starts the count again -- a device that says "no" is talking.
 */

#ifndef LH_NET_MODBUS_HEALTH_H
#define LH_NET_MODBUS_HEALTH_H

#include <lh/core/bool.h>
#include <lh/core/cast/static.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/void.h>

/**
 * @brief Failures in a row after which a device counts as gone, by default.
 *
 * Ten: long enough that a busy device or a lost packet never counts as a lost device,
 * short enough that a device that is really gone is said to be within a few periods.
 */
#define LH_MB_HEALTH_LIMIT (lh_cast_static(lh_u16_t, 10U))

/** @brief Whether a device is still there. */
typedef struct lh_mb_health
{
    lh_u16_t failures; /**< failed requests in a row, up to ::limit */
    lh_u16_t limit;    /**< failures in a row that make the device gone; at least 1 */
} lh_mb_health_t;      /**< Whether a device is still there. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Failed requests in a row (level 1).
 *
 * @param self The health; checked by ::lh_assert_runtime_ref.
 */
lh_u16_t
lh_mb_health_get_failures(const lh_mb_health_t *self);

/**
 * @brief Store the failed requests in a row (level 1).
 *
 * @param self     The health; checked by ::lh_assert_runtime_ref.
 * @param failures The new value.
 */
lh_void
lh_mb_health_set_failures(lh_mb_health_t *self, lh_u16_t failures);

/**
 * @brief Failures in a row that make the device gone (level 1).
 *
 * @param self The health; checked by ::lh_assert_runtime_ref.
 */
lh_u16_t
lh_mb_health_get_limit(const lh_mb_health_t *self);

/**
 * @brief Store the failures in a row that make the device gone (level 1).
 *
 * @param self  The health; checked by ::lh_assert_runtime_ref.
 * @param limit The new value; 0 is taken as 1.
 */
lh_void
lh_mb_health_set_limit(lh_mb_health_t *self, lh_u16_t limit);

/**
 * @brief Whether the device counts as gone: ::lh_mb_health_get_limit failures in a row
 *        (level 2).
 *
 * @param self The health; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_mb_health_is_lost(const lh_mb_health_t *self);

/**
 * @brief The device answered -- with data or with a refusal: the count starts again
 *        (level 2).
 *
 * @param self The health; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_mb_health_count_answer(lh_mb_health_t *self);

/**
 * @brief A request failed: one more in a row, up to the limit (level 2).
 *
 * @param self The health; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_mb_health_count_failure(lh_mb_health_t *self);

/**
 * @brief Start with no failures and @p limit as the limit (0 is taken as 1).
 *
 * @param self  The health; checked by ::lh_assert_runtime_ref.
 * @param limit Failures in a row that make the device gone; ::LH_MB_HEALTH_LIMIT
 *              by default.
 */
lh_void
lh_mb_health_init(lh_mb_health_t *self, lh_u16_t limit);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_MODBUS_HEALTH_H */
