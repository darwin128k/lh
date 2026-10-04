#include <lh/entity/label.h>
#include <lh/assert.h>
#include <lh/float/round.h>
#include <lh/null.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_void
lh_entity_label_fit(lh_entity_label_t *self)
{
    const lh_math_vec2_t size =
        lh_ptr_is_set(self->font) ? lh_ui_font_measure(self->font, self->text)
                                  : lh_math_vec2_make(0.0f, 0.0f);
    lh_entity_2d_set_size(lh_ptr_rcast(lh_entity_2d_t, self), size);
}

lh_void
lh_entity_label_construct(lh_entity_t *self)
{
    lh_ptr_rcast(lh_entity_label_t, self)->font = lh_ui_font_get_default();
}

lh_void
lh_entity_label_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_label_t *const label = lh_ptr_rcast(lh_entity_label_t, self);
    const lh_ui_style_t *style;
    lh_ui_color_t color;
    lh_math_mat4_t world;
    lh_math_vec4_t origin;
    lh_math_rect_t clip;
    lh_math_rect_t bounds;
    lh_math_rect_t kept;

    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_DRAW)
    {
        return;
    }
    style = lh_entity_2d_get_style(lh_ptr_rcast(const lh_entity_2d_t, self));
    if (lh_ptr_is_null(style) || lh_ptr_is_null(label->font) || lh_ptr_is_null(label->text))
    {
        return;
    }
    color = lh_ui_style_get_text_color(style);
    if ((lh_ui_color_to_argb(color) >> 24) == 0U)
    {
        return;
    }

    world = lh_entity_2d_get_world_matrix(lh_ptr_rcast(const lh_entity_2d_t, self));
    origin = lh_math_mat4_get_column(lh_addr_of(world), 3);
    clip = lh_ui_canvas_get_clip(lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)));
    bounds = lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
    kept = lh_math_rect_intersection(lh_addr_of(clip), lh_addr_of(bounds));
    lh_ui_canvas_set_clip(lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)), kept);
    lh_ui_canvas_set_draw_z(lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)),
                            lh_math_vec4_get_z(lh_addr_of(origin)));
    lh_ui_font_draw(label->font, lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)),
                    lh_float_floor_to_int(lh_math_vec4_get_x(lh_addr_of(origin))),
                    lh_float_floor_to_int(lh_math_vec4_get_y(lh_addr_of(origin))), label->text,
                    color);
    lh_ui_canvas_set_clip(lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event)), clip);
}

const lh_entity_class_t lh_entity_label_class =
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_label_t),
                                lh_entity_label_construct, lh_null, lh_entity_label_on_event);

const lh_ui_font_t *
lh_entity_label_get_font(const lh_entity_label_t *self)
{
    lh_assert_runtime_ref(self);
    return self->font;
}

lh_void
lh_entity_label_set_font(lh_entity_label_t *self, const lh_ui_font_t *font)
{
    lh_assert_runtime_ref(self);
    self->font = font;
    lh_entity_label_fit(self);
}

const lh_char_t *
lh_entity_label_get_text(const lh_entity_label_t *self)
{
    lh_assert_runtime_ref(self);
    return self->text;
}

lh_void
lh_entity_label_set_text(lh_entity_label_t *self, const lh_char_t *text)
{
    lh_assert_runtime_ref(self);
    self->text = text;
    lh_entity_label_fit(self);
}
