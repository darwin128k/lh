/**
 * @file key.c
 * @brief Implementation of `lh/ui/key.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/ui/key.h>

lh_void
lh_ui_key_input_init_key(lh_ui_key_input_t *self, lh_os_key_t key, lh_bool_t pressed)
{
    lh_ui_key_input_set_key(self, key);
    lh_ui_key_input_set_pressed(self, pressed);
    lh_ui_key_input_set_code(self, 0U);
}

lh_void
lh_ui_key_input_init_text(lh_ui_key_input_t *self, lh_u32_t code)
{
    lh_assert_runtime_ifn(code != 0U, lh_runtime_error_code_invalid_argument);
    lh_ui_key_input_init_key(self, lh_os_key_other, lh_bool_true);
    lh_ui_key_input_set_code(self, code);
}

lh_os_key_t
lh_ui_key_input_get_key(const lh_ui_key_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->key;
}

lh_bool_t
lh_ui_key_input_is_pressed(const lh_ui_key_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pressed;
}

lh_u32_t
lh_ui_key_input_get_code(const lh_ui_key_input_t *self)
{
    lh_assert_runtime_ref(self);
    return self->code;
}

lh_bool_t
lh_ui_key_input_is_down(const lh_ui_key_input_t *self, lh_os_key_t key)
{
    return lh_ui_key_input_get_key(self) == key && lh_ui_key_input_is_pressed(self) &&
                   lh_ui_key_input_get_code(self) == 0U
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_ui_key_input_set_key(lh_ui_key_input_t *self, lh_os_key_t key)
{
    lh_assert_runtime_ref(self);
    self->key = key;
}

lh_void
lh_ui_key_input_set_pressed(lh_ui_key_input_t *self, lh_bool_t pressed)
{
    lh_assert_runtime_ref(self);
    self->pressed = pressed;
}

lh_void
lh_ui_key_input_set_code(lh_ui_key_input_t *self, lh_u32_t code)
{
    lh_assert_runtime_ref(self);
    self->code = code;
}
