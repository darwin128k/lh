/**
 * @file queue.c
 * @brief ::lh_os_logger_queue_t: a single-producer record queue drained by its own worker.
 *
 * The producer side is lock-free: a push copies into the SPSC ring and signals the
 * worker's condition without taking its mutex. A signal that lands just before the
 * worker starts to wait is lost, which costs at most one flush period: the worker
 * never sleeps longer than that. Sync requests and their completion go through
 * counters, compared with wrap-around.
 */

#include <lh/os/logger/queue.h>
#include <lh/assert/runtime.h>
#include <lh/attribute/unused.h>
#include <lh/cast/const.h>
#include <lh/os/thread.h>
#include <lh/os/condition.h>
#include <lh/os/tick.h>
#include <lh/runtime/allocator.h>
#include <lh/spsc.h>
#include <lh/util/return.h>
#include <stdatomic.h>

struct lh_os_logger_queue
{
    lh_os_logger_record_t *storage;
    lh_spsc_t *records;
    lh_os_logger_sink_t sink;
    lh_u32_t flush_ms;
    lh_os_thread_t worker;
    lh_os_mutex_t mutex;
    lh_os_condition_t wake;
    lh_os_condition_t done;
    atomic_uint dropped;
    atomic_uint requested;
    atomic_uint completed;
    atomic_bool stopping;
};

/* -- Level 1: the fields ------------------------------------------------------ */

static lh_os_logger_record_t *
lh_os_logger_queue_get_storage(const lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return self->storage;
}

static lh_void
lh_os_logger_queue_set_storage(lh_os_logger_queue_t *self, lh_os_logger_record_t *storage)
{
    lh_assert_runtime_ref(self);
    self->storage = storage;
}

static lh_spsc_t *
lh_os_logger_queue_get_records(const lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return self->records;
}

static lh_void
lh_os_logger_queue_set_records(lh_os_logger_queue_t *self, lh_spsc_t *records)
{
    lh_assert_runtime_ref(self);
    self->records = records;
}

static const lh_os_logger_sink_t *
lh_os_logger_queue_get_sink(const lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sink);
}

static lh_void
lh_os_logger_queue_set_sink(lh_os_logger_queue_t *self, lh_os_logger_sink_t sink)
{
    lh_assert_runtime_ref(self);
    self->sink = sink;
}

static lh_u32_t
lh_os_logger_queue_get_flush_ms(const lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return self->flush_ms;
}

static lh_void
lh_os_logger_queue_set_flush_ms(lh_os_logger_queue_t *self, lh_u32_t flush_ms)
{
    lh_assert_runtime_ref(self);
    self->flush_ms = flush_ms;
}

static lh_os_thread_t *
lh_os_logger_queue_get_worker_mut(lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->worker);
}

static lh_os_mutex_t *
lh_os_logger_queue_get_mutex_mut(lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->mutex);
}

static lh_os_condition_t *
lh_os_logger_queue_get_wake_mut(lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->wake);
}

static lh_os_condition_t *
lh_os_logger_queue_get_done_mut(lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->done);
}

static lh_u32_t
lh_os_logger_queue_load_dropped(const lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return atomic_load_explicit(lh_cast_const(atomic_uint *, lh_addr_of(self->dropped)),
                                memory_order_relaxed);
}

static lh_void
lh_os_logger_queue_add_dropped(lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    atomic_fetch_add_explicit(lh_addr_of(self->dropped), 1U, memory_order_relaxed);
}

static lh_u32_t
lh_os_logger_queue_load_requested(lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return atomic_load_explicit(lh_addr_of(self->requested), memory_order_acquire);
}

static lh_u32_t
lh_os_logger_queue_add_requested(lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return atomic_fetch_add_explicit(lh_addr_of(self->requested), 1U, memory_order_acq_rel) + 1U;
}

static lh_u32_t
lh_os_logger_queue_load_completed(lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return atomic_load_explicit(lh_addr_of(self->completed), memory_order_acquire);
}

static lh_void
lh_os_logger_queue_store_completed(lh_os_logger_queue_t *self, lh_u32_t completed)
{
    lh_assert_runtime_ref(self);
    atomic_store_explicit(lh_addr_of(self->completed), completed, memory_order_release);
}

static lh_bool_t
lh_os_logger_queue_is_stopping(lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    return atomic_load_explicit(lh_addr_of(self->stopping), memory_order_acquire) ? lh_bool_true
                                                                                  : lh_bool_false;
}

static lh_void
lh_os_logger_queue_set_stopping(lh_os_logger_queue_t *self, lh_bool_t stopping)
{
    lh_assert_runtime_ref(self);
    atomic_store_explicit(lh_addr_of(self->stopping), stopping, memory_order_release);
}

static lh_void
lh_os_logger_queue_init_counters(lh_os_logger_queue_t *self)
{
    lh_assert_runtime_ref(self);
    atomic_init(lh_addr_of(self->dropped), 0U);
    atomic_init(lh_addr_of(self->requested), 0U);
    atomic_init(lh_addr_of(self->completed), 0U);
    atomic_init(lh_addr_of(self->stopping), 0);
}

