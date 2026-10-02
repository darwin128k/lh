#include <lh/memory/sized/allocator.h>
#include <lh/assert.h>
#include <lh/null.h>
#include <lh/numeric/limits.h>
#include <lh/runtime/assert.h>
#include <lh/runtime/error/code.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

/* User data <-> block start, and the size stored at the block start. */
#define lh_memory_sized_allocator_block_of(ptr)                                                    \
    lh_ptr_sub_by_offset_unsafe(lh_void, (ptr), LH_MEMORY_SIZED_ALLOCATOR_HEADER_SIZE)
#define lh_memory_sized_allocator_data_of(block)                                                   \
    lh_ptr_add_by_offset_unsafe(lh_void, (block), LH_MEMORY_SIZED_ALLOCATOR_HEADER_SIZE)
#define lh_memory_sized_allocator_size_at(block) (*lh_ptr_rcast(lh_usize_t, (block)))

/* Bytes `inner` is asked for: the header plus @p size, failing on overflow. */
static lh_usize_t
lh_memory_sized_allocator_block_size(lh_usize_t size)
{
    lh_runtime_check_if(lh_math_gt(size, lh_math_sub(LH_USIZE_T_MAX, LH_MEMORY_SIZED_ALLOCATOR_HEADER_SIZE)),
                        lh_runtime_error_code_overflow);
    return lh_math_add(size, LH_MEMORY_SIZED_ALLOCATOR_HEADER_SIZE);
}

lh_void
lh_memory_sized_allocator_init(lh_memory_sized_allocator_t *self, lh_memory_allocator_t *inner)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(inner);
    self->inner = inner;
}

lh_ptr
lh_memory_sized_allocator_alloc(lh_memory_sized_allocator_t *self, lh_usize_t size)
{
    lh_assert_runtime_ref(self);

    lh_ptr block = lh_memory_allocator_alloc(self->inner, lh_memory_sized_allocator_block_size(size));
    lh_memory_sized_allocator_size_at(block) = size;
    return lh_memory_sized_allocator_data_of(block);
}

lh_void
lh_memory_sized_allocator_dealloc(lh_memory_sized_allocator_t *self, lh_ptr ptr)
{
    lh_return_ifn(ptr);
    lh_assert_runtime_ref(self);

    lh_memory_allocator_dealloc(self->inner, lh_memory_sized_allocator_block_of(ptr));
}

lh_ptr
lh_memory_sized_allocator_realloc(lh_memory_sized_allocator_t *self, lh_ptr ptr, lh_usize_t new_size)
{
    lh_assert_runtime_ref(self);
    lh_return_ifn(ptr, lh_memory_sized_allocator_alloc(self, new_size));
    if (new_size == 0U)
    {
        lh_memory_sized_allocator_dealloc(self, ptr);
        return lh_null;
    }

    lh_ptr old_block = lh_memory_sized_allocator_block_of(ptr);
    const lh_usize_t old_size = lh_memory_sized_allocator_size_at(old_block);
    lh_ptr block = lh_memory_allocator_realloc(self->inner, old_block,
                                               lh_math_add(old_size, LH_MEMORY_SIZED_ALLOCATOR_HEADER_SIZE),
                                               lh_memory_sized_allocator_block_size(new_size));
    lh_memory_sized_allocator_size_at(block) = new_size;
    return lh_memory_sized_allocator_data_of(block);
}

lh_usize_t
lh_memory_sized_allocator_get_size(const lh_ptr ptr)
{
    lh_assert_runtime_ref(ptr);
    return lh_memory_sized_allocator_size_at(lh_memory_sized_allocator_block_of(ptr));
}
