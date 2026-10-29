#include <lh/memory/typed/allocated.h>
#include <lh/math.h>
#include <lh/assert.h>
#include <lh/util/interval.h>
#include <lh/size/limits.h>

lh_void
lh_memory_typed_allocated_clear(lh_memory_typed_allocated_t *self)
{
    lh_memory_bounds_allocated_clear(lh_memory_typed_get_bounds_mut(self));
}

lh_bool_t
lh_memory_typed_allocated_try_resize(lh_memory_typed_allocated_t *self, lh_usize_t count)
{
    const lh_usize_t type_size = lh_memory_typed_get_type_size(self);
    if (lh_interval_closed_is_mul_overflow(count, type_size, 0u, LH_USIZE_T_MAX))
    {
        return lh_bool_false;
    }
    return lh_memory_bounds_allocated_try_resize(lh_memory_typed_get_bounds_mut(self),
                                                 lh_math_mul(count, type_size));
}

lh_void
lh_memory_typed_allocated_exchange(lh_memory_typed_allocated_t *self,
                                   lh_memory_typed_allocated_t *other)
{
    const lh_usize_t other_type_size = lh_memory_typed_get_type_size(other);
    lh_memory_bounds_allocated_exchange(lh_memory_typed_get_bounds_mut(self),
                                        lh_memory_typed_get_bounds_mut(other));
    lh_memory_typed_set_type_size(self, other_type_size);
}

lh_void
lh_memory_typed_allocated_resize(lh_memory_typed_allocated_t *self, lh_usize_t count)
{
    lh_runtime_check_ifn(lh_memory_typed_allocated_try_resize(self, count),
                         lh_runtime_error_code_memory_not_allocated);
}