/* -- Level 2 and up ----------------------------------------------------------- */

static lh_u64_t
lh_os_logger_queue_now_ms(lh_void)
{
    return lh_os_tick_us() / 1000U;
}

/* True when @p done has reached @p wanted, counting through wrap-around. */
static lh_bool_t
lh_os_logger_queue_has_reached(lh_u32_t done, lh_u32_t wanted)
{
    return lh_cast_static(lh_s32_t, done - wanted) >= 0 ? lh_bool_true : lh_bool_false;
}

static lh_bool_t
lh_os_logger_queue_is_empty(lh_os_logger_queue_t *self)
{
    return lh_null_eq(lh_spsc_peek(lh_os_logger_queue_get_records(self)));
}

static lh_void
lh_os_logger_queue_emit_batch(lh_os_logger_queue_t *self)
{
    const lh_os_logger_sink_t *sink = lh_os_logger_queue_get_sink(self);
    lh_spsc_t *records = lh_os_logger_queue_get_records(self);

    /* A batch is bounded, so sustained logging cannot starve the flush or the stop. */
    for (lh_uint_t count = lh_spsc_get_capacity(records); count > 0U; --count)
    {
        const lh_os_logger_record_t *record = lh_spsc_peek(records);

        lh_return_if(lh_null_eq(record));
        lh_os_logger_sink_get_emit_fn(sink)(lh_os_logger_sink_get_user(sink), record);
        lh_spsc_release(records);
    }
}

/* Flush, then mark the sync requests seen before it as done and wake their waiters. */
static lh_void
lh_os_logger_queue_flush(lh_os_logger_queue_t *self, lh_u32_t request)
{
    const lh_os_logger_sink_t *sink = lh_os_logger_queue_get_sink(self);

    lh_os_logger_sink_get_flush_fn(sink)(lh_os_logger_sink_get_user(sink));
    LH_ATTRIBUTE_UNUSED(lh_os_mutex_lock(lh_os_logger_queue_get_mutex_mut(self)));
    lh_os_logger_queue_store_completed(self, request);
    LH_ATTRIBUTE_UNUSED(lh_os_condition_broadcast(lh_os_logger_queue_get_done_mut(self)));
    LH_ATTRIBUTE_UNUSED(lh_os_mutex_unlock(lh_os_logger_queue_get_mutex_mut(self)));
}

/* Sleep until a push, a sync or the end of the flush period. */
static lh_void
lh_os_logger_queue_wait(lh_os_logger_queue_t *self, lh_u32_t timeout_ms)
{
    LH_ATTRIBUTE_UNUSED(lh_os_mutex_lock(lh_os_logger_queue_get_mutex_mut(self)));
    if (lh_os_logger_queue_is_empty(self) && !lh_os_logger_queue_is_stopping(self) &&
        lh_os_logger_queue_load_requested(self) == lh_os_logger_queue_load_completed(self))
    {
        LH_ATTRIBUTE_UNUSED(lh_os_condition_wait_for(lh_os_logger_queue_get_wake_mut(self),
                                                     lh_os_logger_queue_get_mutex_mut(self),
                                                     timeout_ms));
    }
    LH_ATTRIBUTE_UNUSED(lh_os_mutex_unlock(lh_os_logger_queue_get_mutex_mut(self)));
}

static lh_void
lh_os_logger_queue_run(lh_ptr user)
{
    lh_os_logger_queue_t *self = lh_ptr_rcast(lh_os_logger_queue_t, user);
    const lh_os_logger_sink_t *sink = lh_os_logger_queue_get_sink(self);
    const lh_u64_t period = lh_os_logger_queue_get_flush_ms(self);
    lh_u64_t last_flush = lh_os_logger_queue_now_ms();
    lh_u32_t reported = 0U;

    for (;;)
    {
        const lh_u32_t request = lh_os_logger_queue_load_requested(self);
        const lh_u32_t losses = lh_os_logger_queue_load_dropped(self);
        lh_u64_t now;

        lh_os_logger_queue_emit_batch(self);
        if (losses != reported && lh_null_ne(lh_os_logger_sink_get_dropped_fn(sink)))
        {
            lh_os_logger_sink_get_dropped_fn(sink)(lh_os_logger_sink_get_user(sink),
                                                    losses - reported);
            reported = losses;
        }
        now = lh_os_logger_queue_now_ms();
        if (request != lh_os_logger_queue_load_completed(self) || now - last_flush >= period)
        {
            lh_os_logger_queue_flush(self, request);
            last_flush = now;
        }
        if (lh_os_logger_queue_is_empty(self))
        {
            if (lh_os_logger_queue_is_stopping(self))
            {
                break;
            }
            lh_os_logger_queue_wait(
                self, lh_cast_static(lh_u32_t, period - lh_math_min(now - last_flush, period)));
        }
    }
    lh_os_logger_queue_flush(self, lh_os_logger_queue_load_requested(self));
    lh_os_logger_sink_get_close_fn(sink)(lh_os_logger_sink_get_user(sink));
}

