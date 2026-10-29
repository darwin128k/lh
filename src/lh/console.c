/**
 * @file console.c
 * @brief The command registry ::lh_console_t.
 */

#include <lh/console.h>

#include <lh/cast/const.h>
#include <lh/util/return.h>
#include <lh/char/letter.h>
#include <lh/char/digit.h>

const lh_list_t *
lh_console_get_entries(const lh_console_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entries);
}

lh_list_t *
lh_console_get_entries_mut(lh_console_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entries);
}

lh_usize_t
lh_console_get_depth(const lh_console_t *self)
{
    lh_assert_runtime_ref(self);
    return self->depth;
}

lh_void
lh_console_set_depth(lh_console_t *self, lh_usize_t depth)
{
    lh_assert_runtime_ref(self);
    self->depth = depth;
}

/* The characters of a word besides letters and digits. */
static const lh_char_t m_console_word_marks[] = {lh_char_map_underscore, lh_char_map_minus,
                                                 lh_char_map_period};

lh_bool_t
lh_console_is_word(const lh_char_t *text)
{
    lh_return_if(lh_ptr_is_null(text) || lh_math_eq(text[0], lh_char_map_nul), lh_bool_false);
    for (; lh_math_ne(lh_ptr_deref(text), lh_char_map_nul); ++text)
    {
        lh_return_ifn(lh_char_is_letter(lh_ptr_deref(text)) ||
                          lh_char_is_digit(lh_ptr_deref(text)) ||
                          lh_str_ptr_contains_char(m_console_word_marks,
                                                   lh_array_ptr_get_size(m_console_word_marks),
                                                   lh_ptr_deref(text)),
                      lh_bool_false);
    }
    return lh_bool_true;
}

lh_void
lh_console_init(lh_console_t *self)
{
    lh_list_init(lh_console_get_entries_mut(self));
    lh_console_set_depth(self, 0U);
}

const lh_console_entry_t *
lh_console_get_first(const lh_console_t *self)
{
    const lh_list_t *list = lh_console_get_entries(self);

    for (lh_list_node_t *node = lh_list_get_first(list); lh_ptr_is_set(node);
         node = lh_list_get_next(list, node))
    {
        const lh_console_entry_t *entry = lh_list_entry(lh_console_entry_t, node, node);

        lh_return_ifn(lh_console_entry_is_dead(entry), entry);
    }
    return lh_null;
}

const lh_console_entry_t *
lh_console_get_next(const lh_console_t *self, const lh_console_entry_t *entry)
{
    const lh_list_t *list = lh_console_get_entries(self);

    for (lh_list_node_t *node = lh_list_get_next(list, lh_console_entry_get_node(entry));
         lh_ptr_is_set(node); node = lh_list_get_next(list, node))
    {
        const lh_console_entry_t *next = lh_list_entry(lh_console_entry_t, node, node);

        lh_return_ifn(lh_console_entry_is_dead(next), next);
    }
    return lh_null;
}

lh_void
lh_console_collect(lh_console_t *self)
{
    lh_list_t *list = lh_console_get_entries_mut(self);
    lh_list_node_t *node;

    lh_return_if(lh_math_ne(lh_console_get_depth(self), 0U));
    node = lh_list_get_first(list);
    while (lh_ptr_is_set(node))
    {
        lh_list_node_t *next = lh_list_get_next(list, node);
        lh_console_entry_t *entry = lh_list_entry(lh_console_entry_t, node, node);

        if (lh_console_entry_is_dead(entry))
        {
            lh_list_node_unlink(node);
            lh_console_entry_destroy(entry);
        }
        node = next;
    }
}

const lh_console_entry_t *
lh_console_find(const lh_console_t *self, const lh_char_t *scope, const lh_char_t *name)
{
    lh_return_if(lh_ptr_is_null(scope) || lh_ptr_is_null(name), lh_null);
    for (const lh_console_entry_t *entry = lh_console_get_first(self); lh_ptr_is_set(entry);
         entry = lh_console_get_next(self, entry))
    {
        lh_return_if(lh_console_entry_is_named(entry, scope, name), entry);
    }
    return lh_null;
}

lh_bool_t
lh_console_has_scope(const lh_console_t *self, const lh_char_t *scope)
{
    lh_return_if(lh_ptr_is_null(scope), lh_bool_false);
    for (const lh_console_entry_t *entry = lh_console_get_first(self); lh_ptr_is_set(entry);
         entry = lh_console_get_next(self, entry))
    {
        lh_return_if(lh_console_entry_is_named(entry, scope, lh_null), lh_bool_true);
    }
    return lh_bool_false;
}

