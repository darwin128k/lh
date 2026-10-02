#include <lh/runtime/allocator.h>
#include <lh/library/fallback.h>
#include <lh/attribute/thread_local.h>
#include <lh/memory/allocator/initializer.h>
#include <lh/null.h>

#if (LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB == LH_LIBRARY_OPTION_ON)
#    include LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_INCLUDE

/* The configured C functions (malloc/free/realloc, or e.g. pvPortMalloc/
   vPortFree) take no context, so they are wrapped rather than cast to the
   callback types: calling a function through a pointer of another type is
   undefined behavior. */
static lh_ptr
lh_runtime_allocator_default_alloc(lh_ptr context, lh_usize_t size)
{
    (void)context;
    return LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_ALLOC(size);
}

static lh_void
lh_runtime_allocator_default_dealloc(lh_ptr context, lh_ptr ptr)
{
    (void)context;
    LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_DEALLOC(ptr);
}

#    if LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_HAS_REALLOC
static lh_ptr
lh_runtime_allocator_default_realloc(lh_ptr context, lh_ptr ptr, lh_usize_t old_size, lh_usize_t new_size)
{
    (void)context;
    (void)old_size;
    return LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_REALLOC(ptr, new_size);
}
#        define LH_RUNTIME_ALLOCATOR_DEFAULT_REALLOC lh_runtime_allocator_default_realloc
#    else
#        define LH_RUNTIME_ALLOCATOR_DEFAULT_REALLOC lh_null
#    endif

LH_ATTRIBUTE_THREAD_LOCAL
lh_memory_allocator_t m_runtime_allocator =
    lh_memory_allocator_initializer_with_realloc(lh_runtime_allocator_default_alloc,
                                                 lh_runtime_allocator_default_dealloc,
                                                 LH_RUNTIME_ALLOCATOR_DEFAULT_REALLOC);
#else
LH_ATTRIBUTE_THREAD_LOCAL
lh_memory_allocator_t m_runtime_allocator = lh_memory_allocator_empty_initializer();
#endif

lh_memory_allocator_t *
lh_runtime_allocator(void)
{
    return lh_addr_of(m_runtime_allocator);
}

lh_ptr
lh_runtime_allocator_alloc(lh_usize_t size)
{
    return lh_memory_allocator_alloc(lh_runtime_allocator(), size);
}

lh_void
lh_runtime_allocator_free(lh_ptr ptr)
{
    lh_memory_allocator_dealloc(lh_runtime_allocator(), ptr);
}

lh_ptr
lh_runtime_allocator_realloc(lh_ptr old_ptr, lh_usize_t old_size, lh_usize_t new_size)
{
    return lh_memory_allocator_realloc(lh_runtime_allocator(), old_ptr, old_size, new_size);
}
