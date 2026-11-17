/**
 * @file damage.c
 * @brief Implementation of `lh/gx/damage.h`.
 */

#include <lh/core/math.h>
#include <lh/core/util/return.h>
#include <lh/gx/damage.h>

lh_void
lh_gx_damage_init(lh_gx_damage_t *self)
{
    lh_assert_runtime_ref(self);
    lh_gx_damage_set_count(self, 0u);
}

lh_bool_t
lh_gx_damage_is_empty(const lh_gx_damage_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_gx_damage_get_count(self) == 0u ? lh_bool_true : lh_bool_false;
}

lh_u32_t
lh_gx_damage_get_count(const lh_gx_damage_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

const lh_gx_rect_t *
lh_gx_damage_get_rect(const lh_gx_damage_t *self, lh_u32_t index)
{
    lh_assert_runtime_ref(self);
    lh_return_if(index >= lh_gx_damage_get_count(self), lh_null);
    return lh_addr_of(lh_gx_damage_get_rects(self)[index]);
}

const lh_gx_rect_t *
lh_gx_damage_get_rects(const lh_gx_damage_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rects;
}

lh_gx_rect_t *
lh_gx_damage_get_rects_mut(lh_gx_damage_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rects;
}

lh_void
lh_gx_damage_set_count(lh_gx_damage_t *self, lh_u32_t count)
{
    lh_assert_runtime_ref(self);
    lh_return_if(count > (lh_u32_t)LH_GX_DAMAGE_MAX);
    self->count = count;
}

lh_gx_scalar_t
lh_gx_damage_rect_area(const lh_gx_rect_t *rect)
{
    const lh_gx_size_t *size;

    lh_assert_runtime_ref(rect);
    size = lh_gx_rect_get_size(rect);
    return lh_math_max(lh_gx_size_get_width(size), lh_gx_scalar(0)) *
           lh_math_max(lh_gx_size_get_height(size), lh_gx_scalar(0));
}

lh_gx_scalar_t
lh_gx_damage_area(const lh_gx_damage_t *self)
{
    lh_gx_scalar_t area;

    lh_assert_runtime_ref(self);
    area = lh_gx_scalar(0);
    for (lh_u32_t i = 0u; i < lh_gx_damage_get_count(self); ++i)
    {
        area += lh_gx_damage_rect_area(lh_addr_of(lh_gx_damage_get_rects(self)[i]));
    }
    return area;
}

lh_bool_t
lh_gx_damage_joins(const lh_gx_rect_t *a, const lh_gx_rect_t *b)
{
    lh_gx_rect_t hull;

    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);

    lh_gx_rect_union(a, b, lh_addr_of(hull));
    lh_gx_rect_t meet;

    lh_gx_rect_intersection(a, b, lh_addr_of(meet));
    return lh_gx_damage_rect_area(lh_addr_of(hull)) ==
                   lh_gx_damage_rect_area(a) + lh_gx_damage_rect_area(b) -
                       lh_gx_damage_rect_area(lh_addr_of(meet))
               ? lh_bool_true
               : lh_bool_false;
}

lh_gx_scalar_t
lh_gx_damage_waste(const lh_gx_rect_t *a, const lh_gx_rect_t *b)
{
    lh_gx_rect_t hull;

    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);

    lh_gx_rect_union(a, b, lh_addr_of(hull));
    return lh_gx_damage_rect_area(lh_addr_of(hull)) - lh_gx_damage_rect_area(a) -
           lh_gx_damage_rect_area(b);
}

lh_void
lh_gx_damage_close(lh_gx_damage_t *self)
{
    lh_assert_runtime_ref(self);

    for (;;)
    {
        lh_bool_t joined = lh_bool_false;

        for (lh_u32_t keep = 0u; keep < lh_gx_damage_get_count(self); ++keep)
        {
            for (lh_u32_t drop = keep + 1u; drop < lh_gx_damage_get_count(self); ++drop)
            {
                if (!lh_gx_damage_joins(lh_addr_of(lh_gx_damage_get_rects(self)[keep]),
                                        lh_addr_of(lh_gx_damage_get_rects(self)[drop])))
                {
                    continue;
                }
                lh_gx_rect_union(lh_addr_of(lh_gx_damage_get_rects_mut(self)[keep]),
                                 lh_addr_of(lh_gx_damage_get_rects(self)[drop]),
                                 lh_addr_of(lh_gx_damage_get_rects_mut(self)[keep]));
                for (lh_u32_t i = drop; i + 1u < lh_gx_damage_get_count(self); ++i)
                {
                    lh_gx_damage_get_rects_mut(self)[i] = lh_gx_damage_get_rects(self)[i + 1u];
                }
                lh_gx_damage_set_count(self, lh_gx_damage_get_count(self) - 1u);
                joined = lh_bool_true;
                break;
            }
            if (joined == lh_bool_true)
            {
                break;
            }
        }
        if (joined == lh_bool_false)
        {
            return;
        }
    }
}

lh_void
lh_gx_damage_squeeze(lh_gx_damage_t *self, const lh_gx_rect_t *rect)
{
    lh_u32_t cheapest;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    lh_return_if(lh_gx_damage_get_count(self) == 0u);

    cheapest = 0u;
    for (lh_u32_t i = 1u; i < lh_gx_damage_get_count(self); ++i)
    {
        if (lh_gx_damage_waste(lh_addr_of(lh_gx_damage_get_rects(self)[i]), rect) <
            lh_gx_damage_waste(lh_addr_of(lh_gx_damage_get_rects(self)[cheapest]), rect))
        {
            cheapest = i;
        }
    }
    lh_gx_rect_union(lh_addr_of(lh_gx_damage_get_rects_mut(self)[cheapest]), rect,
                     lh_addr_of(lh_gx_damage_get_rects_mut(self)[cheapest]));
    lh_gx_damage_close(self);
}

lh_void
lh_gx_damage_add(lh_gx_damage_t *self, const lh_gx_rect_t *rect)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    lh_return_if(lh_gx_rect_is_empty(rect));

    if (lh_gx_damage_get_count(self) >= (lh_u32_t)LH_GX_DAMAGE_MAX)
    {
        lh_gx_damage_squeeze(self, rect);
        return;
    }
    lh_gx_damage_get_rects_mut(self)[lh_gx_damage_get_count(self)] = lh_ptr_deref(rect);
    lh_gx_damage_set_count(self, lh_gx_damage_get_count(self) + 1u);
    lh_gx_damage_close(self);
}

lh_void
lh_gx_damage_clear(lh_gx_damage_t *self)
{
    lh_assert_runtime_ref(self);
    lh_gx_damage_set_count(self, 0u);
}
