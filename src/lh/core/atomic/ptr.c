#include <lh/core/atomic/ptr.h>
#include <lh/core/assert/runtime.h>
#include <lh/core/assert/static.h>
#include <lh/core/cast/const.h>

#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
lh_assert_static(__atomic_always_lock_free(sizeof(lh_ptr), 0),
                 "lh_atomic_ptr_t needs a lock-free pointer");
#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
#    include <intrin.h>
#else
#    error "Compiler has no atomic operations for lh_atomic_ptr_t"
#endif

lh_ptr
lh_atomic_ptr_get_acquire(const lh_atomic_ptr_t *self)
{
    lh_assert_runtime_ref(self);
#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
    return __atomic_load_n(lh_addr_of(self->value), __ATOMIC_ACQUIRE);
#else
    /* MSVC has no acquire load on every target; a compare-exchange that never
       changes the value is a full barrier, which includes acquire. */
    return _InterlockedCompareExchangePointer(
        lh_cast_const(lh_ptr volatile *, lh_addr_of(self->value)), lh_null, lh_null);
#endif
}

lh_void
lh_atomic_ptr_set_release(lh_atomic_ptr_t *self, lh_ptr value)
{
    lh_assert_runtime_ref(self);
#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
    __atomic_store_n(lh_addr_of(self->value), value, __ATOMIC_RELEASE);
#else
    /* An exchange is a full barrier, which includes release. */
    _InterlockedExchangePointer(lh_addr_of(self->value), value);
#endif
}
