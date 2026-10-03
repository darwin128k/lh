#include <lh/entity.h>
#include <lh/assert.h>
#include <lh/cast/const.h>
#include <lh/entity/handler.h>
#include <lh/memory.h>
#include <lh/memory/tree.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/util/addr.h>
#include <lh/util/bit.h>
#include <lh/util/math.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

const lh_entity_class_t lh_entity_base_class =
    lh_entity_class_initializer(lh_null, sizeof(lh_entity_t), lh_null, lh_null, lh_null);

/* lh's own bit (the top one, above lh_entity_flags_user's range): DELETE was
 * already delivered by lh_entity_delete, while the entity was in its tree. */
#define lh_entity_flags_delete_sent lh_bit_mask(31)

/* Deliver @p event to @p self: its classes, derived first, then its handlers. */
static lh_void
lh_entity_deliver(lh_entity_t *self, lh_entity_event_t *event)
{
    event->current = self;

    for (const lh_entity_class_t *entity_class = self->entity_class;
         lh_ptr_is_set(entity_class) && !event->stopped; entity_class = entity_class->base)
    {
        if (lh_ptr_is_set(entity_class->event))
        {
            entity_class->event(self, event);
        }
    }

    lh_list_node_t *node = lh_list_get_first(lh_addr_of(self->handlers));
    while (lh_ptr_is_set(node) && !event->stopped)
    {
        /* Taken before the call: the handler may remove itself. */
        lh_list_node_t *const next = lh_list_get_next(lh_addr_of(self->handlers), node);
        const lh_entity_handler_t *const handler = lh_list_entry(lh_entity_handler_t, node, node);
        handler->handler(event, handler->user_data);
        node = next;
    }
}

/* The tree destructor of every entity block: runs before its children go. */
static lh_void
lh_entity_destruct(lh_ptr ptr)
{
    lh_entity_t *const self = lh_ptr_rcast(lh_entity_t, ptr);

    if (!lh_entity_has_flags(self, lh_entity_flags_delete_sent))
    {
        lh_entity_notify(self, LH_ENTITY_EVENT_DELETE, lh_null);
    }

    for (const lh_entity_class_t *entity_class = self->entity_class; lh_ptr_is_set(entity_class);
         entity_class = entity_class->base)
    {
        if (lh_ptr_is_set(entity_class->destructor))
        {
            entity_class->destructor(self);
        }
    }

    lh_list_node_unlink(lh_addr_of(self->sibling));
}

/* Constructors of @p entity_class and its bases, base first. */
static lh_void
lh_entity_construct(lh_entity_t *self, const lh_entity_class_t *entity_class)
{
    lh_return_ifn(entity_class);
    lh_entity_construct(self, entity_class->base);
    if (lh_ptr_is_set(entity_class->constructor))
    {
        entity_class->constructor(self);
    }
}

/* Set up the freshly allocated block @p self: shared by create and create_root. */
static lh_entity_t *
lh_entity_init(lh_entity_t *self, const lh_entity_class_t *entity_class, lh_entity_t *parent)
{
    lh_memory_set(self, entity_class->size, 0);
    self->entity_class = entity_class;
    lh_list_node_init(lh_addr_of(self->sibling));
    lh_list_init(lh_addr_of(self->children));
    lh_list_init(lh_addr_of(self->handlers));
    if (lh_ptr_is_set(parent))
    {
        lh_list_push_back(lh_addr_of(parent->children), lh_addr_of(self->sibling));
    }
    lh_memory_tree_set_destructor(self, lh_entity_destruct);
    lh_entity_construct(self, entity_class);
    return self;
}

lh_entity_t *
lh_entity_create_root(const lh_entity_class_t *entity_class, lh_memory_sized_allocator_t *allocator)
{
    lh_assert_runtime_ref(entity_class);
    lh_assert_runtime_if(entity_class->size < sizeof(lh_entity_t),
                         lh_runtime_error_code_invalid_argument);
    return lh_entity_init(
        lh_ptr_rcast(lh_entity_t, lh_memory_tree_alloc(allocator, entity_class->size)),
        entity_class, lh_null);
}

lh_entity_t *
lh_entity_create(const lh_entity_class_t *entity_class, lh_entity_t *parent)
{
    lh_assert_runtime_ref(entity_class);
    lh_assert_runtime_ref(parent);
    lh_assert_runtime_if(entity_class->size < sizeof(lh_entity_t),
                         lh_runtime_error_code_invalid_argument);
    return lh_entity_init(
        lh_ptr_rcast(lh_entity_t, lh_memory_tree_alloc_child(parent, entity_class->size)),
        entity_class, parent);
}

