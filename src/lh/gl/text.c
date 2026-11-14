/**
 * @file text.c
 * @brief Paint one line from a borrowed font, color and string.
 */
#include <lh/gl/text.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/char/map.h>
#include <lh/gl/canvas.h>
#include <lh/gl/entity/class/initializer.h>

lh_gl_entity_t *
lh_gl_text_get_entity_mut(lh_gl_text_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

const lh_gl_entity_t *
lh_gl_text_get_entity(const lh_gl_text_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->entity);
}

const lh_gl_font_t *
lh_gl_text_get_font(const lh_gl_text_t *self)
{
    lh_assert_runtime_ref(self);
    return self->font;
}

lh_void
lh_gl_text_set_font(lh_gl_text_t *self, const lh_gl_font_t *font)
{
    lh_assert_runtime_ref(self);
    self->font = font;
    lh_gl_entity_invalidate(lh_gl_text_get_entity_mut(self));
}

const lh_gl_color_t *
lh_gl_text_get_color(const lh_gl_text_t *self)
{
    lh_assert_runtime_ref(self);
    return self->color;
}

lh_void
lh_gl_text_set_color(lh_gl_text_t *self, const lh_gl_color_t *color)
{
    lh_assert_runtime_ref(self);
    self->color = color;
    lh_gl_entity_invalidate(lh_gl_text_get_entity_mut(self));
}

const lh_char_t *
lh_gl_text_get_text(const lh_gl_text_t *self)
{
    lh_assert_runtime_ref(self);
    return self->text;
}

lh_void
lh_gl_text_set_text(lh_gl_text_t *self, const lh_char_t *text)
{
    lh_assert_runtime_ref(self);
    self->text = text;
    lh_gl_entity_invalidate(lh_gl_text_get_entity_mut(self));
}

const lh_gl_entity_class_t *
lh_gl_text_get_class(void)
{
    static const lh_gl_entity_class_t text_class = lh_gl_entity_class_initializer(
        lh_gl_entity_class_kind_entity, lh_gl_text_draw, lh_bool_false);

    return lh_addr_of(text_class);
}

lh_void
lh_gl_text_init(lh_gl_text_t *self, lh_gl_rect_t rect, const lh_gl_font_t *font,
                const lh_gl_color_t *color, const lh_char_t *text)
{
    lh_gl_entity_init(lh_gl_text_get_entity_mut(self), rect, lh_gl_text_get_class());
    lh_gl_text_set_font(self, font);
    lh_gl_text_set_color(self, color);
    lh_gl_text_set_text(self, text);
}

lh_void
lh_gl_text_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas)
{
    const lh_gl_font_t *font = lh_gl_text_get_font(lh_ptr_ccast(lh_gl_text_t, entity));

    if (lh_ptr_is_null(font))
    {
        return;
    }
    const lh_gl_color_t *color = lh_gl_text_get_color(lh_ptr_ccast(lh_gl_text_t, entity));

    if (lh_ptr_is_null(color))
    {
        return;
    }
    const lh_char_t *cursor = lh_gl_text_get_text(lh_ptr_ccast(lh_gl_text_t, entity));

    if (lh_ptr_is_null(cursor))
    {
        return;
    }
    lh_gl_scalar_t pen;

    pen = 0;
    while (lh_ptr_deref(cursor) != lh_char_map_nul)
    {
        const lh_gl_glyph_t *glyph = lh_gl_font_get_glyph(
            font, lh_cast_static(lh_u32_t, lh_cast_static(lh_byte_t, lh_ptr_deref(cursor))));
        lh_gl_scalar_t advance = 0;

        if (lh_ptr_is_set(glyph))
        {
            advance = lh_gl_glyph_get_advance(glyph);
            const lh_byte_t *bits = lh_gl_glyph_get_bits(glyph);
            lh_gl_scalar_t width = lh_gl_glyph_get_width(glyph);
            lh_gl_scalar_t height = lh_gl_glyph_get_height(glyph);

            if (lh_ptr_is_set(bits) && width > 0 && height > 0)
            {
                lh_gl_scalar_t top = lh_gl_glyph_get_top(glyph);
                lh_gl_scalar_t baseline = lh_gl_font_get_ascent(font) + top;

                for (lh_gl_scalar_t row = 0; row < height; ++row)
                {
                    const lh_byte_t *samples = lh_ptr_add_by_offset(
                        const lh_byte_t, bits, (lh_uaddr_t)row * (lh_uaddr_t)width);

                    for (lh_gl_scalar_t col = 0; col < width;)
                    {
                        lh_byte_t cover = lh_ptr_deref(
                            lh_ptr_add_by_offset(const lh_byte_t, samples, (lh_uaddr_t)col));

                        if (cover == 0)
                        {
                            col = col + 1;
                            continue;
                        }
                        lh_gl_scalar_t start = col;

                        col = col + 1;
                        while (col < width &&
                               lh_ptr_deref(lh_ptr_add_by_offset(const lh_byte_t, samples,
                                                                 (lh_uaddr_t)col)) == cover)
                        {
                            col = col + 1;
                        }
                        lh_gl_rect_t run;

                        lh_gl_rect_init(lh_addr_of(run), pen + start, baseline + row, col - start,
                                        1);
                        if (cover == 255)
                        {
                            lh_gl_canvas_fill_rect(canvas, lh_addr_of(run), color);
                            continue;
                        }
                        lh_gl_color_t ink;

                        lh_gl_color_scale_alpha(color, cover, lh_addr_of(ink));
                        lh_gl_canvas_fill_rect(canvas, lh_addr_of(run), lh_addr_of(ink));
                    }
                }
            }
        }
        pen = pen + advance;
        cursor = lh_ptr_add_by_offset(const lh_char_t, cursor, 1);
    }
}
