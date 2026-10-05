#include <lh/entity/scroll.h>
#include <lh/assert.h>
#include <lh/entity.h>
#include <lh/entity/screen.h>
#include <lh/entity/view.h>
#include <lh/math/point.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/ui/canvas.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_math_rect_t
lh_entity_scroll_thumb(const lh_entity_scroll_t *self, const lh_math_rect_t *bounds)
{
    lh_int_t length;
    lh_int_t quarter;
    lh_int_t inset;
    lh_int_t thumb;
    lh_int_t travel;
    lh_int_t origin;
    lh_int_t x;
    lh_int_t y;
    lh_int_t width;
    lh_int_t height;
    lh_bool_t vertical;
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(bounds);
    x = lh_math_rect_get_x(bounds);
    y = lh_math_rect_get_y(bounds);
    width = lh_math_rect_get_size_width(bounds);
    height = lh_math_rect_get_size_height(bounds);
    vertical = lh_entity_range_is_vertical(lh_addr_of(self->range));
    length = vertical != lh_bool_false ? height : width;
    travel = self->range.maximum - self->range.minimum;
    if (travel <= 0)
    {
        /* Nothing to move. The thumb is the whole track, which is the honest
           reading of an extent of no width. */
        thumb = length;
    }
    else
    {
        /* The page and the travel are both pixels of the content, so together
           they are the content, and the thumb's share of the track is the
           page's share of it. */
        const lh_sllong_t whole = (lh_sllong_t)travel + (lh_sllong_t)self->page;
        thumb = (lh_int_t)(((lh_sllong_t)self->page * (lh_sllong_t)length) / whole);
        if (thumb < self->min_thumb)
        {
            thumb = self->min_thumb;
        }
        if (thumb > length)
        {
            thumb = length;
        }
    }
    /* The thumb keeps at least half the track, so a thin bar still shows one
       and the track's rounded ends stay past it. */
    quarter = (vertical != lh_bool_false ? width : height) / LH_ENTITY_SCROLL_INSET_CAP;
    inset = self->inset > quarter ? quarter : self->inset;
    if (inset < 0)
    {
        inset = 0;
    }
    origin = lh_entity_range_to_pos(lh_addr_of(self->range), length - thumb);
    if (vertical != lh_bool_false)
    {
        return lh_math_rect_make(x + inset, y + origin, width - inset * 2, thumb);
    }
    return lh_math_rect_make(x + origin, y + inset, thumb, height - inset * 2);
}

lh_void
lh_entity_scroll_paint(const lh_entity_scroll_t *self, lh_ui_canvas_t *canvas)
{
    const lh_ui_style_t *const style =
        lh_entity_2d_get_style(lh_ptr_rcast(const lh_entity_2d_t, self));
    lh_math_rect_t bounds;
    lh_math_rect_t thumb;
    lh_ui_color_t color;
    if (lh_ptr_is_null(style))
    {
        return;
    }
    bounds = lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
    lh_ui_canvas_fill_round(canvas, bounds, lh_entity_range_cap(lh_addr_of(bounds)),
                            lh_ui_style_get_bg_color(style));
    thumb = lh_entity_scroll_thumb(self, lh_addr_of(bounds));
    color = lh_ptr_is_set(self->thumb) ? lh_ui_style_get_bg_color(self->thumb)
                                       : lh_ui_style_get_text_color(style);
    lh_ui_canvas_fill_round(canvas, thumb, lh_entity_range_cap(lh_addr_of(thumb)), color);
}

