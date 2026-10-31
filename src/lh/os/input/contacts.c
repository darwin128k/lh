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
    for (lh_os_input_pointer_id_t pointer = 0U; pointer < LH_OS_INPUT_POINTER_MAX; ++pointer)
    {
        lh_os_input_contacts_set_contact(self, pointer, 0U);
        lh_os_input_contacts_set_device_kind(self, pointer, lh_os_input_device_kind_none);
        lh_os_input_contacts_set_taken(self, pointer, lh_bool_false);
    }
}

lh_bool_t
lh_os_input_contacts_find(lh_os_input_contacts_cptr self, lh_os_input_contact_id_t contact,
                          lh_os_input_pointer_id_t *out)
{
    lh_os_input_pointer_id_t pointer;

    lh_assert_runtime_ref(out);
    for (pointer = 1U; pointer < LH_OS_INPUT_POINTER_MAX; ++pointer)
    {
        if (lh_os_input_contacts_is_taken(self, pointer) &&
            lh_os_input_contacts_get_contact(self, pointer) == contact)
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

    lh_assert_runtime_ref(out);
    /* A down the system sends twice for one contact keeps the id it has. */
    lh_return_if(lh_os_input_contacts_find(self, contact, out), lh_bool_true);
    for (pointer = 1U; pointer < LH_OS_INPUT_POINTER_MAX; ++pointer)
    {
        if (!lh_os_input_contacts_is_taken(self, pointer))
        {
            lh_os_input_contacts_set_contact(self, pointer, contact);
            lh_os_input_contacts_set_device_kind(self, pointer, device_kind);
            lh_os_input_contacts_set_taken(self, pointer, lh_bool_true);
            lh_ptr_deref(out) = pointer;
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_os_input_device_kind_t
lh_os_input_contacts_get_device_kind_v(lh_os_input_contacts_cptr self,
                                       lh_os_input_pointer_id_t pointer)
{
    lh_return_if(pointer >= LH_OS_INPUT_POINTER_MAX ||
                     !lh_os_input_contacts_is_taken(self, pointer),
                 lh_os_input_device_kind_none);
    return lh_os_input_contacts_get_device_kind(self, pointer);
}

lh_u32_t
lh_os_input_contacts_get_count(lh_os_input_contacts_cptr self)
{
    lh_os_input_pointer_id_t pointer;
    lh_u32_t count = 0U;

    for (pointer = 1U; pointer < LH_OS_INPUT_POINTER_MAX; ++pointer)
    {
        count += lh_os_input_contacts_is_taken(self, pointer) ? 1U : 0U;
    }
    return count;
}

lh_void
lh_os_input_contacts_release(lh_os_input_contacts_t *self, lh_os_input_pointer_id_t pointer)
{
    lh_return_if(pointer == 0U || pointer >= LH_OS_INPUT_POINTER_MAX);
    lh_os_input_contacts_set_contact(self, pointer, 0U);
    lh_os_input_contacts_set_device_kind(self, pointer, lh_os_input_device_kind_none);
    lh_os_input_contacts_set_taken(self, pointer, lh_bool_false);
}

lh_os_input_contact_id_t
lh_os_input_contacts_get_contact(lh_os_input_contacts_cptr self, lh_os_input_pointer_id_t pointer)
{
    lh_assert_runtime_ref(self);
    return self->contacts[pointer];
}

lh_void
lh_os_input_contacts_set_contact(lh_os_input_contacts_t *self, lh_os_input_pointer_id_t pointer,
                                 lh_os_input_contact_id_t contact)
{
    lh_assert_runtime_ref(self);
    self->contacts[pointer] = contact;
}

lh_os_input_device_kind_t
lh_os_input_contacts_get_device_kind(lh_os_input_contacts_cptr self,
                                     lh_os_input_pointer_id_t pointer)
{
    lh_assert_runtime_ref(self);
    return self->device_kinds[pointer];
}

lh_void
lh_os_input_contacts_set_device_kind(lh_os_input_contacts_t *self, lh_os_input_pointer_id_t pointer,
                                     lh_os_input_device_kind_t device_kind)
{
    lh_assert_runtime_ref(self);
    self->device_kinds[pointer] = device_kind;
}

lh_bool_t
lh_os_input_contacts_is_taken(lh_os_input_contacts_cptr self, lh_os_input_pointer_id_t pointer)
{
    lh_assert_runtime_ref(self);
    return self->taken[pointer];
}

lh_void
lh_os_input_contacts_set_taken(lh_os_input_contacts_t *self, lh_os_input_pointer_id_t pointer,
                               lh_bool_t taken)
{
    lh_assert_runtime_ref(self);
    self->taken[pointer] = taken;
}
