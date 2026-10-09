/**
 * @file label.c
 * @brief Implementation of `lh/ui/label.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/entity/event.h>
#include <lh/ui/entity/transform.h>
#include <lh/ui/label.h>
#include <lh/ui/text.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

/* ── Class ───────────────────────────────────────────────────────────────── */

const lh_ui_entity_class_t lh_ui_label_class = {
    lh_ui_label_event, lh_addr_of(lh_ui_container_class)};

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

lh_void
lh_ui_label_on_draw(const lh_ui_label_t *self, const lh_ui_entity_event_t *event)
{
    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_draw);
    lh_ui_label_draw_text(self, lh_ui_entity_event_get_canvas(event));
}

lh_void
lh_ui_label_on_measure(const lh_ui_label_t *self, const lh_ui_entity_event_t *event)
{
    lh_ui_rect_t *bounds;
    lh_ui_rect_t text;

    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_measure);
    bounds = lh_ui_entity_event_get_bounds(event);
    text = lh_ui_label_get_text_rect(self);
    *bounds = lh_ui_rect_union(bounds, lh_addr_of(text));
}

/* ── Lifetime and fields ─────────────────────────────────────────────────── */

lh_void
lh_ui_label_init(lh_ui_label_t *self, lh_ui_rect_t rect, const lh_char_t *text)
{
    lh_assert_runtime_ref(self);
    lh_ui_container_init(lh_addr_of(self->container), rect);
    self->text = text;
    lh_ui_entity_set_class(lh_ui_label_as_entity(self), lh_addr_of(lh_ui_label_class));
}

lh_ui_entity_t *
lh_ui_label_as_entity(lh_ui_label_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_container_as_entity(lh_addr_of(self->container));
}

lh_ui_container_t *
lh_ui_label_as_container(lh_ui_label_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->container);
}

const lh_char_t *
lh_ui_label_get_text(const lh_ui_label_t *self)
{
    lh_assert_runtime_ref(self);
    return self->text;
}

lh_void
lh_ui_label_set_text(lh_ui_label_t *self, const lh_char_t *text)
{
    lh_assert_runtime_ref(self);
    self->text = text;
}

/* ── Text ────────────────────────────────────────────────────────────────── */

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

lh_ui_point_t
lh_ui_label_get_text_origin(const lh_ui_label_t *self)
{
    const lh_ui_entity_t *entity = lh_addr_of(self->container.entity);
    const lh_ui_insets_t padding = lh_ui_entity_get_padding(entity);
    const lh_ui_style_t *style = lh_ui_entity_get_style(entity);
    const lh_ui_font_t *font = lh_ui_label_get_font(self);
    lh_ui_size_t size;

    /* The padding is what keeps the text off the edges, and the alignment is where
       inside what the padding leaves it starts. Left and top are what this returned
       before a style could say otherwise, so a style that never set one draws the
       label exactly as it did. The size is the ink's
       (::lh_ui_text_get_size), so what is aligned here is the pixels and every
       v_* means what it says: a 22 px line box with 15 px of ink used to be what
       got centred, and the caption sat five rows low. */
    lh_ui_size_init(lh_addr_of(size), 0, 0);
    if (!lh_null_eq(style) && !lh_null_eq(font) && !lh_null_eq(self->text))
    {
        size = lh_ui_text_get_size(font, self->text);
    }
    return lh_ui_text_align_get_origin(lh_addr_of(entity->rect), lh_addr_of(padding), size,
                                       lh_null_eq(style) ? lh_ui_text_align_h_left : lh_ui_style_get_align_h(style),
                                       lh_null_eq(style) ? lh_ui_text_align_v_top : lh_ui_style_get_align_v(style));
}

lh_ui_scalar_t
lh_ui_label_get_baseline(const lh_ui_label_t *self)
{
    const lh_ui_entity_t *entity = lh_addr_of(self->container.entity);
    const lh_ui_font_t *font = lh_ui_label_get_font(self);
    const lh_ui_point_t origin = lh_ui_label_get_text_origin(self);
    const lh_ui_point_t *box = lh_ui_rect_get_origin_as_const(lh_addr_of(entity->rect));
    lh_ui_size_t size;
    lh_ui_scalar_t baseline;

    /* Nothing stands on a line that is not drawn. */
    if (lh_null_eq(font) || lh_null_eq(self->text))
    {
        return lh_ui_scalar(-1);
    }
    size = lh_ui_text_get_size(font, self->text);
    lh_return_if(lh_ui_size_get_height(lh_addr_of(size)) == lh_ui_scalar(0), lh_ui_scalar(-1));
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
               lh_cast_static(lh_ui_scalar_t, lh_ui_font_get_ascent(font)) -
               lh_cast_static(lh_ui_scalar_t, lh_ui_text_get_ink_top(font, self->text));
    return baseline;
}

lh_ui_rect_t
lh_ui_label_get_text_rect(const lh_ui_label_t *self)
{
    const lh_ui_font_t *font = lh_ui_label_get_font(self);
    lh_ui_rect_t empty;

    lh_ui_rect_init_empty(lh_addr_of(empty));
    lh_return_if(lh_null_eq(font) || lh_null_eq(self->text), empty);
    return lh_ui_text_get_rect(font, self->text, lh_ui_label_get_text_origin(self));
}

lh_bool_t
lh_ui_label_can_draw_text(const lh_ui_label_t *self, const lh_ui_canvas_t *canvas)
{
    return lh_null_ne(canvas) && lh_null_ne(self->text) && lh_null_ne(lh_ui_label_get_font(self)) &&
                   lh_null_ne(lh_ui_label_get_text_color(self))
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_ui_label_draw_text(const lh_ui_label_t *self, lh_ui_canvas_t *canvas)
{
    const lh_ui_entity_t *entity = lh_addr_of(self->container.entity);
    lh_bool_t pushed;

    lh_return_if(!lh_ui_label_can_draw_text(self, canvas));
    pushed = lh_ui_entity_push_children(entity, canvas);
    lh_ui_text_draw(canvas, lh_ui_label_get_font(self), self->text, lh_ui_label_get_text_origin(self),
                    lh_ui_label_get_text_color(self));
    lh_return_if(!pushed);
    lh_ui_canvas_pop(canvas);
}
