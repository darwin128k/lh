/**
 * @file queue.c
 * @brief ::lh_os_logger_queue_t: a single-producer record queue drained by an ::lh_os_loop_t.
 *
 * The producer side is lock-free: a push copies into the SPSC ring and hints the
 * loop, which runs the step soon or at the latest after one flush period. The loop's step hands
 * records to the sink, reports drops and flushes on the period or a sync; its last step drains
 * everything and closes the sink. Sync requests and their completion go through counters, compared
 * with wrap-around, under the mutex the sync waits with.
 */

#include <lh/os/logger/queue.h>
#include <lh/assert/runtime.h>
#include <lh/attribute/unused.h>
#include <lh/cast/const.h>
#include <lh/os/tick.h>
#include <lh/runtime/allocator.h>
#include <lh/util/return.h>
#include <stdatomic.h>

struct lh_os_logger_queue
{
    lh_memory_allocator_t allocator;
    lh_os_logger_record_t *storage;
    lh_spsc_t *records;
    lh_os_logger_sink_t sink;
    lh_u32_t flush_ms;
    lh_os_loop_t *loop;
    lh_u64_t last_flush;
    lh_u32_t reported;
    lh_os_mutex_t mutex;
    lh_os_condition_t done;
    atomic_uint dropped;
    atomic_uint requested;
    atomic_uint completed;
};

/* -- Level 1: the fields ------------------------------------------------------ */

lh_memory_allocator_t *
lh_os_logger_queue_get_allocator_mut(lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->allocator);
}

lh_os_logger_record_t *
lh_os_logger_queue_get_storage(const lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return self->storage;
}

lh_void
lh_os_logger_queue_set_storage(lh_os_logger_queue_t *self, lh_os_logger_record_t *storage)
{
    lh_assert_runtime_ref(self);
    self->storage = storage;
}

lh_spsc_t *
lh_os_logger_queue_get_records(const lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return self->records;
}

lh_void
lh_os_logger_queue_set_records(lh_os_logger_queue_t *self, lh_spsc_t *records)
{
    lh_assert_runtime_ref(self);
    self->records = records;
}

const lh_os_logger_sink_t *
lh_os_logger_queue_get_sink(const lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sink);
}

lh_void
lh_os_logger_queue_set_sink(lh_os_logger_queue_t *self, lh_os_logger_sink_t sink)
{
    lh_assert_runtime_ref(self);
    self->sink = sink;
}

lh_u32_t
lh_os_logger_queue_get_flush_ms(const lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return self->flush_ms;
}

lh_void
lh_os_logger_queue_set_flush_ms(lh_os_logger_queue_t *self, lh_u32_t flush_ms)
{
    lh_assert_runtime_ref(self);
    self->flush_ms = flush_ms;
}

lh_os_loop_t *
lh_os_logger_queue_get_loop(const lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return self->loop;
}

lh_void
lh_os_logger_queue_set_loop(lh_os_logger_queue_t *self, lh_os_loop_t *loop)
{
    lh_assert_runtime_ref(self);
    self->loop = loop;
}

lh_u64_t
lh_os_logger_queue_get_last_flush(const lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return self->last_flush;
}

lh_void
lh_os_logger_queue_set_last_flush(lh_os_logger_queue_t *self, lh_u64_t last_flush)
{
    lh_assert_runtime_ref(self);
    self->last_flush = last_flush;
}

lh_u32_t
lh_os_logger_queue_get_reported(const lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return self->reported;
}

lh_void
lh_os_logger_queue_set_reported(lh_os_logger_queue_t *self, lh_u32_t reported)
{
    lh_assert_runtime_ref(self);
    self->reported = reported;
}

lh_os_mutex_t *
lh_os_logger_queue_get_mutex_mut(lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->mutex);
}

lh_os_condition_t *
lh_os_logger_queue_get_done_mut(lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->done);
}

