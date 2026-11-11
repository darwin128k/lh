/**
 * @file condition.c
 * @brief ::lh_os_condition_t over the native condition variable.
 */

#include <lh/os/condition.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/core/assert/runtime.h>
#include <lh/core/runtime/allocator.h>

/* What an invalid condition variable or lifecycle state reports in the LH error slot. */
static const lh_os_error_t m_os_condition_invalid = lh_os_error_initializer(
    lh_os_error_code_invalid_argument, lh_os_error_desc_lit("invalid condition variable or state"));

const lh_memory_allocator_t *
lh_os_condition_get_allocator(const lh_os_condition_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->allocator);
}

lh_memory_allocator_t *
lh_os_condition_get_allocator_mut(lh_os_condition_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->allocator);
}

lh_os_system_condition_handle_t
lh_os_condition_get_handle(const lh_os_condition_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

lh_void
lh_os_condition_set_handle(lh_os_condition_t *self, lh_os_system_condition_handle_t handle)
{
    lh_assert_runtime_ref(self);
    self->handle = handle;
}

/**
 * @brief Initialize an empty condition-variable object.
 */
lh_void
lh_os_condition_init(lh_os_condition_t *self)
{
    if (lh_ptr_is_set(self))
    {
        lh_os_condition_set_handle(self, lh_null);
    }
}

/**
 * @brief Acquire a native condition variable.
 */
lh_bool_t
lh_os_condition_open(lh_os_condition_t *self)
{
    if (lh_ptr_is_null(self) || lh_ptr_is_set(lh_os_condition_get_handle(self)))
    {
        return lh_os_fail(m_os_condition_invalid);
    }
    lh_memory_allocator_t *allocator = lh_os_condition_get_allocator_mut(self);
    lh_memory_allocator_assign(allocator, lh_runtime_allocator_get());
    lh_ptr storage =
        lh_memory_allocator_try_alloc_zero(allocator, 1U, lh_os_system_condition_get_size());
    if (lh_ptr_is_null(storage))
    {
        return lh_os_fail_lit(lh_os_error_code_out_of_memory, "no memory for a condition variable");
    }
    lh_os_condition_set_handle(self, lh_os_system_condition_open(storage));
    if (lh_ptr_is_null(lh_os_condition_get_handle(self)))
    {
        lh_memory_allocator_dealloc(allocator, storage);
        return lh_bool_false;
    }
    return lh_bool_true;
}

/**
 * @brief Release an unused condition variable.
 */
lh_bool_t
lh_os_condition_close(lh_os_condition_t *self)
{
    if (lh_ptr_is_null(self))
    {
        return lh_os_fail(m_os_condition_invalid);
    }
    if (lh_ptr_is_null(lh_os_condition_get_handle(self)))
    {
        return lh_bool_true;
    }
    if (!lh_os_system_condition_close(lh_os_condition_get_handle(self)))
    {
        return lh_bool_false;
    }
    lh_memory_allocator_dealloc(lh_os_condition_get_allocator_mut(self),
                                lh_os_condition_get_handle(self));
    lh_os_condition_set_handle(self, lh_null);
    return lh_bool_true;
}

/**
 * @brief Atomically release the mutex, wait and reacquire it.
 */
lh_bool_t
lh_os_condition_wait(lh_os_condition_t *self, lh_os_mutex_t *mutex)
{
    return lh_ptr_is_set(self) && lh_ptr_is_set(lh_os_condition_get_handle(self)) &&
                   lh_ptr_is_set(mutex) && lh_ptr_is_set(lh_os_mutex_get_handle(mutex))
               ? lh_os_system_condition_wait(lh_os_condition_get_handle(self),
                                             lh_os_mutex_get_handle(mutex))
               : lh_os_fail(m_os_condition_invalid);
}

/**
 * @brief Release the mutex, wait at most @p timeout_ms and reacquire it.
 */
lh_bool_t
lh_os_condition_wait_for(lh_os_condition_t *self, lh_os_mutex_t *mutex, lh_u32_t timeout_ms)
{
    return lh_ptr_is_set(self) && lh_ptr_is_set(lh_os_condition_get_handle(self)) &&
                   lh_ptr_is_set(mutex) && lh_ptr_is_set(lh_os_mutex_get_handle(mutex))
               ? lh_os_system_condition_wait_for(lh_os_condition_get_handle(self),
                                                 lh_os_mutex_get_handle(mutex), timeout_ms)
               : lh_os_fail(m_os_condition_invalid);
}

/**
 * @brief Wake one current waiter, if any.
 */
lh_bool_t
lh_os_condition_signal(lh_os_condition_t *self)
{
    return lh_ptr_is_set(self) && lh_ptr_is_set(lh_os_condition_get_handle(self))
               ? lh_os_system_condition_signal(lh_os_condition_get_handle(self))
               : lh_os_fail(m_os_condition_invalid);
}

/**
 * @brief Wake all current waiters.
 */
lh_bool_t
lh_os_condition_broadcast(lh_os_condition_t *self)
{
    return lh_ptr_is_set(self) && lh_ptr_is_set(lh_os_condition_get_handle(self))
               ? lh_os_system_condition_broadcast(lh_os_condition_get_handle(self))
               : lh_os_fail(m_os_condition_invalid);
}
