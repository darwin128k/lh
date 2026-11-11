/**
 * @file entry.c
 * @brief ::lh_str_template_registry_entry_t: its fields and its texts.
 */

#include <lh/core/str/template/registry/entry.h>
#include <lh/core/assert/runtime.h>

lh_ptr
lh_str_template_registry_entry_get_owner(const lh_str_template_registry_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return self->owner;
}

lh_void
lh_str_template_registry_entry_set_owner(lh_str_template_registry_entry_t *self, lh_ptr owner)
{
    lh_assert_runtime_ref(self);
    self->owner = owner;
}

const lh_str_t *
lh_str_template_registry_entry_get_name(const lh_str_template_registry_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->name);
}

lh_str_t *
lh_str_template_registry_entry_get_name_mut(lh_str_template_registry_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->name);
}

const lh_str_t *
lh_str_template_registry_entry_get_value(const lh_str_template_registry_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->value);
}

lh_str_t *
lh_str_template_registry_entry_get_value_mut(lh_str_template_registry_entry_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->value);
}

lh_void
lh_str_template_registry_entry_init(lh_str_template_registry_entry_t *self, lh_ptr owner,
                                    lh_str_view_t name, lh_str_view_t value)
{
    lh_str_template_registry_entry_set_owner(self, owner);
    lh_str_init_by_view(lh_str_template_registry_entry_get_name_mut(self), name);
    lh_str_init_by_view(lh_str_template_registry_entry_get_value_mut(self), value);
}

lh_void
lh_str_template_registry_entry_deinit(lh_str_template_registry_entry_t *self)
{
    lh_str_deinit(lh_str_template_registry_entry_get_name_mut(self));
    lh_str_deinit(lh_str_template_registry_entry_get_value_mut(self));
}
