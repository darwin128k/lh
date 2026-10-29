#include <lh/event/bus.h>
#include <lh/runtime/allocator.h>
#include <lh/assert.h>
#include <lh/util/str/ptr.h>
#include <lh/memory/std.h>

const lh_array_t *
lh_event_bus_get_entries(const lh_event_bus_t *self)
{
    return lh_addr_of(self->entries);
}

lh_array_t *
lh_event_bus_get_entries_mut(lh_event_bus_t *self)
{
    return lh_addr_of(self->entries);
}

const lh_list_t *
lh_event_bus_get_retired(const lh_event_bus_t *self)
{
    return lh_addr_of(self->retired);
}

lh_list_t *
lh_event_bus_get_retired_mut(lh_event_bus_t *self)
{
    return lh_addr_of(self->retired);
}

lh_usize_t
lh_event_bus_get_depth(const lh_event_bus_t *self)
{
    return self->depth;
}

lh_void
lh_event_bus_set_depth(lh_event_bus_t *self, lh_usize_t value)
{
    self->depth = value;
}

lh_usize_t
lh_event_bus_get_count(const lh_event_bus_t *self)
{
    return self->count;
}

lh_void
lh_event_bus_set_count(lh_event_bus_t *self, lh_usize_t value)
{
    self->count = value;
}

lh_void
lh_event_bus_init(lh_event_bus_t *self)
{
    lh_array_init(lh_event_bus_get_entries_mut(self), sizeof(lh_event_entry_t *));
    lh_list_init(lh_event_bus_get_retired_mut(self));
    lh_event_bus_set_depth(self, 0u);
    lh_event_bus_set_count(self, 0u);
}

lh_event_entry_t *
lh_event_bus_get_entry(const lh_event_bus_t *self, lh_event_id_t id)
{
    const lh_array_t *entries = lh_event_bus_get_entries(self);
    if (lh_math_eq(id, 0u) || lh_math_gt(id, lh_array_get_size(entries)))
    {
        return lh_null;
    }
    return lh_ptr_deref(
        lh_cast_static(lh_event_entry_t **, lh_array_get_ptr(entries, lh_math_sub_one(id))));
}

lh_void
lh_event_bus_collect(lh_event_bus_t *self)
{
    lh_list_node_t *node;
    if (lh_math_ne(lh_event_bus_get_depth(self), 0u))
    {
        return;
    }
    while (lh_ptr_is_set(node = lh_list_pop_front(lh_event_bus_get_retired_mut(self))))
    {
        lh_event_subscription_destroy(lh_event_subscription_get_by_retired(node));
    }
}

lh_event_id_t
lh_event_bus_find_id(const lh_event_bus_t *self, const lh_char_t *name)
{
    lh_usize_t length;
    lh_usize_t count = lh_array_get_size(lh_event_bus_get_entries(self));
    if (lh_ptr_is_null(name))
    {
        return 0u;
    }
    length = lh_str_ptr_len(name);
    for (lh_event_id_t id = 1u; lh_math_le(id, count); id = lh_math_add_one(id))
    {
        lh_event_entry_t *entry = lh_event_bus_get_entry(self, id);
        if (lh_math_eq(length, lh_event_entry_get_length(entry)) &&
            lh_math_eq(lh_memory_std_compare(name, lh_event_entry_get_name(entry), length), 0))
        {
            return id;
        }
    }
    return 0u;
}

lh_event_subscription_t *
lh_event_bus_get_first(const lh_event_bus_t *self, lh_event_id_t id)
{
    lh_event_entry_t *entry = lh_event_bus_get_entry(self, id);
    const lh_list_t *list;
    if (lh_ptr_is_null(entry))
    {
        return lh_null;
    }
    list = lh_event_entry_get_subscriptions(entry);
    for (lh_list_node_t *node = lh_list_get_first(list); lh_ptr_is_set(node);
         node = lh_list_get_next(list, node))
    {
        lh_event_subscription_t *sub = lh_event_subscription_get_by_node(node);
        if (!lh_event_subscription_get_dead(sub))
        {
            return sub;
        }
    }
    return lh_null;
}

