/**
 * @file event.c
 * @brief Implementation of `lh/entity/event.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/entity/event.h>

lh_entity_event_code_t
lh_entity_event_get_code(const lh_entity_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->code;
}
