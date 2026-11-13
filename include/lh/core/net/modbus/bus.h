/**
 * @file bus.h
 * @brief ::lh_mb_bus_t — one line, one job at a time, from sending to its outcome,
 *        without blocking.
 *
 * ::lh_mb_link_t asks one question and reads one answer. A line in use needs a little
 * more around that, and every master needs the same little more:
 *
 * - **a job, not a question**: the line takes an ::lh_mb_job_t -- a read or a write,
 *   any unit -- so requests from a poll, a screen and a gateway go down the same line;
 * - **retries**: a request that timed out or came back broken is sent again up to
 *   ::lh_mb_bus_get_retries times before it fails. A refusal is an answer and a lost
 *   stream must be opened again first, so neither is retried
 *   (::lh_mb_bus_is_retryable). A device that holds a request for a second now and
 *   then (measured on a real controller: 1.05-1.2 s every 1.3-7 s) costs a retry, not
 *   a failure;
 * - **no blocking**: ::lh_mb_bus_try_start sends, ::lh_mb_bus_poll looks once and says
 *   whether the outcome is there. One thread can serve the line and everything else
 *   between two polls.
 *
 * The outcome is read from the link (::lh_mb_bus_get_link: state, reason, exception,
 * ::lh_mb_link_take for the words) until ::lh_mb_bus_release frees the line for the
 * next job. Which job goes next is not the bus's business: it is the owner's policy.
 *
 * @see lh_mb_job_t, lh_mb_job_queue_t, lh_mb_link_t
 */

#ifndef LH_NET_MODBUS_BUS_H
#define LH_NET_MODBUS_BUS_H

#include <lh/core/net/modbus/job.h>
#include <lh/core/net/modbus/link.h>

/** @brief One line, one job at a time. */
typedef struct lh_mb_bus
{
    lh_mb_link_t link;   /**< the line */
    lh_mb_job_t job;     /**< the job on the line, while ::busy */
    lh_bool_t busy;      /**< whether a job is on the line, its outcome not yet released */
    lh_u8_t retries;     /**< how often a timed-out or broken request is sent again */
    lh_u8_t attempt;     /**< how often the job on the line has been sent again */
    lh_u32_t timeout_us; /**< how long a request may stay unanswered, in microseconds */
    lh_u32_t resent;     /**< requests sent again, over the bus's life */
} lh_mb_bus_t;           /**< One line, one job at a time. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The line (level 1).
 *
 * @param self The bus; checked by ::lh_assert_runtime_ref.
 */
const lh_mb_link_t *
lh_mb_bus_get_link(const lh_mb_bus_t *self);

/**
 * @brief The line, to change: take the answer, set a new stream (level 1).
 *
 * @param self The bus; checked by ::lh_assert_runtime_ref.
 */
lh_mb_link_t *
lh_mb_bus_get_link_mut(lh_mb_bus_t *self);

/**
 * @brief The job on the line, or the last one (level 1).
 *
 * @param self The bus; checked by ::lh_assert_runtime_ref.
 */
const lh_mb_job_t *
lh_mb_bus_get_job(const lh_mb_bus_t *self);

/**
 * @brief The job on the line, to change (level 1).
 *
 * @param self The bus; checked by ::lh_assert_runtime_ref.
 */
lh_mb_job_t *
lh_mb_bus_get_job_mut(lh_mb_bus_t *self);

/**
 * @brief Whether a job is on the line, its outcome not yet released (level 1).
 *
 * @param self The bus; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_mb_bus_is_busy(const lh_mb_bus_t *self);

/**
 * @brief Store whether a job is on the line (level 1).
 *
 * @param self The bus; checked by ::lh_assert_runtime_ref.
 * @param busy The new value.
 */
lh_void
lh_mb_bus_set_busy(lh_mb_bus_t *self, lh_bool_t busy);

/**
 * @brief How often a timed-out or broken request is sent again (level 1).
 *
 * @param self The bus; checked by ::lh_assert_runtime_ref.
 */
lh_u8_t
lh_mb_bus_get_retries(const lh_mb_bus_t *self);

/**
 * @brief Store how often a timed-out or broken request is sent again (level 1).
 *
 * @param self    The bus; checked by ::lh_assert_runtime_ref.
 * @param retries The new value; 0 sends every request once.
 */
lh_void
lh_mb_bus_set_retries(lh_mb_bus_t *self, lh_u8_t retries);

