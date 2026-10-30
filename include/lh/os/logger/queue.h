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
#include <lh/os/loop.h>
#include <lh/spsc.h>

/** @brief A log queue and its worker; opaque. */
typedef struct lh_os_logger_queue lh_os_logger_queue_t;

LH_COMPILER_EXTERN_C_BEGIN

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
 * @brief Drain the queue, flush and close the sink, join the worker and free it all.
 *
 * The producer must have stopped pushing.
 *
 * @param self The queue, or ::lh_null.
 */
lh_void
lh_os_logger_queue_destroy(lh_os_logger_queue_t *self);

/**
 * @brief The allocator of @p self, to change in place (level 1).
 */
lh_memory_allocator_t *
lh_os_logger_queue_get_allocator_mut(lh_os_logger_queue_t *self);

/**
 * @brief The storage of @p self (level 1).
 */
lh_os_logger_record_t *
lh_os_logger_queue_get_storage(const lh_os_logger_queue_t *self);

/**
 * @brief Store the storage of @p self (level 1).
 */
lh_void
lh_os_logger_queue_set_storage(lh_os_logger_queue_t *self, lh_os_logger_record_t *storage);

/**
 * @brief The records of @p self (level 1).
 */
lh_spsc_t *
lh_os_logger_queue_get_records(const lh_os_logger_queue_t *self);

/**
 * @brief Store the records of @p self (level 1).
 */
lh_void
lh_os_logger_queue_set_records(lh_os_logger_queue_t *self, lh_spsc_t *records);

/**
 * @brief The sink of @p self (level 1).
 */
const lh_os_logger_sink_t *
lh_os_logger_queue_get_sink(const lh_os_logger_queue_t *self);

/**
 * @brief Store the sink of @p self (level 1).
 */
lh_void
lh_os_logger_queue_set_sink(lh_os_logger_queue_t *self, lh_os_logger_sink_t sink);

/**
 * @brief The flush ms of @p self (level 1).
 */
lh_u32_t
lh_os_logger_queue_get_flush_ms(const lh_os_logger_queue_t *self);

/**
 * @brief Store the flush ms of @p self (level 1).
 */
lh_void
lh_os_logger_queue_set_flush_ms(lh_os_logger_queue_t *self, lh_u32_t flush_ms);

/**
 * @brief The loop of @p self (level 1).
 */
lh_os_loop_t *
lh_os_logger_queue_get_loop(const lh_os_logger_queue_t *self);

/**
 * @brief Store the loop of @p self (level 1).
 */
lh_void
lh_os_logger_queue_set_loop(lh_os_logger_queue_t *self, lh_os_loop_t *loop);

/**
 * @brief The last flush of @p self (level 1).
 */
lh_u64_t
lh_os_logger_queue_get_last_flush(const lh_os_logger_queue_t *self);

/**
 * @brief Store the last flush of @p self (level 1).
 */
lh_void
lh_os_logger_queue_set_last_flush(lh_os_logger_queue_t *self, lh_u64_t last_flush);

/**
 * @brief The reported of @p self (level 1).
 */
lh_u32_t
lh_os_logger_queue_get_reported(const lh_os_logger_queue_t *self);

/**
 * @brief Store the reported of @p self (level 1).
 */
lh_void
lh_os_logger_queue_set_reported(lh_os_logger_queue_t *self, lh_u32_t reported);

/**
 * @brief The mutex of @p self, to change in place (level 1).
 */
lh_os_mutex_t *
lh_os_logger_queue_get_mutex_mut(lh_os_logger_queue_t *self);

/**
 * @brief The done of @p self, to change in place (level 1).
 */
lh_os_condition_t *
lh_os_logger_queue_get_done_mut(lh_os_logger_queue_t *self);

/**
 * @brief The dropped of @p self, read atomically (level 1).
 */
lh_u32_t
lh_os_logger_queue_load_dropped(const lh_os_logger_queue_t *self);

/**
 * @brief Count one more dropped (level 1).
 */
lh_void
lh_os_logger_queue_add_dropped(lh_os_logger_queue_t *self);

/**
 * @brief The requested of @p self, read atomically (level 1).
 */
lh_u32_t
lh_os_logger_queue_load_requested(lh_os_logger_queue_t *self);

/**
 * @brief Count one more requested (level 1).
 */
lh_u32_t
lh_os_logger_queue_add_requested(lh_os_logger_queue_t *self);

/**
 * @brief The completed of @p self, read atomically (level 1).
 */
lh_u32_t
lh_os_logger_queue_load_completed(lh_os_logger_queue_t *self);

/**
 * @brief Store the completed of @p self, atomically (level 1).
 */
lh_void
lh_os_logger_queue_store_completed(lh_os_logger_queue_t *self, lh_u32_t completed);

/**
 * @brief Start the counters of @p self (level 1).
 */
lh_void
lh_os_logger_queue_init_counters(lh_os_logger_queue_t *self);

/**
 * @brief Now ms (internal).
 */
lh_u64_t lh_os_logger_queue_now_ms(lh_void);

/**
 * @brief True when @p done has reached @p wanted, counting through wrap-around.
 */
lh_bool_t
lh_os_logger_queue_has_reached(lh_u32_t done, lh_u32_t wanted);

/**
 * @brief Whether @p self is empty (level 1).
 */
lh_bool_t
lh_os_logger_queue_is_empty(lh_os_logger_queue_t *self);

/**
 * @brief Hand at most one queue's worth of records to the sink, so a flush or a stop is
   never starved by sustained logging.
 */
lh_void
lh_os_logger_queue_emit_batch(lh_os_logger_queue_t *self);

/**
 * @brief Report drops (internal).
 */
lh_void
lh_os_logger_queue_report_drops(lh_os_logger_queue_t *self);

/**
 * @brief Flush, then mark the sync requests seen before it as done and wake their waiters.
 */
lh_void
lh_os_logger_queue_flush(lh_os_logger_queue_t *self, lh_u32_t request);

/**
 * @brief Free what was made; each part may be missing.
 */
lh_void
lh_os_logger_queue_free(lh_os_logger_queue_t *self);

/**
 * @brief One round of the loop: records, drops, then a flush when one is due.
 */
lh_void
lh_os_logger_queue_step(lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOGGER_QUEUE_H */