lh_event_subscription_t *
lh_event_bus_get_next(const lh_event_bus_t *self, lh_event_id_t id,
                      const lh_event_subscription_t *sub)
{
    const lh_list_t *list = lh_event_entry_get_subscriptions(lh_event_bus_get_entry(self, id));
    for (lh_list_node_t *node = lh_list_get_next(list, lh_event_subscription_get_node(sub));
         lh_ptr_is_set(node); node = lh_list_get_next(list, node))
    {
        lh_event_subscription_t *next = lh_event_subscription_get_by_node(node);
        if (!lh_event_subscription_get_dead(next))
        {
            return next;
        }
    }
    return lh_null;
}

lh_event_subscription_t *
lh_event_bus_try_subscribe(lh_event_bus_t *self, lh_event_id_t id, lh_ptr owner,
                           lh_event_cb callback, lh_ptr context, lh_event_dispose_cb dispose)
{
    lh_event_entry_t *entry = lh_event_bus_get_entry(self, id);
    lh_event_subscription_t *sub;
    if (lh_ptr_is_null(entry) || lh_ptr_is_null(callback))
    {
        return lh_null;
    }
    sub = lh_cast_static(lh_event_subscription_t *,
                         lh_runtime_allocator_try_alloc(sizeof lh_ptr_deref(sub)));
    if (lh_ptr_is_null(sub))
    {
        return lh_null;
    }
    lh_event_subscription_init(sub, owner, callback, context, dispose);
    lh_list_push_back(lh_event_entry_get_subscriptions_mut(entry),
                      lh_event_subscription_get_node_mut(sub));
    lh_event_bus_set_count(self, lh_math_add_one(lh_event_bus_get_count(self)));
    return sub;
}

lh_void
lh_event_bus_remove(lh_event_bus_t *self, lh_event_subscription_t *sub)
{
    if (lh_event_subscription_get_dead(sub))
    {
        return;
    }
    lh_event_subscription_set_dead(sub, lh_bool_true);
    lh_event_bus_set_count(self, lh_math_sub_one(lh_event_bus_get_count(self)));
    lh_list_push_back(lh_event_bus_get_retired_mut(self),
                      lh_event_subscription_get_retired_mut(sub));
    lh_event_bus_collect(self);
}

lh_event_id_t
lh_event_bus_try_get_id(lh_event_bus_t *self, const lh_char_t *name)
{
    lh_event_id_t id = lh_event_bus_find_id(self, name);
    lh_usize_t length;
    lh_event_entry_t *entry;
    lh_array_t *entries = lh_event_bus_get_entries_mut(self);
    lh_usize_t count = lh_array_get_size(entries);
    if (lh_math_ne(id, 0u) || lh_ptr_is_null(name))
    {
        return id;
    }
    length = lh_str_ptr_len(name);
    if (lh_math_eq(length, 0u) || lh_math_eq(length, LH_USIZE_T_MAX) ||
        lh_math_eq(count, LH_USIZE_T_MAX))
    {
        return 0u;
    }
    entry = lh_event_entry_try_create(name, length);
    if (lh_ptr_is_null(entry))
    {
        return 0u;
    }
    if (!lh_array_try_reserve(entries, lh_array_get_grown_capacity(lh_array_get_capacity(entries),
                                                                   lh_math_add_one(count))))
    {
        lh_event_entry_destroy(entry);
        return 0u;
    }
    lh_array_push_back(entries, lh_addr_of(entry));
    return lh_math_add_one(count);
}

lh_event_subscription_t *
lh_event_bus_subscribe(lh_event_bus_t *self, lh_event_id_t id, lh_ptr owner, lh_event_cb callback,
                       lh_ptr context, lh_event_dispose_cb dispose)
{
    lh_event_subscription_t *sub =
        lh_event_bus_try_subscribe(self, id, owner, callback, context, dispose);
    lh_runtime_check_if(lh_ptr_is_null(sub), lh_runtime_error_code_memory_not_allocated);
    return sub;
}

