/**
 * @file queue.h
 * @brief A ring of input events on the caller's storage: ::lh_os_input_queue_t.
 *
 * Optional: a window delivers straight to a sink. The queue is for a source
 * that must not run the consumer itself — a touch controller's interrupt on a
 * microcontroller — and for batching. One producer and one consumer; the
 * producer may be an interrupt that preempts the consumer. No heap and no
 * locks.
 *
 * When the ring is full, a pointer move is folded into a pointer move of the
 * same pointer just before it (only the latest position of a drag matters);
 * anything else is dropped and counted (::lh_os_input_queue_get_dropped).
 *
 * Needs no ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_INPUT_QUEUE_H
#define LH_OS_INPUT_QUEUE_H

#include <lh/os/input/event.h>
#include <lh/os/input/queue/fields.h>
#include <lh/os/input/queue/ptr.h>
#include <lh/os/input/sink/cb.h>

/**
 * @struct lh_os_input_queue
 * @brief A ring of ::lh_os_input_event_t on storage the caller owns.
 */
struct lh_os_input_queue
{
    /** Members: ::lh_os_input_queue_fields. */
    lh_os_input_queue_fields(lh_os_input_event_t, volatile lh_u16_t);
};
/** @copybrief lh_os_input_queue */
typedef struct lh_os_input_queue lh_os_input_queue_t;

LH_COMPILER_EXTERN_C_BEGIN

/** The position after @p index on a ring of @p capacity, counting in `[0, 2 * capacity)`. */
lh_u16_t
lh_os_input_queue_next(lh_u16_t capacity, lh_u16_t index);

/** The position before @p index on a ring of @p capacity. */
lh_u16_t
lh_os_input_queue_previous(lh_u16_t capacity, lh_u16_t index);

/** The storage slot of the position @p index. */
lh_u16_t
lh_os_input_queue_slot(lh_u16_t capacity, lh_u16_t index);

/**
 * @brief Make @p self an empty queue over @p storage of @p capacity events.
 *
 * @p capacity is 1..32767. @p storage is the caller's and must outlive @p self.
 */
lh_void
lh_os_input_queue_init(lh_os_input_queue_t *self, lh_os_input_event_t *storage, lh_u16_t capacity);

/** True when @p event is a pointer move that can replace @p last: the same pointer of the same
   device, nothing but the position changed in between. */
lh_bool_t
lh_os_input_queue_can_fold(const lh_os_input_event_ptr last, const lh_os_input_event_ptr event);

/** Full: fold @p event into the move just before it, or drop and count it. */
lh_bool_t
lh_os_input_queue_push_full(lh_os_input_queue_t *self, const lh_os_input_event_ptr event);

/**
 * @brief Copy @p event in at the end (the producer's side).
 *
 * @return False when it did not fit and was dropped. A pointer move folded
 *         into the move before it counts as stored.
 */
lh_bool_t
lh_os_input_queue_push(lh_os_input_queue_t *self, const lh_os_input_event_ptr event);

/**
 * @brief Take the oldest event into @p out (the consumer's side).
 *
 * @return False, @p out untouched, when the queue is empty.
 */
lh_bool_t
lh_os_input_queue_pop(lh_os_input_queue_t *self, lh_os_input_event_t *out);

/**
 * @brief Pop every event and hand each to @p sink, with @p context as its
 *        `self`, oldest first.
 *
 * Events pushed while it runs are handed on too.
 *
 * @return How many events went to @p sink.
 */
lh_u16_t
lh_os_input_queue_drain(lh_os_input_queue_t *self, lh_os_input_sink_cb sink, lh_ptr context);

/** @brief How many events wait in @p self. */
lh_u16_t
lh_os_input_queue_get_count(const lh_os_input_queue_ptr self);

/** @brief How many events @p self holds at most. */
lh_u16_t
lh_os_input_queue_get_capacity(const lh_os_input_queue_ptr self);

/** @brief How many events were dropped because @p self was full. */
lh_u32_t
lh_os_input_queue_get_dropped(const lh_os_input_queue_ptr self);

/** @brief True when no event waits in @p self. */
lh_bool_t
lh_os_input_queue_is_empty(const lh_os_input_queue_ptr self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_INPUT_QUEUE_H */
