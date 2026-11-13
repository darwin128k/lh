#include <lh/core/net/modbus/schedule.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/memory/std.h>

lh_u32_t
lh_mb_schedule_get_fast(const lh_mb_schedule_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fast;
}

lh_void
lh_mb_schedule_set_fast(lh_mb_schedule_t *self, lh_u32_t fast)
{
    lh_assert_runtime_ref(self);
    self->fast = fast;
}

const lh_array_t *
lh_mb_schedule_get_reads(const lh_mb_schedule_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->reads);
}

lh_array_t *
lh_mb_schedule_get_reads_mut(lh_mb_schedule_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->reads);
}

const lh_array_t *
lh_mb_schedule_get_due(const lh_mb_schedule_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->due);
}

lh_array_t *
lh_mb_schedule_get_due_mut(lh_mb_schedule_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->due);
}

const lh_array_t *
lh_mb_schedule_get_period(const lh_mb_schedule_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->period);
}

lh_array_t *
lh_mb_schedule_get_period_mut(lh_mb_schedule_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->period);
}

const lh_array_t *
lh_mb_schedule_get_watched(const lh_mb_schedule_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->watched);
}

lh_array_t *
lh_mb_schedule_get_watched_mut(lh_mb_schedule_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->watched);
}

lh_u16_t
lh_mb_schedule_get_count(const lh_mb_schedule_t *self)
{
    return lh_cast_static(lh_u16_t, lh_array_get_size(lh_mb_schedule_get_reads(self)));
}

const lh_mb_read_t *
lh_mb_schedule_get_read(const lh_mb_schedule_t *self, lh_u16_t index)
{
    return lh_ptr_ccast(lh_mb_read_t, lh_array_get_ptr(lh_mb_schedule_get_reads(self), index));
}

lh_u64_t
lh_mb_schedule_get_due_us(const lh_mb_schedule_t *self, lh_u16_t index)
{
    return lh_ptr_deref(
        lh_ptr_ccast(lh_u64_t, lh_array_get_ptr(lh_mb_schedule_get_due(self), index)));
}

lh_u32_t
lh_mb_schedule_get_want_period_us(const lh_mb_schedule_t *self, lh_u16_t want)
{
    const lh_u16_t watched = lh_ptr_deref(
        lh_ptr_ccast(lh_u16_t, lh_array_get_ptr(lh_mb_schedule_get_watched(self), want)));
    const lh_u32_t period = lh_ptr_deref(
        lh_ptr_ccast(lh_u32_t, lh_array_get_ptr(lh_mb_schedule_get_period(self), want)));

    return watched != 0 ? lh_math_min(period, lh_mb_schedule_get_fast(self)) : period;
}

lh_void
lh_mb_schedule_set_want_period_us(lh_mb_schedule_t *self, lh_u16_t want, lh_u32_t period_us)
{
    if (period_us != 0)
    {
        lh_ptr_deref(lh_ptr_cast(
            lh_u32_t, lh_array_get_ptr(lh_mb_schedule_get_period_mut(self), want))) = period_us;
    }
}

lh_void
lh_mb_schedule_unwatch(lh_mb_schedule_t *self, lh_u16_t want)
{
    lh_u16_t *watched =
        lh_ptr_cast(lh_u16_t, lh_array_get_ptr(lh_mb_schedule_get_watched_mut(self), want));

    if (lh_ptr_deref(watched) != 0)
    {
        lh_ptr_deref(watched) = lh_cast_static(lh_u16_t, (lh_ptr_deref(watched) - 1U));
    }
}

lh_void
lh_mb_schedule_init(lh_mb_schedule_t *self)
{
    lh_array_init(lh_mb_schedule_get_reads_mut(self), sizeof(lh_mb_read_t));
    lh_array_init(lh_mb_schedule_get_due_mut(self), sizeof(lh_u64_t));
    lh_array_init(lh_mb_schedule_get_period_mut(self), sizeof(lh_u32_t));
    lh_array_init(lh_mb_schedule_get_watched_mut(self), sizeof(lh_u16_t));
    lh_mb_schedule_set_fast(self, LH_MB_SCHEDULE_FAST_US);
}

