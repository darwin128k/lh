/**
 * @file label.c
 * @brief Implementation of `lh/ui/label.h`.
 */

#include <lh/math.h>
#include <lh/ui/label.h>
#include <lh/ui/text.h>
#include <lh/util/return.h>

/* ── Class ───────────────────────────────────────────────────────────────── */

const lh_ui_entity_class_t lh_ui_label_class = {
    lh_ui_label_event, lh_addr_of(lh_ui_container_class)};

lh_void
lh_ui_label_on_children(const lh_ui_label_t *self, const lh_ui_entity_event_t *event)
{
    lh_ui_entity_transform_t *transform;

    (void)self;
    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_children);
    /* The transform is the one thing an event carries that a class fills in, and
       the getter takes it without the const every event handler is given. */
    transform = lh_ui_entity_event_get_transform(lh_ptr_rcast(lh_ui_entity_event_t, event));
    /* The container behind the label cut its children to its own rect, and that
       is right for a button and wrong for a label: the label's rect is where the
       text is *centred* (::lh_ui_text_get_size measures the cap line to the
       baseline), so the tail of a 'p' hangs below it. Cutting there drew half a
       letter — measured on the demo, "Hide panel" lost every pixel under row 199.
       The offset and the placement the container just did stay; only the knife
       goes. */
    lh_ui_entity_transform_set_clip(transform, lh_bool_false);
}

/** FNV-1a over the bytes, with the length beside it: two short strings that collide on
   both are not a case a label meets. */
static lh_u32_t
lh_ui_label_hash(const lh_char_t *text, lh_u32_t *length)
{
    lh_u32_t hash = 2166136261U;
    lh_u32_t n = 0U;

    if (lh_null_ne(text))
    {
        for (; text[n] != '\0'; ++n)
        {
            hash = (hash ^ lh_cast_static(lh_u32_t, lh_cast_static(lh_u8_t, text[n]))) * 16777619U;
        }
    }
    *length = n;
    return hash;
}

lh_ui_entity_t *
lh_ui_label_as_entity(lh_ui_label_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_container_as_entity(lh_addr_of(self->container));
}

lh_void
lh_ui_label_init(lh_ui_label_t *self, lh_ui_rect_t rect, const lh_char_t *text)
{
    lh_assert_runtime_ref(self);
    lh_ui_container_init(lh_addr_of(self->container), rect);
    self->text = text;
    self->metrics_ready = lh_bool_false;
    /* Hashed from the start, so a refresh that sets the text the label was built with
       records nothing. Nothing is noted here: the label is in no tree yet. */
    self->text_hash = lh_ui_label_hash(text, lh_addr_of(self->text_length));
    self->text_hashed = lh_bool_true;
    lh_ui_rect_init_empty(lh_addr_of(self->text_noted));
    lh_ui_entity_set_class(lh_ui_label_as_entity(self), lh_addr_of(lh_ui_label_class));
}

lh_ui_container_t *
lh_ui_label_as_container(lh_ui_label_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->container);
}

/* ── Lifetime and fields ─────────────────────────────────────────────────── */

const lh_char_t *
lh_ui_label_get_text(const lh_ui_label_t *self)
{
    lh_assert_runtime_ref(self);
    return self->text;
}

lh_void
lh_ui_label_drop_metrics(lh_ui_label_t *self)
{
    lh_assert_runtime_ref(self);
    /* The bytes changed under the label: the next ::lh_ui_label_set_text must not
       take them for the same ones. */
    self->metrics_ready = lh_bool_false;
    self->text_hashed = lh_bool_false;
}

const lh_ui_font_t *
lh_ui_label_get_font(const lh_ui_label_t *self)
{
    const lh_ui_style_t *style = lh_ui_entity_get_style(lh_addr_of(self->container.entity));

    return lh_null_eq(style) ? lh_null : lh_ui_style_get_font(style);
}

const lh_ui_color_t *
lh_ui_label_get_text_color(const lh_ui_label_t *self)
{
    /* The paint of the text follows a press, its place does not: the origin
       below reads the font, the padding and the alignment from the entity's own
       style, so nothing moves under the pointer. */
    const lh_ui_style_t *style = lh_ui_entity_get_style_now(lh_addr_of(self->container.entity));

    return lh_null_eq(style) ? lh_null : lh_ui_style_get_text_color(style);
}

/** How wide the lines of the text may be: the padded box when the style wraps, and 0
   -- no width, lines end at line breaks only -- when it does not. */
