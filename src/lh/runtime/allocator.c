#include <lh/library/fallback.h>
#include <lh/attribute/thread_local.h>
#include <lh/memory/allocator/initializer.h>
#include <lh/null.h>
#include <lh/runtime/allocator.h>

#if (LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB == LH_LIBRARY_OPTION_ON)
#    include <stdlib.h>

LH_ATTRIBUTE_THREAD_LOCAL
lh_memory_allocator_t m_runtime_allocator = lh_memory_allocator_initializer(malloc, free);
#else
LH_ATTRIBUTE_THREAD_LOCAL
lh_memory_allocator_t m_runtime_allocator = lh_memory_allocator_empty_initializer();
#endif

lh_memory_allocator_t *
lh_runtime_allocator(void) {
    return lh_addr_of(m_runtime_allocator);
}

lh_ptr
lh_runtime_allocator_alloc(lh_usize_t size) {
    return lh_memory_allocator_alloc(lh_runtime_allocator(), size);
}

lh_void
lh_runtime_allocator_free(lh_ptr ptr) {
    lh_memory_allocator_dealloc(lh_runtime_allocator(), ptr);
}

lh_ptr
lh_runtime_allocator_realloc(lh_ptr old_ptr, lh_usize_t old_size, lh_usize_t new_size) {
    return lh_memory_allocator_realloc(lh_runtime_allocator(), old_ptr, old_size, new_size);
}