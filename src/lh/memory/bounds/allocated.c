#include <lh/memory/bounds/allocated.h>
#include <lh/runtime/allocator.h>
#include <lh/assert.h>
#include <lh/util/return.h>

lh_void
lh_memory_bounds_allocated_clear(lh_memory_bounds_allocated_t *self)
{
    lh_assert_runtime_ref(self);

    lh_runtime_allocator_free(lh_memory_bounds_get_begin(self));
    lh_memory_bounds_clear(self);
}

lh_bool_t
lh_memory_bounds_allocated_try_resize(lh_memory_bounds_allocated_t *self, lh_usize_t size)
{
    lh_ptr allocated;

    lh_assert_runtime_ref(self);
    if (lh_math_eq(size, 0u))
    {
        lh_return_ifn(lh_runtime_allocator_try_free(lh_memory_bounds_get_begin(self)),
                      lh_bool_false);
        lh_memory_bounds_clear(self);
        return lh_bool_true;
    }
    allocated = lh_runtime_allocator_try_realloc(lh_memory_bounds_get_begin(self),
                                                 lh_memory_bounds_get_size(self), size);
    lh_return_if(lh_ptr_is_null(allocated), lh_bool_false);
    lh_memory_bounds_set(self, allocated, size);
    return lh_bool_true;
}

lh_void
lh_memory_bounds_allocated_resize(lh_memory_bounds_allocated_t *self, lh_usize_t size)
{
    lh_runtime_check_ifn(lh_memory_bounds_allocated_try_resize(self, size),
                         lh_runtime_error_code_memory_not_allocated);
}

lh_void
lh_memory_bounds_allocated_exchange(lh_memory_bounds_allocated_t *self,
                                    lh_memory_bounds_allocated_t *other)
{
    lh_memory_bounds_allocated_clear(self);
    lh_memory_bounds_swap_and_clear(self, other);
}
