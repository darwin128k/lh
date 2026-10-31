/**
 * @file rects.c
 * @brief Implementation of `lh/ui/rects.h`.
 */
#include <lh/assert.h>

#include <lh/math.h>
#include <lh/ui/rects.h>
#include <lh/util/return.h>

lh_void
lh_ui_rects_init(lh_ui_rects_t *self)
{
    lh_assert_runtime_ref(self);
    self->count = 0u;
}

lh_bool_t
lh_ui_rects_is_empty(const lh_ui_rects_t *self)
{
    return lh_ui_rects_get_count(self) == 0u ? lh_bool_true : lh_bool_false;
}

lh_u32_t
lh_ui_rects_get_count(const lh_ui_rects_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

const lh_ui_rect_t *
lh_ui_rects_get(const lh_ui_rects_t *self, lh_u32_t index)
{
    lh_return_if(index >= lh_ui_rects_get_count(self), lh_null);
    return lh_addr_of(lh_ui_rects_get_rects(self)[index]);
}

lh_ui_scalar_t
lh_ui_rects_area(const lh_ui_rect_t *rect)
{
    const lh_ui_size_t *size;

    lh_assert_runtime_ref(rect);
    size = lh_ui_rect_get_size(rect);
    return lh_ui_size_get_width(size) * lh_ui_size_get_height(size);
}

lh_bool_t
lh_ui_rects_touch(const lh_ui_rect_t *a, const lh_ui_rect_t *b)
{
    lh_ui_rect_t reach;

    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    /* One pixel of slack on every side, which is what makes sharing an edge
       count: two rects that meet can become one without covering anything
       either of them was not already covering. */
    lh_ui_rect_inset(b, lh_ui_scalar(-1), lh_ui_scalar(-1), lh_addr_of(reach));
    return lh_ui_rect_intersects(a, lh_addr_of(reach));
}

lh_ui_scalar_t
lh_ui_rects_waste(const lh_ui_rect_t *a, const lh_ui_rect_t *b)
{
    lh_ui_rect_t hull;

    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    lh_ui_rect_union(a, b, lh_addr_of(hull));
    return lh_ui_rects_area(lh_addr_of(hull)) - lh_ui_rects_area(a) - lh_ui_rects_area(b);
}

lh_void
lh_ui_rects_drop(lh_ui_rects_t *self, lh_u32_t index)
{
    lh_u32_t i;

    lh_return_if(index >= lh_ui_rects_get_count(self));
    for (i = index; i + 1u < lh_ui_rects_get_count(self); ++i)
    {
        lh_ui_rects_get_rects_mut(self)[i] = lh_ui_rects_get_rects(self)[i + 1u];
    }
    lh_ui_rects_set_count(self, lh_ui_rects_get_count(self) - 1);
}

lh_bool_t
lh_ui_rects_join_once(lh_ui_rects_t *self)
{

    for (lh_u32_t keep = 0u; keep < lh_ui_rects_get_count(self); ++keep)
    {
        for (lh_u32_t drop = keep + 1u; drop < lh_ui_rects_get_count(self); ++drop)
        {
            if (!lh_ui_rects_touch(lh_addr_of(lh_ui_rects_get_rects(self)[keep]),
                                   lh_addr_of(lh_ui_rects_get_rects(self)[drop])))
            {
                continue;
            }
            lh_ui_rect_union(lh_addr_of(lh_ui_rects_get_rects(self)[keep]),
                             lh_addr_of(lh_ui_rects_get_rects(self)[drop]),
                             lh_addr_of(lh_ui_rects_get_rects_mut(self)[keep]));
            lh_ui_rects_drop(self, drop);
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_void
lh_ui_rects_close(lh_ui_rects_t *self)
{
    while (lh_ui_rects_join_once(self))
    {
    }
}

lh_void
lh_ui_rects_squeeze(lh_ui_rects_t *self, const lh_ui_rect_t *rect)
{

    lh_assert_runtime_ref(rect);
    lh_return_if(lh_ui_rects_get_count(self) == 0u);
    lh_u32_t cheapest = 0u;
    for (lh_u32_t i = 1u; i < lh_ui_rects_get_count(self); ++i)
    {
        if (lh_ui_rects_waste(lh_addr_of(lh_ui_rects_get_rects(self)[i]), rect) <
            lh_ui_rects_waste(lh_addr_of(lh_ui_rects_get_rects(self)[cheapest]), rect))
        {
            cheapest = i;
        }
    }
    lh_ui_rect_union(lh_addr_of(lh_ui_rects_get_rects_mut(self)[cheapest]), rect,
                     lh_addr_of(lh_ui_rects_get_rects_mut(self)[cheapest]));
    lh_ui_rects_close(self);
}

lh_void
lh_ui_rects_add(lh_ui_rects_t *self, const lh_ui_rect_t *rect)
{
    lh_assert_runtime_ref(rect);
    lh_return_if(lh_ui_rect_is_empty(rect));
    if (lh_ui_rects_get_count(self) >= lh_cast_static(lh_u32_t, LH_UI_RECTS_MAX))
    {
        lh_ui_rects_squeeze(self, rect);
        return;
    }
    lh_ui_rects_get_rects_mut(self)[lh_ui_rects_get_count(self)] = lh_ptr_deref(rect);
    lh_ui_rects_set_count(self, lh_ui_rects_get_count(self) + 1);
    lh_ui_rects_close(self);
}

const lh_ui_rect_t *
lh_ui_rects_get_rects(const lh_ui_rects_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rects;
}

lh_ui_rect_t *
lh_ui_rects_get_rects_mut(lh_ui_rects_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rects;
}

lh_void
lh_ui_rects_set_count(lh_ui_rects_t *self, lh_u32_t count)
{
    lh_assert_runtime_ref(self);
    self->count = count;
}
