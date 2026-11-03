#include <lh/runtime/allocator.h>
#include <lh/assert/runtime.h>
#include <lh/atomic/ptr.h>
#include <lh/atomic/ptr/initializer.h>
#include <lh/attribute/unused.h>
#include <lh/attribute/thread_local.h>
#include <lh/cast/const.h>
#include <lh/memory/allocator/initializer.h>

#if (LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB == LH_LIBRARY_OPTION_ON)
#    include LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_INCLUDE

/* The configured C functions (malloc/free/realloc, or e.g. pvPortMalloc/
   vPortFree) take no context, so they are wrapped rather than cast to the
   callback types: calling a function through a pointer of another type is
   undefined behavior. */
lh_ptr
lh_runtime_allocator_default_alloc(lh_self_ptr self, lh_usize_t size)
{
    LH_ATTRIBUTE_UNUSED(self);
    return LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_ALLOC(size);
}

lh_void
lh_runtime_allocator_default_dealloc(lh_self_ptr self, lh_ptr ptr)
{
    LH_ATTRIBUTE_UNUSED(self);
    LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_DEALLOC(ptr);
}

#    if LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_HAS_REALLOC
lh_ptr
lh_runtime_allocator_default_realloc(lh_self_ptr self, lh_ptr ptr, lh_usize_t old_size,
                                     lh_usize_t new_size)
{
    LH_ATTRIBUTE_UNUSED(self);
    LH_ATTRIBUTE_UNUSED(old_size);
    return LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_REALLOC(ptr, new_size);
}

#        define LH_RUNTIME_ALLOCATOR_DEFAULT_REALLOC lh_runtime_allocator_default_realloc
#    else
#        define LH_RUNTIME_ALLOCATOR_DEFAULT_REALLOC lh_null
#    endif

const lh_memory_allocator_t m_runtime_allocator_initial =
    lh_memory_allocator_initializer_with_realloc(lh_runtime_allocator_default_alloc,
                                                 lh_runtime_allocator_default_dealloc,
                                                 LH_RUNTIME_ALLOCATOR_DEFAULT_REALLOC);
#else
const lh_memory_allocator_t m_runtime_allocator_initial = lh_memory_allocator_empty_initializer();
#endif

/* The process slot is the address of an allocator nobody changes, published
   with release and read with acquire: a thread copying it gets one whole
   object, the old or the new, never callbacks of two different heaps. */
lh_atomic_ptr_t m_runtime_allocator_process =
    lh_atomic_ptr_initializer(lh_cast_const(lh_ptr, lh_addr_of(m_runtime_allocator_initial)));

/* Each thread's slot starts as a copy of the process slot, taken on the
   thread's first use: a thread LH did not start (the host's own, a library's
   pool) sees what the host installed for the process, not an empty slot. */
LH_ATTRIBUTE_THREAD_LOCAL
lh_memory_allocator_t m_runtime_allocator = lh_memory_allocator_empty_initializer();

LH_ATTRIBUTE_THREAD_LOCAL
lh_bool_t m_runtime_allocator_is_seeded = lh_bool_false;

const lh_memory_allocator_t *
lh_runtime_allocator_process(void)
{
    return lh_cast_static(const lh_memory_allocator_t *,
                          lh_atomic_ptr_get_acquire(lh_addr_of(m_runtime_allocator_process)));
}

lh_void
lh_runtime_allocator_set_process(const lh_memory_allocator_t *allocator)
{
    lh_assert_runtime_ref(allocator);
    lh_atomic_ptr_set_release(lh_addr_of(m_runtime_allocator_process),
                              lh_cast_const(lh_ptr, lh_cast_static(const lh_void *, allocator)));
}

lh_memory_allocator_t *
lh_runtime_allocator(void)
{
    if (!m_runtime_allocator_is_seeded)
    {
        lh_memory_allocator_assign(lh_addr_of(m_runtime_allocator), lh_runtime_allocator_process());
        m_runtime_allocator_is_seeded = lh_bool_true;
    }
    return lh_addr_of(m_runtime_allocator);
}

lh_void
lh_runtime_allocator_init(lh_memory_allocator_alloc_cb alloc_cb,
                          lh_memory_allocator_dealloc_cb dealloc_cb)
{
    lh_memory_allocator_init(lh_runtime_allocator(), alloc_cb, dealloc_cb);
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

lh_ptr
lh_runtime_allocator_alloc_zero(lh_usize_t count, lh_usize_t item_size)
{
    return lh_memory_allocator_alloc_zero(lh_runtime_allocator(), count, item_size);
}

lh_ptr
lh_runtime_allocator_try_alloc(lh_usize_t size)
{
    return lh_memory_allocator_try_alloc(lh_runtime_allocator(), size);
}

lh_ptr
lh_runtime_allocator_try_alloc_zero(lh_usize_t count, lh_usize_t item_size)
{
    return lh_memory_allocator_try_alloc_zero(lh_runtime_allocator(), count, item_size);
}

lh_bool_t
lh_runtime_allocator_try_free(lh_ptr ptr)
{
    return lh_memory_allocator_try_dealloc(lh_runtime_allocator(), ptr);
}

lh_ptr
lh_runtime_allocator_try_realloc(lh_ptr old_ptr, lh_usize_t old_size, lh_usize_t new_size)
{
    return lh_memory_allocator_try_realloc(lh_runtime_allocator(), old_ptr, old_size, new_size);
}
