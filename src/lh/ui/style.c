#include <lh/ui/style.h>
#include <lh/assert.h>

lh_ui_color_t
lh_ui_style_get_bg_color(const lh_ui_style_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bg_color;
}

lh_void
lh_ui_style_set_bg_color(lh_ui_style_t *self, lh_ui_color_t bg_color)
{
    lh_assert_runtime_ref(self);
    self->bg_color = bg_color;
}
