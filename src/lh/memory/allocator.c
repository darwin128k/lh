#include <lh/memory/allocator.h>
#include <lh/memory.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>
#include <lh/assert.h>
#include <lh/config.h>
#include <lh/null.h>

lh_void
lh_memory_allocator_set_alloc_cb(lh_memory_allocator_t *self, lh_memory_allocator_alloc_cb alloc_cb)
{
    lh_assert_runtime_ref(self);
    self->alloc_cb = alloc_cb;
    self->realloc_cb = lh_null; /* would no longer match the heap */
}

lh_void
lh_memory_allocator_set_dealloc_cb(lh_memory_allocator_t *self,
                                   lh_memory_allocator_dealloc_cb dealloc_cb)
{
    lh_assert_runtime_ref(self);
    self->dealloc_cb = dealloc_cb;
    self->realloc_cb = lh_null; /* would no longer match the heap */
}

lh_void
lh_memory_allocator_set_realloc_cb(lh_memory_allocator_t *self, lh_memory_allocator_realloc_cb realloc_cb)
{
    lh_assert_runtime_ref(self);
    self->realloc_cb = realloc_cb;
}

lh_void
lh_memory_allocator_set(lh_memory_allocator_t *self, lh_memory_allocator_alloc_cb alloc_cb,
                        lh_memory_allocator_dealloc_cb dealloc_cb)
{
    lh_memory_allocator_set_alloc_cb(self, alloc_cb);
    lh_memory_allocator_set_dealloc_cb(self, dealloc_cb);
}

lh_void
lh_memory_allocator_assign(lh_memory_allocator_t *self, const lh_memory_allocator_t *other)
{
    lh_memory_allocator_set(self, lh_memory_allocator_get_alloc_cb(other),
                            lh_memory_allocator_get_dealloc_cb(other));
    lh_memory_allocator_set_realloc_cb(self, lh_memory_allocator_get_realloc_cb(other));
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
lh_memory_allocator_get_alloc_cb(const lh_memory_allocator_t *self)
{
    lh_assert_runtime_ref(self);
    return self->alloc_cb;
}

lh_memory_allocator_dealloc_cb
lh_memory_allocator_get_dealloc_cb(const lh_memory_allocator_t *self)
{
    lh_assert_runtime_ref(self);
    return self->dealloc_cb;
}

lh_memory_allocator_realloc_cb
lh_memory_allocator_get_realloc_cb(const lh_memory_allocator_t *self)
{
    lh_assert_runtime_ref(self);
    return self->realloc_cb;
}

lh_ptr
lh_memory_allocator_alloc(lh_memory_allocator_t *self, lh_usize_t size)
{
    lh_memory_allocator_alloc_cb alloc_cb = lh_memory_allocator_get_alloc_cb(self);
    lh_assert_runtime_ifn(alloc_cb, lh_runtime_error_make_by_code(
                                        lh_runtime_error_code_allocator_function_not_initialized));

    lh_ptr ptr = alloc_cb(size);
    lh_assert_runtime_ifn(
        ptr, lh_runtime_error_make_by_code(lh_runtime_error_code_memory_not_allocated));

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
    lh_assert_runtime_ifn(
        dealloc_cb,
        lh_runtime_error_make_by_code(lh_runtime_error_code_deallocator_function_not_initialized));

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

    lh_memory_allocator_realloc_cb realloc_cb = lh_memory_allocator_get_realloc_cb(self);
    if (lh_ptr_is_set(realloc_cb))
    {
        /* Native realloc may extend the block in place — no copy at all. */
        lh_ptr grown = realloc_cb(old_ptr, new_size);
        lh_assert_runtime_ifn(
            grown, lh_runtime_error_make_by_code(lh_runtime_error_code_memory_not_allocated));
#if (LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_INIT_ALLOCATED == LH_LIBRARY_OPTION_ON)
        /* Same promise as lh_memory_allocator_alloc: new bytes read as zero. */
        if (lh_math_gt(new_size, old_size))
        {
            lh_memory_set(lh_ptr_add_by_offset_unsafe(lh_void, grown, old_size), lh_math_sub(new_size, old_size), 0);
        }
#endif
        return grown;
    }

    lh_ptr new_ptr = lh_memory_allocator_alloc(self, new_size);
    lh_memory_copy(new_ptr, new_size, old_ptr, old_size);
    lh_memory_allocator_dealloc(self, old_ptr);

    return new_ptr;
}
