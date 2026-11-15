/**
 * @file touch.c
 * @brief Diff one touch driver's frame into pointer events.
 */
#include <lh/os/input/touch.h>

#include <lh/core/assert/runtime.h>
#include <lh/os/input/event.h>

const lh_os_input_contacts_t *
lh_os_input_touch_get_contacts(const lh_os_input_touch_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->contacts);
}

lh_os_input_contacts_t *
lh_os_input_touch_get_contacts_mut(lh_os_input_touch_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->contacts);
}

lh_u8_t
lh_os_input_touch_get_count(const lh_os_input_touch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_os_input_touch_set_count(lh_os_input_touch_t *self, lh_u8_t count)
{
    lh_assert_runtime_ref(self);
    self->count = count;
}

lh_u8_t
lh_os_input_touch_get_device(const lh_os_input_touch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->device;
}

lh_void
lh_os_input_touch_set_device(lh_os_input_touch_t *self, lh_u8_t device)
{
    lh_assert_runtime_ref(self);
    self->device = device;
}

lh_os_input_device_kind_t
lh_os_input_touch_get_device_kind(const lh_os_input_touch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->device_kind;
}

lh_void
lh_os_input_touch_set_device_kind(lh_os_input_touch_t *self, lh_os_input_device_kind_t device_kind)
{
    lh_assert_runtime_ref(self);
    self->device_kind = device_kind;
}

const lh_os_input_sample_t *
lh_os_input_touch_get_sample(const lh_os_input_touch_t *self, lh_u8_t index)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->samples[index]);
}

lh_os_input_sample_t *
lh_os_input_touch_get_sample_mut(lh_os_input_touch_t *self, lh_u8_t index)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->samples[index]);
}

lh_void
lh_os_input_touch_init(lh_os_input_touch_t *self, lh_os_input_device_kind_t device_kind,
                       lh_u8_t device)
{
    lh_os_input_contacts_init(lh_os_input_touch_get_contacts_mut(self));
    lh_os_input_touch_set_count(self, 0U);
    lh_os_input_touch_set_device(self, device);
    lh_os_input_touch_set_device_kind(self, device_kind);
}

lh_void
lh_os_input_touch_emit(lh_os_input_touch_t *self, lh_os_input_kind_t kind, lh_u8_t pointer,
                       lh_os_input_button_t button, lh_u32_t buttons, lh_os_input_coord_t x,
                       lh_os_input_coord_t y, lh_tick_t time, lh_os_input_sink_fn *sink,
                       lh_self_ptr sink_self)
{
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_null(sink))
    {
        return;
    }
    lh_os_input_event_t event;

    lh_os_input_event_init_pointer(lh_addr_of(event), kind, lh_os_input_touch_get_device_kind(self),
                                   pointer, button, x, y, time);
    lh_os_input_event_set_device(lh_addr_of(event), lh_os_input_touch_get_device(self));
    lh_os_input_event_set_buttons(lh_addr_of(event), buttons);
    sink(sink_self, lh_addr_of(event));
}