static lh_ui_scalar_t
lh_ui_label_get_wrap_width(const lh_ui_label_t *self)
{
    const lh_ui_entity_t *entity = lh_addr_of(self->container.entity);
    const lh_ui_style_t *style = lh_ui_entity_get_style(entity);
    lh_ui_insets_t padding;
    lh_ui_scalar_t width;

    lh_return_if(lh_null_eq(style) || lh_ui_style_get_wrap(style) != lh_ui_text_wrap_words, lh_ui_scalar(0));
    lh_ui_entity_get_padding(entity, lh_addr_of(padding));
    width = lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(entity->rect))) -
            lh_ui_insets_get_left(lh_addr_of(padding)) - lh_ui_insets_get_right(lh_addr_of(padding));
    /* A box with no room still wraps, one letter a line, rather than not at all. */
    return lh_math_max(width, lh_ui_scalar(1));
}

/** Where each line of a wrapped text sits inside the widest: the alignment the text
   is placed by. */
static lh_ui_text_align_h_t
lh_ui_label_get_line_align(const lh_ui_label_t *self)
{
    const lh_ui_style_t *style = lh_ui_entity_get_style(lh_addr_of(self->container.entity));

    return lh_null_eq(style) ? lh_ui_text_align_h_left : lh_ui_style_get_align_h(style);
}

/** True when @p a and @p b keep the same distances off every edge. */
static lh_bool_t
lh_ui_label_same_insets(const lh_ui_insets_t *a, const lh_ui_insets_t *b)
{
    return lh_ui_insets_get_left(a) == lh_ui_insets_get_left(b) &&
                   lh_ui_insets_get_top(a) == lh_ui_insets_get_top(b) &&
                   lh_ui_insets_get_right(a) == lh_ui_insets_get_right(b) &&
                   lh_ui_insets_get_bottom(a) == lh_ui_insets_get_bottom(b)
               ? lh_bool_true
               : lh_bool_false;
}

/** @p self with what its text measures worked out: from the metrics fields when
   nothing they were worked out from has changed, else measured again and kept.

   Measuring is laying the text out glyph by glyph, wrapped or not, and every frame
   asks for it several times over -- whether the label shows at all, where its text
   starts, where its line is, what its ink covers -- of every label, drawn or not.
   It is the same answer each time until the text (::lh_ui_label_set_text drops it),
   the font, the box, the padding, the alignment or the wrapping width changes, and
   those are what is compared here. The label is const to its callers because none
   of this is what it looks like; only the remembering writes. */
static const lh_ui_label_t *
lh_ui_label_measured(const lh_ui_label_t *self)
{
    lh_ui_label_t *label = lh_ptr_rcast(lh_ui_label_t, self);
    const lh_ui_entity_t *entity = lh_addr_of(self->container.entity);
    const lh_ui_style_t *style = lh_ui_entity_get_style(entity);
    const lh_ui_font_t *font = lh_ui_label_get_font(self);
    lh_ui_insets_t padding;
    lh_ui_entity_get_padding(entity, lh_addr_of(padding));
    const lh_ui_text_align_h_t align_h =
        lh_null_eq(style) ? lh_ui_text_align_h_left : lh_ui_style_get_align_h(style);
    const lh_ui_text_align_v_t align_v = lh_null_eq(style) ? lh_ui_text_align_v_top : lh_ui_style_get_align_v(style);
    const lh_ui_scalar_t wrap = lh_ui_label_get_wrap_width(self);
    lh_ui_size_t aligned;

    if (self->metrics_ready && self->metrics_font == font && self->metrics_wrap == wrap &&
        self->metrics_align_h == align_h && self->metrics_align_v == align_v &&
        lh_ui_rect_eq(lh_addr_of(self->metrics_rect), lh_addr_of(entity->rect)) &&
        lh_ui_label_same_insets(lh_addr_of(self->metrics_padding), lh_addr_of(padding)))
    {
        return self;
    }
    label->metrics_font = font;
    label->metrics_wrap = wrap;
    label->metrics_align_h = align_h;
    label->metrics_align_v = align_v;
    label->metrics_rect = entity->rect;
    label->metrics_padding = padding;
    lh_ui_size_init(lh_addr_of(label->metrics_size), 0, 0);
    label->metrics_ink_top = 0;
    if (lh_null_ne(font) && lh_null_ne(self->text))
    {
        lh_ui_text_block_t block;

        /* One walk of the text for both: the size and the first ink row are two
           questions about the same lines. */
        lh_ui_text_block_measure(lh_addr_of(block), font, self->text, wrap);
        lh_ui_text_block_get_size(lh_addr_of(block), font, lh_addr_of(label->metrics_size));
        label->metrics_ink_top = lh_ui_text_block_get_ink_top(lh_addr_of(block));
    }
    /* The padding is what keeps the text off the edges, and the alignment is where
       inside what the padding leaves it starts. Left and top are what this returned
       before a style could say otherwise, so a style that never set one draws the
       label exactly as it did. The size is the ink's
       (::lh_ui_text_get_size), so what is aligned here is the pixels and every
       v_* means what it says: a 22 px line box with 15 px of ink used to be what
       got centred, and the caption sat five rows low. */
    lh_ui_size_init(lh_addr_of(aligned), 0, 0);
    if (lh_null_ne(style))
    {
        aligned = label->metrics_size;
    }
    lh_ui_text_align_get_origin(lh_addr_of(entity->rect), lh_addr_of(padding), aligned, align_h, align_v, lh_addr_of(label->metrics_origin));
    lh_ui_rect_init_empty(lh_addr_of(label->metrics_ink));
    if (lh_null_ne(font) && lh_null_ne(self->text))
    {
        lh_ui_text_get_ink_rect_wrapped(font, self->text, label->metrics_origin, wrap, align_h, lh_addr_of(label->metrics_ink));
    }
    label->metrics_ready = lh_bool_true;
    return self;
}

