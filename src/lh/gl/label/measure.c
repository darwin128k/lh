/**
 * @file measure.c
 * @brief Implementation of `lh/gl/label.h`: the measure: what the text measures, kept until
 * something it depends on changes.
 */

#include <lh/gl/label.h>

#include <lh/core/assert.h>
#include <lh/core/math.h>
#include <lh/gl/text.h>
#include <lh/core/util/return.h>

lh_bool_t
lh_gl_label_same_insets(const lh_gl_insets_t *a, const lh_gl_insets_t *b)
{
    return lh_gl_insets_get_left(a) == lh_gl_insets_get_left(b) &&
                   lh_gl_insets_get_top(a) == lh_gl_insets_get_top(b) &&
                   lh_gl_insets_get_right(a) == lh_gl_insets_get_right(b) &&
                   lh_gl_insets_get_bottom(a) == lh_gl_insets_get_bottom(b)
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_gl_label_drop_metrics(lh_gl_label_t *self)
{
    /* The bytes changed under the label: the next ::lh_gl_label_set_text_v must not
       take them for the same ones. */
    lh_gl_label_set_metrics_ready(self, lh_bool_false);
    lh_gl_label_set_text_hashed(self, lh_bool_false);
}

lh_gl_scalar_t
lh_gl_label_get_wrap_width(const lh_gl_label_t *self)
{
    const lh_gl_entity_t *entity = lh_gl_container_get_entity(lh_gl_label_get_container(self));
    const lh_gl_style_t *style = lh_gl_entity_get_style(entity);

    lh_return_if(lh_null_eq(style) || lh_gl_style_get_wrap(style) != lh_gl_text_wrap_words,
                 lh_gl_scalar(0));
    lh_gl_insets_t padding;
    lh_gl_entity_get_padding(entity, lh_addr_of(padding));
    lh_gl_scalar_t width =
        lh_gl_size_get_width(lh_gl_rect_get_size(lh_gl_entity_get_rect(entity))) -
        lh_gl_insets_get_left(lh_addr_of(padding)) - lh_gl_insets_get_right(lh_addr_of(padding));
    /* A box with no room still wraps, one letter a line, rather than not at all. */
    return lh_math_max(width, lh_gl_scalar(1));
}

lh_bool_t
lh_gl_label_can_draw_text(const lh_gl_label_t *self, const lh_gl_canvas_t *canvas)
{
    return lh_null_ne(canvas) && lh_null_ne(lh_gl_label_get_text(self)) &&
                   lh_null_ne(lh_gl_label_get_font(self)) &&
                   lh_null_ne(lh_gl_label_get_text_color(self))
               ? lh_bool_true
               : lh_bool_false;
}

const lh_gl_label_t *
lh_gl_label_measured(const lh_gl_label_t *self)
{
    lh_gl_label_t *label = lh_ptr_rcast(lh_gl_label_t, self);
    const lh_gl_entity_t *entity = lh_gl_container_get_entity(lh_gl_label_get_container(self));
    const lh_gl_style_t *style = lh_gl_entity_get_style(entity);
    const lh_gl_font_t *font = lh_gl_label_get_font(self);
    lh_gl_insets_t padding;
    lh_gl_entity_get_padding(entity, lh_addr_of(padding));
    const lh_gl_text_align_h_t align_h =
        lh_null_eq(style) ? lh_gl_text_align_h_left : lh_gl_style_get_align_h(style);
    const lh_gl_text_align_v_t align_v =
        lh_null_eq(style) ? lh_gl_text_align_v_top : lh_gl_style_get_align_v(style);
    const lh_gl_scalar_t wrap = lh_gl_label_get_wrap_width(self);

    if (lh_gl_label_is_metrics_ready(self) && lh_gl_label_get_metrics_font(self) == font &&
        lh_gl_label_get_metrics_wrap(self) == wrap &&
        lh_gl_label_get_metrics_align_h(self) == align_h &&
        lh_gl_label_get_metrics_align_v(self) == align_v &&
        lh_gl_rect_eq(lh_gl_label_get_metrics_rect(self), lh_gl_entity_get_rect(entity)) &&
        lh_gl_label_same_insets(lh_gl_label_get_metrics_padding(self), lh_addr_of(padding)))
    {
        return self;
    }
    lh_gl_label_set_metrics_font(label, font);
    lh_gl_label_set_metrics_wrap(label, wrap);
    lh_gl_label_set_metrics_align_h(label, align_h);
    lh_gl_label_set_metrics_align_v(label, align_v);
    lh_gl_label_set_metrics_rect(label, lh_ptr_deref(lh_gl_entity_get_rect(entity)));
    lh_gl_label_set_metrics_padding(label, padding);
    lh_gl_size_init(lh_gl_label_get_metrics_size_mut(label), 0, 0);
    lh_gl_label_set_metrics_ink_top(label, 0);
    if (lh_null_ne(font) && lh_null_ne(lh_gl_label_get_text(self)))
    {
        lh_gl_text_block_t block;

        /* One walk of the text for both: the size and the first ink row are two
           questions about the same lines. */
        lh_gl_text_block_measure(lh_addr_of(block), font, lh_gl_label_get_text(self), wrap);
        lh_gl_text_block_get_size(lh_addr_of(block), font, lh_gl_label_get_metrics_size_mut(label));
        lh_gl_label_set_metrics_ink_top(label, lh_gl_text_block_get_ink_top(lh_addr_of(block)));
    }
    /* The padding is what keeps the text off the edges, and the alignment is where
       inside what the padding leaves it starts. Left and top are what this returned
       before a style could say otherwise, so a style that never set one draws the
       label exactly as it did. The size is the ink's
       (::lh_gl_text_get_size), so what is aligned here is the pixels and every
       v_* means what it says: a 22 px line box with 15 px of ink used to be what
       got centred, and the caption sat five rows low. */
    lh_gl_size_t aligned;
    lh_gl_size_init(lh_addr_of(aligned), 0, 0);
    if (lh_null_ne(style))
    {
        aligned = lh_ptr_deref(lh_gl_label_get_metrics_size(label));
    }
    lh_gl_text_align_get_origin(lh_gl_entity_get_rect(entity), lh_addr_of(padding), aligned,
                                align_h, align_v, lh_gl_label_get_metrics_origin_mut(label));
    lh_gl_rect_init_empty(lh_gl_label_get_metrics_ink_mut(label));
    if (lh_null_ne(font) && lh_null_ne(lh_gl_label_get_text(self)))
    {
        lh_gl_text_get_ink_rect_wrapped(font, lh_gl_label_get_text(self),
                                        lh_ptr_deref(lh_gl_label_get_metrics_origin(label)), wrap,
                                        align_h, lh_gl_label_get_metrics_ink_mut(label));
    }
    lh_gl_label_set_metrics_ready(label, lh_bool_true);
    return self;
}

lh_gl_scalar_t
lh_gl_label_get_ink_top(const lh_gl_label_t *self)
{
    return lh_gl_label_get_metrics_ink_top(lh_gl_label_measured(self));
}

lh_void
lh_gl_label_get_text_origin(const lh_gl_label_t *self, lh_gl_point_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_ptr_deref(lh_gl_label_get_metrics_origin(lh_gl_label_measured(self)));
}

lh_gl_scalar_t
lh_gl_label_get_baseline(const lh_gl_label_t *self)
{
    const lh_gl_entity_t *entity = lh_gl_container_get_entity(lh_gl_label_get_container(self));
    const lh_gl_font_t *font = lh_gl_label_get_font(self);
    const lh_gl_label_t *measured = lh_gl_label_measured(self);
    const lh_gl_point_t origin = lh_ptr_deref(lh_gl_label_get_metrics_origin(measured));
    const lh_gl_point_t *box = lh_gl_rect_get_origin(lh_gl_entity_get_rect(entity));

    /* Nothing stands on a line that is not drawn. */
    if (lh_null_eq(font) || lh_null_eq(lh_gl_label_get_text(self)))
    {
        return lh_gl_scalar(-1);
    }
    lh_return_if(lh_gl_size_get_height(lh_gl_label_get_metrics_size(measured)) == lh_gl_scalar(0),
                 lh_gl_scalar(-1));
    /* The line the letters stand on is where the **tallest letter of this run**
       ends, and the origin is where the ink of the run goes, so the line is the
       ascent down from where that tallest letter's ink begins — `ascent -
       ink_top(run)`, because ::lh_gl_text_get_ink_top is the ascent plus the top of
       the highest glyph and the two ascents cancel.

       It is not the cap height, though those agree on most captions: a run with a
       capital in it is exactly as tall as the cap line, and the demo's "Hide panel"
       is one of them (measured: ink top 5, ascent 17, so 12 = the cap height, and
       the line lands on row 200). A run whose letters rise above the cap line — a
       'd', an 'h', a 't' — is taller than the cap block, the flow centres that
       block, and taking the cap height would stand the icon a row below where the
       letters stand. Measured on ::lh_test::cap_font (line 8, ascent 6, cap 4):
       "H" gives 4 and "d" gives 5, and both letters' ink ends on row 6.

       The first line's, which is what a row lines up on. */
    lh_gl_scalar_t baseline = lh_gl_point_get_y(lh_addr_of(origin)) - lh_gl_point_get_y(box) +
                              lh_cast_static(lh_gl_scalar_t, lh_gl_font_get_ascent(font)) -
                              lh_gl_label_get_metrics_ink_top(measured);
    return baseline;
}

lh_void
lh_gl_label_get_text_rect(const lh_gl_label_t *self, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_ptr_deref(lh_gl_label_get_metrics_ink(lh_gl_label_measured(self)));
}

lh_void
lh_gl_label_get_ink_reach(lh_gl_label_t *self, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_rect_t text;
    lh_gl_label_get_text_rect(self, lh_addr_of(text));
    lh_gl_rect_t box;
    lh_gl_entity_copy_rect(lh_gl_label_as_entity(self), lh_addr_of(box));
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(lh_addr_of(box));

    if (lh_gl_rect_is_empty(lh_addr_of(text)))
    {
        lh_ptr_deref(out) = text;
        return;
    }
    lh_gl_rect_offset(lh_addr_of(text), lh_math_neg(lh_gl_point_get_x(origin)),
                      lh_math_neg(lh_gl_point_get_y(origin)), out);
}
