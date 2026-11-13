/**
 * @file batch.h
 * @brief ::lh_mb_batch_t — several values at scattered addresses written as one action,
 *        each checked by a read-back, with the word on every one of them.
 *
 * A device that is told to write a register can be told nothing more: Modbus has no
 * transaction, so "all of them or none" cannot be had from the device. What can be had
 * is the truth about each value. A batch takes the values (addresses ascending, each
 * with its word), works out the requests, and reports for every value how it ended
 * (::lh_mb_batch_item_t): confirmed by a read-back, refused, unanswered, read back as
 * another word, or written but not verified.
 *
 * It is a machine and does no I/O. The owner of a line asks it for the next step
 * (::lh_mb_batch_try_next), which is an ::lh_mb_job_t -- a write or a read -- puts that
 * on the line, and hands the outcome back (::lh_mb_batch_settle). So the same batch
 * runs over a ::lh_mb_bus_t, over a test, or over anything else that carries jobs.
 *
 * What the steps are:
 *
 * - **writes**: values at consecutive addresses go in one request (::LH_MB_FC_WRITE_MANY,
 *   at most ::LH_MB_WRITE_MAX registers); a value alone goes as ::LH_MB_FC_WRITE_ONE.
 *   Registers between two values are never written: a write cannot cross a gap without
 *   writing what is in it;
 * - **read-backs**: once the writes are done, the values that were written are read
 *   back, and a read may cross a gap of ::lh_mb_batch_get_gap registers, because reading
 *   what is not wanted costs only bytes and saves a request;
 * - **again**: a value read back as another word is written once more, up to
 *   ::lh_mb_batch_get_max_tries writes in all, for a device that applies a setting a
 *   moment late; the ones that took are not written again.
 *
 * A refused or unanswered write is not read back and not written again: a refusal is an
 * answer, and the silence of a device is the line's matter (::lh_mb_bus_t retries it).
 *
 * Lifecycle: ::lh_mb_batch_init once, then ::lh_mb_batch_try_begin; ask
 * ::lh_mb_batch_try_next and ::lh_mb_batch_settle in turn until
 * ::lh_mb_batch_is_finished; read the words of the outcome; ::lh_mb_batch_release.
 */

#ifndef LH_NET_MODBUS_BATCH_H
#define LH_NET_MODBUS_BATCH_H

#include <lh/core/net/modbus/batch/item.h>
#include <lh/core/net/modbus/batch/phase.h>
#include <lh/core/net/modbus/job.h>

/** @brief The most values one batch holds. */
#define LH_MB_BATCH_MAX (lh_cast_static(lh_u16_t, 32))

/** @brief A batch of writes at scattered addresses, with the outcome of each. */
typedef struct lh_mb_batch
{
    lh_u8_t unit;                        /**< the unit the batch is for */
    lh_u8_t phase;                       /**< what the batch is doing, a ::lh_mb_batch_phase_t */
    lh_u16_t count;                      /**< how many values the batch holds */
    lh_u16_t addresses[LH_MB_BATCH_MAX]; /**< where each value goes, ascending */
    lh_u16_t wanted[LH_MB_BATCH_MAX];    /**< the word to write for each */
    lh_u16_t held[LH_MB_BATCH_MAX];      /**< the word the last read-back showed for each */
    lh_u8_t items[LH_MB_BATCH_MAX];      /**< how far each has got, a ::lh_mb_batch_item_t */
    lh_u8_t exceptions[LH_MB_BATCH_MAX]; /**< the device's refusal for each refused value */
    lh_u16_t cursor;                     /**< the value the next step starts looking from */
    lh_u16_t first;                      /**< the first value of the step last made */
    lh_u16_t span;                       /**< how many values the step last made covers */
    lh_u8_t tries;                       /**< how many rounds of writes have been made */
    lh_u8_t max_tries;                   /**< how many rounds of writes there may be */
    lh_u16_t gap;                        /**< the most registers a read-back may cross */
} lh_mb_batch_t;                         /**< A batch of writes at scattered addresses. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The unit the batch is for (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 */
lh_u8_t
lh_mb_batch_get_unit(const lh_mb_batch_t *self);