lh_bool_t
lh_entity_scroll_press(lh_entity_scroll_t *self, const lh_math_vec2_t *point)
{
    const lh_math_rect_t bounds =
        lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
    lh_math_rect_t thumb;
    lh_int_t at;
    lh_int_t origin;
    lh_int_t away;
    lh_bool_t vertical;
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(point);
    vertical = lh_entity_range_is_vertical(lh_addr_of(self->range));
    thumb = lh_entity_scroll_thumb(self, lh_addr_of(bounds));
    if (lh_math_rect_contains_point(lh_addr_of(thumb), lh_math_vec2_to_point(*point)) !=
        lh_bool_false)
    {
        /* On the thumb. The point of it the pointer landed on is the point that
           then has to stay under the pointer, so an edge taken hold of is not
           the middle thrown away on the first move. */
        at = vertical != lh_bool_false
                 ? (lh_int_t)lh_math_vec2_get_y(point) - lh_math_rect_get_y(lh_addr_of(bounds))
                 : (lh_int_t)lh_math_vec2_get_x(point) - lh_math_rect_get_x(lh_addr_of(bounds));
        origin = vertical != lh_bool_false
                     ? lh_math_rect_get_y(lh_addr_of(thumb)) -
                           lh_math_rect_get_y(lh_addr_of(bounds))
                     : lh_math_rect_get_x(lh_addr_of(thumb)) -
                           lh_math_rect_get_x(lh_addr_of(bounds));
        self->grab = at - origin;
        return lh_bool_true;
    }
    /* Not on the thumb, so the track means one page towards the pointer, which
       is the way the value runs: a pointer before the thumb pages back. */
    away = vertical != lh_bool_false
               ? (lh_int_t)lh_math_vec2_get_y(point) - lh_math_rect_get_y(lh_addr_of(thumb))
               : (lh_int_t)lh_math_vec2_get_x(point) - lh_math_rect_get_x(lh_addr_of(thumb));
    lh_entity_scroll_page_by(self, away < 0 ? -1 : 1);
    return lh_bool_false;
}

lh_void
lh_entity_scroll_apply(lh_entity_scroll_t *self, const lh_math_vec2_t *point)
{
    const lh_math_rect_t bounds =
        lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
    lh_math_rect_t thumb;
    lh_int_t pos;
    lh_int_t span;
    lh_bool_t vertical;
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(point);
    vertical = lh_entity_range_is_vertical(lh_addr_of(self->range));
    thumb = lh_entity_scroll_thumb(self, lh_addr_of(bounds));
    if (vertical != lh_bool_false)
    {
        pos = (lh_int_t)lh_math_vec2_get_y(point) - lh_math_rect_get_y(lh_addr_of(bounds));
        span = lh_math_rect_get_size_height(lh_addr_of(bounds)) -
               lh_math_rect_get_size_height(lh_addr_of(thumb));
    }
    else
    {
        pos = (lh_int_t)lh_math_vec2_get_x(point) - lh_math_rect_get_x(lh_addr_of(bounds));
        span = lh_math_rect_get_size_width(lh_addr_of(bounds)) -
               lh_math_rect_get_size_width(lh_addr_of(thumb));
    }
    /* The point of the thumb that was taken hold of rides under the pointer,
       and the span is the room its origin has, which is the same room
       lh_entity_scroll_thumb places it in. */
    lh_entity_scroll_set_value(
        self, lh_entity_range_to_value(lh_addr_of(self->range), pos - self->grab, span));
}

lh_void
lh_entity_scroll_page_by(lh_entity_scroll_t *self, lh_int_t pages)
{
    lh_sllong_t step;
    lh_sllong_t next;
    lh_assert_runtime_ref(self);
    step = (lh_sllong_t)pages * (lh_sllong_t)self->page;
    next = (lh_sllong_t)lh_entity_range_get_value(lh_addr_of(self->range)) + step;
    /* The ends are the range's own, so a page past the end of the content is
       the end of it. The step is taken wide, so any count of pages lands on an
       end rather than running over one. */
    if (next > (lh_sllong_t)self->range.maximum)
    {
        next = self->range.maximum;
    }
    else if (next < (lh_sllong_t)self->range.minimum)
    {
        next = self->range.minimum;
    }
    lh_entity_scroll_set_value(self, (lh_int_t)next);
}

