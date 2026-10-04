#include <lh/entity/spin.h>
#include <lh/assert.h>
#include <lh/entity/flex.h>
#include <lh/entity/screen.h>
#include <lh/null.h>
#include <lh/str/format/sint.h>
#include <lh/ui/canvas.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_spin_show(lh_entity_spin_t *self)
{
    lh_char_t fresh[16];
    lh_usize_t count;
    lh_usize_t i;
    count = lh_str_ptr_format_sint((lh_sint_t)lh_entity_range_get_value(lh_addr_of(self->range)),
                                   fresh, sizeof fresh - 1U);
    if (count >= sizeof fresh)
    {
        count = sizeof fresh - 1U;
    }
    fresh[count] = 0;
    for (i = 0; i <= count; ++i)
    {
        if (self->digits[i] != fresh[i])
        {
            break;
        }
    }
    if (i > count)
    {
        return;
    }
    for (i = 0; i <= count; ++i)
    {
        self->digits[i] = fresh[i];
    }
    if (lh_ptr_is_set(self->value))
    {
        lh_entity_label_set_text(self->value, self->digits);
    }
}

lh_void
lh_entity_spin_dress(lh_entity_t *entity, const lh_ui_style_t *style, const lh_ui_font_t *font)
{
    lh_entity_label_t *const label = lh_entity_cast(entity, lh_addr_of(lh_entity_label_class));
    lh_entity_2d_t *const box = lh_entity_cast(entity, lh_addr_of(lh_entity_2d_class));
    if (lh_ptr_is_set(label) && lh_entity_label_get_font(label) != font)
    {
        lh_entity_label_set_font(label, font);
    }
    if (lh_ptr_is_set(box) && lh_ptr_is_set(style) && lh_entity_2d_get_style(box) != style)
    {
        lh_entity_2d_set_style(box, style);
    }
    lh_entity_foreach_child(child, entity)
    {
        lh_entity_spin_dress(child, style, font);
    }
}

lh_void
lh_entity_spin_on_side(lh_entity_event_t *event, lh_ptr user)
{
    lh_entity_spin_t *const spin = lh_ptr_rcast(lh_entity_spin_t, user);
    lh_entity_t *const target = lh_entity_event_get_target(event);
    lh_int_t before;
    lh_int_t next;
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_CLICKED || lh_ptr_is_null(spin))
    {
        return;
    }
    before = lh_entity_range_get_value(lh_addr_of(spin->range));
    next = before;
    if (target == lh_ptr_rcast(lh_entity_t, spin->minus))
    {
        next -= spin->step;
    }
    else
    {
        next += spin->step;
    }
    lh_entity_range_set_value(lh_addr_of(spin->range), next);
    lh_entity_spin_show(spin);
    if (lh_entity_range_get_value(lh_addr_of(spin->range)) != before)
    {
        lh_entity_send_event(lh_ptr_rcast(lh_entity_t, spin), LH_ENTITY_EVENT_CLICKED, lh_null);
    }
}

lh_void
lh_entity_spin_seat(lh_entity_spin_t *self)
{
    lh_entity_2d_t *const box = lh_ptr_rcast(lh_entity_2d_t, self);
    lh_entity_2d_t *bar;
    const lh_math_vec2_t size = lh_entity_2d_get_size(box);
    lh_math_vec2_t have;
    if (lh_ptr_is_null(self->bar))
    {
        return;
    }
    bar = lh_ptr_rcast(lh_entity_2d_t, self->bar);
    have = lh_entity_2d_get_size(bar);
    if (lh_math_vec2_get_x(lh_addr_of(size)) != lh_math_vec2_get_x(lh_addr_of(have)) ||
        lh_math_vec2_get_y(lh_addr_of(size)) != lh_math_vec2_get_y(lh_addr_of(have)))
    {
        lh_entity_2d_set_size(bar, size);
    }
    lh_entity_flex_layout(lh_ptr_rcast(lh_entity_t, self->bar));
}