/**
 * @brief Store the unit the batch is for (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @param unit The new value.
 */
lh_void
lh_mb_batch_set_unit(lh_mb_batch_t *self, lh_u8_t unit);

/**
 * @brief What the batch is doing, a ::lh_mb_batch_phase_t (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 */
lh_u8_t
lh_mb_batch_get_phase(const lh_mb_batch_t *self);

/**
 * @brief Store what the batch is doing, a ::lh_mb_batch_phase_t (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @param phase The new value.
 */
lh_void
lh_mb_batch_set_phase(lh_mb_batch_t *self, lh_u8_t phase);

/**
 * @brief How many values the batch holds (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 */
lh_u16_t
lh_mb_batch_get_count(const lh_mb_batch_t *self);

/**
 * @brief Store how many values the batch holds (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @param count The new value.
 */
lh_void
lh_mb_batch_set_count(lh_mb_batch_t *self, lh_u16_t count);

/**
 * @brief Where each value goes, ascending (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 */
const lh_u16_t *
lh_mb_batch_get_addresses(const lh_mb_batch_t *self);

/**
 * @brief Where each value goes, ascending, to change in place (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_u16_t *
lh_mb_batch_get_addresses_mut(lh_mb_batch_t *self);

/**
 * @brief The word to write for each value (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 */
const lh_u16_t *
lh_mb_batch_get_wanted(const lh_mb_batch_t *self);

/**
 * @brief The word to write for each value, to change in place (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_u16_t *
lh_mb_batch_get_wanted_mut(lh_mb_batch_t *self);

/**
 * @brief The word the last read-back showed for each value (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 */
const lh_u16_t *
lh_mb_batch_get_held(const lh_mb_batch_t *self);

/**
 * @brief The word the last read-back showed for each value, to change in place (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_u16_t *
lh_mb_batch_get_held_mut(lh_mb_batch_t *self);

/**
 * @brief How far each value has got, a ::lh_mb_batch_item_t (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 */
const lh_u8_t *
lh_mb_batch_get_items(const lh_mb_batch_t *self);

/**
 * @brief How far each value has got, a ::lh_mb_batch_item_t, to change in place (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_u8_t *
lh_mb_batch_get_items_mut(lh_mb_batch_t *self);

/**
 * @brief The device's refusal for each refused value (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 */
const lh_u8_t *
lh_mb_batch_get_exceptions(const lh_mb_batch_t *self);

/**
 * @brief The device's refusal for each refused value, to change in place (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_u8_t *
lh_mb_batch_get_exceptions_mut(lh_mb_batch_t *self);

/**
 * @brief The value the next step starts looking from (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 */
lh_u16_t
lh_mb_batch_get_cursor(const lh_mb_batch_t *self);

/**
 * @brief Store the value the next step starts looking from (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @param cursor The new value.
 */
lh_void
lh_mb_batch_set_cursor(lh_mb_batch_t *self, lh_u16_t cursor);

/**
 * @brief The first value of the step last made (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 */
lh_u16_t
lh_mb_batch_get_first(const lh_mb_batch_t *self);

/**
 * @brief Store the first value of the step last made (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @param first The new value.
 */
lh_void
lh_mb_batch_set_first(lh_mb_batch_t *self, lh_u16_t first);

/**
 * @brief How many values the step last made covers (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 */
lh_u16_t
lh_mb_batch_get_span(const lh_mb_batch_t *self);

/**
 * @brief Store how many values the step last made covers (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @param span The new value.
 */
lh_void
lh_mb_batch_set_span(lh_mb_batch_t *self, lh_u16_t span);

/**
 * @brief How many rounds of writes have been made (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 */
lh_u8_t
lh_mb_batch_get_tries(const lh_mb_batch_t *self);

/**
 * @brief Store how many rounds of writes have been made (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @param tries The new value.
 */
lh_void
lh_mb_batch_set_tries(lh_mb_batch_t *self, lh_u8_t tries);

/**
 * @brief How many rounds of writes there may be (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 */
