#include <lh/entity/tabs.h>
#include <lh/assert.h>
#include <lh/entity/button.h>
#include <lh/entity/flex.h>
#include <lh/entity/label.h>
#include <lh/entity/screen.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_entity_label_t *
lh_entity_tabs_label(lh_entity_t *button)
{
    lh_entity_foreach_child(child, button)
    {
        return lh_ptr_rcast(lh_entity_label_t,
                            lh_entity_cast(child, lh_addr_of(lh_entity_label_class)));
    }
    return lh_null;
}

lh_void
lh_entity_tabs_on_pick(lh_entity_event_t *event, lh_ptr user)
{
    lh_entity_tabs_t *const tabs = lh_ptr_rcast(lh_entity_tabs_t, user);
    lh_entity_t *const target = lh_entity_event_get_target(event);
    lh_int_t index = 0;
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_CLICKED || lh_ptr_is_null(tabs))
    {
        return;
    }
    lh_entity_foreach_child(child, lh_ptr_rcast(lh_entity_t, tabs))
    {
        if (child == target)
        {
            lh_entity_tabs_set_index(tabs, index);
            lh_entity_send_event(lh_ptr_rcast(lh_entity_t, tabs), LH_ENTITY_EVENT_CLICKED,
                                 lh_entity_event_get_param(event));
            return;
        }
        index += 1;
    }
}

lh_void
lh_entity_tabs_dress(lh_entity_tabs_t *self)
{
    const lh_ui_style_t *const style = self->tab_style;
    lh_int_t index = 0;
    if (lh_ptr_is_null(style))
    {
        return;
    }
    lh_ui_style_set_text_color(lh_addr_of(self->on_title), lh_ui_style_get_bg_color(style));
    lh_ui_style_set_bg_color(lh_addr_of(self->on_title), lh_ui_style_get_text_color(style));
    lh_entity_foreach_child(child, lh_ptr_rcast(lh_entity_t, self))
    {
        lh_entity_label_t *const label = lh_entity_tabs_label(child);
        const lh_ui_style_t *const wanted = index == self->index ? lh_addr_of(self->on_title) : style;
        if (lh_ptr_is_set(label) &&
            lh_entity_2d_get_style(lh_ptr_rcast(const lh_entity_2d_t, label)) != wanted)
        {
            lh_entity_2d_set_style(lh_ptr_rcast(lh_entity_2d_t, label), wanted);
        }
        index += 1;
    }
}

lh_void
lh_entity_tabs_paint(lh_entity_tabs_t *self, lh_ui_canvas_t *canvas)
{
    const lh_entity_2d_t *const box = lh_ptr_rcast(const lh_entity_2d_t, self);
    const lh_ui_style_t *const style = self->tab_style;
    const lh_math_mat4_t world = lh_entity_2d_get_world_matrix(box);
    const lh_math_vec4_t origin = lh_math_mat4_get_column(lh_addr_of(world), 3);
    const lh_math_rect_t bounds = lh_entity_2d_get_screen_bounds(box);
    const lh_int_t radius = self->radius;
    const lh_math_rect_t clip = lh_ui_canvas_get_clip(canvas);
    lh_int_t index = 0;
    if (lh_ptr_is_null(style))
    {
        return;
    }
    lh_ui_canvas_set_draw_z(canvas, lh_math_vec4_get_z(lh_addr_of(origin)));
    lh_ui_canvas_fill_round(canvas, bounds, radius, lh_ui_style_get_bg_color(style));
    lh_entity_foreach_child(child, lh_ptr_rcast(lh_entity_t, self))
    {
        lh_math_rect_t row;
        lh_math_rect_t kept;
        if (index != self->index)
        {
            index += 1;
            continue;
        }
        row = lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, child));
        kept = lh_math_rect_intersection(lh_addr_of(clip), lh_addr_of(row));
        lh_ui_canvas_set_clip(canvas, kept);
        lh_ui_canvas_fill_round(canvas, bounds, radius, lh_ui_style_get_text_color(style));
        lh_ui_canvas_set_clip(canvas, clip);
        index += 1;
    }
}

lh_void
lh_entity_tabs_construct(lh_entity_t *self)
{
    lh_entity_tabs_t *const tabs = lh_ptr_rcast(lh_entity_tabs_t, self);
    tabs->font = lh_ui_font_get_default();
    lh_entity_add_flags(self, lh_entity_flags_own_background);
    lh_entity_flex_set_on(self, lh_bool_true);
    lh_entity_flex_set_align(self, LH_ENTITY_FLEX_STRETCH);
}

lh_void
lh_entity_tabs_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_tabs_t *const tabs = lh_ptr_rcast(lh_entity_tabs_t, self);
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_DRAW)
    {
        return;
    }
    lh_entity_tabs_dress(tabs);
    lh_entity_tabs_paint(tabs, lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)));
}

const lh_entity_class_t lh_entity_tabs_class =
    lh_entity_class_initializer(lh_addr_of(lh_entity_2d_class), sizeof(lh_entity_tabs_t),
                                lh_entity_tabs_construct, lh_null, lh_entity_tabs_on_event);

lh_void
lh_entity_tabs_set_font(lh_entity_tabs_t *self, const lh_ui_font_t *font)
{
    lh_assert_runtime_ref(self);
    self->font = font;
}

lh_void
lh_entity_tabs_set_style(lh_entity_tabs_t *self, const lh_ui_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->tab_style = style;
}

lh_int_t
lh_entity_tabs_get_index(const lh_entity_tabs_t *self)
{
    lh_assert_runtime_ref(self);
    return self->index;
}

lh_void
lh_entity_tabs_set_index(lh_entity_tabs_t *self, lh_int_t index)
{
    lh_assert_runtime_ref(self);
    if (index < 0)
    {
        index = 0;
    }
    if (self->index == index)
    {
        return;
    }
    self->index = index;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}

lh_void
lh_entity_tabs_add(lh_entity_tabs_t *self, const lh_char_t *title)
{
    lh_entity_t *button;
    lh_entity_label_t *label;
    lh_assert_runtime_ref(self);
    button = lh_entity_create(&lh_entity_button_class, lh_ptr_rcast(lh_entity_t, self));
    lh_entity_add_flags(button, lh_entity_flags_own_background);
    lh_entity_flex_item_set_grow(button, 1);
    lh_entity_flex_item_set_basis(button, 0);
    label = lh_ptr_rcast(lh_entity_label_t, lh_entity_create(&lh_entity_label_class, button));
    lh_entity_add_flags(lh_ptr_rcast(lh_entity_t, label),
                        lh_entity_flags_event_bubble | lh_entity_flags_own_background);
    lh_entity_label_set_font(label, self->font);
    lh_entity_label_set_text(label, title);
    lh_entity_2d_set_style(lh_ptr_rcast(lh_entity_2d_t, label), self->tab_style);
    lh_entity_add_handler(button, lh_entity_tabs_on_pick, self);
}

lh_int_t
lh_entity_tabs_get_radius(const lh_entity_tabs_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radius;
}

lh_void
lh_entity_tabs_set_radius(lh_entity_tabs_t *self, lh_int_t radius)
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
