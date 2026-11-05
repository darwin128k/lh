#include <lh/core/atomic/ptr.h>
#include <lh/core/assert/runtime.h>
#include <lh/core/assert/static.h>
#include <lh/core/cast/const.h>
#include <lh/core/atomic/option.h>
#include <lh/core/runtime/critical.h>

#if (LH_LIBRARY_OPTION_ATOMIC == LH_ATOMIC_CRITICAL)
/* Plain code inside the slot's critical section: no atomic instruction is needed. */
#elif (LH_COMPILER_TYPE_IS_GCC_LIKE)
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
#if (LH_LIBRARY_OPTION_ATOMIC == LH_ATOMIC_CRITICAL)
    const lh_usize_t state = lh_runtime_critical_enter();
    lh_ptr value = lh_ptr_deref(lh_cast_const(lh_ptr volatile *, lh_addr_of(self->value)));
    lh_runtime_critical_leave(state);
    return value;
#elif (LH_COMPILER_TYPE_IS_GCC_LIKE)
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
#if (LH_LIBRARY_OPTION_ATOMIC == LH_ATOMIC_CRITICAL)
    const lh_usize_t state = lh_runtime_critical_enter();
    lh_ptr_deref(lh_cast_static(lh_ptr volatile *, lh_addr_of(self->value))) = value;
    lh_runtime_critical_leave(state);
#elif (LH_COMPILER_TYPE_IS_GCC_LIKE)
    __atomic_store_n(lh_addr_of(self->value), value, __ATOMIC_RELEASE);
#else
    /* An exchange is a full barrier, which includes release. */
    _InterlockedExchangePointer(lh_addr_of(self->value), value);
#endif
}

lh_void
lh_atomic_ptr_set_relaxed(lh_atomic_ptr_t *self, lh_ptr value)
{
    lh_assert_runtime_ref(self);
#if (LH_LIBRARY_OPTION_ATOMIC == LH_ATOMIC_CRITICAL)
    const lh_usize_t state = lh_runtime_critical_enter();
    lh_ptr_deref(lh_cast_static(lh_ptr volatile *, lh_addr_of(self->value))) = value;
    lh_runtime_critical_leave(state);
#elif (LH_COMPILER_TYPE_IS_GCC_LIKE)
    __atomic_store_n(lh_addr_of(self->value), value, __ATOMIC_RELAXED);
#else
    /* An aligned pointer store is atomic on every MSVC target. */
    lh_ptr_deref(lh_cast_static(lh_ptr volatile *, lh_addr_of(self->value))) = value;
#endif
}

lh_ptr
lh_atomic_ptr_exchange_acq_rel(lh_atomic_ptr_t *self, lh_ptr value)
{
    lh_assert_runtime_ref(self);
#if (LH_LIBRARY_OPTION_ATOMIC == LH_ATOMIC_CRITICAL)
    const lh_usize_t state = lh_runtime_critical_enter();
    lh_ptr old = lh_ptr_deref(lh_cast_static(lh_ptr volatile *, lh_addr_of(self->value)));
    lh_ptr_deref(lh_cast_static(lh_ptr volatile *, lh_addr_of(self->value))) = value;
    lh_runtime_critical_leave(state);
    return old;
#elif (LH_COMPILER_TYPE_IS_GCC_LIKE)
    return __atomic_exchange_n(lh_addr_of(self->value), value, __ATOMIC_ACQ_REL);
#else
    /* Every _Interlocked read-modify-write is a full barrier. */
    return _InterlockedExchangePointer(lh_addr_of(self->value), value);
#endif
}