/**
 * @brief How often the job on the line has been sent again (level 1).
 *
 * @param self The bus; checked by ::lh_assert_runtime_ref.
 */
lh_u8_t
lh_mb_bus_get_attempt(const lh_mb_bus_t *self);

/**
 * @brief Store how often the job on the line has been sent again (level 1).
 *
 * @param self    The bus; checked by ::lh_assert_runtime_ref.
 * @param attempt The new value.
 */
lh_void
lh_mb_bus_set_attempt(lh_mb_bus_t *self, lh_u8_t attempt);

/**
 * @brief How long a request may stay unanswered, in microseconds (level 1).
 *
 * @param self The bus; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_bus_get_timeout_us(const lh_mb_bus_t *self);

/**
 * @brief Store how long a request may stay unanswered (level 1).
 *
 * @param self       The bus; checked by ::lh_assert_runtime_ref.
 * @param timeout_us The new value, in microseconds.
 */
lh_void
lh_mb_bus_set_timeout_us(lh_mb_bus_t *self, lh_u32_t timeout_us);

/**
 * @brief Requests sent again, over the bus's life (level 1).
 *
 * @param self The bus; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_mb_bus_get_resent(const lh_mb_bus_t *self);

/**
 * @brief Store the requests sent again (level 1).
 *
 * @param self   The bus; checked by ::lh_assert_runtime_ref.
 * @param resent The new value.
 */
lh_void
lh_mb_bus_set_resent(lh_mb_bus_t *self, lh_u32_t resent);

/**
 * @brief Whether a request that failed for @p reason is worth sending again.
 *
 * A timeout or a broken answer, yes: the next try may well get through. A refusal
 * (::lh_mb_status_exception) is an answer -- the same question gets the same "no" --
 * and a lost stream (::lh_mb_status_lost) has to be opened again first.
 *
 * @param reason Why the request failed.
 */
lh_bool_t
lh_mb_bus_is_retryable(lh_mb_status_t reason);

/**
 * @brief Start idle on @p stream, sending every request once (level 2).
 *
 * @param self       The bus; checked by ::lh_assert_runtime_ref.
 * @param stream     What the bytes go through, as ::lh_mb_link_init.
 * @param framing    What the line carries.
 * @param now_us     The clock, as ::lh_mb_link_init.
 * @param clock      Passed to every @p now_us call.
 * @param timeout_us How long a request may stay unanswered, in microseconds.
 */
lh_void
lh_mb_bus_init(lh_mb_bus_t *self, const lh_io_stream_t *stream, lh_mb_framing_t framing,
               lh_mb_clock_cb now_us, lh_ptr clock, lh_u32_t timeout_us);

/**
 * @brief Put the job on the line on the wire, as its function says (level 2).
 *
 * @param self The bus; checked by ::lh_assert_runtime_ref.
 *
 * @return ::lh_bool_false when the link would not send it: a function the bus does
 *         not know, a count out of range, a read for unit 0, a stream that failed.
 */
lh_bool_t
lh_mb_bus_try_send(lh_mb_bus_t *self);

/**
 * @brief Put a copy of @p job on the line (level 3).
 *
 * @param self The bus; checked by ::lh_assert_runtime_ref.
 * @param job  The job; checked by ::lh_assert_runtime_ref.
 *
 * @return ::lh_bool_false when the line is busy or the job could not be sent
 *         (::lh_mb_bus_try_send); the bus is then as it was, or idle.
 */
lh_bool_t
lh_mb_bus_try_start(lh_mb_bus_t *self, const lh_mb_job_t *job);

/**
 * @brief Look once whether the job on the line has its outcome (level 3).
 *
 * A request that fails for a reason worth retrying (::lh_mb_bus_is_retryable) is sent
 * again while ::lh_mb_bus_get_attempt is below ::lh_mb_bus_get_retries, and the job
 * goes on waiting.
 *
 * @param self The bus; checked by ::lh_assert_runtime_ref.
 *
 * @return ::lh_mb_link_waiting while it waits; ::lh_mb_link_answered or
 *         ::lh_mb_link_failed when it is over -- read the link, then
 *         ::lh_mb_bus_release; ::lh_mb_link_idle when no job is on the line.
 */
lh_mb_link_state_t
lh_mb_bus_poll(lh_mb_bus_t *self);

/**
 * @brief The outcome is read: free the line for the next job (level 2).
 *
 * @param self The bus; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_mb_bus_release(lh_mb_bus_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_MODBUS_BUS_H */
