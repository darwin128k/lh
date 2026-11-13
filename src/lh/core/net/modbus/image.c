#include <lh/core/net/modbus/image.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/memory/std.h>
#include <lh/core/util/return.h>

const lh_array_t *
lh_mb_image_get_wants(const lh_mb_image_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->wants);
}

lh_array_t *
lh_mb_image_get_wants_mut(lh_mb_image_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->wants);
}

const lh_array_t *
lh_mb_image_get_first(const lh_mb_image_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->first);
}

lh_array_t *
lh_mb_image_get_first_mut(lh_mb_image_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->first);
}

const lh_array_t *
lh_mb_image_get_values(const lh_mb_image_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->values);
}

lh_array_t *
lh_mb_image_get_values_mut(lh_mb_image_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->values);
}

const lh_array_t *
lh_mb_image_get_status(const lh_mb_image_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->status);
}

lh_array_t *
lh_mb_image_get_status_mut(lh_mb_image_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->status);
}

const lh_array_t *
lh_mb_image_get_updated(const lh_mb_image_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->updated);
}

lh_array_t *
lh_mb_image_get_updated_mut(lh_mb_image_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->updated);
}

lh_u16_t
lh_mb_image_get_count(const lh_mb_image_t *self)
{
    return lh_cast_static(lh_u16_t, lh_array_get_size(lh_mb_image_get_wants(self)));
}

const lh_mb_want_t *
lh_mb_image_get_want(const lh_mb_image_t *self, lh_u16_t index)
{
    return lh_ptr_ccast(lh_mb_want_t, lh_array_get_ptr(lh_mb_image_get_wants(self), index));
}

const lh_u16_t *
lh_mb_image_get_value(const lh_mb_image_t *self, lh_u16_t index)
{
    const lh_u32_t first =
        lh_ptr_deref(lh_ptr_ccast(lh_u32_t, lh_array_get_ptr(lh_mb_image_get_first(self), index)));

    return lh_ptr_ccast(lh_u16_t, lh_array_get_ptr(lh_mb_image_get_values(self), first));
}

lh_mb_value_status_t
lh_mb_image_get_value_status(const lh_mb_image_t *self, lh_u16_t index)
{
    return lh_cast_static(
        lh_mb_value_status_t,
        lh_ptr_deref(lh_ptr_ccast(lh_u8_t, lh_array_get_ptr(lh_mb_image_get_status(self), index))));
}

lh_u64_t
lh_mb_image_get_updated_us(const lh_mb_image_t *self, lh_u16_t index)
{
    return lh_ptr_deref(
        lh_ptr_ccast(lh_u64_t, lh_array_get_ptr(lh_mb_image_get_updated(self), index)));
}

lh_void
lh_mb_image_fail(lh_mb_image_t *self, const lh_mb_read_t *read, lh_mb_value_status_t status)
{
    lh_assert_runtime_ref(read);
    lh_u8_t *statuses = lh_ptr_cast(lh_u8_t, lh_array_get_data(lh_mb_image_get_status_mut(self)));

    for (lh_u32_t i = lh_mb_read_get_first(read); i <= lh_mb_read_get_last(read); ++i)
    {
        /* A value never read stays unread on a timeout: there is nothing to be old. */
        if (status == lh_mb_value_refused || statuses[i] != lh_mb_value_unread)
        {
            statuses[i] = lh_cast_static(lh_u8_t, status);
        }
    }
}

lh_void
lh_mb_image_init(lh_mb_image_t *self)
{
    lh_array_init(lh_mb_image_get_wants_mut(self), sizeof(lh_mb_want_t));
    lh_array_init(lh_mb_image_get_first_mut(self), sizeof(lh_u32_t));
    lh_array_init(lh_mb_image_get_values_mut(self), sizeof(lh_u16_t));
    lh_array_init(lh_mb_image_get_status_mut(self), sizeof(lh_u8_t));
    lh_array_init(lh_mb_image_get_updated_mut(self), sizeof(lh_u64_t));
}

lh_void
lh_mb_image_deinit(lh_mb_image_t *self)
{
    lh_array_deinit(lh_mb_image_get_wants_mut(self));
    lh_array_deinit(lh_mb_image_get_first_mut(self));
    lh_array_deinit(lh_mb_image_get_values_mut(self));
    lh_array_deinit(lh_mb_image_get_status_mut(self));
    lh_array_deinit(lh_mb_image_get_updated_mut(self));
}

lh_u16_t
lh_mb_image_find(const lh_mb_image_t *self, lh_u16_t address)
{
    const lh_u16_t count = lh_mb_image_get_count(self);
    lh_u16_t low = 0;
    lh_u16_t high = count;

    /* The first want that ends past the address; it covers it if it starts at or before. */
    while (low < high)
    {
        const lh_u16_t middle = lh_cast_static(lh_u16_t, (low + (high - low) / 2U));

        if (lh_mb_want_get_end(lh_mb_image_get_want(self, middle)) <= address)
        {
            low = lh_cast_static(lh_u16_t, (middle + 1U));
        }
        else
        {
            high = middle;
        }
    }
    lh_return_if(low == count, count);
    return lh_mb_want_get_address(lh_mb_image_get_want(self, low)) <= address ? low : count;
}

