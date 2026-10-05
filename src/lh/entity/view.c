#include <lh/entity/view.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/entity.h>
#include <lh/entity/2d.h>
#include <lh/entity/screen.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_view_construct(lh_entity_t *self)
{
    lh_entity_view_t *const view = lh_ptr_rcast(lh_entity_view_t, self);
    lh_int_t index;
    for (index = 0; index < LH_ENTITY_VIEW_SCROLL_LIMIT; ++index)
    {
        view->bars[index] = lh_null;
    }
    view->content = lh_null;
    view->x = 0;
    view->y = 0;
    view->carry_x = 0.0f;
    view->carry_y = 0.0f;
}

/* Whether a bar of @p self runs along @p axis. A side with no bar is not a
   side the view scrolls, whatever the content happens to leave over. */
static lh_bool_t
lh_entity_view_wheel_side(lh_entity_view_t *self, lh_int_t axis)
{
    const lh_int_t want = axis == LH_ENTITY_RANGE_AXIS_VERTICAL ? 1 : 0;
    lh_int_t index;
    for (index = 0; index < LH_ENTITY_VIEW_SCROLL_LIMIT; ++index)
    {
        lh_entity_scroll_t *const bar = self->bars[index];
        lh_int_t running;
        if (lh_ptr_is_null(bar))
        {
            continue;
        }
        running = lh_entity_range_is_vertical(lh_entity_scroll_get_range(bar)) != lh_bool_false
                      ? 1
                      : 0;
        if (running == want)
        {
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_void
lh_entity_view_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_view_t *const view = lh_ptr_rcast(lh_entity_view_t, self);
    const lh_uint_t code = lh_entity_event_get_code(event);
    lh_math_vec2_t delta;
    lh_int_t before_x;
    lh_int_t before_y;
    lh_int_t step_x;
    lh_int_t step_y;
    lh_assert_runtime_ref(view);
    if (code == LH_ENTITY_EVENT_DRAW)
    {
        lh_entity_view_sync(view);
        return;
    }
    if (code != LH_ENTITY_EVENT_WHEEL)
    {
        return;
    }
    delta = *lh_ptr_rcast(const lh_math_vec2_t, lh_entity_event_get_param(event));
    /* The bar's value is the offset, so a wheel is a wheel on the bar: the
       same number, moved. A fraction of a pixel waits in the carry, because a
       touchpad sends less than one at a time and rounding it away would leave
       a slow drag that never moves. */
    step_x = 0;
    step_y = 0;
    if (lh_entity_view_wheel_side(view, LH_ENTITY_RANGE_AXIS_HORIZONTAL) != lh_bool_false)
    {
        view->carry_x += lh_math_vec2_get_x(lh_addr_of(delta));
        step_x = lh_cast_static(lh_int_t, view->carry_x);
        view->carry_x -= (lh_float_t)step_x;
    }
    if (lh_entity_view_wheel_side(view, LH_ENTITY_RANGE_AXIS_VERTICAL) != lh_bool_false)
    {
        view->carry_y += lh_math_vec2_get_y(lh_addr_of(delta));
        step_y = lh_cast_static(lh_int_t, view->carry_y);
        view->carry_y -= (lh_float_t)step_y;
    }
    before_x = view->x;
    before_y = view->y;
    lh_entity_view_set_offset(view, before_x + step_x, before_y + step_y);
    if (view->x == before_x && view->y == before_y)
    {
        /* Nothing moved: not a whole pixel yet, or no bar on that side, or the
           end of the content already. None of those are this view's to take,
           so the wheel goes on up and an outer view gets its turn. */
        return;
    }
    lh_entity_event_stop(event);
}

const lh_entity_class_t lh_entity_view_class =
    lh_entity_class_initializer(lh_addr_of(lh_entity_2d_class), sizeof(lh_entity_view_t),
                                lh_entity_view_construct, lh_null, lh_entity_view_on_event);

lh_void
lh_entity_view_set_content(lh_entity_view_t *self, lh_entity_t *content)
{
    lh_assert_runtime_ref(self);
    self->content = content;
}

lh_int_t
lh_entity_view_get_travel(const lh_entity_view_t *self, lh_int_t axis)
{
    const lh_math_vec2_t box =
        lh_entity_2d_get_size(lh_ptr_rcast(const lh_entity_2d_t, self));
    lh_math_vec2_t page;
    lh_int_t width;
    lh_int_t height;
    lh_int_t travel;
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_null(self->content))
    {
        return 0;
    }
    page = lh_entity_2d_get_size(lh_ptr_rcast(const lh_entity_2d_t, self->content));
    width = lh_cast_static(lh_int_t, lh_math_vec2_get_x(lh_addr_of(page))) -
            lh_cast_static(lh_int_t, lh_math_vec2_get_x(lh_addr_of(box)));
    height = lh_cast_static(lh_int_t, lh_math_vec2_get_y(lh_addr_of(page))) -
             lh_cast_static(lh_int_t, lh_math_vec2_get_y(lh_addr_of(box)));
    if (axis == LH_ENTITY_RANGE_AXIS_HORIZONTAL)
    {
        travel = width;
    }
    else if (axis == LH_ENTITY_RANGE_AXIS_VERTICAL)
    {
        travel = height;
    }
    else
    {
        travel = width > height ? width : height;
    }
    return travel > 0 ? travel : 0;
}

lh_int_t
lh_entity_view_get_offset(const lh_entity_view_t *self, lh_int_t axis)
{
    const lh_math_vec2_t box =
        lh_entity_2d_get_size(lh_ptr_rcast(const lh_entity_2d_t, self));
    lh_float_t width;
    lh_float_t height;
    lh_assert_runtime_ref(self);
    if (axis == LH_ENTITY_RANGE_AXIS_HORIZONTAL)
    {
        return self->x;
    }
    if (axis == LH_ENTITY_RANGE_AXIS_VERTICAL)
    {
        return self->y;
    }
    width = lh_math_vec2_get_x(lh_addr_of(box));
    height = lh_math_vec2_get_y(lh_addr_of(box));
    return height > width ? self->y : self->x;
}

lh_void
lh_entity_view_set_offset(lh_entity_view_t *self, lh_int_t x, lh_int_t y)
{
    lh_int_t travel;
    lh_int_t index;
    lh_bool_t moved;
    lh_assert_runtime_ref(self);
    travel = lh_entity_view_get_travel(self, LH_ENTITY_RANGE_AXIS_HORIZONTAL);
    x = lh_entity_range_clamp(x, 0, travel);
    travel = lh_entity_view_get_travel(self, LH_ENTITY_RANGE_AXIS_VERTICAL);
    y = lh_entity_range_clamp(y, 0, travel);
    moved = (x != self->x || y != self->y) ? lh_bool_true : lh_bool_false;
    self->x = x;
    self->y = y;
    for (index = 0; index < LH_ENTITY_VIEW_SCROLL_LIMIT; ++index)
    {
        lh_entity_scroll_t *const bar = self->bars[index];
        lh_bool_t vertical;
        lh_int_t pos;
        if (lh_ptr_is_null(bar))
        {
            continue;
        }
        /* The bar's ends are the travel on its own way, so its value is the
           offset in those pixels. A bar whose ends are not the travel yet
           takes the mapping it was given, and the next sync makes them so. */
        vertical = lh_entity_range_is_vertical(lh_entity_scroll_get_range(bar));
        pos = vertical != lh_bool_false ? y : x;
        travel = vertical != lh_bool_false
                     ? lh_entity_view_get_travel(self, LH_ENTITY_RANGE_AXIS_VERTICAL)
                     : lh_entity_view_get_travel(self, LH_ENTITY_RANGE_AXIS_HORIZONTAL);
        lh_entity_range_set_from_pos(lh_entity_scroll_get_range(bar), pos, travel);
    }
    if (moved == lh_bool_false || lh_ptr_is_null(self->content))
    {
        return;
    }
    /* set_position dirties the content where it was and where it is, which is
       what makes the page move on screen. */
    lh_entity_2d_set_position(lh_ptr_rcast(lh_entity_2d_t, self->content),
                              lh_math_vec2_make((lh_float_t)(-x), (lh_float_t)(-y)));
}

lh_void
lh_entity_view_set_scrollbar(lh_entity_view_t *self, lh_entity_scroll_t *scroll)
{
    lh_entity_scroll_t *previous;
    lh_int_t slot;
    lh_int_t index;
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_null(scroll))
    {
        for (index = 0; index < LH_ENTITY_VIEW_SCROLL_LIMIT; ++index)
        {
            lh_entity_scroll_t *const bar = self->bars[index];
            if (lh_ptr_is_null(bar))
            {
                continue;
            }
            self->bars[index] = lh_null;
            lh_entity_scroll_set_view(bar, lh_null);
        }
        return;
    }
    slot = lh_entity_range_is_vertical(lh_entity_scroll_get_range(scroll)) == lh_bool_true
               ? LH_ENTITY_VIEW_SCROLL_LIMIT - 1
               : 0;
    previous = self->bars[slot];
    self->bars[slot] = scroll;
    if (lh_ptr_is_set(previous) && previous != scroll)
    {
        lh_entity_scroll_set_view(previous, lh_null);
    }
    lh_entity_scroll_set_view(scroll, self);
}