lh_void
lh_os_input_touch_report(lh_os_input_touch_t *self, const lh_os_input_sample_t *samples,
                         lh_u8_t count, lh_tick_t time, lh_os_input_sink_fn *sink,
                         lh_self_ptr sink_self)
{
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_null(samples))
    {
        count = 0U;
    }
    lh_u32_t held = lh_os_input_button_to_mask(lh_os_input_button_primary);
    lh_os_input_contacts_t *contacts = lh_os_input_touch_get_contacts_mut(self);

    for (lh_u8_t at = 0U; at < lh_os_input_touch_get_count(self); ++at)
    {
        const lh_os_input_sample_t *was = lh_os_input_touch_get_sample(self, at);
        lh_os_input_contact_id_t contact = lh_os_input_sample_get_contact(was);
        lh_u8_t found = 0U;

        if (lh_os_input_sample_find(samples, count, contact, lh_addr_of(found)) == lh_bool_true)
        {
            continue;
        }
        lh_os_input_pointer_id_t pointer = 0U;

        if (lh_os_input_contacts_find(contacts, contact, lh_addr_of(pointer)) == lh_bool_true)
        {
            lh_os_input_touch_emit(self, lh_os_input_kind_pointer_up, pointer,
                                   lh_os_input_button_primary, 0U, lh_os_input_sample_get_x(was),
                                   lh_os_input_sample_get_y(was), time, sink, sink_self);
            lh_os_input_contacts_release(contacts, pointer);
        }
    }
    lh_os_input_sample_t kept[LH_OS_INPUT_TOUCH_MAX];
    lh_u8_t kept_count = 0U;

    for (lh_u8_t at = 0U; at < count; ++at)
    {
        const lh_os_input_sample_t *sample = lh_ptr_add_by_offset(
            const lh_os_input_sample_t, samples, (lh_uaddr_t)at * sizeof(lh_os_input_sample_t));
        lh_os_input_contact_id_t contact = lh_os_input_sample_get_contact(sample);
        lh_u8_t again = 0U;

        if (lh_os_input_sample_find(kept, kept_count, contact, lh_addr_of(again)) == lh_bool_true)
        {
            continue;
        }
        lh_u8_t previous = 0U;

        if (lh_os_input_sample_find(lh_os_input_touch_get_sample(self, 0U),
                                    lh_os_input_touch_get_count(self), contact,
                                    lh_addr_of(previous)) == lh_bool_true)
        {
            const lh_os_input_sample_t *was = lh_os_input_touch_get_sample(self, previous);
            lh_os_input_pointer_id_t pointer = 0U;

            if (lh_os_input_sample_get_x(was) != lh_os_input_sample_get_x(sample) ||
                lh_os_input_sample_get_y(was) != lh_os_input_sample_get_y(sample))
            {
                if (lh_os_input_contacts_find(contacts, contact, lh_addr_of(pointer)) ==
                    lh_bool_true)
                {
                    lh_os_input_touch_emit(self, lh_os_input_kind_pointer_move, pointer,
                                           lh_os_input_button_none, held,
                                           lh_os_input_sample_get_x(sample),
                                           lh_os_input_sample_get_y(sample), time, sink, sink_self);
                }
            }
        }
        else
        {
            lh_os_input_pointer_id_t pointer = 0U;

            lh_os_input_device_kind_t device_kind = lh_os_input_touch_get_device_kind(self);

            if (lh_os_input_contacts_take(contacts, contact, device_kind, lh_addr_of(pointer)) ==
                lh_bool_false)
            {
                continue;
            }
            lh_os_input_touch_emit(self, lh_os_input_kind_pointer_down, pointer,
                                   lh_os_input_button_primary, held,
                                   lh_os_input_sample_get_x(sample),
                                   lh_os_input_sample_get_y(sample), time, sink, sink_self);
        }
        if (kept_count >= LH_OS_INPUT_TOUCH_MAX)
        {
            continue;
        }
        lh_os_input_sample_init(lh_addr_of(kept[kept_count]), contact,
                                lh_os_input_sample_get_x(sample), lh_os_input_sample_get_y(sample));
        kept_count = lh_cast_static(lh_u8_t, kept_count + 1U);
    }
    for (lh_u8_t at = 0U; at < kept_count; ++at)
    {
        lh_os_input_sample_t *slot = lh_os_input_touch_get_sample_mut(self, at);

        lh_os_input_sample_set_contact(slot, lh_os_input_sample_get_contact(lh_addr_of(kept[at])));
        lh_os_input_sample_set_x(slot, lh_os_input_sample_get_x(lh_addr_of(kept[at])));
        lh_os_input_sample_set_y(slot, lh_os_input_sample_get_y(lh_addr_of(kept[at])));
    }
    lh_os_input_touch_set_count(self, kept_count);
}
