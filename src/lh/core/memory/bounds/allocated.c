#include <lh/core/memory/bounds/allocated.h>
#include <lh/core/runtime/allocator.h>
#include <lh/core/assert.h>
#include <lh/core/util/return.h>

lh_void
lh_memory_bounds_allocated_clear_by(lh_memory_bounds_allocated_t *self,
                                    lh_memory_allocator_t *allocator)
{
    lh_assert_runtime_ref(self);

    lh_memory_allocator_dealloc(allocator, lh_memory_bounds_get_begin(self));
    lh_memory_bounds_clear(self);
}

lh_void
lh_memory_bounds_allocated_clear(lh_memory_bounds_allocated_t *self)
{
    lh_memory_bounds_allocated_clear_by(self, lh_runtime_allocator_get_mut());
}

lh_bool_t
lh_memory_bounds_allocated_try_resize_by(lh_memory_bounds_allocated_t *self,
                                         lh_memory_allocator_t *allocator, lh_usize_t size)
{
    lh_assert_runtime_ref(self);
    if (lh_math_eq(size, 0u))
    {
        lh_return_ifn(lh_memory_allocator_try_dealloc(allocator, lh_memory_bounds_get_begin(self)),
                      lh_bool_false);
        lh_memory_bounds_clear(self);
        return lh_bool_true;
    }

    lh_ptr allocated = lh_memory_allocator_try_realloc(allocator, lh_memory_bounds_get_begin(self),
                                                       lh_memory_bounds_get_size(self), size);
    lh_return_if(lh_ptr_is_null(allocated), lh_bool_false);
    lh_memory_bounds_set(self, allocated, size);
    return lh_bool_true;
}

lh_bool_t
lh_memory_bounds_allocated_try_resize(lh_memory_bounds_allocated_t *self, lh_usize_t size)
{
    return lh_memory_bounds_allocated_try_resize_by(self, lh_runtime_allocator_get_mut(), size);
}

lh_void
lh_memory_bounds_allocated_resize(lh_memory_bounds_allocated_t *self, lh_usize_t size)
{
    lh_runtime_check_ifn(lh_memory_bounds_allocated_try_resize(self, size),
                         lh_runtime_error_code_memory_not_allocated);
}

lh_void
lh_memory_bounds_allocated_exchange_by(lh_memory_bounds_allocated_t *self,
                                       lh_memory_allocator_t *allocator,
                                       lh_memory_bounds_allocated_t *other)
{
    lh_memory_bounds_allocated_clear_by(self, allocator);
    lh_memory_bounds_swap_and_clear(self, other);
}

lh_void
lh_memory_bounds_allocated_exchange(lh_memory_bounds_allocated_t *self,
                                    lh_memory_bounds_allocated_t *other)
{
    lh_memory_bounds_allocated_exchange_by(self, lh_runtime_allocator_get_mut(), other);
}