lh_void
lh_entity_view_sync(lh_entity_view_t *self)
{
    const lh_math_vec2_t box =
        lh_entity_2d_get_size(lh_ptr_rcast(const lh_entity_2d_t, self));
    lh_int_t width;
    lh_int_t height;
    lh_int_t offset_x;
    lh_int_t offset_y;
    lh_int_t index;
    lh_assert_runtime_ref(self);
    width = lh_cast_static(lh_int_t, lh_math_vec2_get_x(lh_addr_of(box)));
    height = lh_cast_static(lh_int_t, lh_math_vec2_get_y(lh_addr_of(box)));
    offset_x = self->x;
    offset_y = self->y;
    for (index = 0; index < LH_ENTITY_VIEW_SCROLL_LIMIT; ++index)
    {
        lh_entity_scroll_t *const bar = self->bars[index];
        lh_entity_range_t *range;
        lh_bool_t vertical;
        lh_int_t axis;
        lh_int_t travel;
        lh_int_t page;
        if (lh_ptr_is_null(bar))
        {
            continue;
        }
        range = lh_entity_scroll_get_range(bar);
        /* The bar's own axis decides, not its slot: a bar turned on its side
           after it was bound still has to be fed along the side it runs. */
        vertical = lh_entity_range_is_vertical(range);
        axis = vertical != lh_bool_false ? LH_ENTITY_RANGE_AXIS_VERTICAL
                                         : LH_ENTITY_RANGE_AXIS_HORIZONTAL;
        travel = lh_entity_view_get_travel(self, axis);
        page = vertical != lh_bool_false ? height : width;
        lh_entity_scroll_set_page(bar, page);
        lh_entity_range_set_ends(range, 0, travel);
        lh_entity_scroll_update_mode(bar);
        /* The bar's value is the offset, so it is read and not written: a drag
           that happened since the last draw is the one that counts, and a
           caller that moved the offset already put its value there. The ends
           above have just pulled that value into the travel that is left, so
           a shrunken page brings the offset back with it. */
        if (vertical != lh_bool_false)
        {
            offset_y = lh_entity_range_get_value(range);
        }
        else
        {
            offset_x = lh_entity_range_get_value(range);
        }
    }
    /* One write at the end, so the bars get the same numbers this window has
       and neither side ends up correcting the other. */
    lh_entity_view_set_offset(self, offset_x, offset_y);
}
