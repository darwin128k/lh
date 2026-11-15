/**
 * @file card.c
 * @brief A layer with a face and a line.
 */
#include <lh/gl/card.h>

#include <lh/core/assert/runtime.h>
#include <lh/gl/entity/class/initializer.h>

const lh_gl_entity_class_t *
lh_gl_card_get_class(void)
{
    static const lh_gl_entity_class_t card_class = lh_gl_entity_class_initializer(
        lh_gl_entity_class_kind_layer, lh_null, lh_gl_card_pick, lh_bool_false);

    return lh_addr_of(card_class);
}

lh_gl_entity_t *
lh_gl_card_get_entity_mut(lh_gl_card_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_gl_layer_get_entity_mut(lh_addr_of(self->layer));
}

lh_gl_layer_t *
lh_gl_card_get_layer_mut(lh_gl_card_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->layer);
}

const lh_gl_entity_t *
lh_gl_card_get_entity(const lh_gl_card_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_gl_layer_get_entity(lh_addr_of(self->layer));
}

lh_gl_scalar_t
lh_gl_card_get_radius(const lh_gl_card_t *self)
{
    lh_assert_runtime_ref(self);
    return self->radius;
}

lh_void
lh_gl_card_set_radius(lh_gl_card_t *self, lh_gl_scalar_t radius)
{
    lh_assert_runtime_ref(self);
    self->radius = radius < 0 ? 0 : radius;
    lh_gl_layer_set_radius(lh_addr_of(self->layer), lh_gl_card_get_radius(self));
    lh_gl_fill_set_radius(lh_addr_of(self->face), 0);
}

lh_bool_t
lh_gl_card_get_shown(const lh_gl_card_t *self)
{
    lh_assert_runtime_ref(self);
    return self->shown;
}

lh_void
lh_gl_card_set_shown(lh_gl_card_t *self, lh_bool_t shown)
{
    lh_assert_runtime_ref(self);
    self->shown = shown;
    lh_gl_fill_set_shown(lh_addr_of(self->face), shown);
    lh_gl_text_set_shown(lh_addr_of(self->label), shown);
}

lh_gl_text_t *
lh_gl_card_get_line_mut(lh_gl_card_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->label);
}

const lh_gl_text_t *
lh_gl_card_get_line(const lh_gl_card_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->label);
}

const lh_gl_color_t *
lh_gl_card_get_ink(const lh_gl_card_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_gl_text_get_color(lh_gl_card_get_line(self));
}

lh_void
lh_gl_card_set_ink(lh_gl_card_t *self, const lh_gl_color_t *ink)
{
    lh_assert_runtime_ref(self);
    lh_gl_text_set_color(lh_gl_card_get_line_mut(self), ink);
}

lh_void
lh_gl_card_init(lh_gl_card_t *self, lh_gl_rect_t rect, const lh_gl_color_t *color,
                const lh_gl_font_t *font, const lh_char_t *text)
{
    lh_gl_rect_t inner;
    lh_gl_scalar_t width = lh_gl_size_get_width(lh_gl_rect_get_size(lh_addr_of(rect)));
    lh_gl_scalar_t height = lh_gl_size_get_height(lh_gl_rect_get_size(lh_addr_of(rect)));

    lh_assert_runtime_ref(self);
    lh_gl_layer_init(lh_addr_of(self->layer), rect);
    lh_gl_entity_set_class(lh_gl_card_get_entity_mut(self), lh_gl_card_get_class());
    lh_gl_rect_init(lh_addr_of(inner), 0, 0, width, height);
    lh_gl_fill_init(lh_addr_of(self->face), inner, color);
    lh_gl_fill_set_target(lh_addr_of(self->face), lh_bool_false);
    lh_gl_text_init(lh_addr_of(self->label), inner, font, color, text);
    lh_gl_text_set_target(lh_addr_of(self->label), lh_bool_false);
    lh_gl_layer_add(lh_addr_of(self->layer), lh_gl_fill_get_entity_mut(lh_addr_of(self->face)));
    lh_gl_layer_add(lh_addr_of(self->layer), lh_gl_text_get_entity_mut(lh_addr_of(self->label)));
    lh_gl_card_set_radius(self, 0);
    lh_gl_card_set_shown(self, lh_bool_true);
}

lh_bool_t
lh_gl_card_pick(const lh_gl_entity_t *entity, lh_gl_point_t point, lh_gl_scalar_t slop)
{
    const lh_gl_card_t *card = lh_ptr_ccast(lh_gl_card_t, entity);

    if (lh_gl_card_get_shown(card) == lh_bool_false)
    {
        return lh_bool_false;
    }
    return lh_gl_entity_hit(entity, lh_gl_card_get_radius(card), point, slop);
}
