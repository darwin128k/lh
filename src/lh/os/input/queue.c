/**
 * @file queue.c
 * @brief Implementation of `lh/os/input/queue.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/os/input/queue.h>
#include <lh/util/return.h>

lh_u16_t
lh_os_input_queue_next(lh_u16_t capacity, lh_u16_t index)
{
    const lh_u16_t next = lh_type_cast(lh_u16_t, index + 1U);

    return next == lh_type_cast(lh_u16_t, capacity * 2U) ? lh_type_cast(lh_u16_t, 0U) : next;
}

lh_u16_t
lh_os_input_queue_previous(lh_u16_t capacity, lh_u16_t index)
{
    return index == 0U ? lh_type_cast(lh_u16_t, capacity * 2U - 1U)
                       : lh_type_cast(lh_u16_t, index - 1U);
}

lh_u16_t
lh_os_input_queue_slot(lh_u16_t capacity, lh_u16_t index)
{
    return index < capacity ? index : lh_type_cast(lh_u16_t, index - capacity);
}

lh_void
lh_os_input_queue_init(lh_os_input_queue_t *self, lh_os_input_event_t *storage, lh_u16_t capacity)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(storage);
    lh_assert_runtime_ifn(capacity > 0U && capacity <= 32767U,
                          lh_runtime_error_code_invalid_argument);
    self->storage = storage;
    self->capacity = capacity;
    self->head = 0U;
    self->tail = 0U;
    self->dropped = 0U;
}

lh_u16_t
lh_os_input_queue_get_capacity(lh_os_input_queue_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->capacity;
}

lh_u16_t
lh_os_input_queue_get_count(lh_os_input_queue_cptr self)
{
    lh_u16_t head;
    lh_u16_t tail;
    lh_u16_t capacity;

    lh_assert_runtime_ref(self);
    head = self->head;
    tail = self->tail;
    capacity = lh_os_input_queue_get_capacity(self);

    return tail >= head ? lh_type_cast(lh_u16_t, tail - head)
                        : lh_type_cast(lh_u16_t, capacity * 2U - head + tail);
}

lh_u32_t
lh_os_input_queue_get_dropped(lh_os_input_queue_cptr self)
{
    lh_assert_runtime_ref(self);
    return self->dropped;
}

lh_bool_t
lh_os_input_queue_is_empty(lh_os_input_queue_cptr self)
{
    return lh_type_cast(lh_bool_t, lh_os_input_queue_get_count(self) == 0U);
}

lh_bool_t
lh_os_input_queue_can_fold(lh_os_input_event_cptr last, lh_os_input_event_cptr event)
{
    return lh_type_cast(
        lh_bool_t,
        lh_os_input_event_get_kind(event) == lh_os_input_kind_pointer_move &&
            lh_os_input_event_get_kind(last) == lh_os_input_kind_pointer_move &&
            lh_os_input_event_get_device(last) == lh_os_input_event_get_device(event) &&
            lh_os_input_event_get_pointer(last) == lh_os_input_event_get_pointer(event) &&
            lh_os_input_event_get_buttons(last) == lh_os_input_event_get_buttons(event));
}

lh_bool_t
lh_os_input_queue_push_full(lh_os_input_queue_t *self, lh_os_input_event_cptr event)
{
    const lh_u16_t capacity = lh_os_input_queue_get_capacity(self);
    lh_os_input_event_t *last = lh_addr_of(self->storage[lh_os_input_queue_slot(
        capacity, lh_os_input_queue_previous(capacity, self->tail))]);

    if (lh_os_input_queue_get_count(self) > 1U && lh_os_input_queue_can_fold(last, event))
    {
        lh_ptr_deref(last) = lh_ptr_deref(event);
        return lh_bool_true;
    }
    ++self->dropped;
    return lh_bool_false;
}

lh_bool_t
lh_os_input_queue_push(lh_os_input_queue_t *self, lh_os_input_event_cptr event)
{
    const lh_u16_t capacity = lh_os_input_queue_get_capacity(self);
    const lh_u16_t tail = self->tail;

    lh_assert_runtime_ref(event);
    lh_return_if(lh_os_input_queue_get_count(self) == capacity,
                 lh_os_input_queue_push_full(self, event));
    self->storage[lh_os_input_queue_slot(capacity, tail)] = lh_ptr_deref(event);
    /* The slot is written before the tail moves past it: the consumer reads only what the tail
       has passed. */
    self->tail = lh_os_input_queue_next(capacity, tail);
    return lh_bool_true;
}

lh_bool_t
lh_os_input_queue_pop(lh_os_input_queue_t *self, lh_os_input_event_t *out)
{
    const lh_u16_t capacity = lh_os_input_queue_get_capacity(self);
    const lh_u16_t head = self->head;

    lh_assert_runtime_ref(out);
    lh_return_if(lh_os_input_queue_is_empty(self), lh_bool_false);
    lh_ptr_deref(out) = self->storage[lh_os_input_queue_slot(capacity, head)];
    self->head = lh_os_input_queue_next(capacity, head);
    return lh_bool_true;
}

lh_u16_t
lh_os_input_queue_drain(lh_os_input_queue_t *self, lh_os_input_sink_cb sink, lh_ptr context)
{
    lh_os_input_event_t event;
    lh_u16_t count = 0U;

    lh_assert_runtime_ref(sink);
    while (lh_os_input_queue_pop(self, lh_addr_of(event)))
    {
        sink(context, lh_addr_of(event));
        ++count;
    }
    return count;
}
