#include <lh/entity/spin.h>
#include <lh/assert.h>
#include <lh/entity.h>
#include <lh/entity/screen.h>
#include <lh/null.h>
#include <lh/str/format/sint.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_spin_construct(lh_entity_t *self)
{
    lh_entity_spin_t *const spin = lh_ptr_rcast(lh_entity_spin_t, self);
    lh_entity_range_reset(lh_addr_of(spin->range));
    spin->font = lh_ui_font_get_default();
    spin->step = 1;
}

lh_void
lh_entity_spin_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_spin_t *const spin = lh_ptr_rcast(lh_entity_spin_t, self);
    const lh_uint_t code = lh_entity_event_get_code(event);
    const lh_ui_style_t *style;
    lh_ui_canvas_t *canvas;
    lh_math_rect_t bounds;
    lh_char_t digits[16];
    lh_usize_t count;
    lh_int_t third;
    if (code == LH_ENTITY_EVENT_POINTER_UP)
    {
        const lh_math_vec2_t *const point =
            lh_ptr_rcast(const lh_math_vec2_t, lh_entity_event_get_param(event));
        const lh_int_t before = lh_entity_range_get_value(lh_addr_of(spin->range));
        bounds = lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
        third = lh_math_rect_get_size_width(lh_addr_of(bounds)) / 3;
        if (lh_ptr_is_null(point) ||
            !lh_entity_2d_contains(lh_ptr_rcast(const lh_entity_2d_t, self), *point))
        {
            return;
        }
        if ((lh_int_t)lh_math_vec2_get_x(point) < lh_math_rect_get_x(lh_addr_of(bounds)) + third)
        {
            lh_entity_range_set_value(lh_addr_of(spin->range), before - spin->step);
        }
        else if ((lh_int_t)lh_math_vec2_get_x(point) >=
                 lh_math_rect_get_x(lh_addr_of(bounds)) + third * 2)
        {
            lh_entity_range_set_value(lh_addr_of(spin->range), before + spin->step);
        }
        if (lh_entity_range_get_value(lh_addr_of(spin->range)) != before)
        {
            lh_entity_send_event(self, LH_ENTITY_EVENT_CLICKED, lh_null);
        }
        return;
    }
    if (code != LH_ENTITY_EVENT_DRAW)
    {
        return;
    }
    style = lh_entity_2d_get_style(lh_ptr_rcast(const lh_entity_2d_t, self));
    canvas = lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event));
    if (lh_ptr_is_null(style) || lh_ptr_is_null(spin->font))
    {
        return;
    }
    bounds = lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
    third = lh_math_rect_get_size_width(lh_addr_of(bounds)) / 3;
    lh_ui_canvas_fill_rect(canvas, bounds, lh_ui_style_get_bg_color(style));
    lh_ui_font_draw(spin->font, canvas, lh_math_rect_get_x(lh_addr_of(bounds)) + third / 2,
                    lh_math_rect_get_y(lh_addr_of(bounds)) + 4, "-", lh_ui_style_get_text_color(style));
    lh_ui_font_draw(spin->font, canvas,
                    lh_math_rect_get_x(lh_addr_of(bounds)) + third * 2 + third / 2,
                    lh_math_rect_get_y(lh_addr_of(bounds)) + 4, "+",
                    lh_ui_style_get_text_color(style));
    count = lh_str_ptr_format_sint((lh_sint_t)lh_entity_range_get_value(lh_addr_of(spin->range)),
                                   digits, sizeof digits - 1U);
    if (count >= sizeof digits)
    {
        count = sizeof digits - 1U;
    }
    digits[count] = 0;
    lh_ui_font_draw(spin->font, canvas, lh_math_rect_get_x(lh_addr_of(bounds)) + third,
                    lh_math_rect_get_y(lh_addr_of(bounds)) + 4, digits,
                    lh_ui_style_get_text_color(style));
}

const lh_entity_class_t lh_entity_spin_class =
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_spin_t),
                                lh_entity_spin_construct, lh_null, lh_entity_spin_on_event);

lh_int_t
lh_entity_spin_get_step(const lh_entity_spin_t *self)
{
    lh_assert_runtime_ref(self);
    return self->step;
}

lh_void
lh_entity_spin_set_step(lh_entity_spin_t *self, lh_int_t step)
{
    lh_assert_runtime_ref(self);
    self->step = step < 1 ? 1 : step;
}

const lh_ui_font_t *
lh_entity_spin_get_font(const lh_entity_spin_t *self)
{
    lh_assert_runtime_ref(self);
    return self->font;
}

lh_void
lh_entity_spin_set_font(lh_entity_spin_t *self, const lh_ui_font_t *font)
{
    lh_assert_runtime_ref(self);
    self->font = font;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}
