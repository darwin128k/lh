#include <lh/memory/allocator.h>
#include <lh/memory.h>
#include <lh/optional/ref.h>
#include <lh/runtime/assert.h>
#include <lh/util/return.h>
#include <lh/util/addr.h>

lh_void
lh_memory_allocator_pack(lh_memory_allocator_t *self, lh_memory_allocator_alloc_cb *alloc_cb,
                         lh_memory_allocator_dealloc_cb *dealloc_cb)
{
    lh_runtime_assert_ref(self);
    lh_optional_ref(alloc_cb)
    {
        self->alloc_cb = lh_ptr_deref(alloc_cb);
    }

    lh_optional_ref(dealloc_cb)
    {
        self->dealloc_cb = lh_ptr_deref(dealloc_cb);
    }
}

lh_void
lh_memory_allocator_unpack(const lh_memory_allocator_t *self,
                           lh_memory_allocator_alloc_cb *alloc_cb,
                           lh_memory_allocator_dealloc_cb *dealloc_cb)
{
    lh_runtime_assert_ref(self);
    lh_optional_ref(alloc_cb)
    {
        lh_ptr_deref(alloc_cb) = self->alloc_cb;
    }

    lh_optional_ref(dealloc_cb)
    {
        lh_ptr_deref(dealloc_cb) = self->dealloc_cb;
    }
}

lh_void
lh_memory_allocator_set(lh_memory_allocator_t *self, lh_memory_allocator_alloc_cb alloc_cb,
                        lh_memory_allocator_dealloc_cb dealloc_cb)
{
    lh_memory_allocator_pack(self, lh_addr_of(alloc_cb), lh_addr_of(dealloc_cb));
}

lh_void
lh_memory_allocator_init(lh_memory_allocator_t *self, lh_memory_allocator_alloc_cb alloc_cb,
                         lh_memory_allocator_dealloc_cb dealloc_cb)
{
    lh_memory_allocator_set(self, alloc_cb, dealloc_cb);
}

lh_void
lh_memory_allocator_deinit(lh_memory_allocator_t *self)
{
    lh_memory_allocator_set(self, lh_null, lh_null);
}

lh_memory_allocator_alloc_cb
lh_memory_allocator_get_alloc_cb(lh_memory_allocator_t *self)
{
    lh_memory_allocator_alloc_cb alloc_cb;
    lh_memory_allocator_unpack(self, lh_addr_of(alloc_cb), lh_null);
    return alloc_cb;
}

lh_memory_allocator_dealloc_cb
lh_memory_allocator_get_dealloc_cb(lh_memory_allocator_t *self)
{
    lh_memory_allocator_dealloc_cb dealloc_cb;
    lh_memory_allocator_unpack(self, lh_null, lh_addr_of(dealloc_cb));
    return dealloc_cb;
}

lh_ptr
lh_memory_allocator_alloc(lh_memory_allocator_t *self, lh_usize_t size)
{
    lh_memory_allocator_alloc_cb alloc_cb = lh_memory_allocator_get_alloc_cb(self);
    lh_runtime_assert_ifn(alloc_cb, lh_runtime_error_code_allocator_function_not_initialized);

    lh_ptr ptr = alloc_cb(size);
    lh_runtime_assert_ifn(ptr, lh_runtime_error_code_memory_not_allocated);

#if (LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_INIT_ALLOCATED == LH_LIBRARY_OPTION_ON)
    lh_memory_set(ptr, size, 0);
#endif

    return ptr;
}

lh_void
lh_memory_allocator_dealloc(lh_memory_allocator_t *self, lh_ptr ptr)
{
    lh_return_ifn(ptr);

    lh_memory_allocator_dealloc_cb dealloc_cb = lh_memory_allocator_get_dealloc_cb(self);
    lh_runtime_assert_ifn(dealloc_cb, lh_runtime_error_code_deallocator_function_not_initialized);

    dealloc_cb(ptr);
}

lh_ptr
lh_memory_allocator_realloc(lh_memory_allocator_t *self, lh_ptr old_ptr, lh_usize_t old_size,
                            lh_usize_t new_size)
{
    lh_return_if(old_size == new_size, old_ptr);
    lh_return_ifn(old_ptr, lh_memory_allocator_alloc(self, new_size));

    if (new_size == 0)
    {
        lh_memory_allocator_dealloc(self, old_ptr);
        return lh_null;
    }

    lh_ptr new_ptr = lh_memory_allocator_alloc(self, new_size);
    lh_memory_copy(new_ptr, new_size, old_ptr, old_size);
    lh_memory_allocator_dealloc(self, old_ptr);

    return new_ptr;
}
