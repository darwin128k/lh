/**
 * @file key.c
 * @brief Implementation of `lh/ui/key.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/key.h>

lh_void
lh_ui_key_input_init_key(lh_ui_key_input_t *self, lh_key_t key, lh_bool_t pressed)
{
    lh_assert_runtime_ref(self);
    self->key = key;
    self->pressed = pressed;
    self->code = 0U;
}

lh_void
lh_ui_key_input_init_text(lh_ui_key_input_t *self, lh_u32_t code)
{
    lh_assert_runtime_ifn(code != 0U, lh_runtime_error_code_invalid_argument);
    lh_ui_key_input_init_key(self, lh_key_other, lh_bool_true);
    self->code = code;
}

lh_key_t
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
lh_ui_key_input_is_down(const lh_ui_key_input_t *self, lh_key_t key)
{
    lh_assert_runtime_ref(self);
    return self->key == key && self->pressed && self->code == 0U ? lh_bool_true : lh_bool_false;
}
