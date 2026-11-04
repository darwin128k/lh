/**
 * @file queue.h
 * @brief ::lh_exec_queue_t — tasks handed from any thread to one consumer.
 *
 * A first-in, first-out queue of ::lh_exec_task_t with many producers and one
 * consumer, and no lock. ::lh_exec_queue_push may run on any thread at any
 * time: one atomic exchange, never a wait. ::lh_exec_queue_try_pop runs on the
 * one consumer thread only. Tasks pushed by one thread come out in that
 * thread's order; tasks from different threads interleave in the order their
 * exchanges happened.
 *
 * The queue is intrusive: it links the tasks through their own `next` and
 * allocates nothing. A pushed task belongs to the queue until it is popped.
 * The queue owns a placeholder task of its own (`stub`), so it holds the
 * address of its own field: it must not be moved or copied after
 * ::lh_exec_queue_init. It needs no teardown; tasks still in it when it goes
 * away are the owner's to account for.
 *
 * This is Dmitry Vyukov's intrusive MPSC queue. One property comes with it:
 * while a producer is between its exchange and its link, the consumer cannot
 * see that task — or the ones pushed after it — yet, and ::lh_exec_queue_try_pop
 * returns ::lh_null although the queue is not empty. The task appears as soon
 * as the producer's next instruction runs; a consumer that sleeps until woken
 * simply looks again.
 */

#ifndef LH_EXEC_QUEUE_H
#define LH_EXEC_QUEUE_H

#include <lh/exec/queue/fields.h>
#include <lh/exec/task.h>

/**
 * @brief Tasks from many producer threads to one consumer thread.
 */
struct lh_exec_queue
{
    /** Members: ::lh_exec_queue_fields. */
    lh_exec_queue_fields(lh_atomic_ptr_t, lh_exec_task_t);
};

/** @copybrief lh_exec_queue */
typedef struct lh_exec_queue lh_exec_queue_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The producers' end of @p self: the task pushed last (level 1).
 * @param self Queue; must not be null.
 * @return The end every push exchanges.
 */
lh_atomic_ptr_t *
lh_exec_queue_get_back_mut(lh_exec_queue_t *self);

/**
 * @brief The consumer's end of @p self: the task taken next (level 1).
 * @param self Queue; must not be null. Consumer thread only.
 * @return The front node (the stub when the queue has been drained).
 */
lh_exec_task_t *
lh_exec_queue_get_front(const lh_exec_queue_t *self);

/**
 * @brief Move the consumer's end of @p self (level 1).
 * @param self  Queue; must not be null. Consumer thread only.
 * @param front The new front node.
 */
lh_void
lh_exec_queue_set_front(lh_exec_queue_t *self, lh_exec_task_t *front);

/**
 * @brief The queue's placeholder task (level 1).
 * @param self Queue; must not be null.
 * @return The stub, which is never handed out.
 */
lh_exec_task_t *
lh_exec_queue_get_stub_mut(lh_exec_queue_t *self);

/**
 * @brief Make @p self an empty queue.
 *
 * Call it before any thread can reach the queue; afterwards the queue must stay
 * where it is.
 *
 * @param self Queue; must not be null.
 */
lh_void
lh_exec_queue_init(lh_exec_queue_t *self);

/**
 * @brief Append @p task; any thread, at any time.
 *
 * One atomic exchange, never a wait. Everything the producer did before the
 * push is visible to the consumer that pops @p task.
 *
 * @param self Queue; must not be null.
 * @param task Task in no queue; must not be null. It belongs to the queue
 *             until it is popped.
 */
lh_void
lh_exec_queue_push(lh_exec_queue_t *self, lh_exec_task_t *task);

/**
 * @brief Take the oldest task; the one consumer thread only.
 *
 * @param self Queue; must not be null.
 * @return The task, now the consumer's; or ::lh_null when the queue is empty
 *         or its next task is still being linked by a producer (see the file
 *         comment).
 */
lh_exec_task_t *
lh_exec_queue_try_pop(lh_exec_queue_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_EXEC_QUEUE_H */