lh_void
lh_mb_schedule_deinit(lh_mb_schedule_t *self)
{
    lh_array_deinit(lh_mb_schedule_get_reads_mut(self));
    lh_array_deinit(lh_mb_schedule_get_due_mut(self));
    lh_array_deinit(lh_mb_schedule_get_period_mut(self));
    lh_array_deinit(lh_mb_schedule_get_watched_mut(self));
}

lh_u32_t
lh_mb_schedule_get_read_period_us(const lh_mb_schedule_t *self, lh_u16_t index)
{
    const lh_mb_read_t *read = lh_mb_schedule_get_read(self, index);
    lh_u32_t shortest = lh_mb_schedule_get_want_period_us(self, lh_mb_read_get_first(read));

    for (lh_u32_t want = lh_mb_read_get_first(read) + 1U; want <= lh_mb_read_get_last(read); ++want)
    {
        shortest = lh_math_min(
            shortest, lh_mb_schedule_get_want_period_us(self, lh_cast_static(lh_u16_t, want)));
    }
    return shortest;
}

lh_u16_t
lh_mb_schedule_find_read(const lh_mb_schedule_t *self, lh_u16_t want)
{
    const lh_u16_t count = lh_mb_schedule_get_count(self);
    lh_u16_t low = 0;
    lh_u16_t high = count;

    /* The first request whose last want is at or past @p want. */
    while (low < high)
    {
        const lh_u16_t middle = lh_cast_static(lh_u16_t, (low + (high - low) / 2U));

        if (lh_mb_read_get_last(lh_mb_schedule_get_read(self, middle)) < want)
        {
            low = lh_cast_static(lh_u16_t, (middle + 1U));
        }
        else
        {
            high = middle;
        }
    }
    if (low < count && lh_mb_read_get_first(lh_mb_schedule_get_read(self, low)) <= want)
    {
        return low;
    }
    return count;
}

