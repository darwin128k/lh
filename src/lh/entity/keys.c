#include <lh/entity/keys.h>
#include <lh/assert.h>
#include <lh/entity/key.h>
#include <lh/entity/screen.h>
#include <lh/null.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

const lh_char_t *const lh_entity_keys_row[4] = {"1234567890", "qwertyuiop", "asdfghjkl",
                                                "zxcvbnm \b"};

lh_void
lh_entity_keys_construct(lh_entity_t *self)
{
    lh_ptr_rcast(lh_entity_keys_t, self)->font = lh_ui_font_get_default();
}

lh_int_t
lh_entity_keys_length(const lh_char_t *text)
{
    lh_int_t n = 0;
    while (lh_ptr_is_set(text) && text[n] != 0)
    {
        n += 1;
    }
    return n;
}

lh_void
lh_entity_keys_on_event(lh_entity_t *self, lh_entity_event_t *event)
{
    lh_entity_keys_t *const keys = lh_ptr_rcast(lh_entity_keys_t, self);
    const lh_uint_t code = lh_entity_event_get_code(event);
    const lh_ui_style_t *style;
    lh_ui_canvas_t *canvas;
    lh_math_rect_t bounds;
    lh_int_t width;
    lh_int_t height;
    lh_int_t cell_w;
    lh_int_t cell_h;
    lh_int_t row;
    if (code == LH_ENTITY_EVENT_POINTER_UP)
    {
        const lh_math_vec2_t *const point =
            lh_ptr_rcast(const lh_math_vec2_t, lh_entity_event_get_param(event));
        lh_entity_screen_t *screen;
        lh_int_t col;
        lh_uint_t key;
        const lh_char_t *line;
        if (lh_ptr_is_null(point) ||
            !lh_entity_2d_contains(lh_ptr_rcast(const lh_entity_2d_t, self), *point))
        {
            return;
        }
        bounds = lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
        width = lh_math_rect_get_size_width(lh_addr_of(bounds));
        height = lh_math_rect_get_size_height(lh_addr_of(bounds));
        if (width < 10 || height < 4)
        {
            return;
        }
        cell_w = width / 10;
        cell_h = height / 4;
        col = ((lh_int_t)lh_math_vec2_get_x(point) - lh_math_rect_get_x(lh_addr_of(bounds))) / cell_w;
        row = ((lh_int_t)lh_math_vec2_get_y(point) - lh_math_rect_get_y(lh_addr_of(bounds))) / cell_h;
        if (row < 0 || row > 3 || col < 0)
        {
            return;
        }
        line = lh_entity_keys_row[row];
        if (col >= lh_entity_keys_length(line))
        {
            return;
        }
        key = (lh_uint_t)(lh_byte_t)line[col];
        if (line[col] == '\b')
        {
            key = LH_ENTITY_KEY_BACKSPACE;
        }
        screen = lh_entity_cast(lh_entity_get_root(self), lh_addr_of(lh_entity_screen_class));
        if (lh_ptr_is_set(screen))
        {
            lh_entity_screen_send_key(screen, key);
        }
        return;
    }
    if (code != LH_ENTITY_EVENT_DRAW)
    {
        return;
    }
    style = lh_entity_2d_get_style(lh_ptr_rcast(const lh_entity_2d_t, self));
    canvas = lh_ptr_rcast(lh_ui_canvas_t, lh_entity_event_get_param(event));
    if (lh_ptr_is_null(style) || lh_ptr_is_null(keys->font))
    {
        return;
    }
    bounds = lh_entity_2d_get_screen_bounds(lh_ptr_rcast(const lh_entity_2d_t, self));
    width = lh_math_rect_get_size_width(lh_addr_of(bounds));
    height = lh_math_rect_get_size_height(lh_addr_of(bounds));
    lh_ui_canvas_fill_rect(canvas, bounds, lh_ui_style_get_bg_color(style));
    if (width < 10 || height < 4)
    {
        return;
    }
    cell_w = width / 10;
    cell_h = height / 4;
    for (row = 0; row < 4; ++row)
    {
        const lh_char_t *const line = lh_entity_keys_row[row];
        lh_int_t col;
        const lh_int_t n = lh_entity_keys_length(line);
        for (col = 0; col < n; ++col)
        {
            lh_char_t label[2];
            lh_math_vec2_t measured;
            lh_int_t glyph_width;
            lh_int_t glyph_height;
            label[0] = line[col] == '\b' ? '<' : line[col];
            label[1] = 0;
            measured = lh_ui_font_measure(keys->font, label);
            glyph_width = (lh_int_t)lh_math_vec2_get_x(lh_addr_of(measured));
            glyph_height = (lh_int_t)lh_math_vec2_get_y(lh_addr_of(measured));
            lh_ui_font_draw(keys->font, canvas,
                            lh_math_rect_get_x(lh_addr_of(bounds)) + col * cell_w +
                                (cell_w - glyph_width) / 2,
                            lh_math_rect_get_y(lh_addr_of(bounds)) + row * cell_h +
                                (cell_h - glyph_height) / 2,
                            label, lh_ui_style_get_text_color(style));
        }
    }
}

const lh_entity_class_t lh_entity_keys_class =
    lh_entity_class_initializer(&lh_entity_2d_class, sizeof(lh_entity_keys_t),
                                lh_entity_keys_construct, lh_null, lh_entity_keys_on_event);

lh_void
lh_entity_keys_set_font(lh_entity_keys_t *self, const lh_ui_font_t *font)
{
    lh_assert_runtime_ref(self);
    self->font = font;
    lh_entity_invalidate(lh_ptr_rcast(lh_entity_t, self));
}