lh_entity_button_t *
lh_entity_spin_side(lh_entity_t *parent, lh_entity_spin_t *spin, const lh_char_t *caption)
{
    lh_entity_t *const button = lh_entity_create(&lh_entity_button_class, parent);
    lh_entity_t *const label = lh_entity_create(&lh_entity_label_class, button);
    lh_entity_button_set_repeat(lh_ptr_rcast(lh_entity_button_t, button), lh_bool_true);
    lh_entity_add_flags(button, lh_entity_flags_own_background);
    lh_entity_flex_item_set_grow(button, 1);
    lh_entity_flex_item_set_basis(button, 0);
    lh_entity_add_flags(label, lh_entity_flags_event_bubble | lh_entity_flags_own_background);
    lh_entity_label_set_text(lh_ptr_rcast(lh_entity_label_t, label), caption);
    lh_entity_add_handler(button, lh_entity_spin_on_side, spin);
    return lh_ptr_rcast(lh_entity_button_t, button);
}

lh_void
lh_entity_spin_construct(lh_entity_t *self)
{
    lh_entity_spin_t *const spin = lh_ptr_rcast(lh_entity_spin_t, self);
    lh_entity_t *host;
    lh_entity_range_reset(lh_addr_of(spin->range));
    spin->font = lh_ui_font_get_default();
    spin->step = 1;
    lh_entity_add_flags(self, lh_entity_flags_own_background);
    spin->bar = lh_ptr_rcast(lh_entity_flex_t,
                             lh_entity_create(lh_addr_of(lh_entity_flex_class), self));
    lh_entity_add_flags(lh_ptr_rcast(lh_entity_t, spin->bar), lh_entity_flags_own_background);
    lh_entity_flex_set_align(spin->bar, LH_ENTITY_FLEX_STRETCH);
    spin->minus = lh_entity_spin_side(lh_ptr_rcast(lh_entity_t, spin->bar), spin, "-");
    host = lh_entity_create(lh_addr_of(lh_entity_2d_class), lh_ptr_rcast(lh_entity_t, spin->bar));
    lh_entity_add_flags(host, lh_entity_flags_own_background);
    lh_entity_flex_item_set_grow(host, 1);
    lh_entity_flex_item_set_basis(host, 0);
    spin->value = lh_ptr_rcast(lh_entity_label_t, lh_entity_create(&lh_entity_label_class, host));
    lh_entity_add_flags(lh_ptr_rcast(lh_entity_t, spin->value), lh_entity_flags_own_background);
    spin->plus = lh_entity_spin_side(lh_ptr_rcast(lh_entity_t, spin->bar), spin, "+");
    lh_entity_spin_show(spin);
}

lh_void
lh_entity_spin_paint(const lh_entity_spin_t *self, lh_ui_canvas_t *canvas)
{
    const lh_entity_2d_t *const box = lh_ptr_rcast(const lh_entity_2d_t, self);
    const lh_ui_style_t *const style = lh_entity_2d_get_style(box);
    const lh_math_rect_t bounds = lh_entity_2d_get_screen_bounds(box);
    const lh_math_mat4_t world = lh_entity_2d_get_world_matrix(box);
    const lh_math_vec4_t origin = lh_math_mat4_get_column(lh_addr_of(world), 3);
    lh_ui_color_t color;
    if (lh_ptr_is_null(style))
    {
        return;
    }
    color = lh_ui_style_get_bg_color(style);
    if (color.a == 0U)
    {
        return;
    }
    lh_ui_canvas_set_draw_z(canvas, lh_math_vec4_get_z(lh_addr_of(origin)));
    lh_ui_canvas_fill_round(canvas, bounds, lh_entity_range_cap(lh_addr_of(bounds)), color);
}

lh_void
lh_entity_spin_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_spin_t *const spin = lh_ptr_rcast(lh_entity_spin_t, self);
    const lh_entity_2d_t *const box = lh_ptr_rcast(const lh_entity_2d_t, self);
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_DRAW)
    {
        return;
    }
    lh_entity_spin_seat(spin);
    lh_entity_spin_paint(spin, lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)));
    lh_entity_spin_dress(self, lh_entity_2d_get_style(box), spin->font);
    lh_entity_spin_show(spin);
}

const lh_entity_class_t lh_entity_spin_class =
    lh_entity_class_initializer(lh_addr_of(lh_entity_2d_class), sizeof(lh_entity_spin_t),
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
    lh_entity_spin_dress(lh_ptr_rcast(lh_entity_t, self),
                         lh_entity_2d_get_style(lh_ptr_rcast(const lh_entity_2d_t, self)), font);
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}


lh_entity_range_value_defs(lh_entity_spin, lh_entity_spin_t)
