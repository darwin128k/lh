#include <lh/memory/typed/allocated.h>
#include <lh/util/math.h>

lh_void
lh_memory_typed_allocated_clear(lh_memory_typed_allocated_t *self)
{
    lh_memory_bounds_allocated_clear(lh_memory_typed_get_bounds(self));
    self->size = 0;
}

lh_void
lh_memory_typed_allocated_resize(lh_memory_typed_allocated_t *self, lh_usize_t count)
{
    /* self->size = count directly — count is already the element count being resized to, so
     * this needs no division (unlike lh_memory_typed_retype, which has to re-derive it from a
     * byte range because it isn't told the count up front). */
    const lh_usize_t type_size = lh_memory_typed_get_type_size(self);
    lh_memory_bounds_allocated_resize(lh_memory_typed_get_bounds(self),
                                      lh_math_mul(count, type_size));
    self->size = count;
}

lh_void
lh_memory_typed_allocated_exchange(lh_memory_typed_allocated_t *self,
                                   lh_memory_typed_allocated_t *other)
{
    const lh_usize_t other_type_size = lh_memory_typed_get_type_size(other);
    const lh_usize_t other_size = lh_memory_typed_get_size(other);
    lh_memory_bounds_allocated_exchange(lh_memory_typed_get_bounds(self),
                                        lh_memory_typed_get_bounds(other));
    self->type_size = other_type_size;
    self->size = other_size;
    /* lh_memory_bounds_allocated_exchange already leaves other's bounds empty; size follows. */
    other->size = 0;
}
