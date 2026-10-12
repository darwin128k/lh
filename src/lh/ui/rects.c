/**
 * @file rects.c
 * @brief Implementation of `lh/ui/rects.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/math.h>
#include <lh/null.h>
#include <lh/runtime/error/code.h>
#include <lh/ui/rects.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>
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
    lh_assert_runtime_ref(self);
    return self->count == 0u ? lh_bool_true : lh_bool_false;
}

lh_u32_t
lh_ui_rects_get_count(const lh_ui_rects_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

const lh_ui_rect_t *
lh_ui_rects_get_as_const(const lh_ui_rects_t *self, lh_u32_t index)
{
    lh_assert_runtime_ref(self);
    lh_return_if(index >= self->count, lh_null);
    return lh_addr_of(self->rects[index]);
}

lh_ui_scalar_t
lh_ui_rects_area(const lh_ui_rect_t *rect)
{
    const lh_ui_size_t *size;

    lh_assert_runtime_ref(rect);
    size = lh_ui_rect_get_size_as_const(rect);
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
    reach = lh_ui_rect_inset(b, lh_ui_scalar(-1), lh_ui_scalar(-1));
    return lh_ui_rect_intersects(a, lh_addr_of(reach));
}

lh_ui_scalar_t
lh_ui_rects_waste(const lh_ui_rect_t *a, const lh_ui_rect_t *b)
{
    lh_ui_rect_t hull;

    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    hull = lh_ui_rect_union(a, b);
    return lh_ui_rects_area(lh_addr_of(hull)) - lh_ui_rects_area(a) - lh_ui_rects_area(b);
}

lh_void
lh_ui_rects_add(lh_ui_rects_t *self, const lh_ui_rect_t *rect)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    lh_return_if(lh_ui_rect_is_empty(rect));
    if (self->count >= (lh_u32_t)LH_UI_RECTS_MAX)
    {
        lh_ui_rects_squeeze(self, rect);
        return;
    }
    self->rects[self->count] = *rect;
    ++self->count;
    lh_ui_rects_close(self);
}

lh_void
lh_ui_rects_close(lh_ui_rects_t *self)
{
    lh_assert_runtime_ref(self);
    while (lh_ui_rects_join_once(self))
    {
    }
}

lh_bool_t
lh_ui_rects_join_once(lh_ui_rects_t *self)
{
    lh_u32_t keep;
    lh_u32_t drop;

    lh_assert_runtime_ref(self);
    for (keep = 0u; keep < self->count; ++keep)
    {
        for (drop = keep + 1u; drop < self->count; ++drop)
        {
            if (!lh_ui_rects_touch(lh_addr_of(self->rects[keep]), lh_addr_of(self->rects[drop])))
            {
                continue;
            }
            self->rects[keep] = lh_ui_rect_union(lh_addr_of(self->rects[keep]), lh_addr_of(self->rects[drop]));
            lh_ui_rects_drop(self, drop);
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_void
lh_ui_rects_drop(lh_ui_rects_t *self, lh_u32_t index)
{
    lh_u32_t i;

    lh_assert_runtime_ref(self);
    lh_return_if(index >= self->count);
    for (i = index; i + 1u < self->count; ++i)
    {
        self->rects[i] = self->rects[i + 1u];
    }
    --self->count;
}

lh_void
lh_ui_rects_squeeze(lh_ui_rects_t *self, const lh_ui_rect_t *rect)
{
    lh_u32_t i;
    lh_u32_t cheapest;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(rect);
    lh_return_if(self->count == 0u);
    cheapest = 0u;
    for (i = 1u; i < self->count; ++i)
    {
        if (lh_ui_rects_waste(lh_addr_of(self->rects[i]), rect)
            < lh_ui_rects_waste(lh_addr_of(self->rects[cheapest]), rect))
        {
            cheapest = i;
        }
    }
    self->rects[cheapest] = lh_ui_rect_union(lh_addr_of(self->rects[cheapest]), rect);
    lh_ui_rects_close(self);
}