lh_u32_t
lh_os_logger_queue_load_dropped(const lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return atomic_load_explicit(lh_cast_const(atomic_uint *, lh_addr_of(self->dropped)),
                                memory_order_relaxed);
}

lh_void
lh_os_logger_queue_add_dropped(lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    atomic_fetch_add_explicit(lh_addr_of(self->dropped), 1U, memory_order_relaxed);
}

lh_u32_t
lh_os_logger_queue_load_requested(lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return atomic_load_explicit(lh_addr_of(self->requested), memory_order_acquire);
}

lh_u32_t
lh_os_logger_queue_add_requested(lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return atomic_fetch_add_explicit(lh_addr_of(self->requested), 1U, memory_order_acq_rel) + 1U;
}

lh_u32_t
lh_os_logger_queue_load_completed(lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return atomic_load_explicit(lh_addr_of(self->completed), memory_order_acquire);
}

lh_void
lh_os_logger_queue_store_completed(lh_os_logger_queue_t *self, lh_u32_t completed)
{
    lh_assert_runtime_ref(self);
    atomic_store_explicit(lh_addr_of(self->completed), completed, memory_order_release);
}

lh_void
lh_os_logger_queue_init_counters(lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    atomic_init(lh_addr_of(self->dropped), 0U);
    atomic_init(lh_addr_of(self->requested), 0U);
    atomic_init(lh_addr_of(self->completed), 0U);
}

/* -- Level 2 and up ----------------------------------------------------------- */

lh_u64_t
lh_os_logger_queue_now_ms(lh_void)
{
    return lh_os_tick_us() / 1000U;
}

/* True when @p done has reached @p wanted, counting through wrap-around. */
lh_bool_t
lh_os_logger_queue_has_reached(lh_u32_t done, lh_u32_t wanted)
{
    return lh_cast_static(lh_s32_t, done - wanted) >= 0 ? lh_bool_true : lh_bool_false;
}

lh_bool_t
lh_os_logger_queue_is_empty(lh_os_logger_queue_t *self)
{
    return lh_null_eq(lh_spsc_peek(lh_os_logger_queue_get_records(self)));
}

/* Hand at most one queue's worth of records to the sink, so a flush or a stop is
   never starved by sustained logging. */
lh_void
lh_os_logger_queue_emit_batch(lh_os_logger_queue_t *self)
{
    const lh_os_logger_sink_t *sink = lh_os_logger_queue_get_sink(self);
    lh_spsc_t *records = lh_os_logger_queue_get_records(self);

    for (lh_uint_t count = lh_spsc_get_capacity(records); count > 0U; --count)
    {
        const lh_os_logger_record_t *record = lh_spsc_peek(records);

        lh_return_if(lh_null_eq(record));
        lh_os_logger_sink_get_emit_fn(sink)(lh_os_logger_sink_get_user(sink), record);
        lh_spsc_release(records);
    }
}

lh_void
lh_os_logger_queue_report_drops(lh_os_logger_queue_t *self)
{
    const lh_os_logger_sink_t *sink = lh_os_logger_queue_get_sink(self);
    const lh_u32_t losses = lh_os_logger_queue_load_dropped(self);

    lh_return_if(losses == lh_os_logger_queue_get_reported(self) ||
                 lh_null_eq(lh_os_logger_sink_get_dropped_fn(sink)));
    lh_os_logger_sink_get_dropped_fn(sink)(lh_os_logger_sink_get_user(sink),
                                           losses - lh_os_logger_queue_get_reported(self));
    lh_os_logger_queue_set_reported(self, losses);
}

