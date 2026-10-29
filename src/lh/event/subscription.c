#include <lh/event/subscription.h>
#include <lh/runtime/allocator.h>
#include <lh/assert.h>

const lh_list_node_t *
lh_event_subscription_get_node(const lh_event_subscription_t *self)
{
    return lh_addr_of(self->node);
}

lh_list_node_t *
lh_event_subscription_get_node_mut(lh_event_subscription_t *self)
{
    return lh_addr_of(self->node);
}

const lh_list_node_t *
lh_event_subscription_get_retired(const lh_event_subscription_t *self)
{
    return lh_addr_of(self->retired);
}

lh_list_node_t *
lh_event_subscription_get_retired_mut(lh_event_subscription_t *self)
{
    return lh_addr_of(self->retired);
}

lh_ptr
lh_event_subscription_get_owner(const lh_event_subscription_t *self)
{
    return self->owner;
}

lh_void
lh_event_subscription_set_owner(lh_event_subscription_t *self, lh_ptr value)
{
    self->owner = value;
}

lh_event_cb
lh_event_subscription_get_callback(const lh_event_subscription_t *self)
{
    return self->callback;
}

lh_void
lh_event_subscription_set_callback(lh_event_subscription_t *self, lh_event_cb value)
{
    self->callback = value;
}

lh_event_dispose_cb
lh_event_subscription_get_dispose(const lh_event_subscription_t *self)
{
    return self->dispose;
}

lh_void
lh_event_subscription_set_dispose(lh_event_subscription_t *self, lh_event_dispose_cb value)
{
    self->dispose = value;
}

lh_ptr
lh_event_subscription_get_context(const lh_event_subscription_t *self)
{
    return self->context;
}

lh_void
lh_event_subscription_set_context(lh_event_subscription_t *self, lh_ptr value)
{
    self->context = value;
}

lh_bool_t
lh_event_subscription_get_dead(const lh_event_subscription_t *self)
{
    return self->dead;
}

lh_void
lh_event_subscription_set_dead(lh_event_subscription_t *self, lh_bool_t value)
{
    self->dead = value;
}


lh_event_subscription_t *
lh_event_subscription_get_by_node(lh_list_node_t *node)
{
    return lh_list_entry(lh_event_subscription_t, node, node);
}

lh_event_subscription_t *
lh_event_subscription_get_by_retired(lh_list_node_t *node)
{
    return lh_list_entry(lh_event_subscription_t, retired, node);
}

lh_void
lh_event_subscription_init(lh_event_subscription_t *self, lh_ptr owner, lh_event_cb callback,
                           lh_ptr context, lh_event_dispose_cb dispose)
{
    lh_list_node_init(lh_event_subscription_get_node_mut(self));
    lh_list_node_init(lh_event_subscription_get_retired_mut(self));
    lh_event_subscription_set_owner(self, owner);
    lh_event_subscription_set_callback(self, callback);
    lh_event_subscription_set_context(self, context);
    lh_event_subscription_set_dispose(self, dispose);
    lh_event_subscription_set_dead(self, lh_bool_false);
}

lh_void
lh_event_subscription_destroy(lh_event_subscription_t *self)
{
    lh_list_node_unlink(lh_event_subscription_get_node_mut(self));
    lh_list_node_unlink(lh_event_subscription_get_retired_mut(self));
    if (lh_ptr_is_set(lh_event_subscription_get_dispose(self)))
    {
        lh_event_subscription_get_dispose(self)(lh_event_subscription_get_context(self));
    }
    lh_runtime_allocator_free(self);
}
