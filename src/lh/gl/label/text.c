/**
 * @file text.c
 * @brief Implementation of `lh/gl/label.h`: the text: telling a new string from the same one,
 * shortening it, its font and colour.
 */

#include <lh/gl/label.h>

#include <lh/core/hash/fnv.h>
#include <lh/core/char/map.h>
#include <lh/core/str/utf8.h>
#include <lh/gl/text.h>
#include <lh/core/util/return.h>
#include <lh/core/util/str/ptr.h>

lh_u32_t
lh_gl_label_hash_text(const lh_char_t *text, lh_u32_t *length)
{
    const lh_usize_t size = lh_null_ne(text) ? lh_str_ptr_len(text) : 0U;

    lh_ptr_deref(length) = lh_cast_static(lh_u32_t, size);
    return lh_hash_fnv1a_32(text, size);
}

lh_bool_t
lh_gl_label_shorten(const lh_gl_font_t *font, const lh_char_t *text, lh_s32_t room, lh_char_t *out,
                    lh_u32_t size)
{
    const lh_bool_t has_ellipsis = lh_gl_font_has_code(font, 0x2026U);
    const lh_s32_t tail =
        has_ellipsis
            ? lh_gl_font_get_advance(font, 0x2026U)
            : 3 * lh_gl_font_get_advance(font, lh_cast_static(lh_u32_t, lh_char_map_period));
    /* `…` is three bytes of UTF-8, and so are three dots. */
    const lh_u32_t tail_bytes = 3U;
    const lh_char_t *cursor = text;
    const lh_char_t *fits = text;
    lh_s32_t width = 0;

    /* A line that fits, or text of several lines, is drawn as it is. */
    for (cursor = text; lh_ptr_deref(cursor) != lh_char_map_nul; ++cursor)
    {
        lh_return_if(lh_ptr_deref(cursor) == lh_char_map_lf, lh_bool_false);
    }
    lh_return_if(lh_gl_text_get_width(font, text) <= room, lh_bool_false);

    cursor = text;
    lh_u32_t code;
    while ((code = lh_str_utf8_get_next_code(lh_addr_of(cursor))) != 0U)
    {
        width += lh_gl_font_get_advance(font, code);
        if (width + tail > room || lh_cast_static(lh_u32_t, cursor - text) + tail_bytes + 1U > size)
        {
            break;
        }
        fits = cursor;
    }
    lh_u32_t used = lh_cast_static(lh_u32_t, fits - text);
    for (cursor = text; cursor < fits; ++cursor)
    {
        out[cursor - text] = lh_ptr_deref(cursor);
    }
    if (has_ellipsis)
    {
        used += lh_str_utf8_encode(out + used, size - used, 0x2026U);
    }
    else
    {
        out[used++] = lh_char_map_period;
        out[used++] = lh_char_map_period;
        out[used++] = lh_char_map_period;
    }
    lh_str_ptr_erase(out, used);
    return lh_bool_true;
}

const lh_gl_font_t *
lh_gl_label_get_font(const lh_gl_label_t *self)
{
    const lh_gl_style_t *style =
        lh_gl_entity_get_style(lh_gl_container_get_entity(lh_gl_label_get_container(self)));

    return lh_null_eq(style) ? lh_null : lh_gl_style_get_font(style);
}

const lh_gl_color_t *
lh_gl_label_get_text_color(const lh_gl_label_t *self)
{
    /* The paint of the text follows a press, its place does not: the origin
       below reads the font, the padding and the alignment from the entity's own
       style, so nothing moves under the pointer. */
    const lh_gl_style_t *style =
        lh_gl_entity_get_style_now(lh_gl_container_get_entity(lh_gl_label_get_container(self)));

    return lh_null_eq(style) ? lh_null : lh_gl_style_get_text_color(style);
}

lh_gl_text_align_h_t
lh_gl_label_get_line_align(const lh_gl_label_t *self)
{
    const lh_gl_style_t *style =
        lh_gl_entity_get_style(lh_gl_container_get_entity(lh_gl_label_get_container(self)));

    return lh_null_eq(style) ? lh_gl_text_align_h_left : lh_gl_style_get_align_h(style);
}

lh_void
lh_gl_label_set_text_v(lh_gl_label_t *self, const lh_char_t *text)
{
    lh_u32_t length;

    lh_u32_t hash = lh_gl_label_hash_text(text, lh_addr_of(length));
    if (lh_gl_label_is_text_hashed(self) && hash == lh_gl_label_get_text_hash(self) &&
        length == lh_gl_label_get_text_length(self))
    {
        /* The same bytes, perhaps at a new address: nothing on screen changes. */
        lh_gl_label_set_text(self, text);
        return;
    }
    lh_gl_entity_t *entity = lh_gl_label_as_entity(self);
    lh_gl_rect_t box;
    lh_gl_entity_copy_rect(entity, lh_addr_of(box));
    /* Before: the box, and the old text where it was last recorded. Measuring the text
       now would measure the **new** bytes whenever the caller rewrote the buffer first,
       which is how a cell is usually updated -- so the remembered reach is what is
       erased, and when there is none yet (the first change) the current measure is. */
    if (lh_gl_rect_is_empty(lh_gl_label_get_text_noted_mut(self)))
    {
        lh_gl_label_get_ink_reach(self, lh_gl_label_get_text_noted_mut(self));
    }
    lh_gl_rect_t old_ink;
    lh_gl_rect_offset(lh_gl_label_get_text_noted_mut(self),
                      lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(box))),
                      lh_gl_point_get_y(lh_gl_rect_get_origin(lh_addr_of(box))),
                      lh_addr_of(old_ink));
    lh_gl_entity_note_rect(entity, lh_addr_of(box));
    lh_gl_entity_note_rect(entity, lh_addr_of(old_ink));

    lh_gl_label_set_text(self, text);
    lh_gl_label_set_text_hashed(self, lh_bool_true);
    lh_gl_label_set_text_hash(self, hash);
    lh_gl_label_set_text_length(self, length);
    lh_gl_label_set_metrics_ready(self, lh_bool_false);
    lh_gl_label_get_ink_reach(self, lh_gl_label_get_text_noted_mut(self));
    /* After: the box and the new text (a leaf's note includes what it measures). */
    lh_gl_entity_note(entity);
}
