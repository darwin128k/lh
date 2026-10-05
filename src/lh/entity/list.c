#include <lh/entity/list.h>
#include <lh/assert.h>
#include <lh/bit.h>
#include <lh/cast/static.h>
#include <lh/entity.h>
#include <lh/entity/button.h>
#include <lh/entity/flex.h>
#include <lh/entity/group.h>
#include <lh/entity/label.h>
#include <lh/entity/screen.h>
#include <lh/null.h>
#include <lh/util/numeric.h>
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_entity_label_t *
lh_entity_list_label(lh_entity_t *button)
{
    lh_entity_foreach_child(child, button)
    {
        return lh_ptr_rcast(lh_entity_label_t,
                            lh_entity_cast(child, lh_addr_of(lh_entity_label_class)));
    }
    return lh_null;
}

lh_void
lh_entity_list_dress(lh_entity_list_t *self)
{
    const lh_entity_2d_t *const box = lh_ptr_rcast(const lh_entity_2d_t, self);
    const lh_ui_style_t *const style = lh_entity_2d_get_style(box);
    const lh_math_vec2_t size = lh_entity_2d_get_size(box);
    const lh_int_t height = lh_cast_static(lh_int_t, lh_math_vec2_get_y(lh_addr_of(size)));
    const lh_int_t room = self->row > 0 && height >= self->row ? height / self->row : 0;
    lh_int_t i;
    if (lh_ptr_is_set(style))
    {
        const lh_ui_color_t ink = lh_ui_style_get_text_color(style);
        const lh_uint_t argb = lh_ui_color_to_argb(ink);
        const lh_uint_t channel = lh_cast_static(lh_uint_t, lh_type_bits(lh_byte_t));
        const lh_uint_t alpha_at = channel + channel + channel;
        const lh_uint_t rgb =
            lh_bit_not(lh_bit_shl(lh_cast_static(lh_uint_t, lh_numeric_limit_umax(lh_byte_t)),
                                  alpha_at));
        lh_ui_style_set_bg_color(lh_addr_of(self->marked),
                                 lh_ui_color_from_argb(lh_bit_or(lh_bit_shl(lh_cast_static(lh_uint_t, 48),
                                                                            alpha_at),
                                                                 lh_bit_and(argb, rgb))));
        lh_ui_style_set_text_color(lh_addr_of(self->marked), ink);
    }
    for (i = 0; i < self->count; ++i)
    {
        lh_entity_t *const button = lh_ptr_rcast(lh_entity_t, self->rows[i]);
        lh_entity_label_t *label;
        const lh_ui_style_t *face;
        const lh_bool_t shown =
            room > 0 && i >= self->origin && i < self->origin + room && lh_ptr_is_set(self->items[i]);
        if (lh_ptr_is_null(button))
        {
            continue;
        }
        label = lh_entity_list_label(button);
        if (shown)
        {
            lh_entity_clear_flags(button, lh_entity_flags_hidden);
        }
        else if (room > 0)
        {
            lh_entity_add_flags(button, lh_entity_flags_hidden);
        }
        face = self->marks[i] && lh_ptr_is_set(style) ? lh_addr_of(self->marked) : style;
        if (lh_ptr_is_set(face) &&
            lh_entity_2d_get_style(lh_ptr_rcast(lh_entity_2d_t, button)) != face)
        {
            lh_entity_2d_set_style(lh_ptr_rcast(lh_entity_2d_t, button), face);
        }
        if (lh_ptr_is_null(label))
        {
            continue;
        }
        if (lh_ptr_is_set(face) && lh_entity_2d_get_style(lh_ptr_rcast(lh_entity_2d_t, label)) != face)
        {
            lh_entity_2d_set_style(lh_ptr_rcast(lh_entity_2d_t, label), face);
        }
        if (lh_entity_label_get_font(label) != self->font)
        {
            lh_entity_label_set_font(label, self->font);
        }
        if (lh_entity_label_get_text(label) != self->items[i])
        {
            lh_entity_label_set_text(label, self->items[i]);
        }
    }
}

