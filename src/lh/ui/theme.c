#include <lh/ui/theme.h>
#include <lh/assert.h>
#include <lh/entity/button.h>
#include <lh/entity/label.h>
#include <lh/entity/screen.h>
#include <lh/util/ptr.h>

/* Neutrals for the two modes. The accent is the caller's primary, not one
   of these. */
#define LH_UI_THEME_DARK_SURFACE 0xFF1C1B1FU
#define LH_UI_THEME_DARK_TEXT 0xFFE6E1E5U
#define LH_UI_THEME_DARK_BUTTON 0xFF2B2930U
#define LH_UI_THEME_DARK_BUTTON_DOWN 0xFF3A3840U
#define LH_UI_THEME_DARK_BORDER 0xFF49454FU

#define LH_UI_THEME_LIGHT_SURFACE 0xFFFFFBFEU
#define LH_UI_THEME_LIGHT_TEXT 0xFF1C1B1FU
#define LH_UI_THEME_LIGHT_BUTTON 0xFFE7E0ECU
#define LH_UI_THEME_LIGHT_BUTTON_DOWN 0xFFD0C9D4U
#define LH_UI_THEME_LIGHT_BORDER 0xFFCAC4D0U

lh_ui_color_t
lh_ui_theme_on_color(lh_ui_color_t color)
{
    const lh_uint_t argb = lh_ui_color_to_argb(color);
    const lh_uint_t red = (argb >> 16) & 0xFFU;
    const lh_uint_t green = (argb >> 8) & 0xFFU;
    const lh_uint_t blue = argb & 0xFFU;
    const lh_uint_t luma = (red * 3U + green * 6U + blue) / 10U;

    if (luma > 160U)
    {
        return lh_ui_color_from_argb(LH_UI_THEME_LIGHT_TEXT);
    }
    return lh_ui_color_from_argb(LH_UI_THEME_DARK_TEXT);
}

lh_void
lh_ui_theme_paint(lh_ui_style_t *style, lh_uint_t background, lh_uint_t text)
{
    lh_ui_style_set_bg_color(style, lh_ui_color_from_argb(background));
    lh_ui_style_set_text_color(style, lh_ui_color_from_argb(text));
}

lh_void
lh_ui_theme_init(lh_ui_theme_t *self, lh_ui_color_t primary, lh_bool_t dark)
{
    const lh_uint_t text = dark != lh_bool_false ? LH_UI_THEME_DARK_TEXT : LH_UI_THEME_LIGHT_TEXT;

    lh_assert_runtime_ref(self);
    self->dark = dark != lh_bool_false ? lh_bool_true : lh_bool_false;
    self->primary = primary;
    if (dark != lh_bool_false)
    {
        lh_ui_theme_paint(&self->surface, LH_UI_THEME_DARK_SURFACE, text);
        lh_ui_theme_paint(&self->button, LH_UI_THEME_DARK_BUTTON, text);
        lh_ui_theme_paint(&self->button_pressed, LH_UI_THEME_DARK_BUTTON_DOWN, text);
        self->border = lh_ui_color_from_argb(LH_UI_THEME_DARK_BORDER);
    }
    else
    {
        lh_ui_theme_paint(&self->surface, LH_UI_THEME_LIGHT_SURFACE, text);
        lh_ui_theme_paint(&self->button, LH_UI_THEME_LIGHT_BUTTON, text);
        lh_ui_theme_paint(&self->button_pressed, LH_UI_THEME_LIGHT_BUTTON_DOWN, text);
        self->border = lh_ui_color_from_argb(LH_UI_THEME_LIGHT_BORDER);
    }
    lh_ui_theme_paint(&self->text, 0x00000000U, text);
    lh_ui_style_set_bg_color(&self->primary_style, primary);
    lh_ui_style_set_text_color(&self->primary_style, lh_ui_theme_on_color(primary));
}

lh_bool_t
lh_ui_theme_is_dark(const lh_ui_theme_t *self)
{
    lh_assert_runtime_ref(self);
    return self->dark;
}

lh_ui_color_t
lh_ui_theme_get_primary(const lh_ui_theme_t *self)
{
    lh_assert_runtime_ref(self);
    return self->primary;
}

lh_ui_color_t
lh_ui_theme_get_border(const lh_ui_theme_t *self)
{
    lh_assert_runtime_ref(self);
    return self->border;
}

const lh_ui_style_t *
lh_ui_theme_get_surface(const lh_ui_theme_t *self)
{
    lh_assert_runtime_ref(self);
    return &self->surface;
}

const lh_ui_style_t *
lh_ui_theme_get_text(const lh_ui_theme_t *self)
{
    lh_assert_runtime_ref(self);
    return &self->text;
}

const lh_ui_style_t *
lh_ui_theme_get_button(const lh_ui_theme_t *self)
{
    lh_assert_runtime_ref(self);
    return &self->button;
}

const lh_ui_style_t *
lh_ui_theme_get_button_pressed(const lh_ui_theme_t *self)
{
    lh_assert_runtime_ref(self);
    return &self->button_pressed;
}

const lh_ui_style_t *
lh_ui_theme_get_primary_style(const lh_ui_theme_t *self)
{
    lh_assert_runtime_ref(self);
    return &self->primary_style;
}

lh_void
lh_ui_theme_apply(const lh_ui_theme_t *self, lh_entity_t *entity)
{
    const lh_entity_class_t *entity_class;

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(entity);
    entity_class = lh_entity_get_class(entity);
    if (entity_class == &lh_entity_screen_class)
    {
        lh_entity_2d_set_style(lh_ptr_rcast(lh_entity_2d_t, entity), &self->surface);
        return;
    }
    if (entity_class == &lh_entity_label_class)
    {
        lh_entity_2d_set_style(lh_ptr_rcast(lh_entity_2d_t, entity), &self->text);
        return;
    }
    if (entity_class == &lh_entity_button_class)
    {
        lh_entity_2d_set_style(lh_ptr_rcast(lh_entity_2d_t, entity), &self->button);
        lh_entity_button_set_pressed_style(lh_ptr_rcast(lh_entity_button_t, entity),
                                           &self->button_pressed);
    }
}
