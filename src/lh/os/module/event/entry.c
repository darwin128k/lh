/**
 * @file entry.c
 * @brief Accessors and lifetime of ::lh_os_module_event_entry_t.
 */

#include <lh/os/module/event/entry.h>
#include <lh/core/runtime/allocator.h>
#include <lh/core/assert.h>

lh_char_t *
lh_os_module_event_entry_get_name(const lh_os_module_event_entry_t *self)
{
    return self->name;
}

lh_void
lh_os_module_event_entry_set_name(lh_os_module_event_entry_t *self, lh_char_t *value)
{
    self->name = value;
}

lh_usize_t
lh_os_module_event_entry_get_length(const lh_os_module_event_entry_t *self)
{
    return self->length;
}

lh_void
lh_os_module_event_entry_set_length(lh_os_module_event_entry_t *self, lh_usize_t value)
{
    self->length = value;
}

const lh_list_t *
lh_os_module_event_entry_get_subscriptions(const lh_os_module_event_entry_t *self)
{
    return lh_addr_of(self->subscriptions);
}

lh_list_t *
lh_os_module_event_entry_get_subscriptions_mut(lh_os_module_event_entry_t *self)
{
    return lh_addr_of(self->subscriptions);
}

lh_void
lh_os_module_event_entry_init(lh_os_module_event_entry_t *self, lh_char_t *name, lh_usize_t length)
{
    lh_os_module_event_entry_set_name(self, name);
    lh_os_module_event_entry_set_length(self, length);
    lh_list_init(lh_os_module_event_entry_get_subscriptions_mut(self));
}

lh_void
lh_os_module_event_entry_destroy(lh_os_module_event_entry_t *self)
{
    lh_runtime_allocator_free(lh_os_module_event_entry_get_name(self));
    lh_runtime_allocator_free(self);
}

lh_os_module_event_entry_t *
lh_os_module_event_entry_try_create(lh_str_view_t name)
{
    const lh_usize_t length = lh_str_view_get_size(lh_addr_of(name));
    if (lh_math_eq(length, 0u))
    {
        return lh_null;
    }
    /* A valid view ends inside the address space, so its size + 1 does not overflow. */
    const lh_usize_t capacity = lh_math_add_one(length);
    lh_char_t *copy = lh_cast_static(lh_char_t *, lh_runtime_allocator_try_alloc(capacity));
    if (lh_ptr_is_null(copy))
    {
        return lh_null;
    }
    lh_os_module_event_entry_t *entry = lh_cast_static(
        lh_os_module_event_entry_t *, lh_runtime_allocator_try_alloc(sizeof lh_ptr_deref(entry)));
    if (lh_ptr_is_null(entry))
    {
        lh_runtime_allocator_free(copy);
        return lh_null;
    }
    lh_os_module_event_entry_init(
        entry, copy,
        lh_str_ptr_copy_cstr(copy, capacity, lh_str_view_get_data(lh_addr_of(name)), length));
    return entry;
}

lh_os_module_event_entry_t *
lh_os_module_event_entry_create(lh_str_view_t name)
{
    lh_os_module_event_entry_t *entry = lh_os_module_event_entry_try_create(name);
    lh_runtime_check_if(lh_ptr_is_null(entry), lh_runtime_error_code_memory_not_allocated);
    return entry;
}
