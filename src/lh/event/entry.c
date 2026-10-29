#include <lh/event/entry.h>
#include <lh/runtime/allocator.h>
#include <lh/assert.h>
#include <lh/size/limits.h>
#include <lh/memory/std.h>

lh_char_t *
lh_event_entry_get_name(const lh_event_entry_t *self)
{
    return self->name;
}

lh_void
lh_event_entry_set_name(lh_event_entry_t *self, lh_char_t * value)
{
    self->name = value;
}

lh_usize_t
lh_event_entry_get_length(const lh_event_entry_t *self)
{
    return self->length;
}

lh_void
lh_event_entry_set_length(lh_event_entry_t *self, lh_usize_t value)
{
    self->length = value;
}

const lh_list_t *
lh_event_entry_get_subscriptions(const lh_event_entry_t *self)
{
    return lh_addr_of(self->subscriptions);
}

lh_list_t *
lh_event_entry_get_subscriptions_mut(lh_event_entry_t *self)
{
    return lh_addr_of(self->subscriptions);
}


lh_void
lh_event_entry_init(lh_event_entry_t *self, lh_char_t *name, lh_usize_t length)
{
    lh_event_entry_set_name(self, name);
    lh_event_entry_set_length(self, length);
    lh_list_init(lh_event_entry_get_subscriptions_mut(self));
}

lh_void
lh_event_entry_destroy(lh_event_entry_t *self)
{
    lh_runtime_allocator_free(lh_event_entry_get_name(self));
    lh_runtime_allocator_free(self);
}


lh_event_entry_t *
lh_event_entry_try_create(const lh_char_t *name, lh_usize_t length)
{
    lh_char_t *copy;
    lh_event_entry_t *entry;
    if (lh_ptr_is_null(name) || lh_math_eq(length, 0u) || lh_math_eq(length, LH_USIZE_T_MAX))
    {
        return lh_null;
    }
    copy = lh_cast_static(lh_char_t *, lh_runtime_allocator_try_alloc(lh_math_add_one(length)));
    if (lh_ptr_is_null(copy))
    {
        return lh_null;
    }
    entry = lh_cast_static(lh_event_entry_t *, lh_runtime_allocator_try_alloc(sizeof *entry));
    if (lh_ptr_is_null(entry))
    {
        lh_runtime_allocator_free(copy);
        return lh_null;
    }
    lh_memory_std_copy(copy, name, lh_math_add_one(length));
    lh_event_entry_init(entry, copy, length);
    return entry;
}

lh_event_entry_t *
lh_event_entry_create(const lh_char_t *name, lh_usize_t length)
{
    lh_event_entry_t *entry = lh_event_entry_try_create(name, length);
    lh_runtime_check_if(lh_ptr_is_null(entry), lh_runtime_error_code_memory_not_allocated);
    return entry;
}