/* Flush, then mark the sync requests seen before it as done and wake their waiters. */
lh_void
lh_os_logger_queue_flush(lh_os_logger_queue_t *self, lh_u32_t request)
{
    const lh_os_logger_sink_t *sink = lh_os_logger_queue_get_sink(self);

    lh_os_logger_sink_get_flush_fn(sink)(lh_os_logger_sink_get_user(sink));
    lh_os_logger_queue_set_last_flush(self, lh_os_logger_queue_now_ms());
    LH_ATTRIBUTE_UNUSED(lh_os_mutex_lock(lh_os_logger_queue_get_mutex_mut(self)));
    lh_os_logger_queue_store_completed(self, request);
    LH_ATTRIBUTE_UNUSED(lh_os_condition_broadcast(lh_os_logger_queue_get_done_mut(self)));
    LH_ATTRIBUTE_UNUSED(lh_os_mutex_unlock(lh_os_logger_queue_get_mutex_mut(self)));
}

/* Free what was made; each part may be missing. */
lh_void
lh_os_logger_queue_free(lh_os_logger_queue_t *self)
{
    lh_os_loop_destroy(lh_os_logger_queue_get_loop(self));
    LH_ATTRIBUTE_UNUSED(lh_os_condition_close(lh_os_logger_queue_get_done_mut(self)));
    LH_ATTRIBUTE_UNUSED(lh_os_mutex_close(lh_os_logger_queue_get_mutex_mut(self)));
    lh_spsc_destroy(lh_os_logger_queue_get_records(self));
    lh_memory_allocator_dealloc(lh_os_logger_queue_get_allocator_mut(self),
                                lh_os_logger_queue_get_storage(self));
    lh_memory_allocator_dealloc(lh_os_logger_queue_get_allocator_mut(self), self);
}

lh_bool_t
lh_os_logger_queue_push(lh_os_logger_queue_t *self, const lh_os_logger_record_t *record)
{
    lh_assert_runtime_ref(record);
    if (!lh_spsc_push(lh_os_logger_queue_get_records(self), record))
    {
        lh_os_logger_queue_add_dropped(self);
        return lh_bool_false;
    }
    lh_os_loop_hint(lh_os_logger_queue_get_loop(self));
    return lh_bool_true;
}

lh_bool_t
lh_os_logger_queue_sync(lh_os_logger_queue_t *self, lh_u32_t timeout_ms)
{
    const lh_u64_t start = lh_os_logger_queue_now_ms();
    lh_bool_t reached;
    lh_u32_t request;

    LH_ATTRIBUTE_UNUSED(lh_os_mutex_lock(lh_os_logger_queue_get_mutex_mut(self)));
    request = lh_os_logger_queue_add_requested(self);
    lh_os_loop_wake(lh_os_logger_queue_get_loop(self));
    for (;;)
    {
        const lh_u64_t waited = lh_os_logger_queue_now_ms() - start;

        reached = lh_os_logger_queue_has_reached(lh_os_logger_queue_load_completed(self), request);
        if (reached || waited >= timeout_ms)
        {
            break;
        }
        LH_ATTRIBUTE_UNUSED(lh_os_condition_wait_for(
            lh_os_logger_queue_get_done_mut(self), lh_os_logger_queue_get_mutex_mut(self),
            lh_cast_static(lh_u32_t, timeout_ms - waited)));
    }
    LH_ATTRIBUTE_UNUSED(lh_os_mutex_unlock(lh_os_logger_queue_get_mutex_mut(self)));
    return reached;
}

lh_u32_t
lh_os_logger_queue_get_dropped(const lh_os_logger_queue_t *self)
{
    return lh_os_logger_queue_load_dropped(self);
}

