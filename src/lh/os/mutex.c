/**
 * @file mutex.c
 * @brief ::lh_os_mutex_t over the native mutex.
 */

#include <lh/os/mutex.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/core/assert/runtime.h>
#include <lh/core/runtime/allocator.h>

/* What an invalid mutex or lifecycle state reports in the LH error slot. */
static const lh_os_error_t m_os_mutex_invalid = lh_os_error_initializer(
    lh_os_error_code_invalid_argument, lh_os_error_desc_lit("invalid mutex or state"));

const lh_memory_allocator_t *
lh_os_mutex_get_allocator(const lh_os_mutex_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->allocator);
}

lh_memory_allocator_t *
lh_os_mutex_get_allocator_mut(lh_os_mutex_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->allocator);
}

lh_os_system_mutex_handle_t
lh_os_mutex_get_handle(const lh_os_mutex_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

lh_void
lh_os_mutex_set_handle(lh_os_mutex_t *self, lh_os_system_mutex_handle_t handle)
{
    lh_assert_runtime_ref(self);
    self->handle = handle;
}

/**
 * @brief Initialize an empty mutex object.
 */
lh_void
lh_os_mutex_init(lh_os_mutex_t *self)
{
    if (lh_ptr_is_set(self))
    {
        lh_os_mutex_set_handle(self, lh_null);
    }
}

/**
 * @brief Acquire a native mutex for an initialized, empty object.
 */
lh_bool_t
lh_os_mutex_open(lh_os_mutex_t *self)
{
    if (lh_ptr_is_null(self) || lh_ptr_is_set(lh_os_mutex_get_handle(self)))
    {
        return lh_os_fail(m_os_mutex_invalid);
    }
    lh_memory_allocator_t *allocator = lh_os_mutex_get_allocator_mut(self);
    lh_memory_allocator_assign(allocator, lh_runtime_allocator_get());
    lh_ptr storage =
        lh_memory_allocator_try_alloc_zero(allocator, 1U, lh_os_system_mutex_get_size());
    if (lh_ptr_is_null(storage))
    {
        return lh_os_fail_lit(lh_os_error_code_out_of_memory, "no memory for a mutex");
    }
    lh_os_mutex_set_handle(self, lh_os_system_mutex_open(storage));
    if (lh_ptr_is_null(lh_os_mutex_get_handle(self)))
    {
        lh_memory_allocator_dealloc(allocator, storage);
        return lh_bool_false;
    }
    return lh_bool_true;
}

/**
 * @brief Release an unused native mutex.
 */
lh_bool_t
lh_os_mutex_close(lh_os_mutex_t *self)
{
    if (lh_ptr_is_null(self))
    {
        return lh_os_fail(m_os_mutex_invalid);
    }
    if (lh_ptr_is_null(lh_os_mutex_get_handle(self)))
    {
        return lh_bool_true;
    }
    if (!lh_os_system_mutex_close(lh_os_mutex_get_handle(self)))
    {
        return lh_bool_false;
    }
    lh_memory_allocator_dealloc(lh_os_mutex_get_allocator_mut(self), lh_os_mutex_get_handle(self));
    lh_os_mutex_set_handle(self, lh_null);
    return lh_bool_true;
}

/**
 * @brief Acquire exclusive ownership of an open mutex.
 */
lh_bool_t
lh_os_mutex_lock(lh_os_mutex_t *self)
{
    return lh_ptr_is_set(self) && lh_ptr_is_set(lh_os_mutex_get_handle(self))
               ? lh_os_system_mutex_lock(lh_os_mutex_get_handle(self))
               : lh_os_fail(m_os_mutex_invalid);
}

/**
 * @brief Release a mutex held by the calling thread.
 */
lh_bool_t
lh_os_mutex_unlock(lh_os_mutex_t *self)
{
    return lh_ptr_is_set(self) && lh_ptr_is_set(lh_os_mutex_get_handle(self))
               ? lh_os_system_mutex_unlock(lh_os_mutex_get_handle(self))
               : lh_os_fail(m_os_mutex_invalid);
}