lh_void
lh_entity_delete(lh_entity_t *self)
{
    lh_return_ifn(self);
    /* Sent here, not by the tree's destructor: by then @p self is already cut
     * from its parent, and receivers (a screen invalidating the area the
     * entity covered) need to see where it was. */
    lh_entity_notify(self, LH_ENTITY_EVENT_DELETE, lh_null);
    lh_entity_add_flags(self, lh_entity_flags_delete_sent);
    lh_memory_tree_free(self);
}

const lh_entity_class_t *
lh_entity_get_class(const lh_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return self->entity_class;
}

lh_bool_t
lh_entity_is_instance_of(const lh_entity_t *self, const lh_entity_class_t *entity_class)
{
    for (const lh_entity_class_t *c = lh_entity_get_class(self); lh_ptr_is_set(c); c = c->base)
    {
        if (c == entity_class)
        {
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_entity_t *
lh_entity_get_parent(const lh_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ptr_rcast(lh_entity_t, lh_memory_tree_get_parent(lh_cast_const(lh_entity_t *, self)));
}

lh_void
lh_entity_set_parent(lh_entity_t *self, lh_entity_t *parent)
{
    lh_memory_tree_set_parent(self, parent); /* refuses a cycle */
    lh_list_node_unlink(lh_addr_of(self->sibling));
    if (lh_ptr_is_set(parent))
    {
        lh_list_push_back(lh_addr_of(parent->children), lh_addr_of(self->sibling));
    }
}

lh_entity_t *
lh_entity_get_first_child(const lh_entity_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_list_entry(lh_entity_t, sibling, lh_list_get_first(lh_addr_of(self->children)));
}

lh_entity_t *
lh_entity_get_next_sibling(const lh_entity_t *self)
{
    const lh_entity_t *const parent = lh_entity_get_parent(self);
    lh_return_ifn(parent, lh_null);
    return lh_list_entry(lh_entity_t, sibling,
                         lh_list_get_next(lh_addr_of(parent->children), lh_addr_of(self->sibling)));
}

lh_void
lh_entity_add_flags(lh_entity_t *self, lh_entity_flags_t flags)
{
    lh_assert_runtime_ref(self);
    lh_math_bit_set(self->flags, flags);
}

lh_void
lh_entity_clear_flags(lh_entity_t *self, lh_entity_flags_t flags)
{
    lh_assert_runtime_ref(self);
    self->flags = lh_math_bit_and(self->flags, lh_math_bit_not(flags));
}

lh_bool_t
lh_entity_has_flags(const lh_entity_t *self, lh_entity_flags_t flags)
{
    lh_assert_runtime_ref(self);
    return lh_math_eq(lh_math_bit_and(self->flags, flags), flags);
}

lh_void
lh_entity_add_handler(lh_entity_t *self, lh_entity_handler_cb handler, lh_ptr user_data)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(handler);

    lh_entity_handler_t *const record = lh_ptr_rcast(
        lh_entity_handler_t, lh_memory_tree_alloc_child(self, sizeof(lh_entity_handler_t)));
    record->handler = handler;
    record->user_data = user_data;
    lh_list_node_init(lh_addr_of(record->node));
    lh_list_push_back(lh_addr_of(self->handlers), lh_addr_of(record->node));
}

lh_bool_t
lh_entity_remove_handler(lh_entity_t *self, lh_entity_handler_cb handler, lh_ptr user_data)
{
    lh_assert_runtime_ref(self);

    for (lh_list_node_t *node = lh_list_get_first(lh_addr_of(self->handlers)); lh_ptr_is_set(node);
         node = lh_list_get_next(lh_addr_of(self->handlers), node))
    {
        lh_entity_handler_t *const record = lh_list_entry(lh_entity_handler_t, node, node);
        if (record->handler == handler && record->user_data == user_data)
        {
            lh_list_node_unlink(node);
            lh_memory_tree_free(record);
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_bool_t
lh_entity_send_event(lh_entity_t *self, lh_uint_t code, lh_ptr param)
{
    lh_assert_runtime_ref(self);

    lh_entity_event_t event;
    event.code = code;
    event.target = self;
    event.param = param;
    event.stopped = lh_bool_false;

    for (lh_entity_t *receiver = self; lh_ptr_is_set(receiver);
         receiver = lh_entity_get_parent(receiver))
    {
        lh_entity_deliver(receiver, lh_addr_of(event));
        if (event.stopped || !lh_entity_has_flags(receiver, lh_entity_flags_event_bubble))
        {
            break;
        }
    }
    return event.stopped;
}

lh_bool_t
lh_entity_notify(lh_entity_t *self, lh_uint_t code, lh_ptr param)
{
    lh_assert_runtime_ref(self);

    lh_entity_event_t event;
    event.code = code;
    event.target = self;
    event.param = param;
    event.stopped = lh_bool_false;
    lh_entity_deliver(self, lh_addr_of(event));
    return event.stopped;
}