lh_u8_t
lh_mb_batch_get_max_tries(const lh_mb_batch_t *self);

/**
 * @brief Store how many rounds of writes there may be (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @param max_tries The new value.
 */
lh_void
lh_mb_batch_set_max_tries(lh_mb_batch_t *self, lh_u8_t max_tries);

/**
 * @brief The most registers a read-back may cross (level 1).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 */
lh_u16_t
lh_mb_batch_get_gap(const lh_mb_batch_t *self);

/**
 * @brief Store the most registers a read-back may cross (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @param gap The new value.
 */
lh_void
lh_mb_batch_set_gap(lh_mb_batch_t *self, lh_u16_t gap);

/**
 * @brief Start idle, taking @p max_tries rounds of writes and reading back across gaps
 *        of @p gap registers (level 2).
 *
 * @param self      The batch; checked by ::lh_assert_runtime_ref.
 * @param max_tries How many times a value that reads back as another word is written,
 *                  the first write included; 0 is taken as 1.
 * @param gap       The most registers a read-back may cross between two values; 0 reads
 *                  only registers that are wanted.
 */
lh_void
lh_mb_batch_init(lh_mb_batch_t *self, lh_u8_t max_tries, lh_u16_t gap);

/**
 * @brief Whether no batch is taken (level 2).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_mb_batch_is_idle(const lh_mb_batch_t *self);

/**
 * @brief Which value is the first at or after @p from that is at @p item, or
 *        ::lh_mb_batch_get_count when none does (level 2).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @param from The value to start looking at.
 * @param item The state looked for.
 */
lh_u16_t
lh_mb_batch_find_item(const lh_mb_batch_t *self, lh_u16_t from, lh_mb_batch_item_t item);

/**
 * @brief How many values are at @p item (level 2).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @param item The state counted.
 */
lh_u16_t
lh_mb_batch_count_items(const lh_mb_batch_t *self, lh_mb_batch_item_t item);

/**
 * @brief Where the run of pending values that starts at @p first ends: the index after
 *        its last value (level 2).
 *
 * A run is values at consecutive addresses that are all pending. A batch holds at most
 * ::LH_MB_BATCH_MAX values, fewer than the ::LH_MB_WRITE_MAX registers one write request
 * can carry, so a run always fits in one.
 *
 * @param self  The batch; checked by ::lh_assert_runtime_ref.
 * @param first A pending value, below ::lh_mb_batch_get_count; not checked.
 */
lh_u16_t
lh_mb_batch_get_run_end(const lh_mb_batch_t *self, lh_u16_t first);

/**
 * @brief Where the read-back that starts at @p first ends: the index after its last
 *        written value (level 2).
 *
 * The read goes from the address of @p first to that of the last written value it
 * takes in. A written value is taken in while it lies at most ::lh_mb_batch_get_gap
 * registers from the one before it and the read stays within ::LH_MB_READ_MAX registers.
 * Values that are not written (refused, say) lie in the span and are not read for.
 *
 * @param self  The batch; checked by ::lh_assert_runtime_ref.
 * @param first A written value, below ::lh_mb_batch_get_count; not checked.
 */
lh_u16_t
lh_mb_batch_get_read_end(const lh_mb_batch_t *self, lh_u16_t first);

/**
 * @brief The write on the line ended with @p status (level 2).
 *
 * @param self      The batch; checked by ::lh_assert_runtime_ref.
 * @param status    ::lh_mb_status_ok when the device answered; ::lh_mb_status_exception
 *                  when it refused; anything else when nothing came back.
 * @param exception The device's refusal, for ::lh_mb_status_exception.
 */
lh_void
lh_mb_batch_settle_write(lh_mb_batch_t *self, lh_mb_status_t status, lh_u8_t exception);

/**
 * @brief The read-back on the line ended with @p status and @p count words (level 2).
 *
 * @param self   The batch; checked by ::lh_assert_runtime_ref.
 * @param status ::lh_mb_status_ok when the device answered; anything else when it did
 *               not, and the values are then unverified.
 * @param values The words the device returned, from the address of the step's first
 *               value on; may be null when @p status is not ok.
 * @param count  How many words @p values holds.
 */
