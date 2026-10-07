/**
 * @file label.c
 * @brief Implementation of `lh/ui/entity/label.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/entity/label.h>
#include <lh/ui/text.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

/* ── Class ───────────────────────────────────────────────────────────────── */

const lh_ui_entity_class_t lh_ui_entity_label_class = {
    lh_ui_entity_label_event, lh_addr_of(lh_ui_entity_container_class)};

lh_void
lh_ui_entity_label_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event)
{
    /* The container (and its entity) is the first field: self is the label. */
    const lh_ui_entity_label_t *label = lh_ptr_rcast(const lh_ui_entity_label_t, self);

    lh_ui_entity_class_event_base(lh_addr_of(lh_ui_entity_label_class), self, event);
    lh_ui_entity_label_on_draw(label, event);
    lh_ui_entity_label_on_measure(label, event);
}

lh_void
lh_ui_entity_label_on_draw(const lh_ui_entity_label_t *self, const lh_ui_entity_event_t *event)
{
    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_draw);
    lh_ui_entity_label_draw_text(self, lh_ui_entity_event_get_canvas(event));
}

lh_void
lh_ui_entity_label_on_measure(const lh_ui_entity_label_t *self, const lh_ui_entity_event_t *event)
{
    lh_ui_rect_t *bounds;
    lh_ui_rect_t text;

    lh_return_if(lh_ui_entity_event_get_code(event) != lh_ui_entity_event_measure);
    bounds = lh_ui_entity_event_get_bounds(event);
    text = lh_ui_entity_label_get_text_rect(self);
    *bounds = lh_ui_rect_union(bounds, lh_addr_of(text));
}

/* ── Lifetime and fields ─────────────────────────────────────────────────── */

lh_void
lh_ui_entity_label_init(lh_ui_entity_label_t *self, lh_ui_rect_t rect, const lh_char_t *text)
{
    lh_assert_runtime_ref(self);
    lh_ui_entity_container_init(lh_addr_of(self->container), rect);
    self->text = text;
    lh_ui_entity_set_class(lh_ui_entity_label_as_entity(self), lh_addr_of(lh_ui_entity_label_class));
}

lh_ui_entity_t *
lh_ui_entity_label_as_entity(lh_ui_entity_label_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_entity_container_as_entity(lh_addr_of(self->container));
}

lh_ui_entity_container_t *
lh_ui_entity_label_as_container(lh_ui_entity_label_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->container);
}

const lh_char_t *
lh_ui_entity_label_get_text(const lh_ui_entity_label_t *self)
{
    lh_assert_runtime_ref(self);
    return self->text;
}

lh_void
lh_ui_entity_label_set_text(lh_ui_entity_label_t *self, const lh_char_t *text)
{
    lh_assert_runtime_ref(self);
    self->text = text;
}

/* ── Text ────────────────────────────────────────────────────────────────── */

const lh_ui_font_t *
lh_ui_entity_label_get_font(const lh_ui_entity_label_t *self)
{
    const lh_ui_style_t *style = lh_ui_entity_get_style(lh_addr_of(self->container.entity));

    return lh_null_eq(style) ? lh_null : lh_ui_style_get_font(style);
}

const lh_ui_color_t *
lh_ui_entity_label_get_text_color(const lh_ui_entity_label_t *self)
{
    const lh_ui_style_t *style = lh_ui_entity_get_style(lh_addr_of(self->container.entity));

    return lh_null_eq(style) ? lh_null : lh_ui_style_get_text_color(style);
}

lh_ui_point_t
lh_ui_entity_label_get_text_origin(const lh_ui_entity_label_t *self)
{
    const lh_ui_entity_t *entity = lh_addr_of(self->container.entity);
    const lh_ui_insets_t padding = lh_ui_entity_get_padding(entity);
    const lh_ui_style_t *style = lh_ui_entity_get_style(entity);
    const lh_ui_font_t *font = lh_ui_entity_label_get_font(self);
    lh_ui_size_t size;

    /* The padding is what keeps the text off the edges, and the alignment is where
       inside what the padding leaves it starts. Left and top are what this returned
       before a style could say otherwise, so a style that never set one draws the
       label exactly as it did. */
    lh_ui_size_init(lh_addr_of(size), 0, 0);
    if (!lh_null_eq(style) && !lh_null_eq(font) && !lh_null_eq(self->text))
    {
        size = lh_ui_text_get_size(font, self->text);
    }
    return lh_ui_text_align_get_origin(lh_addr_of(entity->rect), lh_addr_of(padding), size,
                                       lh_null_eq(style) ? lh_ui_text_align_h_left : lh_ui_style_get_align_h(style),
                                       lh_null_eq(style) ? lh_ui_text_align_v_top : lh_ui_style_get_align_v(style));
}

lh_ui_rect_t
lh_ui_entity_label_get_text_rect(const lh_ui_entity_label_t *self)
{
    const lh_ui_font_t *font = lh_ui_entity_label_get_font(self);
    lh_ui_rect_t empty;

    lh_ui_rect_init_empty(lh_addr_of(empty));
    lh_return_if(lh_null_eq(font) || lh_null_eq(self->text), empty);
    return lh_ui_text_get_rect(font, self->text, lh_ui_entity_label_get_text_origin(self));
}

lh_bool_t
lh_ui_entity_label_can_draw_text(const lh_ui_entity_label_t *self, const lh_ui_canvas_t *canvas)
{
    return lh_null_ne(canvas) && lh_null_ne(self->text) && lh_null_ne(lh_ui_entity_label_get_font(self)) &&
                   lh_null_ne(lh_ui_entity_label_get_text_color(self))
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_ui_entity_label_draw_text(const lh_ui_entity_label_t *self, lh_ui_canvas_t *canvas)
{
    const lh_ui_entity_t *entity = lh_addr_of(self->container.entity);
    lh_bool_t pushed;

    lh_return_if(!lh_ui_entity_label_can_draw_text(self, canvas));
    pushed = lh_ui_entity_push_children(entity, canvas);
    lh_ui_text_draw(canvas, lh_ui_entity_label_get_font(self), self->text, lh_ui_entity_label_get_text_origin(self),
                    lh_ui_entity_label_get_text_color(self));
    lh_return_if(!pushed);
    lh_ui_canvas_pop(canvas);
}
