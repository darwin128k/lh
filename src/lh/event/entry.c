#include <lh/event/entry.h>
#include <lh/runtime/allocator.h>
#include <lh/assert.h>

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
