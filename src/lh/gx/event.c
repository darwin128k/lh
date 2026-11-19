/**
 * @file event.c
 * @brief Implementation of `lh/gx/event.h`.
 */

#include <lh/gx/event.h>

lh_gx_event_kind_t
lh_gx_event_get_kind(const lh_gx_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->kind;
}

lh_void
lh_gx_event_set_kind(lh_gx_event_t *self, lh_gx_event_kind_t kind)
{
    lh_assert_runtime_ref(self);
    self->kind = kind;
}

const lh_gx_point_t *
lh_gx_event_get_at(const lh_gx_event_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->at);
}

lh_void
lh_gx_event_set_at(lh_gx_event_t *self, lh_gx_point_t at)
{
    lh_assert_runtime_ref(self);
    self->at = at;
}

lh_gx_key_t
lh_gx_event_get_key(const lh_gx_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->key;
}

lh_void
lh_gx_event_set_key(lh_gx_event_t *self, lh_gx_key_t key)
{
    lh_assert_runtime_ref(self);
    self->key = key;
}

lh_u32_t
lh_gx_event_get_code(const lh_gx_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->code;
}

lh_void
lh_gx_event_set_code(lh_gx_event_t *self, lh_u32_t code)
{
    lh_assert_runtime_ref(self);
    self->code = code;
}

lh_bool_t
lh_gx_event_is_text_code(lh_u32_t code)
{
    return lh_cast_static(lh_bool_t, code >= 0x20U && code != 0x7FU &&
                                         (code < 0xD800U || code > 0xDFFFU) && code <= 0x10FFFFU);
}

lh_bool_t
lh_gx_event_is_pointer(const lh_gx_event_t *self)
{
    return lh_cast_static(lh_bool_t, lh_gx_event_get_kind(self) <= lh_gx_event_kind_cancel);
}

lh_void
lh_gx_event_init(lh_gx_event_t *self, lh_gx_event_kind_t kind, lh_gx_point_t at)
{
    lh_gx_event_set_kind(self, kind);
    lh_gx_event_set_at(self, at);
    lh_gx_event_set_key(self, lh_gx_key_none);
    lh_gx_event_set_code(self, 0U);
}

lh_void
lh_gx_event_init_kind(lh_gx_event_t *self, lh_gx_event_kind_t kind)
{
    lh_gx_point_t at;

    lh_gx_point_init(lh_addr_of(at), 0, 0);
    lh_gx_event_init(self, kind, at);
}

lh_void
lh_gx_event_init_key(lh_gx_event_t *self, lh_gx_key_t key)
{
    lh_gx_event_init_kind(self, lh_gx_event_kind_key);
    lh_gx_event_set_key(self, key);
}

lh_void
lh_gx_event_init_text(lh_gx_event_t *self, lh_u32_t code)
{
    lh_gx_event_init_kind(self, lh_gx_event_kind_text);
    lh_gx_event_set_code(self, code);
}
