#include <lh/core/atomic/uint.h>
#include <lh/core/assert/runtime.h>
#include <lh/core/assert/static.h>
#include <lh/core/cast/const.h>
#include <lh/core/atomic/option.h>
#include <lh/core/runtime/critical.h>

#if (LH_LIBRARY_OPTION_ATOMIC == LH_ATOMIC_CRITICAL)
/* Plain code inside the slot's critical section: no atomic instruction is needed. */
#elif (LH_COMPILER_TYPE_IS_GCC_LIKE)
lh_assert_static(__atomic_always_lock_free(sizeof(lh_uint_t), 0),
                 "lh_atomic_uint_t needs a lock-free unsigned int");
#elif (LH_COMPILER_TYPE == LH_COMPILER_TYPE_MSVC)
#    include <intrin.h>
/* The _Interlocked* intrinsics work on long, which is 32 bits on every MSVC
   target, as unsigned int is. */
lh_assert_static(sizeof(lh_uint_t) == sizeof(long), "lh_atomic_uint_t needs a 32-bit long");
#else
#    error "Compiler has no atomic operations for lh_atomic_uint_t"
#endif

lh_bool_t
lh_atomic_uint_is_lock_free(void)
{
#if (LH_LIBRARY_OPTION_ATOMIC == LH_ATOMIC_CRITICAL)
    return lh_bool_false;
#else
    return lh_bool_true;
#endif
}

lh_uint_t
lh_atomic_uint_get_relaxed(const lh_atomic_uint_t *self)
{
    lh_assert_runtime_ref(self);
#if (LH_LIBRARY_OPTION_ATOMIC == LH_ATOMIC_CRITICAL)
    const lh_usize_t state = lh_runtime_critical_enter();
    const lh_uint_t value =
        lh_ptr_deref(lh_cast_const(volatile lh_uint_t *, lh_addr_of(self->value)));
    lh_runtime_critical_leave(state);
    return value;
#elif (LH_COMPILER_TYPE_IS_GCC_LIKE)
    return __atomic_load_n(lh_addr_of(self->value), __ATOMIC_RELAXED);
#else
    /* An aligned 32-bit load is atomic on every MSVC target. */
    return lh_ptr_deref(lh_cast_const(volatile lh_uint_t *, lh_addr_of(self->value)));
#endif
}

lh_uint_t
lh_atomic_uint_get_acquire(const lh_atomic_uint_t *self)
{
    lh_assert_runtime_ref(self);
#if (LH_LIBRARY_OPTION_ATOMIC == LH_ATOMIC_CRITICAL)
    const lh_usize_t state = lh_runtime_critical_enter();
    const lh_uint_t value =
        lh_ptr_deref(lh_cast_const(volatile lh_uint_t *, lh_addr_of(self->value)));
    lh_runtime_critical_leave(state);
    return value;
#elif (LH_COMPILER_TYPE_IS_GCC_LIKE)
    return __atomic_load_n(lh_addr_of(self->value), __ATOMIC_ACQUIRE);
#else
    /* A compare-exchange that never changes the value is a full barrier,
       which includes acquire. */
    return lh_cast_static(
        lh_uint_t,
        _InterlockedCompareExchange(lh_cast_const(volatile long *, lh_addr_of(self->value)), 0, 0));
#endif
}

lh_void
lh_atomic_uint_set_relaxed(lh_atomic_uint_t *self, lh_uint_t value)
{
    lh_assert_runtime_ref(self);
#if (LH_LIBRARY_OPTION_ATOMIC == LH_ATOMIC_CRITICAL)
    const lh_usize_t state = lh_runtime_critical_enter();
    lh_ptr_deref(lh_cast_static(volatile lh_uint_t *, lh_addr_of(self->value))) = value;
    lh_runtime_critical_leave(state);
#elif (LH_COMPILER_TYPE_IS_GCC_LIKE)
    __atomic_store_n(lh_addr_of(self->value), value, __ATOMIC_RELAXED);
#else
    lh_ptr_deref(lh_cast_static(volatile lh_uint_t *, lh_addr_of(self->value))) = value;
#endif
}

lh_void
lh_atomic_uint_set_release(lh_atomic_uint_t *self, lh_uint_t value)
{
    lh_assert_runtime_ref(self);
#if (LH_LIBRARY_OPTION_ATOMIC == LH_ATOMIC_CRITICAL)
    const lh_usize_t state = lh_runtime_critical_enter();
    lh_ptr_deref(lh_cast_static(volatile lh_uint_t *, lh_addr_of(self->value))) = value;
    lh_runtime_critical_leave(state);
#elif (LH_COMPILER_TYPE_IS_GCC_LIKE)
    __atomic_store_n(lh_addr_of(self->value), value, __ATOMIC_RELEASE);
#else
    /* An exchange is a full barrier, which includes release. */
    _InterlockedExchange(lh_cast_static(volatile long *, lh_addr_of(self->value)),
                         lh_cast_static(long, value));
#endif
}

