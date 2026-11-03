#include <lh/atomic/fn_ptr.h>
#include <lh/assert/runtime.h>
#include <lh/assert/static.h>
#include <lh/cast/const.h>
#include <lh/compiler/type.h>

#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
lh_assert_static(__atomic_always_lock_free(sizeof(lh_atomic_fn_ptr_cb), 0),
                 "lh_atomic_fn_ptr_t needs a lock-free function pointer");
#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
#    include <intrin.h>
/* The _Interlocked*Pointer intrinsics take a data pointer; MSVC lays code and
   data pointers out alike on every target it supports. */
lh_assert_static(sizeof(lh_atomic_fn_ptr_cb) == sizeof(lh_ptr),
                 "lh_atomic_fn_ptr_t needs code and data pointers of one size");
#else
#    error "Compiler has no atomic operations for lh_atomic_fn_ptr_t"
#endif

lh_atomic_fn_ptr_cb
lh_atomic_fn_ptr_get_acquire(const lh_atomic_fn_ptr_t *self)
{
    lh_assert_runtime_ref(self);
#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
    return __atomic_load_n(lh_addr_of(self->value), __ATOMIC_ACQUIRE);
#else
    /* As lh_atomic_ptr_get_acquire: a compare-exchange that never changes the
       value is a full barrier, which includes acquire. */
    return lh_cast_static(
        lh_atomic_fn_ptr_cb,
        _InterlockedCompareExchangePointer(
            lh_cast_const(lh_ptr volatile *, lh_addr_of(self->value)), lh_null, lh_null));
#endif
}

lh_void
lh_atomic_fn_ptr_set_release(lh_atomic_fn_ptr_t *self, lh_atomic_fn_ptr_cb fn)
{
    lh_assert_runtime_ref(self);
#if (LH_COMPILER_TYPE_IS_GCC_LIKE)
    __atomic_store_n(lh_addr_of(self->value), fn, __ATOMIC_RELEASE);
#else
    /* An exchange is a full barrier, which includes release. */
    _InterlockedExchangePointer(lh_cast_static(lh_ptr volatile *, lh_addr_of(self->value)),
                                lh_cast_static(lh_ptr, fn));
#endif
}
