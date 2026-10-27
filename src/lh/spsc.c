#include <lh/spsc.h>
#include <stdatomic.h>
#include <lh/runtime/allocator.h>
#include <lh/memory/std.h>
#include <lh/byte.h>
#include <lh/size/limits.h>
#include <lh/assert.h>
#include <lh/cast/const.h>

struct lh_spsc
{
    lh_memory_allocator_t allocator;
    lh_byte_t *storage;
    lh_uint_t capacity;
    lh_usize_t item_size;
    atomic_uint written, read;
};

lh_spsc_t *
lh_spsc_create(lh_ptr storage, lh_uint_t capacity, lh_usize_t item_size)
{
    lh_spsc_t *q;
    if (!storage || !capacity || (capacity & (capacity - 1)) || capacity > LH_UINT_T_MAX / 2 ||
        !item_size || item_size > LH_USIZE_T_MAX / capacity)
    {
        return lh_null;
    }
    lh_memory_allocator_t allocator = *lh_runtime_allocator();
    if (!allocator.alloc_cb || !allocator.dealloc_cb)
    {
        return lh_null;
    }
    q = allocator.alloc_cb(allocator.context, sizeof *q);
    if (!q)
    {
        return lh_null;
    }
    q->allocator = allocator;
    q->storage = storage;
    q->capacity = capacity;
    q->item_size = item_size;
    atomic_init(&q->written, 0);
    atomic_init(&q->read, 0);
    if (!atomic_is_lock_free(&q->written) || !atomic_is_lock_free(&q->read))
    {
        lh_memory_allocator_dealloc(&q->allocator, q);
        return lh_null;
    }
    return q;
}

lh_void
lh_spsc_destroy(lh_spsc_t *q)
{
    if (q)
    {
        lh_memory_allocator_dealloc(&q->allocator, q);
    }
}

lh_bool_t
lh_spsc_push(lh_spsc_t *q, const lh_void *item)
{
    lh_assert_runtime_ref(q);
    lh_assert_runtime_ref(item);
    lh_uint_t pos = atomic_load_explicit(&q->written, memory_order_relaxed);
    if (pos - atomic_load_explicit(&q->read, memory_order_acquire) >= q->capacity)
    {
        return lh_bool_false;
    }
    lh_memory_std_copy(q->storage + (pos & (q->capacity - 1)) * q->item_size,
                       lh_cast_const(lh_ptr, item), q->item_size);
    atomic_store_explicit(&q->written, pos + 1, memory_order_release);
    return lh_bool_true;
}

const lh_void *
lh_spsc_peek(lh_spsc_t *q)
{
    lh_assert_runtime_ref(q);
    lh_uint_t pos = atomic_load_explicit(&q->read, memory_order_relaxed);
    if (pos == atomic_load_explicit(&q->written, memory_order_acquire))
    {
        return lh_null;
    }
    return q->storage + (pos & (q->capacity - 1)) * q->item_size;
}

lh_void
lh_spsc_release(lh_spsc_t *q)
{
    lh_assert_runtime_ref(q);
    lh_assert_runtime_ref(lh_spsc_peek(q));
    lh_uint_t pos = atomic_load_explicit(&q->read, memory_order_relaxed);
    atomic_store_explicit(&q->read, pos + 1, memory_order_release);
}
