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
lh_entity_combo_on_list(lh_entity_event_t *event, lh_ptr user)
{
    lh_entity_combo_t *const combo = lh_ptr_rcast(lh_entity_combo_t, user);
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_CLICKED || lh_ptr_is_null(combo) ||
        lh_ptr_is_null(combo->list))
    {
        return;
    }
    lh_entity_add_flags(lh_ptr_rcast(lh_entity_t, combo->list), lh_entity_flags_hidden);
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, combo));
}

lh_void
lh_entity_combo_construct(lh_entity_t *self)
{
    lh_entity_combo_t *const combo = lh_ptr_rcast(lh_entity_combo_t, self);
    combo->font = lh_ui_font_get_default();
    combo->list =
        (lh_entity_list_t *)lh_entity_create(&lh_entity_list_class, self);
    lh_entity_add_flags(lh_ptr_rcast(lh_entity_t, combo->list), lh_entity_flags_hidden);
    lh_entity_add_flags(self, lh_entity_flags_overflow_visible);
    lh_entity_add_handler(lh_ptr_rcast(lh_entity_t, combo->list), lh_entity_combo_on_list, self);
}

lh_void
lh_entity_combo_place(lh_entity_combo_t *self)
{
    const lh_math_vec2_t size = lh_entity_2d_get_size(lh_ptr_rcast(const lh_entity_2d_t, self));
    const lh_int_t rows = lh_entity_list_get_count(self->list);
    const lh_int_t height = rows * lh_entity_list_get_row(self->list);
    lh_entity_list_set_font(self->list, self->font);
    lh_entity_2d_set_style(lh_ptr_rcast(lh_entity_2d_t, self->list),
                           lh_entity_2d_get_style(lh_ptr_rcast(const lh_entity_2d_t, self)));
    lh_entity_2d_set_position(lh_ptr_rcast(lh_entity_2d_t, self->list),
                              lh_math_vec2_make(0.0f, lh_math_vec2_get_y(lh_addr_of(size))));
    lh_entity_2d_set_size(lh_ptr_rcast(lh_entity_2d_t, self->list),
                          lh_math_vec2_make(lh_math_vec2_get_x(lh_addr_of(size)), (lh_float_t)height));
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
        if (lh_entity_has_flags(lh_ptr_rcast(lh_entity_t, combo->list), lh_entity_flags_hidden))
        {
            lh_entity_combo_place(combo);
            lh_entity_clear_flags(lh_ptr_rcast(lh_entity_t, combo->list), lh_entity_flags_hidden);
        }
        else
        {
            lh_entity_add_flags(lh_ptr_rcast(lh_entity_t, combo->list), lh_entity_flags_hidden);
        }
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
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_combo_t),
                                lh_entity_combo_construct, lh_null, lh_entity_combo_on_event);

lh_entity_list_t *
lh_entity_combo_get_list(lh_entity_combo_t *self)
{
    lh_assert_runtime_ref(self);
    return self->list;
}

lh_void
lh_entity_combo_set_item(lh_entity_combo_t *self, lh_int_t index, const lh_char_t *text)
{
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_set(self->list))
    {
        lh_entity_list_set_item(self->list, index, text);
        lh_entity_list_set_font(self->list, self->font);
    }
}