lh_bool_t
lh_entity_scroll_set_value(lh_entity_scroll_t *self, lh_int_t value)
{
    lh_int_t before;
    lh_int_t after;
    lh_bool_t vertical;
    lh_assert_runtime_ref(self);
    before = lh_entity_range_get_value(lh_addr_of(self->range));
    lh_entity_range_set_value(lh_addr_of(self->range), value);
    after = lh_entity_range_get_value(lh_addr_of(self->range));
    if (after == before)
    {
        return lh_bool_false;
    }
    /* The value is the offset, so the view is told by the one number rather
       than by which of the two sides moved. */
    vertical = lh_entity_range_is_vertical(lh_addr_of(self->range));
    if (lh_ptr_is_set(self->view))
    {
        if (vertical != lh_bool_false)
        {
            lh_entity_view_set_offset(
                self->view, lh_entity_view_get_offset(self->view, LH_ENTITY_RANGE_AXIS_HORIZONTAL),
                after);
        }
        else
        {
            lh_entity_view_set_offset(
                self->view, after,
                lh_entity_view_get_offset(self->view, LH_ENTITY_RANGE_AXIS_VERTICAL));
        }
    }
    lh_entity_send_event(lh_ptr_rcast(lh_entity_t, self), LH_ENTITY_EVENT_CLICKED, lh_null);
    return lh_bool_true;
}

lh_void
lh_entity_scroll_construct(lh_entity_t *self)
{
    lh_entity_scroll_t *const scroll = lh_ptr_rcast(lh_entity_scroll_t, self);
    lh_entity_add_flags(self, lh_entity_flags_own_background);
    lh_entity_range_reset(lh_addr_of(scroll->range));
    scroll->view = lh_null;
    scroll->page = LH_ENTITY_SCROLL_PAGE;
    scroll->mode = LH_ENTITY_SCROLL_SHOW_AUTO;
    scroll->min_thumb = LH_ENTITY_SCROLL_THUMB_MIN;
    scroll->inset = LH_ENTITY_SCROLL_INSET;
    scroll->grab = 0;
    scroll->dragging = lh_bool_false;
}

lh_void
lh_entity_scroll_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_scroll_t *const scroll = lh_ptr_rcast(lh_entity_scroll_t, self);
    const lh_uint_t code = lh_entity_event_get_code(event);
    if (code == LH_ENTITY_EVENT_DRAW)
    {
        lh_entity_scroll_paint(scroll, lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)));
        return;
    }
    if (code == LH_ENTITY_EVENT_POINTER_DOWN)
    {
        /* A press holds the thumb only if it landed on it. A press on the track
           turned a page, and a drag that followed would throw the thumb under a
           pointer that never took hold of it. */
        scroll->dragging = lh_entity_scroll_press(
            scroll, lh_ptr_rcast(const lh_math_vec2_t, lh_entity_event_get_param(event)));
        return;
    }
    if (code == LH_ENTITY_EVENT_POINTER_MOVE && scroll->dragging != lh_bool_false)
    {
        lh_entity_scroll_apply(scroll, lh_ptr_rcast(const lh_math_vec2_t,
                                                    lh_entity_event_get_param(event)));
        return;
    }
    if (code == LH_ENTITY_EVENT_POINTER_UP)
    {
        scroll->dragging = lh_bool_false;
    }
}

const lh_entity_class_t lh_entity_scroll_class =
    lh_entity_class_initializer(lh_addr_of(lh_entity_2d_class), sizeof(lh_entity_scroll_t),
                                lh_entity_scroll_construct, lh_null, lh_entity_scroll_on_event);

lh_int_t
lh_entity_scroll_get_page(const lh_entity_scroll_t *self)
{
    lh_assert_runtime_ref(self);
    return self->page;
}

