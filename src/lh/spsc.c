#include <lh/spsc.h>
#include <stdatomic.h>
#include <lh/runtime/allocator.h>
#include <lh/memory/std.h>
#include <lh/size/limits.h>
#include <lh/assert.h>
#include <lh/cast/const.h>
#include <lh/util/return.h>

struct lh_spsc
{
    lh_memory_allocator_t allocator;
    lh_byte_t *storage;
    lh_uint_t capacity;
    lh_usize_t item_size;
    atomic_uint written, read;
};

/* -- Level 1: the fields ------------------------------------------------------ */

lh_memory_allocator_t *
lh_spsc_get_allocator_mut(lh_spsc_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->allocator);
}

lh_byte_t *
lh_spsc_get_storage(const lh_spsc_t *self)
{
    lh_assert_runtime_ref(self);
    return self->storage;
}

lh_void
lh_spsc_set_storage(lh_spsc_t *self, lh_byte_t *storage)
{
    lh_assert_runtime_ref(self);
    self->storage = storage;
}

lh_uint_t
lh_spsc_get_capacity(const lh_spsc_t *self)
{
    lh_assert_runtime_ref(self);
    return self->capacity;
}

lh_void
lh_spsc_set_capacity(lh_spsc_t *self, lh_uint_t capacity)
{
    lh_assert_runtime_ref(self);
    self->capacity = capacity;
}

lh_usize_t
lh_spsc_get_item_size(const lh_spsc_t *self)
{
    lh_assert_runtime_ref(self);
    return self->item_size;
}

lh_void
lh_spsc_set_item_size(lh_spsc_t *self, lh_usize_t item_size)
{
    lh_assert_runtime_ref(self);
    self->item_size = item_size;
}

lh_uint_t
lh_spsc_get_written(lh_spsc_t *self)
{
    lh_assert_runtime_ref(self);
    return atomic_load_explicit(lh_addr_of(self->written), memory_order_acquire);
}

lh_void
lh_spsc_set_written(lh_spsc_t *self, lh_uint_t written)
{
    lh_assert_runtime_ref(self);
    atomic_store_explicit(lh_addr_of(self->written), written, memory_order_release);
}

lh_uint_t
lh_spsc_get_read(lh_spsc_t *self)
{
    lh_assert_runtime_ref(self);
    return atomic_load_explicit(lh_addr_of(self->read), memory_order_acquire);
}

lh_void
lh_spsc_set_read(lh_spsc_t *self, lh_uint_t read)
{
    lh_assert_runtime_ref(self);
    atomic_store_explicit(lh_addr_of(self->read), read, memory_order_release);
}

lh_void
lh_spsc_init_written(lh_spsc_t *self)
{
    lh_assert_runtime_ref(self);
    atomic_init(lh_addr_of(self->written), 0);
}

lh_void
lh_spsc_init_read(lh_spsc_t *self)
{
    lh_assert_runtime_ref(self);
    atomic_init(lh_addr_of(self->read), 0);
}

lh_bool_t
lh_spsc_is_written_lock_free(const lh_spsc_t *self)
{
    lh_assert_runtime_ref(self);
    return atomic_is_lock_free(lh_addr_of(self->written));
}

lh_bool_t
lh_spsc_is_read_lock_free(const lh_spsc_t *self)
{
    lh_assert_runtime_ref(self);
    return atomic_is_lock_free(lh_addr_of(self->read));
}

/* -- Level 2 and up ----------------------------------------------------------- */

lh_bool_t
lh_spsc_is_valid_shape(lh_ptr storage, lh_uint_t capacity, lh_usize_t item_size)
{
    return storage && capacity && !(capacity & (capacity - 1)) && capacity <= LH_UINT_T_MAX / 2 &&
           item_size && item_size <= LH_USIZE_T_MAX / capacity;
}

lh_void
lh_spsc_init_counters(lh_spsc_t *self)
{
    lh_spsc_init_written(self);
    lh_spsc_init_read(self);
}

lh_bool_t
lh_spsc_is_lock_free(const lh_spsc_t *self)
{
    return lh_spsc_is_written_lock_free(self) && lh_spsc_is_read_lock_free(self);
}

lh_byte_t *
lh_spsc_get_slot(const lh_spsc_t *self, lh_uint_t position)
{
    return lh_spsc_get_storage(self) +
           (position & (lh_spsc_get_capacity(self) - 1)) * lh_spsc_get_item_size(self);
}

lh_void
lh_spsc_destroy(lh_spsc_t *self)
{
    lh_return_if(lh_null_eq(self));
    lh_memory_allocator_dealloc(lh_spsc_get_allocator_mut(self), self);
}

lh_spsc_t *
lh_spsc_create(lh_ptr storage, lh_uint_t capacity, lh_usize_t item_size)
{
    lh_return_ifn(lh_spsc_is_valid_shape(storage, capacity, item_size), lh_null);
    lh_memory_allocator_t *runtime = lh_runtime_allocator();
    lh_return_if(lh_null_eq(lh_memory_allocator_get_alloc_cb(runtime)) ||
                     lh_null_eq(lh_memory_allocator_get_dealloc_cb(runtime)),
                 lh_null);
    lh_spsc_t *q = lh_memory_allocator_try_alloc(runtime, sizeof *q);
    lh_return_if(lh_null_eq(q), lh_null);
    /* The queue keeps its own copy: the runtime allocator may change afterwards. */
    lh_memory_allocator_assign(lh_spsc_get_allocator_mut(q), runtime);
    lh_spsc_set_storage(q, storage);
    lh_spsc_set_capacity(q, capacity);
    lh_spsc_set_item_size(q, item_size);
    lh_spsc_init_counters(q);
    if (!lh_spsc_is_lock_free(q))
    {
        lh_memory_allocator_dealloc(lh_spsc_get_allocator_mut(q), q);
        return lh_null;
    }
    return q;
}

lh_bool_t
lh_spsc_push(lh_spsc_t *self, const lh_void *item)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(item);
    const lh_uint_t position = lh_spsc_get_written(self);
    lh_return_if(position - lh_spsc_get_read(self) >= lh_spsc_get_capacity(self), lh_bool_false);
    lh_memory_std_copy(lh_spsc_get_slot(self, position), lh_cast_const(lh_ptr, item),
                       lh_spsc_get_item_size(self));
    lh_spsc_set_written(self, position + 1);
    return lh_bool_true;
}

const lh_void *
lh_spsc_peek(lh_spsc_t *self)
{
    lh_assert_runtime_ref(self);
    const lh_uint_t position = lh_spsc_get_read(self);
    lh_return_if(position == lh_spsc_get_written(self), lh_null);
    return lh_spsc_get_slot(self, position);
}

lh_void
lh_spsc_release(lh_spsc_t *self)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(lh_spsc_peek(self));
    lh_spsc_set_read(self, lh_spsc_get_read(self) + 1);
}
