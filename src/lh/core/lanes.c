#include <lh/core/lanes.h>

#include <lh/core/assert/runtime.h>

lh_u8_t
lh_lanes_get_count(const lh_lanes_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_lanes_set_count(lh_lanes_t *self, lh_u8_t count)
{
    lh_assert_runtime_ref(self);
    self->count = lh_math_min(count, lh_cast_static(lh_u8_t, LH_LANES_MAX));
}

lh_u8_t
lh_lanes_get_patience(const lh_lanes_t *self, lh_u8_t lane)
{
    lh_assert_runtime_ref(self);
    return self->patience[lane];
}

lh_void
lh_lanes_set_patience(lh_lanes_t *self, lh_u8_t lane, lh_u8_t patience)
{
    lh_assert_runtime_ref(self);
    self->patience[lane] = patience;
}

lh_u8_t
lh_lanes_get_waited(const lh_lanes_t *self, lh_u8_t lane)
{
    lh_assert_runtime_ref(self);
    return self->waited[lane];
}

lh_void
lh_lanes_set_waited(lh_lanes_t *self, lh_u8_t lane, lh_u8_t waited)
{
    lh_assert_runtime_ref(self);
    self->waited[lane] = waited;
}

lh_void
lh_lanes_init(lh_lanes_t *self, lh_u8_t count)
{
    lh_lanes_set_count(self, count);
    for (lh_u8_t lane = 0; lane < LH_LANES_MAX; ++lane)
    {
        lh_lanes_set_patience(self, lane, 0);
        lh_lanes_set_waited(self, lane, 0);
    }
}

lh_bool_t
lh_lanes_is_ready(lh_u32_t ready, lh_u8_t lane)
{
    return lh_cast_static(lh_bool_t, !lh_bit_disjoint(ready, lh_bit_mask(lane)));
}

lh_bool_t
lh_lanes_is_impatient(const lh_lanes_t *self, lh_u8_t lane, lh_u32_t ready)
{
    const lh_u8_t patience = lh_lanes_get_patience(self, lane);

    return lh_cast_static(lh_bool_t, lh_lanes_is_ready(ready, lane) && patience != 0 &&
                                         lh_lanes_get_waited(self, lane) >= patience);
}

lh_u8_t
lh_lanes_pick(lh_lanes_t *self, lh_u32_t ready)
{
    const lh_u8_t count = lh_lanes_get_count(self);
    lh_u8_t pick = count;

    for (lh_u8_t lane = 0; lane < count && pick == count; ++lane)
    {
        if (lh_lanes_is_impatient(self, lane, ready))
        {
            pick = lane;
        }
    }
    for (lh_u8_t lane = 0; lane < count && pick == count; ++lane)
    {
        if (lh_lanes_is_ready(ready, lane))
        {
            pick = lane;
        }
    }
    for (lh_u8_t lane = 0; lane < count && pick != count; ++lane)
    {
        if (lane == pick)
        {
            lh_lanes_set_waited(self, lane, 0);
        }
        else if (lh_lanes_is_ready(ready, lane))
        {
            /* Up to 255 and no further: past its patience a lane is impatient anyway. */
            lh_lanes_set_waited(
                self, lane,
                lh_cast_static(lh_u8_t, lh_math_min(lh_lanes_get_waited(self, lane) + 1U, 255U)));
        }
    }
    return pick;
}