/* ── Text ────────────────────────────────────────────────────────────────── */

lh_ui_scalar_t
lh_ui_label_get_ink_top(const lh_ui_label_t *self)
{
    return lh_ui_label_measured(self)->metrics_ink_top;
}

lh_void
lh_ui_label_get_text_origin(const lh_ui_label_t *self, lh_ui_point_t *out)
{
    lh_assert_runtime_ref(out);
    *out = lh_ui_label_measured(self)->metrics_origin;
    return;
}

lh_ui_scalar_t
lh_ui_label_get_baseline(const lh_ui_label_t *self)
{
    const lh_ui_entity_t *entity = lh_addr_of(self->container.entity);
    const lh_ui_font_t *font = lh_ui_label_get_font(self);
    const lh_ui_label_t *measured = lh_ui_label_measured(self);
    const lh_ui_point_t origin = measured->metrics_origin;
    const lh_ui_point_t *box = lh_ui_rect_get_origin_as_const(lh_addr_of(entity->rect));
    lh_ui_scalar_t baseline;

    /* Nothing stands on a line that is not drawn. */
    if (lh_null_eq(font) || lh_null_eq(self->text))
    {
        return lh_ui_scalar(-1);
    }
    lh_return_if(lh_ui_size_get_height(lh_addr_of(measured->metrics_size)) == lh_ui_scalar(0), lh_ui_scalar(-1));
    /* The line the letters stand on is where the **tallest letter of this run**
       ends, and the origin is where the ink of the run goes, so the line is the
       ascent down from where that tallest letter's ink begins — `ascent -
       ink_top(run)`, because ::lh_ui_text_get_ink_top is the ascent plus the top of
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
    baseline = lh_ui_point_get_y(lh_addr_of(origin)) - lh_ui_point_get_y(box) +
               lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_ascent(font)) - measured->metrics_ink_top;
    return baseline;
}

lh_void
lh_ui_label_on_baseline(const lh_ui_label_t *self, const lh_ui_entity_event_t *event)
{
    lh_ui_scalar_t *answer;

    (void)self;
    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_baseline);
    /* The one thing that stands on a line in this picture is the text, and where
       that line is follows the alignment and the padding, not the box: a label
       centred in 28 rows has its baseline in the middle of them. */
    answer = lh_ui_entity_event_get_baseline(lh_ptr_rcast(lh_ui_entity_event_t, event));
    *answer = lh_ui_label_get_baseline(self);
}

lh_void
lh_ui_label_get_text_rect(const lh_ui_label_t *self, lh_ui_rect_t *out)
{
    lh_assert_runtime_ref(out);
    *out = lh_ui_label_measured(self)->metrics_ink;
    return;
}

lh_void
lh_ui_label_on_measure(const lh_ui_label_t *self, const lh_ui_entity_event_t *event)
{
    lh_ui_rect_t *bounds;
    lh_ui_rect_t text;

    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_measure);
    bounds = lh_ui_entity_event_get_bounds(event);
    lh_ui_label_get_text_rect(self, lh_addr_of(text));
    lh_ui_rect_union(bounds, lh_addr_of(text), bounds);
}