lh_void
lh_mb_image_store(lh_mb_image_t *self, const lh_mb_read_t *read, const lh_u16_t *values,
                  lh_u16_t count, lh_u64_t now_us)
{
    lh_assert_runtime_ref(read);
    lh_assert_runtime_ref(values);
    lh_u16_t *registers =
        lh_ptr_cast(lh_u16_t, lh_array_get_data(lh_mb_image_get_values_mut(self)));
    const lh_u32_t *first = lh_ptr_ccast(lh_u32_t, lh_array_get_data(lh_mb_image_get_first(self)));
    lh_u8_t *statuses = lh_ptr_cast(lh_u8_t, lh_array_get_data(lh_mb_image_get_status_mut(self)));
    lh_u64_t *updated = lh_ptr_cast(lh_u64_t, lh_array_get_data(lh_mb_image_get_updated_mut(self)));

    for (lh_u16_t i = lh_mb_read_get_first(read); i <= lh_mb_read_get_last(read); ++i)
    {
        const lh_mb_want_t *want = lh_mb_image_get_want(self, i);
        const lh_u32_t offset = lh_mb_want_get_address(want) - lh_mb_read_get_start(read);
        const lh_u32_t size = lh_mb_want_get_end(want) - lh_mb_want_get_address(want);

        if (offset + size > count)
        {
            /* The answer stopped short of this want: it keeps what it had. */
            if (statuses[i] != lh_mb_value_unread)
            {
                statuses[i] = lh_cast_static(lh_u8_t, lh_mb_value_stale);
            }
            continue;
        }
        lh_memory_std_copy(registers + first[i], values + offset,
                           lh_cast_static(lh_usize_t, size * sizeof(lh_u16_t)));
        statuses[i] = lh_cast_static(lh_u8_t, lh_mb_value_fresh);
        updated[i] = now_us;
    }
}

lh_bool_t
lh_mb_image_try_assign(lh_mb_image_t *self, const lh_mb_want_t *wants, lh_u16_t count)
{
    lh_assert_runtime_ref(wants);
    lh_mb_image_deinit(self);
    lh_u32_t registers = 0;
    for (lh_u16_t i = 0; i < count; ++i)
    {
        registers +=
            lh_mb_want_get_end(lh_addr_of(wants[i])) - lh_mb_want_get_address(lh_addr_of(wants[i]));
    }
    if (!lh_array_try_reserve(lh_mb_image_get_wants_mut(self), count) ||
        !lh_array_try_reserve(lh_mb_image_get_first_mut(self), count) ||
        !lh_array_try_reserve(lh_mb_image_get_values_mut(self), registers) ||
        !lh_array_try_reserve(lh_mb_image_get_status_mut(self), count) ||
        !lh_array_try_reserve(lh_mb_image_get_updated_mut(self), count))
    {
        lh_mb_image_deinit(self);
        return lh_bool_false;
    }
    /* Reserved: the resizes below do not allocate. */
    lh_array_resize(lh_mb_image_get_wants_mut(self), count);
    lh_array_resize(lh_mb_image_get_first_mut(self), count);
    lh_array_resize(lh_mb_image_get_values_mut(self), registers);
    lh_array_resize(lh_mb_image_get_status_mut(self), count);
    lh_array_resize(lh_mb_image_get_updated_mut(self), count);
    lh_memory_std_copy(lh_array_get_data(lh_mb_image_get_wants(self)), wants,
                       lh_cast_static(lh_usize_t, count * sizeof(lh_mb_want_t)));
    lh_memory_std_set(lh_array_get_data(lh_mb_image_get_values(self)), 0,
                      lh_cast_static(lh_usize_t, registers * sizeof(lh_u16_t)));
    lh_memory_std_set(lh_array_get_data(lh_mb_image_get_status(self)), lh_mb_value_unread,
                      lh_cast_static(lh_usize_t, count));
    lh_memory_std_set(lh_array_get_data(lh_mb_image_get_updated(self)), 0,
                      lh_cast_static(lh_usize_t, count * sizeof(lh_u64_t)));
    lh_u32_t *first = lh_ptr_cast(lh_u32_t, lh_array_get_data(lh_mb_image_get_first(self)));
    lh_u32_t at = 0;
    for (lh_u16_t i = 0; i < count; ++i)
    {
        first[i] = at;
        at +=
            lh_mb_want_get_end(lh_addr_of(wants[i])) - lh_mb_want_get_address(lh_addr_of(wants[i]));
    }
    return lh_bool_true;
}
