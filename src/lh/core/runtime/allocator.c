#include <lh/core/runtime/allocator.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/memory/allocator/initializer.h>

/* One object for the whole program, empty until the host fills it: the core
   has no C library and no defaults. The host fills it before the first use
   and before it starts other threads, so every thread sees the same one. */
lh_memory_allocator_t m_runtime_allocator = lh_memory_allocator_empty_initializer();

lh_memory_allocator_t *
lh_runtime_allocator_get_mut(void)
{
    return lh_addr_of(m_runtime_allocator);
}

const lh_memory_allocator_t *
lh_runtime_allocator_get(void)
{
    return lh_addr_of(m_runtime_allocator);
}

lh_ptr
lh_runtime_allocator_alloc(lh_usize_t size)
{
    return lh_memory_allocator_alloc(lh_runtime_allocator_get_mut(), size);
}

lh_void
lh_runtime_allocator_free(lh_ptr ptr)
{
    lh_memory_allocator_dealloc(lh_runtime_allocator_get_mut(), ptr);
}

lh_ptr
lh_runtime_allocator_realloc(lh_ptr old_ptr, lh_usize_t old_size, lh_usize_t new_size)
{
    return lh_memory_allocator_realloc(lh_runtime_allocator_get_mut(), old_ptr, old_size, new_size);
}

lh_ptr
lh_runtime_allocator_alloc_zero(lh_usize_t count, lh_usize_t item_size)
{
    return lh_memory_allocator_alloc_zero(lh_runtime_allocator_get_mut(), count, item_size);
}

lh_ptr
lh_runtime_allocator_try_alloc(lh_usize_t size)
{
    return lh_memory_allocator_try_alloc(lh_runtime_allocator_get_mut(), size);
}

lh_ptr
lh_runtime_allocator_try_alloc_zero(lh_usize_t count, lh_usize_t item_size)
{
    return lh_memory_allocator_try_alloc_zero(lh_runtime_allocator_get_mut(), count, item_size);
}

lh_bool_t
lh_runtime_allocator_try_free(lh_ptr ptr)
{
    return lh_memory_allocator_try_dealloc(lh_runtime_allocator_get_mut(), ptr);
}

lh_ptr
lh_runtime_allocator_try_realloc(lh_ptr old_ptr, lh_usize_t old_size, lh_usize_t new_size)
{
    return lh_memory_allocator_try_realloc(lh_runtime_allocator_get_mut(), old_ptr, old_size,
                                           new_size);
}