/* One round of the loop: records, drops, then a flush when one is due. */
lh_void
lh_os_logger_queue_step(lh_ptr user)
{
    lh_os_logger_queue_t *self = lh_ptr_rcast(lh_os_logger_queue_t, user);
    const lh_bool_t last = lh_os_loop_is_last_step(lh_os_logger_queue_get_loop(self));
    const lh_u32_t request = lh_os_logger_queue_load_requested(self);
    const lh_os_logger_sink_t *sink = lh_os_logger_queue_get_sink(self);

    do
    {
        lh_os_logger_queue_emit_batch(self);
    } while (last && !lh_os_logger_queue_is_empty(self));
    lh_os_logger_queue_report_drops(self);
    if (last || request != lh_os_logger_queue_load_completed(self) ||
        lh_os_logger_queue_now_ms() - lh_os_logger_queue_get_last_flush(self) >=
            lh_os_logger_queue_get_flush_ms(self))
    {
        lh_os_logger_queue_flush(self, last ? lh_os_logger_queue_load_requested(self) : request);
    }
    if (last)
    {
        lh_os_logger_sink_get_close_fn(sink)(lh_os_logger_sink_get_user(sink));
    }
    else if (!lh_os_logger_queue_is_empty(self))
    {
        /* The batch stopped short: come back at once, not at the end of the period. */
        lh_os_loop_wake(lh_os_logger_queue_get_loop(self));
    }
}

lh_os_logger_queue_t *
lh_os_logger_queue_try_create(lh_uint_t capacity, lh_u32_t flush_ms,
                              const lh_os_logger_sink_t *sink)
{
    lh_memory_allocator_t allocator = lh_ptr_deref(lh_runtime_allocator());
    lh_os_logger_queue_t *self;

    lh_return_if(lh_null_eq(sink) || lh_null_eq(lh_os_logger_sink_get_emit_fn(sink)) ||
                     lh_null_eq(lh_os_logger_sink_get_flush_fn(sink)) ||
                     lh_null_eq(lh_os_logger_sink_get_close_fn(sink)) || flush_ms == 0U ||
                     capacity == 0U,
                 lh_null);
    self = lh_ptr_rcast(
        lh_os_logger_queue_t,
        lh_memory_allocator_try_alloc_zero(lh_addr_of(allocator), 1U, sizeof(lh_ptr_deref(self))));
    lh_return_if(lh_null_eq(self), lh_null);
    lh_memory_allocator_assign(lh_os_logger_queue_get_allocator_mut(self), lh_addr_of(allocator));
    lh_os_logger_queue_set_sink(self, lh_ptr_deref(sink));
    lh_os_logger_queue_set_flush_ms(self, flush_ms);
    lh_os_logger_queue_set_last_flush(self, lh_os_logger_queue_now_ms());
    lh_os_logger_queue_set_reported(self, 0U);
    lh_os_logger_queue_init_counters(self);
    lh_os_mutex_init(lh_os_logger_queue_get_mutex_mut(self));
    lh_os_condition_init(lh_os_logger_queue_get_done_mut(self));
    lh_os_logger_queue_set_storage(
        self, lh_ptr_rcast(lh_os_logger_record_t, lh_memory_allocator_try_alloc_zero(
                                                      lh_os_logger_queue_get_allocator_mut(self),
                                                      capacity, sizeof(lh_os_logger_record_t))));
    if (lh_null_ne(lh_os_logger_queue_get_storage(self)))
    {
        lh_os_logger_queue_set_records(self,
                                       lh_spsc_create(lh_os_logger_queue_get_storage(self),
                                                      capacity, sizeof(lh_os_logger_record_t)));
    }
    lh_os_logger_queue_set_loop(self,
                                lh_os_loop_try_create(flush_ms, lh_os_logger_queue_step, self));
    if (lh_null_eq(lh_os_logger_queue_get_records(self)) ||
        lh_null_eq(lh_os_logger_queue_get_loop(self)) ||
        !lh_os_mutex_open(lh_os_logger_queue_get_mutex_mut(self)) ||
        !lh_os_condition_open(lh_os_logger_queue_get_done_mut(self)) ||
        !lh_os_loop_try_start(lh_os_logger_queue_get_loop(self)))
    {
        lh_os_logger_queue_free(self);
        return lh_null;
    }
    return self;
}

lh_void
lh_os_logger_queue_destroy(lh_os_logger_queue_t *self)
{
    lh_return_if(lh_null_eq(self));
    lh_os_logger_queue_free(self);
}