lh_void
lh_entity_scroll_set_page(lh_entity_scroll_t *self, lh_int_t page)
{
    lh_int_t next;
    lh_assert_runtime_ref(self);
    next = page < 1 ? 1 : page;
    if (self->page == next)
    {
        return;
    }
    self->page = next;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

const lh_ui_style_t *
lh_entity_scroll_get_thumb(const lh_entity_scroll_t *self)
{
    lh_assert_runtime_ref(self);
    return self->thumb;
}

lh_void
lh_entity_scroll_set_thumb(lh_entity_scroll_t *self, const lh_ui_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->thumb = style;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

lh_entity_range_t *
lh_entity_scroll_get_range(lh_entity_scroll_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->range);
}

lh_int_t
lh_entity_scroll_get_thumb_min(const lh_entity_scroll_t *self)
{
    lh_assert_runtime_ref(self);
    return self->min_thumb;
}

lh_void
lh_entity_scroll_set_thumb_min(lh_entity_scroll_t *self, lh_int_t min_thumb)
{
    lh_int_t next;
    lh_assert_runtime_ref(self);
    next = min_thumb < 0 ? 0 : min_thumb;
    if (self->min_thumb == next)
    {
        return;
    }
    self->min_thumb = next;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

lh_int_t
lh_entity_scroll_get_thumb_inset(const lh_entity_scroll_t *self)
{
    lh_assert_runtime_ref(self);
    return self->inset;
}

lh_void
lh_entity_scroll_set_thumb_inset(lh_entity_scroll_t *self, lh_int_t inset)
{
    lh_int_t next;
    lh_assert_runtime_ref(self);
    next = inset < 0 ? 0 : inset;
    if (self->inset == next)
    {
        return;
    }
    self->inset = next;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

lh_int_t
lh_entity_scroll_get_mode(const lh_entity_scroll_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mode;
}

lh_void
lh_entity_scroll_set_mode(lh_entity_scroll_t *self, lh_int_t mode)
{
    lh_int_t next;
    lh_assert_runtime_ref(self);
    next = (mode == LH_ENTITY_SCROLL_SHOW_ALWAYS || mode == LH_ENTITY_SCROLL_SHOW_NEVER)
               ? mode
               : LH_ENTITY_SCROLL_SHOW_AUTO;
    if (self->mode == next)
    {
        return;
    }
    self->mode = next;
    lh_entity_scroll_update_mode(self);
}

lh_bool_t
lh_entity_scroll_is_needed(const lh_entity_scroll_t *self)
{
    lh_assert_runtime_ref(self);
    return self->range.maximum > self->range.minimum ? lh_bool_true : lh_bool_false;
}

lh_void
lh_entity_scroll_update_mode(lh_entity_scroll_t *self)
{
    lh_entity_t *const entity = lh_ptr_rcast(lh_entity_t, self);
    lh_bool_t shown;
    lh_assert_runtime_ref(self);
    if (self->mode == LH_ENTITY_SCROLL_SHOW_ALWAYS)
    {
        shown = lh_bool_true;
    }
    else if (self->mode == LH_ENTITY_SCROLL_SHOW_AUTO)
    {
        shown = lh_entity_scroll_is_needed(self);
    }
    else
    {
        shown = lh_bool_false;
    }
    if (lh_entity_has_flags(entity, lh_entity_flags_hidden) == lh_bool_false)
    {
        if (shown == lh_bool_true)
        {
            return;
        }
        lh_entity_add_flags(entity, lh_entity_flags_hidden);
    }
    else
    {
        if (shown == lh_bool_false)
        {
            return;
        }
        lh_entity_clear_flags(entity, lh_entity_flags_hidden);
    }
    /* The flag itself is not a setter and does not redraw, and a bar that has
       just appeared or gone is the one thing the caller cannot see without
       being told. */
    lh_entity_invalidate(entity);
}

lh_void
lh_entity_scroll_set_view(lh_entity_scroll_t *self, lh_entity_view_t *view)
{
    lh_assert_runtime_ref(self);
    self->view = view;
}
