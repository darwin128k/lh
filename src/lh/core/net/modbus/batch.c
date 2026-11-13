#include <lh/core/net/modbus/batch.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/memory/std.h>
#include <lh/core/util/return.h>

lh_u8_t
lh_mb_batch_get_unit(const lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->unit;
}

lh_void
lh_mb_batch_set_unit(lh_mb_batch_t *self, lh_u8_t unit)
{
    lh_assert_runtime_ref(self);
    self->unit = unit;
}

lh_u8_t
lh_mb_batch_get_phase(const lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->phase;
}

lh_void
lh_mb_batch_set_phase(lh_mb_batch_t *self, lh_u8_t phase)
{
    lh_assert_runtime_ref(self);
    self->phase = phase;
}

lh_u16_t
lh_mb_batch_get_count(const lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_mb_batch_set_count(lh_mb_batch_t *self, lh_u16_t count)
{
    lh_assert_runtime_ref(self);
    self->count = count;
}

const lh_u16_t *
lh_mb_batch_get_addresses(const lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->addresses;
}

lh_u16_t *
lh_mb_batch_get_addresses_mut(lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->addresses;
}

const lh_u16_t *
lh_mb_batch_get_wanted(const lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wanted;
}

lh_u16_t *
lh_mb_batch_get_wanted_mut(lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wanted;
}

const lh_u16_t *
lh_mb_batch_get_held(const lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->held;
}

lh_u16_t *
lh_mb_batch_get_held_mut(lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->held;
}

const lh_u8_t *
lh_mb_batch_get_items(const lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->items;
}

lh_u8_t *
lh_mb_batch_get_items_mut(lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->items;
}

const lh_u8_t *
lh_mb_batch_get_exceptions(const lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->exceptions;
}

lh_u8_t *
lh_mb_batch_get_exceptions_mut(lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->exceptions;
}

lh_u16_t
lh_mb_batch_get_cursor(const lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->cursor;
}

lh_void
lh_mb_batch_set_cursor(lh_mb_batch_t *self, lh_u16_t cursor)
{
    lh_assert_runtime_ref(self);
    self->cursor = cursor;
}

lh_u16_t
lh_mb_batch_get_first(const lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->first;
}

lh_void
lh_mb_batch_set_first(lh_mb_batch_t *self, lh_u16_t first)
{
    lh_assert_runtime_ref(self);
    self->first = first;
}

lh_u16_t
lh_mb_batch_get_span(const lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->span;
}

lh_void
lh_mb_batch_set_span(lh_mb_batch_t *self, lh_u16_t span)
{
    lh_assert_runtime_ref(self);
    self->span = span;
}

lh_u8_t
lh_mb_batch_get_tries(const lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->tries;
}

lh_void
lh_mb_batch_set_tries(lh_mb_batch_t *self, lh_u8_t tries)
{
    lh_assert_runtime_ref(self);
    self->tries = tries;
}

lh_u8_t
lh_mb_batch_get_max_tries(const lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->max_tries;
}

lh_void
lh_mb_batch_set_max_tries(lh_mb_batch_t *self, lh_u8_t max_tries)
{
    lh_assert_runtime_ref(self);
    self->max_tries = max_tries;
}

lh_u16_t
lh_mb_batch_get_gap(const lh_mb_batch_t *self)
{
    lh_assert_runtime_ref(self);
    return self->gap;
}

lh_void
lh_mb_batch_set_gap(lh_mb_batch_t *self, lh_u16_t gap)
{
    lh_assert_runtime_ref(self);
    self->gap = gap;
}

lh_void
lh_mb_batch_init(lh_mb_batch_t *self, lh_u8_t max_tries, lh_u16_t gap)
{
    lh_mb_batch_set_unit(self, 0);
    lh_mb_batch_set_phase(self, lh_cast_static(lh_u8_t, lh_mb_batch_idle));
    lh_mb_batch_set_count(self, 0);
    lh_mb_batch_set_cursor(self, 0);
    lh_mb_batch_set_first(self, 0);
    lh_mb_batch_set_span(self, 0);
    lh_mb_batch_set_tries(self, 0);
    lh_mb_batch_set_max_tries(self, lh_math_max(max_tries, lh_cast_static(lh_u8_t, 1)));
    lh_mb_batch_set_gap(self, gap);
}

lh_bool_t
lh_mb_batch_is_idle(const lh_mb_batch_t *self)
{
    return lh_cast_static(lh_bool_t,
                          lh_mb_batch_get_phase(self) == lh_cast_static(lh_u8_t, lh_mb_batch_idle));
}

lh_u16_t
lh_mb_batch_find_item(const lh_mb_batch_t *self, lh_u16_t from, lh_mb_batch_item_t item)
{
    lh_u16_t found = from;

    while (found < lh_mb_batch_get_count(self) &&
           lh_mb_batch_get_items(self)[found] != lh_cast_static(lh_u8_t, item))
    {
        ++found;
    }
    return found;
}

lh_u16_t
lh_mb_batch_count_items(const lh_mb_batch_t *self, lh_mb_batch_item_t item)
{
    lh_u16_t counted = 0;

    for (lh_u16_t i = 0; i < lh_mb_batch_get_count(self); ++i)
    {
        counted += lh_cast_static(lh_u16_t,
                                  lh_mb_batch_get_items(self)[i] == lh_cast_static(lh_u8_t, item));
    }
    return counted;
}

lh_u16_t
lh_mb_batch_get_run_end(const lh_mb_batch_t *self, lh_u16_t first)
{
    const lh_u16_t *addresses = lh_mb_batch_get_addresses(self);
    lh_u16_t end = first + 1;

    while (end < lh_mb_batch_get_count(self) &&
           lh_mb_batch_get_items(self)[end] == lh_cast_static(lh_u8_t, lh_mb_batch_item_pending) &&
           addresses[end] == addresses[end - 1] + 1)
    {
        ++end;
    }
    return end;
}

lh_u16_t
lh_mb_batch_get_read_end(const lh_mb_batch_t *self, lh_u16_t first)
{
    const lh_u16_t *addresses = lh_mb_batch_get_addresses(self);
    lh_u16_t last = first;

    for (lh_u16_t i = first + 1; i < lh_mb_batch_get_count(self); ++i)
    {
        if (lh_mb_batch_get_items(self)[i] != lh_cast_static(lh_u8_t, lh_mb_batch_item_written))
        {
            continue;
        }
        if (addresses[i] - addresses[last] - 1 > lh_mb_batch_get_gap(self) ||
            addresses[i] - addresses[first] + 1 > LH_MB_READ_MAX)
        {
            break;
        }
        last = i;
    }
    return last + 1;
}

lh_void
lh_mb_batch_settle_write(lh_mb_batch_t *self, lh_mb_status_t status, lh_u8_t exception)
{
    lh_u8_t outcome = lh_cast_static(lh_u8_t, lh_mb_batch_item_written);

    if (status == lh_mb_status_exception)
    {
        outcome = lh_cast_static(lh_u8_t, lh_mb_batch_item_refused);
    }
    else if (status != lh_mb_status_ok)
    {
        outcome = lh_cast_static(lh_u8_t, lh_mb_batch_item_failed);
    }
    const lh_u16_t end = lh_mb_batch_get_first(self) + lh_mb_batch_get_span(self);

    for (lh_u16_t i = lh_mb_batch_get_first(self); i < end; ++i)
    {
        lh_mb_batch_get_items_mut(self)[i] = outcome;
        lh_mb_batch_get_exceptions_mut(self)[i] =
            outcome == lh_cast_static(lh_u8_t, lh_mb_batch_item_refused) ? exception : 0;
    }
}

lh_void
lh_mb_batch_settle_read(lh_mb_batch_t *self, lh_mb_status_t status, const lh_u16_t *values,
                        lh_u16_t count)
{
    const lh_u16_t *addresses = lh_mb_batch_get_addresses(self);
    const lh_u16_t end = lh_mb_batch_get_first(self) + lh_mb_batch_get_span(self);
    const lh_u16_t base = addresses[lh_mb_batch_get_first(self)];
    const lh_bool_t more_writes =
        lh_cast_static(lh_bool_t, lh_mb_batch_get_tries(self) < lh_mb_batch_get_max_tries(self));

    for (lh_u16_t i = lh_mb_batch_get_first(self); i < end; ++i)
    {
        lh_u8_t *item = lh_mb_batch_get_items_mut(self) + i;

        if (lh_ptr_deref(item) != lh_cast_static(lh_u8_t, lh_mb_batch_item_written))
        {
            continue;
        }
        if (status != lh_mb_status_ok || lh_null_eq(values) || addresses[i] - base >= count)
        {
            lh_ptr_deref(item) = lh_cast_static(lh_u8_t, lh_mb_batch_item_unverified);
            continue;
        }
        lh_mb_batch_get_held_mut(self)[i] = values[addresses[i] - base];
        if (lh_mb_batch_get_held(self)[i] == lh_mb_batch_get_wanted(self)[i])
        {
            lh_ptr_deref(item) = lh_cast_static(lh_u8_t, lh_mb_batch_item_confirmed);
        }
        else
        {
            lh_ptr_deref(item) = lh_cast_static(lh_u8_t, more_writes ? lh_mb_batch_item_pending
                                                                     : lh_mb_batch_item_mismatch);
        }
    }
}

lh_void
lh_mb_batch_release(lh_mb_batch_t *self)
{
    lh_mb_batch_set_phase(self, lh_cast_static(lh_u8_t, lh_mb_batch_idle));
}

lh_bool_t
lh_mb_batch_is_finished(const lh_mb_batch_t *self)
{
    const lh_u16_t count = lh_mb_batch_get_count(self);

    return lh_cast_static(lh_bool_t,
                          lh_mb_batch_find_item(self, 0, lh_mb_batch_item_pending) == count &&
                              lh_mb_batch_find_item(self, 0, lh_mb_batch_item_written) == count);
}

lh_bool_t
lh_mb_batch_try_begin(lh_mb_batch_t *self, lh_u8_t unit, const lh_u16_t *addresses,
                      const lh_u16_t *values, lh_u16_t count)
{
    lh_assert_runtime_ref(addresses);
    lh_assert_runtime_ref(values);
    lh_return_ifn(lh_mb_batch_is_idle(self) && count > 0 && count <= LH_MB_BATCH_MAX,
                  lh_bool_false);
    for (lh_u16_t i = 1; i < count; ++i)
    {
        lh_return_if(addresses[i] <= addresses[i - 1], lh_bool_false);
    }
    const lh_usize_t bytes = lh_cast_static(lh_usize_t, count * sizeof(lh_u16_t));

    lh_memory_std_copy(lh_mb_batch_get_addresses_mut(self), addresses, bytes);
    lh_memory_std_copy(lh_mb_batch_get_wanted_mut(self), values, bytes);
    lh_memory_std_set(lh_mb_batch_get_held_mut(self), 0, bytes);
    lh_memory_std_set(lh_mb_batch_get_items_mut(self), 0, count);
    lh_memory_std_set(lh_mb_batch_get_exceptions_mut(self), 0, count);
    lh_mb_batch_set_unit(self, unit);
    lh_mb_batch_set_count(self, count);
    lh_mb_batch_set_cursor(self, 0);
    lh_mb_batch_set_first(self, 0);
    lh_mb_batch_set_span(self, 0);
    lh_mb_batch_set_tries(self, 1);
    lh_mb_batch_set_phase(self, lh_cast_static(lh_u8_t, lh_mb_batch_writing));
    return lh_bool_true;
}

lh_bool_t
lh_mb_batch_try_next_write(lh_mb_batch_t *self, lh_mb_job_t *job)
{
    lh_assert_runtime_ref(job);
    const lh_u16_t first =
        lh_mb_batch_find_item(self, lh_mb_batch_get_cursor(self), lh_mb_batch_item_pending);

    lh_return_if(first == lh_mb_batch_get_count(self), lh_bool_false);
    const lh_u16_t end = lh_mb_batch_get_run_end(self, first);

    lh_mb_job_init_write(job, lh_mb_batch_get_unit(self),
                         end - first == 1 ? LH_MB_FC_WRITE_ONE : LH_MB_FC_WRITE_MANY,
                         lh_mb_batch_get_addresses(self)[first],
                         lh_mb_batch_get_wanted(self) + first, end - first);
    lh_mb_batch_set_first(self, first);
    lh_mb_batch_set_span(self, end - first);
    lh_mb_batch_set_cursor(self, end);
    return lh_bool_true;
}

lh_bool_t
lh_mb_batch_try_next_read(lh_mb_batch_t *self, lh_mb_job_t *job)
{
    lh_assert_runtime_ref(job);
    const lh_u16_t first =
        lh_mb_batch_find_item(self, lh_mb_batch_get_cursor(self), lh_mb_batch_item_written);

    lh_return_if(first == lh_mb_batch_get_count(self), lh_bool_false);
    const lh_u16_t end = lh_mb_batch_get_read_end(self, first);
    const lh_u16_t *addresses = lh_mb_batch_get_addresses(self);

    lh_mb_job_init_read(job, lh_mb_batch_get_unit(self), LH_MB_FC_READ_HOLDING, addresses[first],
                        addresses[end - 1] - addresses[first] + 1);
    lh_mb_batch_set_first(self, first);
    lh_mb_batch_set_span(self, end - first);
    lh_mb_batch_set_cursor(self, end);
    return lh_bool_true;
}

lh_void
lh_mb_batch_settle(lh_mb_batch_t *self, lh_mb_status_t status, lh_u8_t exception,
                   const lh_u16_t *values, lh_u16_t count)
{
    if (lh_mb_batch_get_phase(self) == lh_cast_static(lh_u8_t, lh_mb_batch_writing))
    {
        lh_mb_batch_settle_write(self, status, exception);
        return;
    }
    lh_return_ifn(lh_mb_batch_get_phase(self) == lh_cast_static(lh_u8_t, lh_mb_batch_verifying));
    lh_mb_batch_settle_read(self, status, values, count);
}

lh_bool_t
lh_mb_batch_try_next(lh_mb_batch_t *self, lh_mb_job_t *job)
{
    lh_assert_runtime_ref(job);
    if (lh_mb_batch_get_phase(self) == lh_cast_static(lh_u8_t, lh_mb_batch_writing))
    {
        lh_return_if(lh_mb_batch_try_next_write(self, job), lh_bool_true);
        lh_mb_batch_set_phase(self, lh_cast_static(lh_u8_t, lh_mb_batch_verifying));
        lh_mb_batch_set_cursor(self, 0);
    }
    lh_return_ifn(lh_mb_batch_get_phase(self) == lh_cast_static(lh_u8_t, lh_mb_batch_verifying),
                  lh_bool_false);
    lh_return_if(lh_mb_batch_try_next_read(self, job), lh_bool_true);
    /* Every written value has been read back. What read back as another word is pending
       again: write it once more, the ones that took stay as they are. */
    lh_return_if(lh_mb_batch_find_item(self, 0, lh_mb_batch_item_pending) ==
                     lh_mb_batch_get_count(self),
                 lh_bool_false);
    lh_mb_batch_set_tries(self, lh_cast_static(lh_u8_t, lh_mb_batch_get_tries(self) + 1));
    lh_mb_batch_set_phase(self, lh_cast_static(lh_u8_t, lh_mb_batch_writing));
    lh_mb_batch_set_cursor(self, 0);
    return lh_mb_batch_try_next_write(self, job);
}