/** The ink of the current text, relative to the label's own origin, so it stays right
   when the label moves before its text changes again. */
static lh_void
lh_ui_label_get_ink_reach(lh_ui_label_t *self, lh_ui_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_rect_t text;
    lh_ui_label_get_text_rect(self, lh_addr_of(text));
    lh_ui_rect_t box;
    lh_ui_entity_get_rect(lh_ui_label_as_entity(self), lh_addr_of(box));
    const lh_ui_point_t *origin = lh_ui_rect_get_origin_as_const(lh_addr_of(box));

    if (lh_ui_rect_is_empty(lh_addr_of(text)))
    {
        *out = text;
        return;
    }
    lh_ui_rect_offset(lh_addr_of(text), lh_math_neg(lh_ui_point_get_x(origin)),
                             lh_math_neg(lh_ui_point_get_y(origin)), out);
    return;
}

lh_void
lh_ui_label_set_text(lh_ui_label_t *self, const lh_char_t *text)
{
    lh_ui_entity_t *entity;
    lh_ui_rect_t box;
    lh_ui_rect_t old_ink;
    lh_u32_t length;
    lh_u32_t hash;

    lh_assert_runtime_ref(self);
    hash = lh_ui_label_hash(text, lh_addr_of(length));
    if (self->text_hashed && hash == self->text_hash && length == self->text_length)
    {
        /* The same bytes, perhaps at a new address: nothing on screen changes. */
        self->text = text;
        return;
    }
    entity = lh_ui_label_as_entity(self);
    lh_ui_entity_get_rect(entity, lh_addr_of(box));
    /* Before: the box, and the old text where it was last recorded. Measuring the text
       now would measure the **new** bytes whenever the caller rewrote the buffer first,
       which is how a cell is usually updated -- so the remembered reach is what is
       erased, and when there is none yet (the first change) the current measure is. */
    if (lh_ui_rect_is_empty(lh_addr_of(self->text_noted)))
    {
        lh_ui_label_get_ink_reach(self, lh_addr_of(self->text_noted));
    }
    lh_ui_rect_offset(lh_addr_of(self->text_noted), lh_ui_point_get_x(lh_ui_rect_get_origin_as_const(lh_addr_of(box))),
                                lh_ui_point_get_y(lh_ui_rect_get_origin_as_const(lh_addr_of(box))), lh_addr_of(old_ink));
    lh_ui_entity_note_rect(entity, lh_addr_of(box));
    lh_ui_entity_note_rect(entity, lh_addr_of(old_ink));

    self->text = text;
    self->text_hashed = lh_bool_true;
    self->text_hash = hash;
    self->text_length = length;
    self->metrics_ready = lh_bool_false;
    lh_ui_label_get_ink_reach(self, lh_addr_of(self->text_noted));
    /* After: the box and the new text (a leaf's note includes what it measures). */
    lh_ui_entity_note(entity);
}

lh_bool_t
lh_ui_label_can_draw_text(const lh_ui_label_t *self, const lh_ui_canvas_t *canvas)
{
    return lh_null_ne(canvas) && lh_null_ne(self->text) && lh_null_ne(lh_ui_label_get_font(self)) &&
                   lh_null_ne(lh_ui_label_get_text_color(self))
               ? lh_bool_true
               : lh_bool_false;
}

/** The longest start of @p text that fits in @p room with an ellipsis after it, written
   into @p out with the ellipsis, or `lh_bool_false` when the whole line fits and is to
   be drawn as it is.

   The ellipsis is the font's own `…` when it has one and three dots when it does not:
   a box of a glyph nobody baked is worse than three full stops. The cut is between
   code points, never inside one, and a start that would not fit in @p out is cut
   shorter rather than written past it. */
