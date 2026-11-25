/**
 * @file contacts.c
 * @brief Implementation of `lh/core/input/contacts.h`.
 */

#include <lh/core/assert/runtime.h>
#include <lh/core/input/contacts.h>
#include <lh/core/util/return.h>

lh_void
lh_input_contacts_init(lh_input_contacts_t *self)
{
    for (lh_input_pointer_id_t pointer = 0U; pointer < LH_INPUT_POINTER_MAX; ++pointer)
    {
        lh_input_contacts_set_contact(self, pointer, 0U);
        lh_input_contacts_set_device_kind(self, pointer, lh_input_device_kind_none);
        lh_input_contacts_set_taken(self, pointer, lh_bool_false);
    }
}

lh_bool_t
lh_input_contacts_find(lh_input_contacts_cptr self, lh_input_contact_id_t contact,
                       lh_input_pointer_id_t *out)
{
    lh_assert_runtime_ref(out);
    for (lh_input_pointer_id_t pointer = 1U; pointer < LH_INPUT_POINTER_MAX; ++pointer)
    {
        if (lh_input_contacts_is_taken(self, pointer) &&
            lh_input_contacts_get_contact(self, pointer) == contact)
        {
            lh_ptr_deref(out) = pointer;
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_bool_t
lh_input_contacts_take(lh_input_contacts_t *self, lh_input_contact_id_t contact,
                       lh_input_device_kind_t device_kind, lh_input_pointer_id_t *out)
{
    lh_assert_runtime_ref(out);
    /* A down the system sends twice for one contact keeps the id it has. */
    lh_return_if(lh_input_contacts_find(self, contact, out), lh_bool_true);
    for (lh_input_pointer_id_t pointer = 1U; pointer < LH_INPUT_POINTER_MAX; ++pointer)
    {
        if (!lh_input_contacts_is_taken(self, pointer))
        {
            lh_input_contacts_set_contact(self, pointer, contact);
            lh_input_contacts_set_device_kind(self, pointer, device_kind);
            lh_input_contacts_set_taken(self, pointer, lh_bool_true);
            lh_ptr_deref(out) = pointer;
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_input_device_kind_t
lh_input_contacts_get_device_kind_v(lh_input_contacts_cptr self, lh_input_pointer_id_t pointer)
{
    lh_return_if(pointer >= LH_INPUT_POINTER_MAX || !lh_input_contacts_is_taken(self, pointer),
                 lh_input_device_kind_none);
    return lh_input_contacts_get_device_kind(self, pointer);
}

lh_u32_t
lh_input_contacts_get_count(lh_input_contacts_cptr self)
{
    lh_u32_t count = 0U;

    for (lh_input_pointer_id_t pointer = 1U; pointer < LH_INPUT_POINTER_MAX; ++pointer)
    {
        count += lh_input_contacts_is_taken(self, pointer) ? 1U : 0U;
    }
    return count;
}

lh_void
lh_input_contacts_release(lh_input_contacts_t *self, lh_input_pointer_id_t pointer)
{
    lh_return_if(pointer == 0U || pointer >= LH_INPUT_POINTER_MAX);
    lh_input_contacts_set_contact(self, pointer, 0U);
    lh_input_contacts_set_device_kind(self, pointer, lh_input_device_kind_none);
    lh_input_contacts_set_taken(self, pointer, lh_bool_false);
}

lh_input_contact_id_t
lh_input_contacts_get_contact(lh_input_contacts_cptr self, lh_input_pointer_id_t pointer)
{
    lh_assert_runtime_ref(self);
    return self->contacts[pointer];
}

lh_void
lh_input_contacts_set_contact(lh_input_contacts_t *self, lh_input_pointer_id_t pointer,
                              lh_input_contact_id_t contact)
{
    lh_assert_runtime_ref(self);
    self->contacts[pointer] = contact;
}

lh_input_device_kind_t
lh_input_contacts_get_device_kind(lh_input_contacts_cptr self, lh_input_pointer_id_t pointer)
{
    lh_assert_runtime_ref(self);
    return self->device_kinds[pointer];
}

lh_void
lh_input_contacts_set_device_kind(lh_input_contacts_t *self, lh_input_pointer_id_t pointer,
                                  lh_input_device_kind_t device_kind)
{
    lh_assert_runtime_ref(self);
    self->device_kinds[pointer] = device_kind;
}

lh_bool_t
lh_input_contacts_is_taken(lh_input_contacts_cptr self, lh_input_pointer_id_t pointer)
{
    lh_assert_runtime_ref(self);
    return self->taken[pointer];
}

lh_void
lh_input_contacts_set_taken(lh_input_contacts_t *self, lh_input_pointer_id_t pointer,
                            lh_bool_t taken)
{
    lh_assert_runtime_ref(self);
    self->taken[pointer] = taken;
}
