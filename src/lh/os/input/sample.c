/**
 * @file sample.c
 * @brief One contact a driver read.
 */
#include <lh/os/input/sample.h>

#include <lh/core/assert/runtime.h>

lh_os_input_contact_id_t
lh_os_input_sample_get_contact(const lh_os_input_sample_t *self)
{
    lh_assert_runtime_ref(self);
    return self->contact;
}

lh_void
lh_os_input_sample_set_contact(lh_os_input_sample_t *self, lh_os_input_contact_id_t contact)
{
    lh_assert_runtime_ref(self);
    self->contact = contact;
}

lh_os_input_coord_t
lh_os_input_sample_get_x(const lh_os_input_sample_t *self)
{
    lh_assert_runtime_ref(self);
    return self->x;
}

lh_void
lh_os_input_sample_set_x(lh_os_input_sample_t *self, lh_os_input_coord_t x)
{
    lh_assert_runtime_ref(self);
    self->x = x;
}

lh_os_input_coord_t
lh_os_input_sample_get_y(const lh_os_input_sample_t *self)
{
    lh_assert_runtime_ref(self);
    return self->y;
}

lh_void
lh_os_input_sample_set_y(lh_os_input_sample_t *self, lh_os_input_coord_t y)
{
    lh_assert_runtime_ref(self);
    self->y = y;
}

lh_void
lh_os_input_sample_init(lh_os_input_sample_t *self, lh_os_input_contact_id_t contact,
                        lh_os_input_coord_t x, lh_os_input_coord_t y)
{
    lh_os_input_sample_set_contact(self, contact);
    lh_os_input_sample_set_x(self, x);
    lh_os_input_sample_set_y(self, y);
}

lh_bool_t
lh_os_input_sample_find(const lh_os_input_sample_t *samples, lh_u8_t count,
                        lh_os_input_contact_id_t contact, lh_u8_t *index)
{
    lh_assert_runtime_ref(index);
    if (lh_ptr_is_null(samples) || count == 0U)
    {
        return lh_bool_false;
    }
    for (lh_u8_t at = 0U; at < count; ++at)
    {
        const lh_os_input_sample_t *sample = lh_ptr_add_by_offset(
            const lh_os_input_sample_t, samples, (lh_uaddr_t)at * sizeof(lh_os_input_sample_t));

        if (lh_os_input_sample_get_contact(sample) == contact)
        {
            lh_ptr_deref(index) = at;
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}