static lh_bool_t
lh_ui_label_shorten(const lh_ui_font_t *font, const lh_char_t *text, lh_s32_t room, lh_char_t *out,
                    lh_u32_t size)
{
    const lh_bool_t has_ellipsis = lh_ui_font_has_code(font, 0x2026U);
    const lh_s32_t tail = has_ellipsis ? lh_ui_font_get_advance(font, 0x2026U)
                                       : 3 * lh_ui_font_get_advance(font, (lh_u32_t)'.');
    /* `…` is three bytes of UTF-8, and so are three dots. */
    const lh_u32_t tail_bytes = 3U;
    const lh_char_t *cursor = text;
    const lh_char_t *fits = text;
    lh_s32_t width = 0;
    lh_u32_t code;
    lh_u32_t used;

    /* A line that fits, or text of several lines, is drawn as it is. */
    for (cursor = text; *cursor != '\0'; ++cursor)
    {
        lh_return_if(*cursor == '\n', lh_bool_false);
    }
    lh_return_if(lh_ui_text_get_width(font, text) <= room, lh_bool_false);

    cursor = text;
    while ((code = lh_ui_text_next_code(lh_addr_of(cursor))) != 0U)
    {
        width += lh_ui_font_get_advance(font, code);
        if (width + tail > room || lh_cast_static(lh_u32_t, (cursor - text))+ tail_bytes + 1U > size)
        {
            break;
        }
        fits = cursor;
    }
    used = lh_cast_static(lh_u32_t, (fits - text));
    for (cursor = text; cursor < fits; ++cursor)
    {
        out[cursor - text] = *cursor;
    }
    if (has_ellipsis)
    {
        used += lh_ui_text_encode_code(out + used, size - used, 0x2026U);
    }
    else
    {
        out[used++] = '.';
        out[used++] = '.';
        out[used++] = '.';
    }
    out[used] = '\0';
    return lh_bool_true;
}

lh_void
lh_ui_label_draw_text(const lh_ui_label_t *self, lh_ui_canvas_t *canvas)
{
    const lh_ui_entity_t *entity = lh_addr_of(self->container.entity);
    const lh_ui_style_t *style = lh_ui_entity_get_style(entity);
    const lh_char_t *text = self->text;
    lh_char_t shortened[256];
    lh_bool_t pushed;

    lh_return_if(!lh_ui_label_can_draw_text(self, canvas));
    /* A text that wraps has no line that runs past the box to shorten. */
    if (lh_null_ne(style) && lh_ui_style_get_overflow(style) == lh_ui_text_overflow_ellipsis &&
        lh_ui_style_get_wrap(style) == lh_ui_text_wrap_none)
    {
        lh_ui_insets_t padding;
        lh_ui_entity_get_padding(entity, lh_addr_of(padding));
        lh_ui_rect_t rect;
        lh_ui_entity_get_rect(entity, lh_addr_of(rect));
        const lh_s32_t room = lh_ui_scalar_floor_s32(
            lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(rect))) -
            lh_ui_insets_get_left(lh_addr_of(padding)) - lh_ui_insets_get_right(lh_addr_of(padding)));

        /* A line that does not fit is drawn from where it starts anyway -- a line wider
           than its box starts at the left whatever the alignment -- so the shortened
           one is drawn from the same place. */
        if (lh_ui_label_shorten(lh_ui_label_get_font(self), self->text, room, shortened,
                                lh_cast_static(lh_u32_t, sizeof(shortened))))
        {
            text = shortened;
        }
    }
    pushed = lh_ui_entity_push_children(entity, canvas);
    if (text == self->text)
    {
        /* Wrapped or not: with no width to wrap at, these are the plain lines. */
        lh_ui_point_t point;
        lh_ui_label_get_text_origin(self, lh_addr_of(point));
        lh_ui_text_draw_wrapped_from(canvas, lh_ui_label_get_font(self), text, point,
                                     lh_ui_label_get_text_color(self), lh_ui_label_get_ink_top(self),
                                     lh_ui_label_get_wrap_width(self), lh_ui_label_get_line_align(self));
    }
    else
    {
        lh_ui_point_t point1;
        lh_ui_label_get_text_origin(self, lh_addr_of(point1));
        lh_ui_text_draw(canvas, lh_ui_label_get_font(self), text, point1,
                        lh_ui_label_get_text_color(self));
    }
    lh_return_if(!pushed);
    lh_ui_canvas_pop(canvas);
}

lh_void
lh_ui_label_on_draw(const lh_ui_label_t *self, const lh_ui_entity_event_t *event)
{
    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_draw);
    lh_ui_label_draw_text(self, lh_ui_entity_event_get_canvas(event));
}

lh_void
lh_ui_label_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event)
{
    /* The container (and its entity) is the first field: self is the label. */
    const lh_ui_label_t *label = lh_ptr_rcast(const lh_ui_label_t, self);

    lh_ui_entity_class_event_base(lh_addr_of(lh_ui_label_class), self, event);
    lh_ui_label_on_children(label, event);
    lh_ui_label_on_baseline(label, event);
    lh_ui_label_on_draw(label, event);
    lh_ui_label_on_measure(label, event);
}