lh_void
lh_entity_list_set_on(lh_entity_list_t *self, lh_int_t index)
{
    lh_int_t i;
    lh_assert_runtime_ref(self);
    if (index < 0 || index >= self->count)
    {
        return;
    }
    if (self->mode == LH_ENTITY_GROUP_MANY)
    {
        self->marks[index] = self->marks[index] ? 0 : 1;
    }
    else
    {
        for (i = 0; i < self->count; ++i)
        {
            self->marks[i] = i == index ? 1 : 0;
        }
    }
    lh_entity_list_dress(self);
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

lh_void
lh_entity_list_on_row(lh_entity_event_t *event, lh_ptr user)
{
    lh_entity_list_t *const list = lh_ptr_rcast(lh_entity_list_t, user);
    lh_entity_t *const target = lh_entity_event_get_target(event);
    lh_int_t i;
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_CLICKED || lh_ptr_is_null(list))
    {
        return;
    }
    for (i = 0; i < list->count; ++i)
    {
        if (target == lh_ptr_rcast(lh_entity_t, list->rows[i]))
        {
            lh_entity_list_set_on(list, i);
            lh_entity_send_event(lh_ptr_rcast(lh_entity_t, list), LH_ENTITY_EVENT_CLICKED,
                                 lh_entity_event_get_param(event));
            return;
        }
    }
}

lh_entity_t *
lh_entity_list_make(lh_entity_list_t *self, lh_int_t index)
{
    lh_entity_t *button;
    lh_entity_t *label;
    if (lh_ptr_is_set(self->rows[index]))
    {
        return lh_ptr_rcast(lh_entity_t, self->rows[index]);
    }
    button = lh_entity_create(lh_addr_of(lh_entity_button_class), lh_ptr_rcast(lh_entity_t, self));
    label = lh_entity_create(lh_addr_of(lh_entity_label_class), button);
    lh_entity_add_flags(label, lh_entity_flags_event_bubble | lh_entity_flags_own_background);
    lh_entity_2d_set_size(lh_ptr_rcast(lh_entity_2d_t, button),
                          lh_math_vec2_make(0.0f, (lh_float_t)self->row));
    lh_entity_add_flags(button, lh_entity_flags_own_background);
    lh_entity_add_handler(button, lh_entity_list_on_row, self);
    self->rows[index] = lh_ptr_rcast(lh_entity_button_t, button);
    return button;
}

lh_void
lh_entity_list_construct(lh_entity_t *self)
{
    lh_entity_list_t *const list = lh_ptr_rcast(lh_entity_list_t, self);
    list->font = lh_ui_font_get_default();
    list->mode = LH_ENTITY_GROUP_ONE;
    list->row = 22;
    lh_entity_add_flags(self, lh_entity_flags_own_background);
    lh_entity_flex_set_direction(lh_ptr_rcast(lh_entity_flex_t, self), LH_ENTITY_FLEX_COLUMN);
    lh_entity_flex_set_align(lh_ptr_rcast(lh_entity_flex_t, self), LH_ENTITY_FLEX_STRETCH);
}

lh_void
lh_entity_list_paint(lh_entity_list_t *self, lh_ui_canvas_t *canvas)
{
    const lh_entity_2d_t *const box = lh_ptr_rcast(const lh_entity_2d_t, self);
    const lh_ui_style_t *const style = lh_entity_2d_get_style(box);
    const lh_math_mat4_t world = lh_entity_2d_get_world_matrix(box);
    const lh_math_vec4_t origin = lh_math_mat4_get_column(lh_addr_of(world), 3);
    const lh_math_rect_t bounds = lh_entity_2d_get_screen_bounds(box);
    const lh_int_t radius = self->radius;
    const lh_math_rect_t clip = lh_ui_canvas_get_clip(canvas);
    lh_int_t i;
    if (lh_ptr_is_null(style))
    {
        return;
    }
    lh_ui_canvas_set_draw_z(canvas, lh_math_vec4_get_z(lh_addr_of(origin)));
    lh_ui_canvas_fill_round(canvas, bounds, radius, lh_ui_style_get_bg_color(style));
    for (i = 0; i < self->count; ++i)
    {
        const lh_entity_t *const row = lh_ptr_rcast(const lh_entity_t, self->rows[i]);
        lh_math_rect_t row_bounds;
        lh_math_rect_t kept;
        if (lh_ptr_is_null(row) || self->marks[i] == 0 ||
            lh_entity_has_flags(row, lh_entity_flags_hidden))
        {
            continue;
        }
        row_bounds = lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, row));
        kept = lh_math_rect_intersection(lh_addr_of(clip), lh_addr_of(row_bounds));
        lh_ui_canvas_set_clip(canvas, kept);
        lh_ui_canvas_fill_round(canvas, bounds, radius,
                                lh_ui_style_get_bg_color(lh_addr_of(self->marked)));
        lh_ui_canvas_set_clip(canvas, clip);
    }
}