lh_bool_t
lh_mb_schedule_is_read_watched(const lh_mb_schedule_t *self, lh_u16_t index)
{
    const lh_mb_read_t *read = lh_mb_schedule_get_read(self, index);

    for (lh_u32_t want = lh_mb_read_get_first(read); want <= lh_mb_read_get_last(read); ++want)
    {
        if (lh_ptr_deref(lh_ptr_ccast(
                lh_u16_t, lh_array_get_ptr(lh_mb_schedule_get_watched(self), want))) != 0)
        {
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_u64_t
lh_mb_schedule_get_next_due_us(const lh_mb_schedule_t *self)
{
    lh_u64_t next = ~lh_cast_static(lh_u64_t, 0U);

    for (lh_u16_t i = 0; i < lh_mb_schedule_get_count(self); ++i)
    {
        next = lh_math_min(next, lh_mb_schedule_get_due_us(self, i));
    }
    return next;
}

lh_bool_t
lh_mb_schedule_try_assign(lh_mb_schedule_t *self, const lh_mb_read_t *reads, lh_u16_t read_count,
                          lh_u16_t want_count, lh_u64_t now_us)
{
    lh_assert_runtime_ref(reads);
    lh_mb_schedule_deinit(self);
    if (!lh_array_try_reserve(lh_mb_schedule_get_reads_mut(self), read_count) ||
        !lh_array_try_reserve(lh_mb_schedule_get_due_mut(self), read_count) ||
        !lh_array_try_reserve(lh_mb_schedule_get_period_mut(self), want_count) ||
        !lh_array_try_reserve(lh_mb_schedule_get_watched_mut(self), want_count))
    {
        lh_mb_schedule_deinit(self);
        return lh_bool_false;
    }
    /* Reserved: the resizes below do not allocate. */
    lh_array_resize(lh_mb_schedule_get_reads_mut(self), read_count);
    lh_array_resize(lh_mb_schedule_get_due_mut(self), read_count);
    lh_array_resize(lh_mb_schedule_get_period_mut(self), want_count);
    lh_array_resize(lh_mb_schedule_get_watched_mut(self), want_count);
    lh_memory_std_copy(lh_array_get_data(lh_mb_schedule_get_reads(self)), reads,
                       lh_cast_static(lh_usize_t, read_count * sizeof(lh_mb_read_t)));
    lh_memory_std_set(lh_array_get_data(lh_mb_schedule_get_watched(self)), 0,
                      lh_cast_static(lh_usize_t, want_count * sizeof(lh_u16_t)));
    const lh_u32_t period = LH_MB_SCHEDULE_PERIOD_US;
    lh_array_fill(lh_mb_schedule_get_period_mut(self), lh_addr_of(period));
    lh_array_fill(lh_mb_schedule_get_due_mut(self), lh_addr_of(now_us));
    return lh_bool_true;
}

lh_u64_t
lh_mb_schedule_get_lateness(const lh_mb_schedule_t *self, lh_u16_t index, lh_u64_t now_us)
{
    const lh_u64_t due = lh_mb_schedule_get_due_us(self, index);

    if (now_us < due)
    {
        return 0;
    }
    /* Late by one period is 1024; a period of 0 would be due all the time, count it as 1. */
    return (now_us - due) * 1024U /
           lh_math_max(lh_cast_static(lh_u64_t, lh_mb_schedule_get_read_period_us(self, index)),
                       lh_cast_static(lh_u64_t, 1U));
}

lh_void
lh_mb_schedule_mark_done(lh_mb_schedule_t *self, lh_u16_t index, lh_u64_t now_us)
{
    lh_ptr_deref(lh_ptr_cast(lh_u64_t, lh_array_get_ptr(lh_mb_schedule_get_due_mut(self), index))) =
        now_us + lh_mb_schedule_get_read_period_us(self, index);
}

lh_void
lh_mb_schedule_watch(lh_mb_schedule_t *self, lh_u16_t want, lh_u64_t now_us)
{
    lh_u16_t *watched =
        lh_ptr_cast(lh_u16_t, lh_array_get_ptr(lh_mb_schedule_get_watched_mut(self), want));

    if (lh_ptr_deref(watched) != 0xFFFFU)
    {
        lh_ptr_deref(watched) = lh_cast_static(lh_u16_t, (lh_ptr_deref(watched) + 1U));
    }
    /* A screen that opens shows fresh values, not ones up to a period old. */
    const lh_u16_t read = lh_mb_schedule_find_read(self, want);
    if (read < lh_mb_schedule_get_count(self))
    {
        lh_u64_t *due =
            lh_ptr_cast(lh_u64_t, lh_array_get_ptr(lh_mb_schedule_get_due_mut(self), read));
        lh_ptr_deref(due) = lh_math_min(lh_ptr_deref(due), now_us);
    }
}

lh_u16_t
lh_mb_schedule_find_due(const lh_mb_schedule_t *self, lh_u64_t now_us)
{
    const lh_u16_t count = lh_mb_schedule_get_count(self);
    lh_u16_t best = count;
    lh_bool_t best_watched = lh_bool_false;
    lh_u64_t best_lateness = 0;

    for (lh_u16_t i = 0; i < count; ++i)
    {
        if (lh_mb_schedule_get_due_us(self, i) > now_us)
        {
            continue;
        }
        const lh_bool_t watched = lh_mb_schedule_is_read_watched(self, i);
        const lh_u64_t lateness = lh_mb_schedule_get_lateness(self, i, now_us);

        /* What is on a screen first; then the latest for its period. */
        if (best == count || (watched && !best_watched) ||
            (watched == best_watched && lateness > best_lateness))
        {
            best = i;
            best_watched = watched;
            best_lateness = lateness;
        }
    }
    return best;
}
