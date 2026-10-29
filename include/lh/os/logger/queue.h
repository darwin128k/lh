/**
 * @file queue.h
 * @brief ::lh_os_logger_queue_t — a bounded log queue and the worker thread that
 *        drains it into a sink.
 *
 * One producer (say, a game's main thread) pushes records; pushing copies the
 * record into the queue and never allocates, locks, touches a file or waits. A
 * full queue drops the new record and counts it. The worker hands records to the
 * sink (`lh/os/logger/sink.h`) in order, tells it of drops, and has it flush every
 * `flush_ms` and on ::lh_os_logger_queue_sync. It sleeps on a condition between
 * records, woken by a push, a sync or its flush period. Destroying the queue
 * drains it, flushes, closes the sink and joins the worker.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_LOGGER_QUEUE_H
#define LH_OS_LOGGER_QUEUE_H

#include <lh/os/logger/sink.h>

/** @brief A log queue and its worker; opaque. */
typedef struct lh_os_logger_queue lh_os_logger_queue_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Create a queue of @p capacity records and start its worker.
 *
 * @param capacity How many records it holds; a power of two.
 * @param flush_ms How often the worker flushes the sink, in milliseconds; not 0.
 * @param sink     Where records go: emit, flush and close set. Copied.
 * @return The queue, or ::lh_null when the arguments are wrong, memory is short or
 *         the worker could not start.
 */
lh_os_logger_queue_t *
lh_os_logger_queue_try_create(lh_uint_t capacity, lh_u32_t flush_ms,
                              const lh_os_logger_sink_t *sink);

/**
 * @brief Copy @p record into the queue without waiting.
 *
 * The producer's call: one thread only.
 *
 * @param self   The queue.
 * @param record The record; copied.
 * @return ::lh_bool_false when the queue was full and the record was dropped.
 */
lh_bool_t
lh_os_logger_queue_push(lh_os_logger_queue_t *self, const lh_os_logger_record_t *record);

/**
 * @brief Wait until everything pushed before this call is written and flushed.
 *
 * For tests and administration, not for a frame loop.
 *
 * @param self       The queue.
 * @param timeout_ms Longest wait, in milliseconds.
 * @return ::lh_bool_false when the time ran out first.
 */
lh_bool_t
lh_os_logger_queue_sync(lh_os_logger_queue_t *self, lh_u32_t timeout_ms);

/**
 * @brief How many records were dropped since the queue was created.
 *
 * @param self The queue.
 */
lh_u32_t
lh_os_logger_queue_get_dropped(const lh_os_logger_queue_t *self);

/**
 * @brief Drain the queue, flush and close the sink, join the worker and free it all.
 *
 * The producer must have stopped pushing.
 *
 * @param self The queue, or ::lh_null.
 */
lh_void
lh_os_logger_queue_destroy(lh_os_logger_queue_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOGGER_QUEUE_H */