lh_uint_t
lh_atomic_uint_fetch_add_relaxed(lh_atomic_uint_t *self, lh_uint_t amount)
{
    lh_assert_runtime_ref(self);
#if (LH_LIBRARY_OPTION_ATOMIC == LH_ATOMIC_CRITICAL)
    const lh_usize_t state = lh_runtime_critical_enter();
    const lh_uint_t old =
        lh_ptr_deref(lh_cast_static(volatile lh_uint_t *, lh_addr_of(self->value)));
    lh_ptr_deref(lh_cast_static(volatile lh_uint_t *, lh_addr_of(self->value))) =
        lh_math_add(old, amount);
    lh_runtime_critical_leave(state);
    return old;
#elif (LH_COMPILER_TYPE_IS_GCC_LIKE)
    return __atomic_fetch_add(lh_addr_of(self->value), amount, __ATOMIC_RELAXED);
#else
    return lh_cast_static(
        lh_uint_t, _InterlockedExchangeAdd(lh_cast_static(volatile long *, lh_addr_of(self->value)),
                                           lh_cast_static(long, amount)));
#endif
}

lh_uint_t
lh_atomic_uint_fetch_add_acq_rel(lh_atomic_uint_t *self, lh_uint_t amount)
{
    lh_assert_runtime_ref(self);
#if (LH_LIBRARY_OPTION_ATOMIC == LH_ATOMIC_CRITICAL)
    const lh_usize_t state = lh_runtime_critical_enter();
    const lh_uint_t old =
        lh_ptr_deref(lh_cast_static(volatile lh_uint_t *, lh_addr_of(self->value)));
    lh_ptr_deref(lh_cast_static(volatile lh_uint_t *, lh_addr_of(self->value))) =
        lh_math_add(old, amount);
    lh_runtime_critical_leave(state);
    return old;
#elif (LH_COMPILER_TYPE_IS_GCC_LIKE)
    return __atomic_fetch_add(lh_addr_of(self->value), amount, __ATOMIC_ACQ_REL);
#else
    /* Every _Interlocked read-modify-write is a full barrier. */
    return lh_cast_static(
        lh_uint_t, _InterlockedExchangeAdd(lh_cast_static(volatile long *, lh_addr_of(self->value)),
                                           lh_cast_static(long, amount)));
#endif
}

lh_uint_t
lh_atomic_uint_exchange_acq_rel(lh_atomic_uint_t *self, lh_uint_t value)
{
    lh_assert_runtime_ref(self);
#if (LH_LIBRARY_OPTION_ATOMIC == LH_ATOMIC_CRITICAL)
    const lh_usize_t state = lh_runtime_critical_enter();
    const lh_uint_t old =
        lh_ptr_deref(lh_cast_static(volatile lh_uint_t *, lh_addr_of(self->value)));
    lh_ptr_deref(lh_cast_static(volatile lh_uint_t *, lh_addr_of(self->value))) = value;
    lh_runtime_critical_leave(state);
    return old;
#elif (LH_COMPILER_TYPE_IS_GCC_LIKE)
    return __atomic_exchange_n(lh_addr_of(self->value), value, __ATOMIC_ACQ_REL);
#else
    return lh_cast_static(
        lh_uint_t, _InterlockedExchange(lh_cast_static(volatile long *, lh_addr_of(self->value)),
                                        lh_cast_static(long, value)));
#endif
}

lh_uint_t
lh_atomic_uint_fetch_sub_acq_rel(lh_atomic_uint_t *self, lh_uint_t amount)
{
    lh_assert_runtime_ref(self);
#if (LH_LIBRARY_OPTION_ATOMIC == LH_ATOMIC_CRITICAL)
    const lh_usize_t state = lh_runtime_critical_enter();
    const lh_uint_t old =
        lh_ptr_deref(lh_cast_static(volatile lh_uint_t *, lh_addr_of(self->value)));
    lh_ptr_deref(lh_cast_static(volatile lh_uint_t *, lh_addr_of(self->value))) =
        lh_math_sub(old, amount);
    lh_runtime_critical_leave(state);
    return old;
#elif (LH_COMPILER_TYPE_IS_GCC_LIKE)
    return __atomic_fetch_sub(lh_addr_of(self->value), amount, __ATOMIC_ACQ_REL);
#else
    /* Adding the two's complement subtracts, modulo 2^32. */
    return lh_cast_static(
        lh_uint_t, _InterlockedExchangeAdd(lh_cast_static(volatile long *, lh_addr_of(self->value)),
                                           lh_cast_static(long, lh_math_sub(0U, amount))));
#endif
}

lh_bool_t
lh_atomic_uint_try_compare_exchange_acq_rel(lh_atomic_uint_t *self, lh_uint_t expected,
                                            lh_uint_t desired)
{
    lh_assert_runtime_ref(self);
#if (LH_LIBRARY_OPTION_ATOMIC == LH_ATOMIC_CRITICAL)
    const lh_usize_t state = lh_runtime_critical_enter();
    const lh_bool_t same = lh_math_eq(
        lh_ptr_deref(lh_cast_static(volatile lh_uint_t *, lh_addr_of(self->value))), expected);
    if (same)
    {
        lh_ptr_deref(lh_cast_static(volatile lh_uint_t *, lh_addr_of(self->value))) = desired;
    }
    lh_runtime_critical_leave(state);
    return same;
#elif (LH_COMPILER_TYPE_IS_GCC_LIKE)
    return __atomic_compare_exchange_n(lh_addr_of(self->value), lh_addr_of(expected), desired,
                                       lh_bool_false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
#else
    return lh_math_eq(
        lh_cast_static(lh_uint_t,
                       _InterlockedCompareExchange(
                           lh_cast_static(volatile long *, lh_addr_of(self->value)),
                           lh_cast_static(long, desired), lh_cast_static(long, expected))),
        expected);
#endif
}
