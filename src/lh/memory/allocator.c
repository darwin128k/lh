#include <lh/memory.h>
#include <lh/math/ops.h>
#include <lh/memory/std.h>
#include <lh/math.h>
#include <lh/util/return.h>
#include <lh/assert.h>
#include <lh/config.h>

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

lh_void
lh_memory_allocator_dealloc(lh_memory_allocator_t *self, lh_ptr ptr)
{
    lh_return_ifn(ptr);

    lh_memory_allocator_dealloc_cb dealloc_cb = lh_memory_allocator_get_dealloc_cb(self);
    lh_runtime_check_ifn(dealloc_cb, lh_runtime_error_code_deallocator_function_not_initialized);

    dealloc_cb(lh_memory_allocator_get_context(self), ptr);
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
    if (lh_math_is_positive(new_size) &&
        (lh_null_eq(old_ptr) ||
         (lh_math_ne(old_size, new_size) && lh_null_eq(lh_memory_allocator_get_realloc_cb(self)))))
    {
        lh_runtime_check_ifn(lh_memory_allocator_get_alloc_cb(self),
                             lh_runtime_error_code_allocator_function_not_initialized);
    }
    lh_ptr block = lh_memory_allocator_try_realloc(self, old_ptr, old_size, new_size);
    if (lh_math_is_positive(new_size))
    {
        lh_runtime_check_ifn(block, lh_runtime_error_code_memory_not_allocated);
    }
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

lh_ptr
lh_memory_allocator_try_alloc(lh_memory_allocator_t *self, lh_usize_t size)
{
    lh_memory_allocator_alloc_cb const allocate = lh_memory_allocator_get_alloc_cb(self);
    if (lh_null_eq(allocate))
    {
        return lh_null;
    }
    lh_ptr block = allocate(lh_memory_allocator_get_context(self), size);
#if (lh_math_eq(LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_INIT_ALLOCATED, LH_LIBRARY_OPTION_ON))
    if (lh_null_ne(block))
    {
        lh_memory_std_set(block, 0, size);
    }
#endif
    return block;
}

lh_ptr
lh_memory_allocator_try_realloc(lh_memory_allocator_t *self, lh_ptr old_ptr, lh_usize_t old_size,
                                lh_usize_t new_size)
{
    if (lh_math_is_zero(new_size))
    {
        lh_memory_allocator_dealloc(self, old_ptr);
        return lh_null;
    }
    if (lh_null_eq(old_ptr))
    {
        return lh_memory_allocator_try_alloc(self, new_size);
    }
    if (lh_math_eq(old_size, new_size))
    {
        return old_ptr;
    }
    lh_memory_allocator_realloc_cb const resize = lh_memory_allocator_get_realloc_cb(self);
    if (lh_null_ne(resize))
    {
        lh_ptr block = resize(lh_memory_allocator_get_context(self), old_ptr, old_size, new_size);
#if (lh_math_eq(LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_INIT_ALLOCATED, LH_LIBRARY_OPTION_ON))
        if (lh_null_ne(block) && lh_math_gt(new_size, old_size))
        {
            lh_memory_std_set(lh_ptr_add_by_offset_unsafe(lh_void, block, old_size), 0,
                              lh_math_sub(new_size, old_size));
        }
#endif
        return block;
    }
    lh_ptr block = lh_memory_allocator_try_alloc(self, new_size);
    if (lh_null_ne(block))
    {
        lh_memory_std_copy(block, old_ptr, lh_math_min(old_size, new_size));
        lh_memory_allocator_dealloc(self, old_ptr);
    }
    return block;
}
