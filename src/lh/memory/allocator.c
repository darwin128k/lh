#include <lh/memory.h>
#include <lh/math.h>
#include <lh/util/return.h>
#include <lh/assert.h>
#include <lh/config.h>
#include <lh/size/limits.h>
#include <lh/util/interval.h>
#include <lh/attribute/unused.h>

lh_void
lh_memory_allocator_set_realloc_cb(lh_memory_allocator_t *self,
                                   lh_memory_allocator_realloc_cb realloc_cb)
{
    lh_assert_runtime_ref(self);
    self->realloc_cb = realloc_cb;
}

lh_void
lh_memory_allocator_set_context(lh_memory_allocator_t *self, lh_ptr context)
{
    lh_assert_runtime_ref(self);
    self->context = context;
}

lh_ptr
lh_memory_allocator_get_context(const lh_memory_allocator_t *self)
{
    lh_assert_runtime_ref(self);
    return self->context;
}

lh_memory_allocator_alloc_cb
lh_memory_allocator_get_alloc_cb(const lh_memory_allocator_t *self)
{
    lh_assert_runtime_ref(self);
    return self->alloc_cb;
}

lh_memory_allocator_dealloc_cb
lh_memory_allocator_get_dealloc_cb(const lh_memory_allocator_t *self)
{
    lh_assert_runtime_ref(self);
    return self->dealloc_cb;
}

lh_memory_allocator_realloc_cb
lh_memory_allocator_get_realloc_cb(const lh_memory_allocator_t *self)
{
    lh_assert_runtime_ref(self);
    return self->realloc_cb;
}

lh_void
lh_memory_allocator_set_alloc_cb(lh_memory_allocator_t *self, lh_memory_allocator_alloc_cb alloc_cb)
{
    lh_assert_runtime_ref(self);
    self->alloc_cb = alloc_cb;
    lh_memory_allocator_set_realloc_cb(self, lh_null); /* would no longer match the heap */
}

lh_void
lh_memory_allocator_set_dealloc_cb(lh_memory_allocator_t *self,
                                   lh_memory_allocator_dealloc_cb dealloc_cb)
{
    lh_assert_runtime_ref(self);
    self->dealloc_cb = dealloc_cb;
    lh_memory_allocator_set_realloc_cb(self, lh_null); /* would no longer match the heap */
}

lh_ptr
lh_memory_allocator_alloc(lh_memory_allocator_t *self, lh_usize_t size)
{
    lh_runtime_check_ifn(lh_memory_allocator_get_alloc_cb(self),
                         lh_runtime_error_code_allocator_function_not_initialized);
    lh_ptr block = lh_memory_allocator_try_alloc(self, size);
    lh_runtime_check_ifn(block, lh_runtime_error_code_memory_not_allocated);
    return block;
}

lh_ptr
lh_memory_allocator_alloc_zero(lh_memory_allocator_t *self, lh_usize_t count,
                               lh_usize_t item_size)
{
    lh_runtime_check_ifn(lh_memory_allocator_get_alloc_cb(self),
                         lh_runtime_error_code_allocator_function_not_initialized);
    lh_ptr block = lh_memory_allocator_try_alloc_zero(self, count, item_size);
    lh_runtime_check_ifn(block, lh_runtime_error_code_memory_not_allocated);
    return block;
}

lh_void
lh_memory_allocator_dealloc(lh_memory_allocator_t *self, lh_ptr ptr)
{
    lh_runtime_check_ifn(lh_memory_allocator_try_dealloc(self, ptr),
                         lh_runtime_error_code_deallocator_function_not_initialized);
}

lh_void
lh_memory_allocator_set(lh_memory_allocator_t *self, lh_memory_allocator_alloc_cb alloc_cb,
                        lh_memory_allocator_dealloc_cb dealloc_cb)
{
    lh_memory_allocator_set_alloc_cb(self, alloc_cb);
    lh_memory_allocator_set_dealloc_cb(self, dealloc_cb);
}

lh_ptr
lh_memory_allocator_realloc(lh_memory_allocator_t *self, lh_ptr old_ptr, lh_usize_t old_size,
                            lh_usize_t new_size)
{
    if (lh_math_is_zero(new_size))
    {
        lh_memory_allocator_dealloc(self, old_ptr);
        return lh_null;
    }
    lh_runtime_check_if(lh_memory_allocator_realloc_needs_alloc(self, old_ptr, old_size, new_size) &&
                            lh_null_eq(lh_memory_allocator_get_alloc_cb(self)),
                        lh_runtime_error_code_allocator_function_not_initialized);
    lh_ptr block = lh_memory_allocator_try_realloc(self, old_ptr, old_size, new_size);
    lh_runtime_check_ifn(block, lh_runtime_error_code_memory_not_allocated);
    return block;
}

lh_void
lh_memory_allocator_init(lh_memory_allocator_t *self, lh_memory_allocator_alloc_cb alloc_cb,
                         lh_memory_allocator_dealloc_cb dealloc_cb)
{
    lh_memory_allocator_set(self, alloc_cb, dealloc_cb);
    lh_memory_allocator_set_context(self, lh_null);
}

