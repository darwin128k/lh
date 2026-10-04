#include <lh/entity/keys.h>
#include <lh/assert.h>
#include <lh/entity/flex.h>
#include <lh/entity/key.h>
#include <lh/entity/label.h>
#include <lh/entity/screen.h>
#include <lh/memory/tree.h>
#include <lh/null.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

struct lh_entity_keys_code
{
    lh_uint_t code;
};

lh_char_t *
lh_entity_keys_copy(lh_entity_t *owner, const lh_char_t *text)
{
    lh_int_t n = 0;
    lh_int_t i;
    lh_char_t *stored;
    if (lh_ptr_is_null(text))
    {
        text = "";
    }
    while (text[n] != 0 && n < 15)
    {
        n += 1;
    }
    stored = lh_ptr_rcast(lh_char_t, lh_memory_tree_alloc_child(owner, (lh_usize_t)n + 1U));
    if (lh_ptr_is_null(stored))
    {
        return lh_null;
    }
    for (i = 0; i < n; ++i)
    {
        stored[i] = text[i];
    }
    stored[n] = 0;
    return stored;
}

lh_void
lh_entity_keys_on_click(lh_entity_event_t *event, lh_ptr user)
{
    const struct lh_entity_keys_code *const binding =
        lh_ptr_rcast(const struct lh_entity_keys_code, user);
    lh_entity_screen_t *screen;
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_CLICKED || lh_ptr_is_null(binding))
    {
        return;
    }
    screen = lh_entity_cast(lh_entity_get_root(lh_entity_event_get_target(event)),
                            lh_addr_of(lh_entity_screen_class));
    if (lh_ptr_is_set(screen))
    {
        lh_entity_screen_send_key(screen, binding->code);
    }
}

lh_void
lh_entity_keys_construct(lh_entity_t *self)
{
    lh_entity_keys_t *const keys = lh_ptr_rcast(lh_entity_keys_t, self);
    lh_entity_add_flags(self, lh_entity_flags_own_background);
    keys->font = lh_ui_font_get_default();
    keys->key_width = 26;
    keys->key_height = 26;
    keys->gap = 4;
    keys->radius = 6;
    lh_entity_flex_set_direction(lh_ptr_rcast(lh_entity_flex_t, self), LH_ENTITY_FLEX_COLUMN);
    lh_entity_flex_set_gap(lh_ptr_rcast(lh_entity_flex_t, self), keys->gap);
}

const lh_entity_class_t lh_entity_keys_class =
    lh_entity_class_initializer(lh_addr_of(lh_entity_flex_class), sizeof(lh_entity_keys_t),
                                lh_entity_keys_construct, lh_null, lh_null);

lh_void
lh_entity_keys_set_font(lh_entity_keys_t *self, const lh_ui_font_t *font)
{
    lh_assert_runtime_ref(self);
    self->font = font;
}

lh_void
lh_entity_keys_set_key_size(lh_entity_keys_t *self, lh_int_t width, lh_int_t height)
{
    lh_assert_runtime_ref(self);
    self->key_width = width < 1 ? 1 : width;
    self->key_height = height < 1 ? 1 : height;
}

lh_void
lh_entity_keys_set_gap(lh_entity_keys_t *self, lh_int_t gap)
{
    lh_assert_runtime_ref(self);
    self->gap = gap < 0 ? 0 : gap;
    lh_entity_flex_set_gap(lh_ptr_rcast(lh_entity_flex_t, self), self->gap);
}

lh_void
lh_entity_keys_set_radius(lh_entity_keys_t *self, lh_int_t radius)
{
    lh_assert_runtime_ref(self);
    self->radius = radius < 0 ? 0 : radius;
}

lh_void
lh_entity_keys_set_pressed_style(lh_entity_keys_t *self, const lh_ui_style_t *style)
{
    lh_assert_runtime_ref(self);
    self->pressed = style;
}

lh_void
lh_entity_keys_break(lh_entity_keys_t *self)
{
    lh_entity_t *row;
    lh_assert_runtime_ref(self);
    row = lh_entity_create(lh_addr_of(lh_entity_flex_class), lh_ptr_rcast(lh_entity_t, self));
    lh_entity_flex_set_justify(lh_ptr_rcast(lh_entity_flex_t, row), LH_ENTITY_FLEX_CENTER);
    lh_entity_flex_set_align(lh_ptr_rcast(lh_entity_flex_t, row), LH_ENTITY_FLEX_CENTER);
    lh_entity_flex_set_gap(lh_ptr_rcast(lh_entity_flex_t, row), self->gap);
    self->row = row;
}

lh_entity_button_t *
lh_entity_keys_add(lh_entity_keys_t *self, const lh_char_t *text, lh_uint_t code)
{
    const lh_ui_style_t *style;
    lh_entity_t *button;
    lh_entity_t *label;
    lh_char_t *stored;
    struct lh_entity_keys_code *binding;
    lh_assert_runtime_ref(self);
    if (lh_ptr_is_null(self->row))
    {
        lh_entity_keys_break(self);
    }
    style = lh_entity_2d_get_style(lh_ptr_rcast(const lh_entity_2d_t, self));
    button = lh_entity_create(&lh_entity_button_class, self->row);
    lh_entity_2d_set_style(lh_ptr_rcast(lh_entity_2d_t, button), style);
    lh_entity_2d_set_size(lh_ptr_rcast(lh_entity_2d_t, button),
                          lh_math_vec2_make((lh_float_t)self->key_width,
                                            (lh_float_t)self->key_height));
    lh_entity_button_set_pressed_style(lh_ptr_rcast(lh_entity_button_t, button), self->pressed);
    lh_entity_button_set_radius(lh_ptr_rcast(lh_entity_button_t, button), self->radius);
    label = lh_entity_create(&lh_entity_label_class, button);
    lh_entity_add_flags(label, lh_entity_flags_event_bubble | lh_entity_flags_own_background);
    lh_entity_2d_set_style(lh_ptr_rcast(lh_entity_2d_t, label), style);
    lh_entity_label_set_font(lh_ptr_rcast(lh_entity_label_t, label), self->font);
    stored = lh_entity_keys_copy(button, text);
    if (lh_ptr_is_set(stored))
    {
        lh_entity_label_set_text(lh_ptr_rcast(lh_entity_label_t, label), stored);
    }
    binding = lh_ptr_rcast(struct lh_entity_keys_code,
                           lh_memory_tree_alloc_child(button, sizeof(struct lh_entity_keys_code)));
    if (lh_ptr_is_set(binding))
    {
        binding->code = code;
        lh_entity_add_handler(button, lh_entity_keys_on_click, binding);
    }
    return lh_ptr_rcast(lh_entity_button_t, button);
}