/* Free what was made; each part may be missing. */
static lh_void
lh_os_logger_queue_free(lh_os_logger_queue_t *self)
{
    LH_ATTRIBUTE_UNUSED(lh_os_condition_close(lh_os_logger_queue_get_done_mut(self)));
    LH_ATTRIBUTE_UNUSED(lh_os_condition_close(lh_os_logger_queue_get_wake_mut(self)));
    LH_ATTRIBUTE_UNUSED(lh_os_mutex_close(lh_os_logger_queue_get_mutex_mut(self)));
    lh_spsc_destroy(lh_os_logger_queue_get_records(self));
    lh_runtime_allocator_free(lh_os_logger_queue_get_storage(self));
    lh_runtime_allocator_free(self);
}

lh_os_logger_queue_t *
lh_os_logger_queue_try_create(lh_uint_t capacity, lh_u32_t flush_ms,
                              const lh_os_logger_sink_t *sink)
{
    lh_os_logger_queue_t *self;

    lh_return_if(lh_null_eq(sink) || lh_null_eq(lh_os_logger_sink_get_emit_fn(sink)) ||
                     lh_null_eq(lh_os_logger_sink_get_flush_fn(sink)) ||
                     lh_null_eq(lh_os_logger_sink_get_close_fn(sink)) || flush_ms == 0U ||
                     capacity == 0U,
                 lh_null);
    self = lh_ptr_rcast(lh_os_logger_queue_t, lh_runtime_allocator_try_alloc_zero(
                                                  1U, sizeof(lh_ptr_deref(self))));
    lh_return_if(lh_null_eq(self), lh_null);
    lh_os_logger_queue_set_sink(self, lh_ptr_deref(sink));
    lh_os_logger_queue_set_flush_ms(self, flush_ms);
    lh_os_logger_queue_init_counters(self);
    lh_os_thread_init(lh_os_logger_queue_get_worker_mut(self));
    lh_os_mutex_init(lh_os_logger_queue_get_mutex_mut(self));
    lh_os_condition_init(lh_os_logger_queue_get_wake_mut(self));
    lh_os_condition_init(lh_os_logger_queue_get_done_mut(self));
    lh_os_logger_queue_set_storage(
        self, lh_ptr_rcast(lh_os_logger_record_t, lh_runtime_allocator_try_alloc_zero(
                                                      capacity, sizeof(lh_os_logger_record_t))));
    if (lh_null_ne(lh_os_logger_queue_get_storage(self)))
    {
        lh_os_logger_queue_set_records(self,
                                       lh_spsc_create(lh_os_logger_queue_get_storage(self),
                                                      capacity, sizeof(lh_os_logger_record_t)));
    }
    if (lh_null_eq(lh_os_logger_queue_get_records(self)) ||
        !lh_os_mutex_open(lh_os_logger_queue_get_mutex_mut(self)) ||
        !lh_os_condition_open(lh_os_logger_queue_get_wake_mut(self)) ||
        !lh_os_condition_open(lh_os_logger_queue_get_done_mut(self)) ||
        !lh_os_thread_start(lh_os_logger_queue_get_worker_mut(self), lh_os_logger_queue_run, self,
                            -1, 0U))
    {
        lh_os_logger_queue_free(self);
        return lh_null;
    }
    return self;
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
    LH_ATTRIBUTE_UNUSED(lh_os_condition_signal(lh_os_logger_queue_get_wake_mut(self)));
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
    LH_ATTRIBUTE_UNUSED(lh_os_condition_signal(lh_os_logger_queue_get_wake_mut(self)));
    for (;;)
    {
        const lh_u64_t waited = lh_os_logger_queue_now_ms() - start;

        reached = lh_os_logger_queue_has_reached(lh_os_logger_queue_load_completed(self), request);
        if (reached || waited >= timeout_ms)
        {
            break;
        }
        LH_ATTRIBUTE_UNUSED(lh_os_condition_wait_for(lh_os_logger_queue_get_done_mut(self),
                                                     lh_os_logger_queue_get_mutex_mut(self),
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

lh_void
lh_os_logger_queue_destroy(lh_os_logger_queue_t *self)
{
    lh_return_if(lh_null_eq(self));
    LH_ATTRIBUTE_UNUSED(lh_os_mutex_lock(lh_os_logger_queue_get_mutex_mut(self)));
    lh_os_logger_queue_set_stopping(self, lh_bool_true);
    LH_ATTRIBUTE_UNUSED(lh_os_condition_signal(lh_os_logger_queue_get_wake_mut(self)));
    LH_ATTRIBUTE_UNUSED(lh_os_mutex_unlock(lh_os_logger_queue_get_mutex_mut(self)));
    LH_ATTRIBUTE_UNUSED(lh_os_thread_join(lh_os_logger_queue_get_worker_mut(self)));
    lh_os_logger_queue_free(self);
}
