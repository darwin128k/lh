#include <lh/memory/bounds/allocated.h>
#include <lh/runtime/allocator.h>

lh_usize_t
lh_memory_bounds_allocated_get_size(const lh_memory_bounds_allocated_t *self) {
    return lh_memory_bounds_is_uninitialized(self) ? 0 : lh_memory_bounds_get_size(self);
}

lh_void
lh_memory_bounds_allocated_clear(lh_memory_bounds_allocated_t *self) {
    lh_ptr ptr = lh_memory_bounds_get_begin(self);
    lh_runtime_allocator_free(ptr);
    lh_memory_bounds_clear(self);
}

lh_void
lh_memory_bounds_allocated_exchange(lh_memory_bounds_allocated_t *self,
                                   lh_memory_bounds_allocated_t *other) {
    lh_memory_bounds_allocated_clear(self);
    lh_memory_bounds_swap(self, other);
}

lh_void
lh_memory_bounds_allocated_resize(lh_memory_bounds_allocated_t *self, lh_usize_t size) {
    lh_ptr old_ptr = lh_memory_bounds_get_begin(self);
    lh_usize_t cur_size = lh_memory_bounds_allocated_get_size(self);

    lh_ptr new_ptr = lh_runtime_allocator_realloc(old_ptr, cur_size, size);
    lh_memory_bounds_set_by_size_or_clear(self, new_ptr, size);
}
