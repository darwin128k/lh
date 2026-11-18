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

lh_void
lh_gx_event_init(lh_gx_event_t *self, lh_gx_event_kind_t kind, lh_gx_point_t at)
{
    lh_gx_event_set_kind(self, kind);
    lh_gx_event_set_at(self, at);
}
