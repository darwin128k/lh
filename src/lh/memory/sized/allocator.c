#include <lh/memory/sized/allocator.h>
#include <lh/assert.h>
#include <lh/size/ops.h>
#include <lh/util/return.h>

lh_ptr
lh_memory_sized_allocator_get_block(const lh_ptr ptr)
{
    return lh_ptr_sub_by_offset_unsafe(lh_void, ptr, LH_MEMORY_SIZED_ALLOCATOR_HEADER_SIZE);
}

lh_ptr
lh_memory_sized_allocator_get_data(const lh_ptr block)
{
    return lh_ptr_add_by_offset_unsafe(lh_void, block, LH_MEMORY_SIZED_ALLOCATOR_HEADER_SIZE);
}

lh_usize_t
lh_memory_sized_allocator_get_block_size(lh_usize_t size)
{
    return lh_size_add(size, LH_MEMORY_SIZED_ALLOCATOR_HEADER_SIZE);
}

/* The size sits at the block start, in front of the user data. */
lh_usize_t
lh_memory_sized_allocator_get_size(const lh_ptr ptr)
{
    lh_assert_runtime_ref(ptr);
    return lh_ptr_deref_of(lh_usize_t, lh_memory_sized_allocator_get_block(ptr));
}

lh_void
lh_memory_sized_allocator_set_size(lh_ptr ptr, lh_usize_t size)
{
    lh_assert_runtime_ref(ptr);
    lh_ptr_deref_of(lh_usize_t, lh_memory_sized_allocator_get_block(ptr)) = size;
}

lh_ptr
lh_memory_sized_allocator_alloc(lh_memory_sized_allocator_t *self, lh_usize_t size)
{
    lh_assert_runtime_ref(self);

    lh_ptr block = lh_memory_allocator_alloc(self, lh_memory_sized_allocator_get_block_size(size));
    lh_ptr data = lh_memory_sized_allocator_get_data(block);
    lh_memory_sized_allocator_set_size(data, size);
    return data;
}

lh_void
lh_memory_sized_allocator_dealloc(lh_memory_sized_allocator_t *self, lh_ptr ptr)
{
    lh_return_ifn(ptr);
    lh_assert_runtime_ref(self);

    lh_memory_allocator_dealloc(self, lh_memory_sized_allocator_get_block(ptr));
}

lh_ptr
lh_memory_sized_allocator_realloc(lh_memory_sized_allocator_t *self, lh_ptr ptr,
                                  lh_usize_t new_size)
{
    lh_assert_runtime_ref(self);
    lh_return_ifn(ptr, lh_memory_sized_allocator_alloc(self, new_size));
    if (new_size == 0U)
    {
        lh_memory_sized_allocator_dealloc(self, ptr);
        return lh_null;
    }

    const lh_usize_t old_size = lh_memory_sized_allocator_get_size(ptr);
    lh_ptr block = lh_memory_allocator_realloc(
        self, lh_memory_sized_allocator_get_block(ptr),
        lh_memory_sized_allocator_get_block_size(old_size),
        lh_memory_sized_allocator_get_block_size(new_size));
    lh_ptr data = lh_memory_sized_allocator_get_data(block);
    lh_memory_sized_allocator_set_size(data, new_size);
    return data;
}