lh_usize_t
lh_console_get_count(const lh_console_t *self)
{
    lh_usize_t count = 0U;

    for (const lh_console_entry_t *entry = lh_console_get_first(self); lh_ptr_is_set(entry);
         entry = lh_console_get_next(self, entry))
    {
        count = lh_math_add_one(count);
    }
    return count;
}

lh_void
lh_console_clear(lh_console_t *self)
{
    for (const lh_console_entry_t *entry = lh_console_get_first(self); lh_ptr_is_set(entry);
         entry = lh_console_get_next(self, entry))
    {
        lh_console_entry_set_dead(lh_cast_const(lh_console_entry_t *, entry), lh_bool_true);
    }
    lh_console_collect(self);
}

lh_void
lh_console_drop_owner(lh_console_t *self, lh_ptr owner)
{
    for (const lh_console_entry_t *entry = lh_console_get_first(self); lh_ptr_is_set(entry);
         entry = lh_console_get_next(self, entry))
    {
        if (lh_math_eq(lh_console_entry_get_owner(entry), owner))
        {
            lh_console_entry_set_dead(lh_cast_const(lh_console_entry_t *, entry), lh_bool_true);
        }
    }
    lh_console_collect(self);
}

lh_bool_t
lh_console_add(lh_console_t *self, lh_ptr owner, const lh_char_t *scope,
               const lh_console_command_t *command)
{
    const lh_console_entry_t *found;
    lh_console_entry_t *entry;

    lh_return_if(lh_ptr_is_null(command) || lh_ptr_is_null(lh_console_command_get_fn(command)) ||
                     !lh_console_is_word(scope) ||
                     !lh_console_is_word(lh_console_command_get_name(command)),
                 lh_bool_false);
    found = lh_console_find(self, scope, lh_console_command_get_name(command));
    lh_return_if(lh_ptr_is_set(found), lh_console_entry_is_same(found, owner, command));
    entry = lh_console_entry_try_create(owner, scope, command);
    lh_return_if(lh_ptr_is_null(entry), lh_bool_false);
    lh_list_push_back(lh_console_get_entries_mut(self), lh_console_entry_get_node_mut(entry));
    return lh_bool_true;
}

lh_bool_t
lh_console_remove(lh_console_t *self, lh_ptr owner, const lh_char_t *scope, const lh_char_t *name)
{
    lh_console_entry_t *entry =
        lh_cast_const(lh_console_entry_t *, lh_console_find(self, scope, name));

    lh_return_if(lh_ptr_is_null(entry) || lh_math_ne(lh_console_entry_get_owner(entry), owner),
                 lh_bool_false);
    lh_console_entry_set_dead(entry, lh_bool_true);
    lh_console_collect(self);
    return lh_bool_true;
}

lh_console_result_t
lh_console_run(lh_console_t *self, const lh_char_t *scope, const lh_char_t *name, lh_usize_t argc,
               const lh_char_t *const *argv, lh_console_sink_cb sink, lh_ptr out,
               const lh_console_entry_t **ran)
{
    const lh_console_entry_t *entry = lh_console_find(self, scope, name);
    lh_console_call_t call;
    lh_bool_t accepted;

    if (lh_ptr_is_set(ran))
    {
        lh_ptr_deref(ran) = entry;
    }
    lh_return_if(lh_ptr_is_null(entry), lh_console_result_not_found);
    lh_console_call_init(lh_addr_of(call), lh_str_get_data(lh_console_entry_get_scope(entry)),
                         lh_str_get_data(lh_console_entry_get_name(entry)), argc, argv, sink, out);
    lh_console_set_depth(self, lh_math_add_one(lh_console_get_depth(self)));
    accepted = lh_console_entry_get_fn(entry)(lh_addr_of(call), lh_console_entry_get_user(entry));
    lh_console_set_depth(self, lh_math_sub_one(lh_console_get_depth(self)));
    /* The handler may have removed itself: the entry stays readable until this
       collect, which the caller's use of `ran` must come before. */
    lh_console_collect(self);
    return accepted ? lh_console_result_done : lh_console_result_bad_usage;
}

lh_void
lh_console_deinit(lh_console_t *self)
{
    lh_runtime_assert_if(lh_math_ne(lh_console_get_depth(self), 0U),
                         lh_runtime_error_code_invalid_argument);
    lh_console_clear(self);
}
