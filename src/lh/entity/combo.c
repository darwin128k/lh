#include <lh/entity/combo.h>
#include <lh/assert.h>
#include <lh/entity.h>
#include <lh/entity/screen.h>
#include <lh/null.h>
#include <lh/ui/font.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_combo_open(lh_entity_combo_t *self, lh_bool_t open);

lh_void
lh_entity_combo_on_list(lh_entity_event_t *event, lh_ptr user)
{
    lh_entity_combo_t *const combo = lh_ptr_rcast(lh_entity_combo_t, user);
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_CLICKED || lh_ptr_is_null(combo) ||
        lh_ptr_is_null(combo->list))
    {
        return;
    }
    lh_entity_combo_open(combo, lh_bool_false);
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, combo));
}

lh_void
lh_entity_combo_construct(lh_entity_t *self)
{
    lh_entity_combo_t *const combo = lh_ptr_rcast(lh_entity_combo_t, self);
    combo->font = lh_ui_font_get_default();
    combo->list =
        lh_ptr_rcast(lh_entity_list_t, lh_entity_create(&lh_entity_list_class, self));
    lh_entity_add_flags(lh_ptr_rcast(lh_entity_t, combo->list), lh_entity_flags_hidden);
    lh_entity_add_flags(self, lh_entity_flags_overflow_visible);
    lh_entity_add_handler(lh_ptr_rcast(lh_entity_t, combo->list), lh_entity_combo_on_list, self);
}

lh_void
lh_entity_combo_destruct(lh_entity_t *self)
{
    lh_entity_combo_t *const combo = lh_ptr_rcast(lh_entity_combo_t, self);
    lh_entity_t *const list = lh_ptr_rcast(lh_entity_t, combo->list);
    if (lh_ptr_is_set(list) && lh_entity_get_parent(list) != self)
    {
        lh_entity_set_parent(list, self);
    }
}

lh_void
lh_entity_combo_place(lh_entity_combo_t *self)
{
    lh_entity_t *const list = lh_ptr_rcast(lh_entity_t, self->list);
    const lh_entity_2d_t *const box = lh_ptr_rcast(const lh_entity_2d_t, self);
    const lh_math_vec2_t size = lh_entity_2d_get_size(box);
    const lh_math_vec2_t at = lh_entity_2d_get_position(box);
    const lh_int_t rows = lh_entity_list_get_count(self->list);
    const lh_int_t height = rows * lh_entity_list_get_row(self->list);
    lh_float_t x = 0.0f;
    lh_float_t y = lh_math_vec2_get_y(lh_addr_of(size));
    lh_entity_list_set_font(self->list, self->font);
    lh_entity_2d_set_style(lh_ptr_rcast(lh_entity_2d_t, list), lh_entity_2d_get_style(box));
    if (lh_entity_get_parent(list) == lh_entity_get_parent(lh_ptr_rcast(lh_entity_t, self)))
    {
        x = lh_math_vec2_get_x(lh_addr_of(at));
        y += lh_math_vec2_get_y(lh_addr_of(at));
    }
    lh_entity_2d_set_position(lh_ptr_rcast(lh_entity_2d_t, list), lh_math_vec2_make(x, y));
    lh_entity_2d_set_size(lh_ptr_rcast(lh_entity_2d_t, list),
                          lh_math_vec2_make(lh_math_vec2_get_x(lh_addr_of(size)), (lh_float_t)height));
}

lh_void
lh_entity_combo_open(lh_entity_combo_t *self, lh_bool_t open)
{
    lh_entity_t *const list = lh_ptr_rcast(lh_entity_t, self->list);
    lh_entity_t *const host = lh_entity_get_parent(lh_ptr_rcast(lh_entity_t, self));
    if (open)
    {
        if (lh_ptr_is_set(host))
        {
            lh_entity_set_parent(list, host);
        }
        lh_entity_combo_place(self);
        lh_entity_clear_flags(list, lh_entity_flags_hidden);
    }
    else
    {
        lh_entity_invalidate(list);
        lh_entity_add_flags(list, lh_entity_flags_hidden);
        lh_entity_set_parent(list, lh_ptr_rcast(lh_entity_t, self));
        return;
    }
    lh_entity_invalidate(list);
}

lh_void
lh_entity_combo_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_combo_t *const combo = lh_ptr_rcast(lh_entity_combo_t, self);
    const lh_uint_t code = lh_entity_event_get_code(event);
    const lh_ui_style_t *style;
    lh_int_t i;
    const lh_char_t *text;
    if (code == LH_ENTITY_EVENT_POINTER_UP)
    {
        const lh_math_vec2_t *const point =
            lh_ptr_rcast(const lh_math_vec2_t, lh_entity_event_get_param(event));
        if (lh_ptr_is_null(point) || lh_ptr_is_null(combo->list) ||
            !lh_entity_2d_contains(lh_ptr_rcast(const lh_entity_2d_t, self), *point))
        {
            return;
        }
        lh_entity_combo_open(combo, lh_entity_has_flags(lh_ptr_rcast(lh_entity_t, combo->list),
                                                      lh_entity_flags_hidden));
        lh_entity_invalidate(self);
        return;
    }
    if (code != LH_ENTITY_EVENT_DRAW)
    {
        return;
    }
    style = lh_entity_2d_get_style(lh_ptr_rcast(const lh_entity_2d_t, self));
    if (lh_ptr_is_null(style) || lh_ptr_is_null(combo->font))
    {
        return;
    }
    text = lh_null;
    for (i = 0; i < lh_entity_list_get_count(combo->list); ++i)
    {
        if (lh_entity_list_is_on(combo->list, i))
        {
            text = lh_entity_list_get_item(combo->list, i);
            break;
        }
    }
    {
        const lh_math_rect_t bounds =
            lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
        lh_ui_canvas_t *const canvas =
            lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event));
        lh_ui_canvas_fill_rect(canvas, bounds, lh_ui_style_get_bg_color(style));
        if (lh_ptr_is_set(text))
        {
            lh_ui_font_draw(combo->font, canvas, lh_math_rect_get_x(lh_addr_of(bounds)) + 6,
                            lh_math_rect_get_y(lh_addr_of(bounds)) + 4, text,
                            lh_ui_style_get_text_color(style));
        }
    }
}

const lh_entity_class_t lh_entity_combo_class =
    lh_entity_class_initializer(lh_addr_of(lh_entity_2d_class), sizeof(lh_entity_combo_t),
                                lh_entity_combo_construct, lh_entity_combo_destruct,
                                lh_entity_combo_on_event);

lh_entity_list_t *
lh_entity_combo_get_list(lh_entity_combo_t *self)
{
    lh_assert_runtime_ref(self);
    return self->list;
}

lh_void
lh_entity_combo_set_item(lh_entity_combo_t *self, lh_int_t index, const lh_char_t *text)
{
    lh_int_t before;
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_null(self->list))
    {
        return;
    }
    before = lh_entity_list_get_count(self->list);
    lh_entity_list_set_item(self->list, index, text);
    lh_entity_list_set_font(self->list, self->font);
    if (before == 0 && lh_entity_list_get_count(self->list) > 0)
    {
        lh_entity_list_set_on(self->list, 0);
    }
}
