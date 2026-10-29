/**
 * @file sink.h
 * @brief ::lh_os_logger_sink_t — where a log queue's records go.
 *
 * Four callbacks and the value they are given. The queue calls them on its worker
 * thread only, one at a time, so a sink needs no lock of its own. Daily files
 * (`lh/os/logger/daily.h`) are one sink; a test or a network forwarder is another.
 */

#ifndef LH_OS_LOGGER_SINK_H
#define LH_OS_LOGGER_SINK_H

#include <lh/os/logger/sink/close/cb.h>
#include <lh/os/logger/sink/dropped/cb.h>
#include <lh/os/logger/sink/emit/cb.h>
#include <lh/os/logger/sink/flush/cb.h>

/** @brief Where a log queue's records go. */
struct lh_os_logger_sink
{
    lh_os_logger_sink_emit_cb emit;       /**< writes one record; required */
    lh_os_logger_sink_flush_cb flush;     /**< puts written records on disk; required */
    lh_os_logger_sink_close_cb close;     /**< releases everything at the end; required */
    lh_os_logger_sink_dropped_cb dropped; /**< hears of dropped records; may be ::lh_null */
    lh_ptr user;                          /**< the value the callbacks are given */
};
/** @copybrief lh_os_logger_sink */
typedef struct lh_os_logger_sink lh_os_logger_sink_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The callback that writes one record (level 1).
 *
 * @param self The sink; checked by ::lh_assert_runtime_ref.
 */
lh_os_logger_sink_emit_cb
lh_os_logger_sink_get_emit_fn(const lh_os_logger_sink_t *self);

/**
 * @brief Store the callback that writes one record (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The sink; checked by ::lh_assert_runtime_ref.
 * @param emit The new value.
 */
lh_void
lh_os_logger_sink_set_emit_fn(lh_os_logger_sink_t *self, lh_os_logger_sink_emit_cb emit);

/**
 * @brief The callback that puts written records on disk (level 1).
 *
 * @param self The sink; checked by ::lh_assert_runtime_ref.
 */
lh_os_logger_sink_flush_cb
lh_os_logger_sink_get_flush_fn(const lh_os_logger_sink_t *self);

/**
 * @brief Store the callback that puts written records on disk (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The sink; checked by ::lh_assert_runtime_ref.
 * @param flush The new value.
 */
lh_void
lh_os_logger_sink_set_flush_fn(lh_os_logger_sink_t *self, lh_os_logger_sink_flush_cb flush);

/**
 * @brief The callback that releases everything (level 1).
 *
 * @param self The sink; checked by ::lh_assert_runtime_ref.
 */
lh_os_logger_sink_close_cb
lh_os_logger_sink_get_close_fn(const lh_os_logger_sink_t *self);

/**
 * @brief Store the callback that releases everything (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The sink; checked by ::lh_assert_runtime_ref.
 * @param close The new value.
 */
lh_void
lh_os_logger_sink_set_close_fn(lh_os_logger_sink_t *self, lh_os_logger_sink_close_cb close);

/**
 * @brief The callback that hears of dropped records (level 1).
 *
 * @param self The sink; checked by ::lh_assert_runtime_ref.
 */
lh_os_logger_sink_dropped_cb
lh_os_logger_sink_get_dropped_fn(const lh_os_logger_sink_t *self);

/**
 * @brief Store the callback that hears of dropped records (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The sink; checked by ::lh_assert_runtime_ref.
 * @param dropped The new value.
 */
lh_void
lh_os_logger_sink_set_dropped_fn(lh_os_logger_sink_t *self, lh_os_logger_sink_dropped_cb dropped);

/**
 * @brief The value the callbacks are given (level 1).
 *
 * @param self The sink; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_os_logger_sink_get_user(const lh_os_logger_sink_t *self);

/**
 * @brief Store the value the callbacks are given (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The sink; checked by ::lh_assert_runtime_ref.
 * @param user The new value.
 */
lh_void
lh_os_logger_sink_set_user(lh_os_logger_sink_t *self, lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOGGER_SINK_H */
