/**
 * @file token.c
 * @brief Level-1 accessors of ::lh_web_js_token_t.
 */

#include <lh/core/web/js/lexer/token.h>

#include <lh/core/assert/runtime.h>

lh_web_js_token_kind_t
lh_web_js_token_get_kind(const lh_web_js_token_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

lh_void
lh_web_js_token_set_kind(lh_web_js_token_t *self, lh_web_js_token_kind_t kind)
{
    lh_assert_runtime_ref(self);
    self->kind = kind;
}

const lh_str_ptr
lh_web_js_token_get_text(const lh_web_js_token_t *self)
{
    lh_assert_runtime_ref(self);
    return self->text;
}

lh_void
lh_web_js_token_set_text(lh_web_js_token_t *self, lh_str_cptr text)
{
    lh_assert_runtime_ref(self);
    self->text = text;
}

lh_usize_t
lh_web_js_token_get_text_len(const lh_web_js_token_t *self)
{
    lh_assert_runtime_ref(self);
    return self->text_len;
}

lh_void
lh_web_js_token_set_text_len(lh_web_js_token_t *self, lh_usize_t len)
{
    lh_assert_runtime_ref(self);
    self->text_len = len;
}

lh_real_t
lh_web_js_token_get_value(const lh_web_js_token_t *self)
{
    lh_assert_runtime_ref(self);
    return self->value;
}

lh_void
lh_web_js_token_set_value(lh_web_js_token_t *self, lh_real_t value)
{
    lh_assert_runtime_ref(self);
    self->value = value;
}
