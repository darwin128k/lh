/**
 * @file contacts.c
 * @brief Implementation of `lh/os/input/contacts.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/os/input/contacts.h>
#include <lh/util/return.h>

lh_void
lh_os_input_contacts_init(lh_os_input_contacts_t *self)
{
    lh_os_input_pointer_id_t pointer;

    lh_assert_runtime_ref(self);
    for (pointer = 0U; pointer < LH_OS_INPUT_POINTER_MAX; ++pointer)
    {
        self->contacts[pointer] = 0U;
        self->device_kinds[pointer] = lh_os_input_device_kind_none;
        self->taken[pointer] = lh_bool_false;
    }
}

lh_bool_t
lh_os_input_contacts_find(lh_os_input_contacts_cptr self, lh_os_input_contact_id_t contact,
                          lh_os_input_pointer_id_t *out)
{
    lh_os_input_pointer_id_t pointer;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(out);
    for (pointer = 1U; pointer < LH_OS_INPUT_POINTER_MAX; ++pointer)
    {
        if (self->taken[pointer] && self->contacts[pointer] == contact)
        {
            lh_ptr_deref(out) = pointer;
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_bool_t
lh_os_input_contacts_take(lh_os_input_contacts_t *self, lh_os_input_contact_id_t contact,
                          lh_os_input_device_kind_t device_kind, lh_os_input_pointer_id_t *out)
{
    lh_os_input_pointer_id_t pointer;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(out);
    /* A down the system sends twice for one contact keeps the id it has. */
    lh_return_if(lh_os_input_contacts_find(self, contact, out), lh_bool_true);
    for (pointer = 1U; pointer < LH_OS_INPUT_POINTER_MAX; ++pointer)
    {
        if (!self->taken[pointer])
        {
            self->contacts[pointer] = contact;
            self->device_kinds[pointer] = device_kind;
            self->taken[pointer] = lh_bool_true;
            lh_ptr_deref(out) = pointer;
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_os_input_device_kind_t
lh_os_input_contacts_get_device_kind(lh_os_input_contacts_cptr self,
                                     lh_os_input_pointer_id_t pointer)
{
    lh_assert_runtime_ref(self);
    lh_return_if(pointer >= LH_OS_INPUT_POINTER_MAX || !self->taken[pointer],
                 lh_os_input_device_kind_none);
    return self->device_kinds[pointer];
}

lh_u32_t
lh_os_input_contacts_get_count(lh_os_input_contacts_cptr self)
{
    lh_os_input_pointer_id_t pointer;
    lh_u32_t count = 0U;

    lh_assert_runtime_ref(self);
    for (pointer = 1U; pointer < LH_OS_INPUT_POINTER_MAX; ++pointer)
    {
        count += self->taken[pointer] ? 1U : 0U;
    }
    return count;
}

lh_void
lh_os_input_contacts_release(lh_os_input_contacts_t *self, lh_os_input_pointer_id_t pointer)
{
    lh_assert_runtime_ref(self);
    lh_return_if(pointer == 0U || pointer >= LH_OS_INPUT_POINTER_MAX);
    self->contacts[pointer] = 0U;
    self->device_kinds[pointer] = lh_os_input_device_kind_none;
    self->taken[pointer] = lh_bool_false;
}