lh_void
lh_entity_list_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_list_t *const list = lh_ptr_rcast(lh_entity_list_t, self);
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_DRAW)
    {
        return;
    }
    lh_entity_list_dress(list);
    lh_entity_list_paint(list, lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)));
}

const lh_entity_class_t lh_entity_list_class =
    lh_entity_class_initializer(lh_addr_of(lh_entity_flex_class), sizeof(lh_entity_list_t),
                                lh_entity_list_construct, lh_null, lh_entity_list_on_event);

lh_int_t
lh_entity_list_get_count(const lh_entity_list_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_entity_list_set_item(lh_entity_list_t *self, lh_int_t index, const lh_char_t *text)
{
    lh_int_t i;
    lh_assert_runtime_ref(self);
    if (index < 0 || index >= LH_ENTITY_LIST_LIMIT)
    {
        return;
    }
    if (lh_ptr_is_null(text))
    {
        if (index < self->count && lh_ptr_is_set(self->rows[index]))
        {
            lh_entity_delete(lh_ptr_rcast(lh_entity_t, self->rows[index]));
        }
        for (i = index; i + 1 < self->count; ++i)
        {
            self->items[i] = self->items[i + 1];
            self->marks[i] = self->marks[i + 1];
            self->rows[i] = self->rows[i + 1];
        }
        if (self->count > index)
        {
            self->count -= 1;
            self->items[self->count] = lh_null;
            self->marks[self->count] = 0;
            self->rows[self->count] = lh_null;
        }
    }
    else
    {
        self->items[index] = text;
        lh_entity_list_make(self, index);
        if (index >= self->count)
        {
            self->count = index + 1;
        }
    }
    lh_entity_list_dress(self);
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

const lh_char_t *
lh_entity_list_get_item(const lh_entity_list_t *self, lh_int_t index)
{
    lh_assert_runtime_ref(self);
    if (index < 0 || index >= self->count)
    {
        return lh_null;
    }
    return self->items[index];
}

lh_bool_t
lh_entity_list_is_on(const lh_entity_list_t *self, lh_int_t index)
{
    lh_assert_runtime_ref(self);
    if (index < 0 || index >= self->count)
    {
        return lh_bool_false;
    }
    return self->marks[index] ? lh_bool_true : lh_bool_false;
}

lh_void
lh_entity_list_set_mode(lh_entity_list_t *self, lh_int_t mode)
{
    lh_assert_runtime_ref(self);
    self->mode = lh_entity_group_normalize_mode(mode);
}

lh_void
lh_entity_list_set_origin(lh_entity_list_t *self, lh_int_t origin)
{
    lh_assert_runtime_ref(self);
    self->origin = origin < 0 ? 0 : origin;
    lh_entity_list_dress(self);
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

lh_void
lh_entity_list_set_font(lh_entity_list_t *self, const lh_ui_font_t *font)
{
    lh_assert_runtime_ref(self);
    self->font = font;
    lh_entity_list_dress(self);
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

lh_int_t
lh_entity_list_get_row(const lh_entity_list_t *self)
{
    lh_assert_runtime_ref(self);
    return self->row;
}

lh_int_t
lh_entity_list_get_radius(const lh_entity_list_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radius;
}

lh_void
lh_entity_list_set_radius(lh_entity_list_t *self, lh_int_t radius)
{
    lh_assert_runtime_ref(self);
    if (radius < 0)
    {
        radius = 0;
    }
    if (self->radius == radius)
    {
        return;
    }
    self->radius = radius;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}