lh_void
lh_memory_allocator_deinit(lh_memory_allocator_t *self)
{
    lh_memory_allocator_set(self, lh_null, lh_null);
    lh_memory_allocator_set_context(self, lh_null);
}

lh_void
lh_memory_allocator_assign(lh_memory_allocator_t *self, const lh_memory_allocator_t *other)
{
    lh_memory_allocator_set(self, lh_memory_allocator_get_alloc_cb(other),
                            lh_memory_allocator_get_dealloc_cb(other));
    lh_memory_allocator_set_realloc_cb(self, lh_memory_allocator_get_realloc_cb(other));
    lh_memory_allocator_set_context(self, lh_memory_allocator_get_context(other));
}

lh_void
lh_memory_allocator_init_tail(lh_ptr block, lh_usize_t old_size, lh_usize_t new_size)
{
#if (lh_math_eq(LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_INIT_ALLOCATED, LH_LIBRARY_OPTION_ON))
    lh_return_if(lh_null_eq(block) || lh_math_le(new_size, old_size));
    lh_memory_std_set(lh_ptr_add_by_offset_unsafe(lh_void, block, old_size), 0,
                      lh_math_sub(new_size, old_size));
#else
    LH_ATTRIBUTE_UNUSED(block);
    LH_ATTRIBUTE_UNUSED(old_size);
    LH_ATTRIBUTE_UNUSED(new_size);
#endif
}

lh_ptr
lh_memory_allocator_try_alloc(lh_memory_allocator_t *self, lh_usize_t size)
{
    lh_memory_allocator_alloc_cb const allocate = lh_memory_allocator_get_alloc_cb(self);
    lh_return_if(lh_null_eq(allocate), lh_null);
    lh_ptr block = allocate(lh_memory_allocator_get_context(self), size);
    lh_memory_allocator_init_tail(block, 0, size);
    return block;
}

lh_ptr
lh_memory_allocator_try_alloc_zero(lh_memory_allocator_t *self, lh_usize_t count,
                                   lh_usize_t item_size)
{
    lh_return_if(lh_interval_closed_is_mul_overflow(count, item_size, 0, LH_USIZE_T_MAX),
                 lh_null);
    const lh_usize_t size = lh_math_mul(count, item_size);
    lh_ptr block = lh_memory_allocator_try_alloc(self, size);
    lh_return_if(lh_null_eq(block), lh_null);
    lh_memory_std_set(block, 0, size);
    return block;
}

lh_bool_t
lh_memory_allocator_try_dealloc(lh_memory_allocator_t *self, lh_ptr ptr)
{
    lh_return_ifn(ptr, lh_bool_true);
    lh_memory_allocator_dealloc_cb const release = lh_memory_allocator_get_dealloc_cb(self);
    lh_return_if(lh_null_eq(release), lh_bool_false);
    release(lh_memory_allocator_get_context(self), ptr);
    return lh_bool_true;
}

lh_ptr
lh_memory_allocator_try_resize(lh_memory_allocator_t *self, lh_ptr old_ptr, lh_usize_t old_size,
                               lh_usize_t new_size)
{
    lh_memory_allocator_realloc_cb const resize = lh_memory_allocator_get_realloc_cb(self);
    lh_return_if(lh_null_eq(resize), lh_null);
    lh_ptr block = resize(lh_memory_allocator_get_context(self), old_ptr, old_size, new_size);
    lh_memory_allocator_init_tail(block, old_size, new_size);
    return block;
}

lh_ptr
lh_memory_allocator_try_move(lh_memory_allocator_t *self, lh_ptr old_ptr, lh_usize_t old_size,
                             lh_usize_t new_size)
{
    lh_return_if(lh_null_eq(lh_memory_allocator_get_dealloc_cb(self)), lh_null);
    lh_ptr block = lh_memory_allocator_try_alloc(self, new_size);
    lh_return_if(lh_null_eq(block), lh_null);
    lh_memory_std_copy(block, old_ptr, lh_math_min(old_size, new_size));
    lh_memory_allocator_try_dealloc(self, old_ptr);
    return block;
}

lh_bool_t
lh_memory_allocator_realloc_needs_alloc(const lh_memory_allocator_t *self, lh_ptr old_ptr,
                                        lh_usize_t old_size, lh_usize_t new_size)
{
    return lh_math_is_positive(new_size) &&
           (lh_null_eq(old_ptr) ||
            (lh_math_ne(old_size, new_size) &&
             lh_null_eq(lh_memory_allocator_get_realloc_cb(self))));
}

lh_ptr
lh_memory_allocator_try_realloc(lh_memory_allocator_t *self, lh_ptr old_ptr, lh_usize_t old_size,
                                lh_usize_t new_size)
{
    if (lh_math_is_zero(new_size))
    {
        lh_memory_allocator_try_dealloc(self, old_ptr);
        return lh_null;
    }
    lh_return_if(lh_null_eq(old_ptr), lh_memory_allocator_try_alloc(self, new_size));
    lh_return_if(lh_math_eq(old_size, new_size), old_ptr);
    lh_return_if(lh_null_ne(lh_memory_allocator_get_realloc_cb(self)),
                 lh_memory_allocator_try_resize(self, old_ptr, old_size, new_size));
    return lh_memory_allocator_try_move(self, old_ptr, old_size, new_size);
}
