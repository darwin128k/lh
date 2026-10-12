#include <lh/memory/bounds/allocated.h>
#include <lh/runtime/allocator.h>
#include <lh/util/ptr.h>
#include <lh/assert.h>

lh_usize_t
lh_memory_bounds_allocated_get_size(const lh_memory_bounds_allocated_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_memory_bounds_is_uninitialized(self) ? 0 : lh_memory_bounds_get_size(self);
}

lh_void
lh_memory_bounds_allocated_clear(lh_memory_bounds_allocated_t *self)
{
    lh_assert_runtime_ref(self);

    lh_runtime_allocator_free(lh_memory_bounds_get_begin(self));
    lh_memory_bounds_clear(self);
}

lh_void
lh_memory_bounds_allocated_resize(lh_memory_bounds_allocated_t *self, lh_usize_t size)
{
    lh_assert_runtime_ref(self);

    lh_ptr begin = lh_memory_bounds_get_begin(self);
    const lh_usize_t cur_size = lh_memory_bounds_allocated_get_size(self);

    lh_ptr allocated = lh_runtime_allocator_realloc(begin, cur_size, size);
    if (lh_ptr_is_set(allocated))
    {
        lh_memory_bounds_set_by_size(self, allocated, size);
        return;
    }

    lh_memory_bounds_clear(self);
}

lh_void
lh_memory_bounds_allocated_exchange(lh_memory_bounds_allocated_t *self,
                                    lh_memory_bounds_allocated_t *other)
{
    lh_memory_bounds_allocated_clear(self);
    lh_memory_bounds_swap_and_clear(self, other);
}
