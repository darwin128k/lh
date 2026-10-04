#include <lh/entity/range.h>
#include <lh/assert.h>
#include <lh/entity.h>
#include <lh/entity/screen.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_range_reset(lh_entity_range_t *self)
{
    self->minimum = 0;
    self->maximum = 100;
    self->value = 0;
}

lh_int_t
lh_entity_range_clamp(lh_int_t value, lh_int_t minimum, lh_int_t maximum)
{
    if (value < minimum)
    {
        return minimum;
    }
    if (value > maximum)
    {
        return maximum;
    }
    return value;
}

lh_void
lh_entity_progress_construct(lh_entity_t *self)
{
    lh_entity_add_flags(self, lh_entity_flags_own_background);
    lh_entity_range_reset(lh_ptr_rcast(lh_entity_range_t, self));
}

lh_void
lh_entity_progress_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_DRAW)
    {
        return;
    }
    lh_entity_range_paint(lh_ptr_rcast(const lh_entity_range_t, self),
                          lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)));
}

const lh_entity_class_t lh_entity_progress_class =
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_range_t),
                                lh_entity_progress_construct, lh_null, lh_entity_progress_on_event);

lh_int_t
lh_entity_range_get_minimum(const lh_entity_range_t *self)
{
    lh_assert_runtime_ref(self);
    return self->minimum;
}

lh_int_t
lh_entity_range_get_maximum(const lh_entity_range_t *self)
{
    lh_assert_runtime_ref(self);
    return self->maximum;
}

lh_int_t
lh_entity_range_get_value(const lh_entity_range_t *self)
{
    lh_assert_runtime_ref(self);
    return self->value;
}

lh_void
lh_entity_range_set_ends(lh_entity_range_t *self, lh_int_t minimum, lh_int_t maximum)
{
    lh_assert_runtime_ref(self);
    if (maximum < minimum)
    {
        maximum = minimum;
    }
    self->minimum = minimum;
    self->maximum = maximum;
    self->value = lh_entity_range_clamp(self->value, minimum, maximum);
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

lh_void
lh_entity_range_set_value(lh_entity_range_t *self, lh_int_t value)
{
    lh_int_t next;
    lh_assert_runtime_ref(self);
    next = lh_entity_range_clamp(value, self->minimum, self->maximum);
    if (next == self->value)
    {
        return;
    }
    self->value = next;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

lh_void
lh_entity_range_set_from_pos(lh_entity_range_t *self, lh_int_t pos, lh_int_t span)
{
    lh_sllong_t wide;
    lh_assert_runtime_ref(self);
    if (span <= 0)
    {
        return;
    }
    if (pos < 0)
    {
        pos = 0;
    }
    if (pos > span)
    {
        pos = span;
    }
    wide = (lh_sllong_t)(self->maximum - self->minimum) * (lh_sllong_t)pos;
    lh_entity_range_set_value(self, self->minimum + (lh_int_t)(wide / (lh_sllong_t)span));
}

lh_int_t
lh_entity_range_to_pos(const lh_entity_range_t *self, lh_int_t span)
{
    lh_int_t ends;
    lh_assert_runtime_ref(self);
    ends = self->maximum - self->minimum;
    if (span <= 0 || ends <= 0)
    {
        return 0;
    }
    return (lh_int_t)(((lh_sllong_t)(self->value - self->minimum) * (lh_sllong_t)span) /
                      (lh_sllong_t)ends);
}

lh_void
lh_entity_range_paint(const lh_entity_range_t *self, lh_ui_canvas_t *canvas)
{
    const lh_ui_style_t *style;
    lh_math_rect_t bounds;
    lh_int_t length;
    lh_int_t filled;
    lh_int_t vertical;
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(canvas);
    style = lh_entity_2d_get_style(lh_ptr_rcast(const lh_entity_2d_t, self));
    if (lh_ptr_is_null(style))
    {
        return;
    }
    bounds = lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
    vertical = lh_math_rect_get_size_height(lh_addr_of(bounds)) >
               lh_math_rect_get_size_width(lh_addr_of(bounds));
    {
        const lh_int_t x = lh_math_rect_get_x(lh_addr_of(bounds));
        const lh_int_t y = lh_math_rect_get_y(lh_addr_of(bounds));
        const lh_int_t width = lh_math_rect_get_size_width(lh_addr_of(bounds));
        const lh_int_t height = lh_math_rect_get_size_height(lh_addr_of(bounds));
        lh_int_t thick = 4;
        lh_math_rect_t track;
        if (vertical != 0)
        {
            if (thick > width)
            {
                thick = width;
            }
            length = height;
            track = lh_math_rect_make(x + (width - thick) / 2, y, thick, height);
        }
        else
        {
            if (thick > height)
            {
                thick = height;
            }
            length = width;
            track = lh_math_rect_make(x, y + (height - thick) / 2, width, thick);
        }
        filled = lh_entity_range_to_pos(self, length);
        lh_ui_canvas_fill_rect(canvas, track, lh_ui_style_get_bg_color(style));
        if (filled <= 0)
        {
            return;
        }
        if (vertical != 0)
        {
            track = lh_math_rect_make(lh_math_rect_get_x(lh_addr_of(track)),
                                      lh_math_rect_get_y(lh_addr_of(track)) + length - filled,
                                      lh_math_rect_get_size_width(lh_addr_of(track)), filled);
        }
        else
        {
            track = lh_math_rect_make(lh_math_rect_get_x(lh_addr_of(track)),
                                      lh_math_rect_get_y(lh_addr_of(track)), filled,
                                      lh_math_rect_get_size_height(lh_addr_of(track)));
        }
        lh_ui_canvas_fill_rect(canvas, track, lh_ui_style_get_text_color(style));
    }
}