lh_void
lh_mb_batch_settle_read(lh_mb_batch_t *self, lh_mb_status_t status, const lh_u16_t *values,
                        lh_u16_t count);

/**
 * @brief Let go of the finished batch: the owner has read the outcome (level 2).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_mb_batch_release(lh_mb_batch_t *self);

/**
 * @brief Whether no step is left: no value waits to be written and none to be read
 *        back (level 3).
 *
 * True with no batch taken too. Once it is true the state of every value is final.
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_mb_batch_is_finished(const lh_mb_batch_t *self);

/**
 * @brief Take a batch: write @p count words at @p addresses of @p unit (level 3).
 *
 * @param self      The batch; checked by ::lh_assert_runtime_ref.
 * @param unit      The unit the values are for.
 * @param addresses The registers, strictly ascending; checked by ::lh_assert_runtime_ref.
 * @param values    The word for each; checked by ::lh_assert_runtime_ref.
 * @param count     How many, 1 to ::LH_MB_BATCH_MAX.
 *
 * @return ::lh_bool_false when a batch is already taken (::lh_mb_batch_release the
 *         last one first), when @p count is out of range or when the addresses are not
 *         strictly ascending; the batch is as it was then.
 */
lh_bool_t
lh_mb_batch_try_begin(lh_mb_batch_t *self, lh_u8_t unit, const lh_u16_t *addresses,
                      const lh_u16_t *values, lh_u16_t count);

/**
 * @brief Make the next write of the batch into @p job (level 3).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @param job  Where the job goes; checked by ::lh_assert_runtime_ref.
 *
 * @return ::lh_bool_false when no value is pending; @p job is untouched then.
 */
lh_bool_t
lh_mb_batch_try_next_write(lh_mb_batch_t *self, lh_mb_job_t *job);

/**
 * @brief Make the next read-back of the batch into @p job (level 3).
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @param job  Where the job goes; checked by ::lh_assert_runtime_ref.
 *
 * @return ::lh_bool_false when no written value is left to read back; @p job is
 *         untouched then.
 */
lh_bool_t
lh_mb_batch_try_next_read(lh_mb_batch_t *self, lh_mb_job_t *job);

/**
 * @brief The step on the line ended: put its outcome into the values it covered
 *        (level 3).
 *
 * After a write, the values of the step are at written, refused or failed. After a
 * read-back, they are at confirmed, unverified, mismatch, or pending again for another
 * write while writes are left.
 *
 * @param self      The batch; checked by ::lh_assert_runtime_ref.
 * @param status    ::lh_mb_status_ok when the device answered; ::lh_mb_status_exception
 *                  when it refused; anything else when nothing came back.
 * @param exception The device's refusal, for ::lh_mb_status_exception.
 * @param values    A read-back's words, from the address of the step's first value on;
 *                  may be null when @p status is not ok or the step was a write.
 * @param count     How many words @p values holds.
 */
lh_void
lh_mb_batch_settle(lh_mb_batch_t *self, lh_mb_status_t status, lh_u8_t exception,
                   const lh_u16_t *values, lh_u16_t count);

/**
 * @brief Make the next step of the batch into @p job: a write of the values that are
 *        pending, or the read-back of the values that were written (level 4).
 *
 * The writes come first, then the read-backs; a value that read back as another word
 * sends the batch round to the writes again. The step stays in ::lh_mb_batch_get_first
 * and ::lh_mb_batch_get_span until the next one is made, so that its outcome
 * (::lh_mb_batch_settle) is known to belong to it.
 *
 * @param self The batch; checked by ::lh_assert_runtime_ref.
 * @param job  Where the job goes, owned by nobody; checked by ::lh_assert_runtime_ref.
 *
 * @return ::lh_bool_false when no step is left (::lh_mb_batch_is_finished) or no batch
 *         is taken; @p job is untouched then.
 */
lh_bool_t
lh_mb_batch_try_next(lh_mb_batch_t *self, lh_mb_job_t *job);

LH_COMPILER_EXTERN_C_END

#endif /* LH_NET_MODBUS_BATCH_H */