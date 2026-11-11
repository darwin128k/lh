/**
 * @file registry.c
 * @brief ::lh_str_template_registry_t: named values with owners.
 */

#include <lh/core/str/template/registry.h>
#include <lh/core/assert/runtime.h>
#include <lh/core/math.h>
#include <lh/core/str/template.h>

const lh_array_t *
lh_str_template_registry_get_entries(const lh_str_template_registry_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entries);
}

lh_array_t *
lh_str_template_registry_get_entries_mut(lh_str_template_registry_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entries);
}

lh_void
lh_str_template_registry_init(lh_str_template_registry_t *self)
{
    lh_array_init(lh_str_template_registry_get_entries_mut(self),
                  sizeof(lh_str_template_registry_entry_t));
}

lh_usize_t
lh_str_template_registry_get_count(const lh_str_template_registry_t *self)
{
    return lh_array_get_size(lh_str_template_registry_get_entries(self));
}

lh_str_template_registry_entry_t *
lh_str_template_registry_get_entry_mut(lh_str_template_registry_t *self, lh_usize_t index)
{
    return lh_array_get_ptr(lh_str_template_registry_get_entries_mut(self), index);
}

const lh_str_template_registry_entry_t *
lh_str_template_registry_get_entry(const lh_str_template_registry_t *self, lh_usize_t index)
{
    return lh_array_get_ptr(lh_str_template_registry_get_entries(self), index);
}

lh_void
lh_str_template_registry_erase(lh_str_template_registry_t *self, lh_usize_t index)
{
    lh_str_template_registry_entry_deinit(lh_str_template_registry_get_entry_mut(self, index));
    lh_array_erase(lh_str_template_registry_get_entries_mut(self), index, lh_null);
}

lh_usize_t
lh_str_template_registry_find(const lh_str_template_registry_t *self, const lh_str_view_t *name)
{
    for (lh_usize_t i = 0U; lh_math_lt(i, lh_str_template_registry_get_count(self)); ++i)
    {
        lh_str_view_t entry_name;

        lh_str_as_view(
            lh_str_template_registry_entry_get_name(lh_str_template_registry_get_entry(self, i)),
            lh_addr_of(entry_name));
        if (lh_str_view_equals(lh_addr_of(entry_name), name, lh_bool_false))
        {
            return i;
        }
    }
    return LH_STR_VIEW_INVALID;
}

lh_void
lh_str_template_registry_deinit(lh_str_template_registry_t *self)
{
    while (!lh_math_is_zero(lh_str_template_registry_get_count(self)))
    {
        lh_str_template_registry_erase(self,
                                       lh_math_sub_one(lh_str_template_registry_get_count(self)));
    }
    lh_array_deinit(lh_str_template_registry_get_entries_mut(self));
}

lh_bool_t
lh_str_template_registry_try_get_value(const lh_str_template_registry_t *self,
                                       const lh_str_view_t *name, lh_str_view_t *value)
{
    const lh_usize_t index = lh_str_template_registry_find(self, name);

    if (lh_math_eq(index, LH_STR_VIEW_INVALID))
    {
        return lh_bool_false;
    }
    lh_str_as_view(
        lh_str_template_registry_entry_get_value(lh_str_template_registry_get_entry(self, index)),
        value);
    return lh_bool_true;
}

lh_bool_t
lh_str_template_registry_try_add(lh_str_template_registry_t *self, lh_ptr owner,
                                 const lh_str_view_t *name, const lh_str_view_t *value)
{
    lh_str_template_registry_entry_t entry;

    if (!lh_str_template_is_name(name) ||
        lh_math_ne(lh_str_template_registry_find(self, name), LH_STR_VIEW_INVALID))
    {
        return lh_bool_false;
    }
    lh_str_template_registry_entry_init(lh_addr_of(entry), owner, lh_ptr_deref(name),
                                        lh_ptr_deref(value));
    lh_array_push_back(lh_str_template_registry_get_entries_mut(self), lh_addr_of(entry));
    return lh_bool_true;
}

lh_bool_t
lh_str_template_registry_try_remove(lh_str_template_registry_t *self, lh_ptr owner,
                                    const lh_str_view_t *name)
{
    const lh_usize_t index = lh_str_template_registry_find(self, name);

    if (lh_math_eq(index, LH_STR_VIEW_INVALID) ||
        lh_math_ne(lh_str_template_registry_entry_get_owner(
                       lh_str_template_registry_get_entry(self, index)),
                   owner))
    {
        return lh_bool_false;
    }
    lh_str_template_registry_erase(self, index);
    return lh_bool_true;
}

lh_void
lh_str_template_registry_remove_owner(lh_str_template_registry_t *self, lh_ptr owner)
{
    lh_usize_t i = 0U;

    while (lh_math_lt(i, lh_str_template_registry_get_count(self)))
    {
        if (lh_math_eq(lh_str_template_registry_entry_get_owner(
                           lh_str_template_registry_get_entry(self, i)),
                       owner))
        {
            lh_str_template_registry_erase(self, i);
        }
        else
        {
            ++i;
        }
    }
}

lh_bool_t
lh_str_template_registry_lookup(lh_ptr user, const lh_str_view_t *name, lh_str_view_t *value)
{
    return lh_str_template_registry_try_get_value(
        lh_cast_static(const lh_str_template_registry_t *, user), name, value);
}
