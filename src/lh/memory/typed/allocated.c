#include <lh/memory/typed/allocated.h>
#include <lh/util/math.h>

lh_void
lh_memory_typed_allocated_clear(lh_memory_typed_allocated_t *self)
{
    lh_memory_bounds_allocated_clear(lh_memory_typed_get_bounds(self));
}

lh_void
lh_memory_typed_allocated_resize(lh_memory_typed_allocated_t *self, lh_usize_t count)
{
    const lh_usize_t type_size = lh_memory_typed_get_type_size(self);
    lh_memory_bounds_allocated_resize(lh_memory_typed_get_bounds(self), lh_math_mul(count, type_size));
}

lh_void
lh_memory_typed_allocated_exchange(lh_memory_typed_allocated_t *self,
                                   lh_memory_typed_allocated_t *other)
{
    const lh_usize_t other_type_size = lh_memory_typed_get_type_size(other);
    lh_memory_bounds_allocated_exchange(lh_memory_typed_get_bounds(self),
                                        lh_memory_typed_get_bounds(other));
    lh_memory_typed_retype(self, other_type_size);
}
