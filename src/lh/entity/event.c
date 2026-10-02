#include <lh/entity/event.h>
#include <lh/assert.h>

lh_uint_t
lh_entity_event_get_code(const lh_entity_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->code;
}

struct lh_entity *
lh_entity_event_get_target(const lh_entity_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->target;
}

struct lh_entity *
lh_entity_event_get_current(const lh_entity_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->current;
}

lh_ptr
lh_entity_event_get_param(const lh_entity_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->param;
}

lh_void
lh_entity_event_stop(lh_entity_event_t *self)
{
    lh_assert_runtime_ref(self);
    self->stopped = lh_bool_true;
}

lh_bool_t
lh_entity_event_is_stopped(const lh_entity_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->stopped;
}
