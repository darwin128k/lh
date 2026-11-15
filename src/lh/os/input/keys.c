/**
 * @file keys.c
 * @brief Diff one polled keyboard into key events.
 */
#include <lh/os/input/keys.h>

#include <lh/core/assert/runtime.h>
#include <lh/os/input/event.h>

lh_u8_t
lh_os_input_keys_get_device(const lh_os_input_keys_t *self)
{
    lh_assert_runtime_ref(self);
    return self->device;
}

lh_void
lh_os_input_keys_set_device(lh_os_input_keys_t *self, lh_u8_t device)
{
    lh_assert_runtime_ref(self);
    self->device = device;
}

lh_u32_t
lh_os_input_keys_get_down(const lh_os_input_keys_t *self)
{
    lh_assert_runtime_ref(self);
    return self->down;
}

lh_void
lh_os_input_keys_set_down(lh_os_input_keys_t *self, lh_u32_t down)
{
    lh_assert_runtime_ref(self);
    self->down = down;
}

lh_void
lh_os_input_keys_init(lh_os_input_keys_t *self, lh_u8_t device)
{
    lh_os_input_keys_set_device(self, device);
    lh_os_input_keys_set_down(self, 0U);
}

lh_bool_t
lh_os_input_keys_is_down(const lh_os_input_keys_t *self, lh_os_key_t key)
{
    lh_assert_runtime_ref(self);
    if (key == lh_os_key_other || key >= LH_OS_KEY_COUNT)
    {
        return lh_bool_false;
    }
    lh_u32_t bit = lh_bit_mask(key);

    if (lh_bit_and(lh_os_input_keys_get_down(self), bit) == 0U)
    {
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_void
lh_os_input_keys_report(lh_os_input_keys_t *self, lh_os_key_t key, lh_bool_t down,
                        lh_u32_t scancode, lh_tick_t time, lh_os_input_sink_fn *sink,
                        lh_self_ptr sink_self)
{
    lh_assert_runtime_ref(self);
    if (key == lh_os_key_other || key >= LH_OS_KEY_COUNT)
    {
        return;
    }
    if (lh_os_input_keys_is_down(self, key) == down)
    {
        return;
    }
    lh_u32_t held = lh_os_input_keys_get_down(self);
    lh_u32_t bit = lh_bit_mask(key);

    if (down == lh_bool_true)
    {
        held = lh_bit_or(held, bit);
    }
    else
    {
        held = lh_bit_and(held, lh_bit_not(bit));
    }
    lh_os_input_keys_set_down(self, held);
    if (lh_ptr_is_null(sink))
    {
        return;
    }
    lh_os_input_event_t event;

    lh_os_input_event_init_key(lh_addr_of(event), key, down, lh_bool_false, scancode, time);
    lh_os_input_event_set_device(lh_addr_of(event), lh_os_input_keys_get_device(self));
    lh_os_input_mods_t mods = lh_os_input_mods_if(lh_os_input_keys_is_down(self, lh_os_key_shift),
                                                  LH_OS_INPUT_MODS_SHIFT);

    mods = lh_bit_or(mods, lh_os_input_mods_if(lh_os_input_keys_is_down(self, lh_os_key_control),
                                               LH_OS_INPUT_MODS_CONTROL));
    mods = lh_bit_or(mods, lh_os_input_mods_if(lh_os_input_keys_is_down(self, lh_os_key_alt),
                                               LH_OS_INPUT_MODS_ALT));
    lh_os_input_event_set_mods(lh_addr_of(event), mods);
    sink(sink_self, lh_addr_of(event));
}
