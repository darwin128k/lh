/**
 * @file counters.h
 * @brief ::lh_mb_server_counters_t — what a server has done so far.
 *
 * The same idea as ::lh_mb_link_counters_t on the other side of the line: a server
 * that answers is easy to see, and these say what it did with everything else --
 * the frames for other units it stepped over, the frames it could not use, the
 * requests it refused.
 */

#ifndef LH_NET_MODBUS_SERVER_COUNTERS_H
#define LH_NET_MODBUS_SERVER_COUNTERS_H

#include <lh/core/numeric/fixed/types.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/void.h>

/** @brief What a server has done so far. */
typedef struct lh_mb_server_counters
{
    lh_u32_t received;     /**< requests for this unit or for every unit, whole and checked */
    lh_u32_t answered;     /**< requests done and answered */
    lh_u32_t refused;      /**< requests answered with an exception */
    lh_u32_t broadcast;    /**< writes to every unit, done and not answered */
    lh_u32_t ignored;      /**< whole frames for other units, stepped over */
    lh_u32_t broken;       /**< frames dropped: a bad CRC, an unsized code, noise */
    lh_u32_t bytes;        /**< request and answer bytes both ways */
} lh_mb_server_counters_t; /**< What a server has done so far. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The count of requests for this unit or for every unit, whole and checked (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_server_counters_get_received(const lh_mb_server_counters_t *self);

/**
 * @brief Store the count of requests for this unit or for every unit, whole and checked (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 * @param received The new value.
 */
lh_void
lh_mb_server_counters_set_received(lh_mb_server_counters_t *self, lh_u32_t received);

/**
 * @brief The count of requests done and answered (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_server_counters_get_answered(const lh_mb_server_counters_t *self);

/**
 * @brief Store the count of requests done and answered (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 * @param answered The new value.
 */
lh_void
lh_mb_server_counters_set_answered(lh_mb_server_counters_t *self, lh_u32_t answered);

/**
 * @brief The count of requests answered with an exception (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_server_counters_get_refused(const lh_mb_server_counters_t *self);

/**
 * @brief Store the count of requests answered with an exception (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 * @param refused The new value.
 */
lh_void
lh_mb_server_counters_set_refused(lh_mb_server_counters_t *self, lh_u32_t refused);

/**
 * @brief The count of writes to every unit, done and not answered (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_server_counters_get_broadcast(const lh_mb_server_counters_t *self);

/**
 * @brief Store the count of writes to every unit, done and not answered (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 * @param broadcast The new value.
 */
lh_void
lh_mb_server_counters_set_broadcast(lh_mb_server_counters_t *self, lh_u32_t broadcast);

/**
 * @brief The count of whole frames for other units, stepped over (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_server_counters_get_ignored(const lh_mb_server_counters_t *self);

/**
 * @brief Store the count of whole frames for other units, stepped over (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 * @param ignored The new value.
 */
lh_void
lh_mb_server_counters_set_ignored(lh_mb_server_counters_t *self, lh_u32_t ignored);

/**
 * @brief The count of frames dropped: a bad CRC, an unsized code, noise (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_server_counters_get_broken(const lh_mb_server_counters_t *self);

/**
 * @brief Store the count of frames dropped: a bad CRC, an unsized code, noise (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 * @param broken The new value.
 */
lh_void
lh_mb_server_counters_set_broken(lh_mb_server_counters_t *self, lh_u32_t broken);

/**
 * @brief The count of request and answer bytes both ways (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_server_counters_get_bytes(const lh_mb_server_counters_t *self);

/**
 * @brief Store the count of request and answer bytes both ways (level 1).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 * @param bytes The new value.
 */
lh_void
lh_mb_server_counters_set_bytes(lh_mb_server_counters_t *self, lh_u32_t bytes);

/**
 * @brief Count one more of the requests for this unit or for every unit, whole and checked (level
 * 2).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_mb_server_counters_count_received(lh_mb_server_counters_t *self);

/**
 * @brief Count one more of the requests done and answered (level 2).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_mb_server_counters_count_answered(lh_mb_server_counters_t *self);

/**
 * @brief Count one more of the requests answered with an exception (level 2).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_mb_server_counters_count_refused(lh_mb_server_counters_t *self);

/**
 * @brief Count one more of the writes to every unit, done and not answered (level 2).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_mb_server_counters_count_broadcast(lh_mb_server_counters_t *self);

/**
 * @brief Count one more of the whole frames for other units, stepped over (level 2).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_mb_server_counters_count_ignored(lh_mb_server_counters_t *self);

/**
 * @brief Count one more of the frames dropped: a bad CRC, an unsized code, noise (level 2).
 *
 * @param self The counters; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_mb_server_counters_count_broken(lh_mb_server_counters_t *self);

/**
 * @brief Add @p bytes to the bytes that went both ways (level 2).
 *
 * @param self  The counters; checked by ::lh_assert_runtime_ref.
 * @param bytes How many more.
 */
lh_void
lh_mb_server_counters_add_bytes(lh_mb_server_counters_t *self, lh_u32_t bytes);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_MODBUS_SERVER_COUNTERS_H */