lh_event_result_t
lh_event_bus_publish(lh_event_bus_t *self, lh_event_id_t id, lh_ptr payload)
{
    lh_event_entry_t *entry = lh_event_bus_get_entry(self, id);
    lh_event_result_t best = lh_event_result_continue;
    if (lh_ptr_is_null(entry))
    {
        return best;
    }
    lh_event_bus_set_depth(self, lh_math_add_one(lh_event_bus_get_depth(self)));
    for (lh_event_subscription_t *sub = lh_event_bus_get_first(self, id); lh_ptr_is_set(sub);
         sub = lh_event_bus_get_next(self, id, sub))
    {
        lh_event_result_t result = lh_event_subscription_get_callback(sub)(
            lh_event_entry_get_name(entry), payload, lh_event_subscription_get_context(sub));
        best = lh_math_max(best, result);
        if (lh_math_eq(result, lh_event_result_stop))
        {
            break;
        }
    }
    lh_event_bus_set_depth(self, lh_math_sub_one(lh_event_bus_get_depth(self)));
    lh_event_bus_collect(self);
    return best;
}

lh_void
lh_event_bus_drop_owner(lh_event_bus_t *self, lh_ptr owner)
{
    lh_usize_t count = lh_array_get_size(lh_event_bus_get_entries(self));
    for (lh_event_id_t id = 1u; lh_math_le(id, count); id = lh_math_add_one(id))
    {
        lh_event_subscription_t *sub = lh_event_bus_get_first(self, id);
        while (lh_ptr_is_set(sub))
        {
            lh_event_subscription_t *next = lh_event_bus_get_next(self, id, sub);
            if (lh_math_eq(lh_event_subscription_get_owner(sub), owner))
            {
                lh_event_bus_remove(self, sub);
            }
            sub = next;
        }
    }
}

lh_void
lh_event_bus_clear(lh_event_bus_t *self)
{
    lh_usize_t count = lh_array_get_size(lh_event_bus_get_entries(self));
    for (lh_event_id_t id = 1u; lh_math_le(id, count); id = lh_math_add_one(id))
    {
        lh_event_subscription_t *sub = lh_event_bus_get_first(self, id);
        while (lh_ptr_is_set(sub))
        {
            lh_event_subscription_t *next = lh_event_bus_get_next(self, id, sub);
            lh_event_bus_remove(self, sub);
            sub = next;
        }
    }
}

lh_event_subscription_t *
lh_event_bus_find_subscription(const lh_event_bus_t *self, lh_event_id_t id, lh_ptr owner,
                               lh_event_match_cb match, lh_ptr key)
{
    for (lh_event_subscription_t *sub = lh_event_bus_get_first(self, id); lh_ptr_is_set(sub);
         sub = lh_event_bus_get_next(self, id, sub))
    {
        if (lh_math_eq(lh_event_subscription_get_owner(sub), owner) &&
            (lh_ptr_is_null(match) || match(lh_event_subscription_get_context(sub), key)))
        {
            return sub;
        }
    }
    return lh_null;
}

lh_event_id_t
lh_event_bus_get_id(lh_event_bus_t *self, const lh_char_t *name)
{
    lh_event_id_t id = lh_event_bus_try_get_id(self, name);
    lh_runtime_check_if(lh_math_eq(id, 0u), lh_runtime_error_code_memory_not_allocated);
    return id;
}

lh_event_id_t
lh_event_bus_try_get_cached_id(lh_event_bus_t *self, lh_event_id_t *cache, const lh_char_t *name)
{
    if (lh_math_eq(lh_ptr_deref(cache), 0u))
    {
        lh_ptr_deref(cache) = lh_event_bus_try_get_id(self, name);
    }
    return lh_ptr_deref(cache);
}

lh_void
lh_event_bus_deinit(lh_event_bus_t *self)
{
    lh_usize_t count = lh_array_get_size(lh_event_bus_get_entries(self));
    lh_runtime_check_if(lh_math_ne(lh_event_bus_get_depth(self), 0u),
                        lh_runtime_error_code_invalid_argument);
    lh_event_bus_clear(self);
    for (lh_event_id_t id = 1u; lh_math_le(id, count); id = lh_math_add_one(id))
    {
        lh_event_entry_destroy(lh_event_bus_get_entry(self, id));
    }
    lh_array_deinit(lh_event_bus_get_entries_mut(self));
}

lh_event_id_t
lh_event_bus_get_cached_id(lh_event_bus_t *self, lh_event_id_t *cache, const lh_char_t *name)
{
    lh_event_id_t id = lh_event_bus_try_get_cached_id(self, cache, name);
    lh_runtime_check_if(lh_math_eq(id, 0u), lh_runtime_error_code_memory_not_allocated);
    return id;
}
