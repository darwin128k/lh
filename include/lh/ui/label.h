/**
 * @file label.h
 * @brief An entity that shows text: ::lh_ui_label_t.
 *
 * The label embeds an ::lh_ui_container_t and adds a string. Use the
 * entity API on ::lh_ui_label_as_entity for rect, style, class,
 * children, and draw. The string is not copied and not owned. Its class is
 * ::lh_ui_label_class, which extends ::lh_ui_container_class.
 *
 * The text is drawn on the draw event with the style font and text color
 * (`lh/ui/text.h`), from the top-left of the rect, inside the same canvas
 * push as the children: the container scroll moves it and its rect cuts it.
 * On the measure event the text rect joins the content bounds, so long text
 * makes the container scroll.
 */

#ifndef LH_UI_LABEL_H
#define LH_UI_LABEL_H

#include <lh/ui/container.h>
#include <lh/ui/label/fields.h>

/**
 * @struct lh_ui_entity_label
 * @brief A container and the text it shows.
 */
struct lh_ui_entity_label
{
    /** Members: ::lh_ui_label_fields. */
    lh_ui_label_fields(lh_ui_container_t, lh_char_t);
};
/** @copybrief lh_ui_entity_label */
typedef struct lh_ui_entity_label lh_ui_label_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_ui_label_t, derived from
 *        ::lh_ui_container_class.
 */
extern const lh_ui_entity_class_t lh_ui_label_class;

/**
 * @brief Event function of ::lh_ui_label_class: the container class
 *        first (fill and scrolling kept), then the draw and measure handlers
 *        below. A derived class may call it directly.
 */
lh_void
lh_ui_label_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event);

/**
 * @brief On ::lh_ui_entity_event_draw: ::lh_ui_label_draw_text.
 *        Other events are ignored.
 */
lh_void
lh_ui_label_on_draw(const lh_ui_label_t *self, const lh_ui_entity_event_t *event);

/**
 * @brief Where the text of @p self stands: rows down from the top of the
 *        entity's rect to the baseline under its first line, or -1 when there is
 *        no text or no font to draw it in.
 *
 * This is the line a row lines up on (::lh_ui_place_align_baseline), and it is
 * not the bottom of the box: the text is aligned inside it, so a label centred in
 * 28 rows has its baseline at 20 and one at the top has it at 12.
 *
 * The baseline is where the **tallest letter of this run** ends, not the cap height
 * and not the line box. The two agree on most captions and disagree on the ones
 * with an ascender in them, which is the point: measured on the demo's "Hide panel"
 * the ink starts 5 rows below the cap line with an ascent of 17, so the baseline is
 * 12 rows down and the cap height is 12 as well; measured on a font whose 'd' rises
 * a row above the cap line (::lh_test::cap_font: line 8, ascent 6, cap 4) a 'f' gives
 * 4 and a 'd' gives 5, and taking the cap height would stand a row's icon below the
 * letters.
 */
lh_ui_scalar_t
lh_ui_label_get_baseline(const lh_ui_label_t *self);

/**
 * @brief On ::lh_ui_entity_event_children: take the offset and the placement the
 *        container behind @p self just made and take the clip back off. Other
 *        events are ignored.
 *
 * A container cuts its children to its own rect, which is right for a button and
 * wrong for a label: the label's rect is where its text is centred
 * (::lh_ui_text_get_size measures the cap line to the baseline), so the tail of a
 * 'p' hangs below it and a clip there draws half a letter.
 */
lh_void
lh_ui_label_on_children(const lh_ui_label_t *self, const lh_ui_entity_event_t *event);

/**
 * @brief On ::lh_ui_entity_event_baseline: answer ::lh_ui_label_get_baseline.
 *        Other events are ignored.
 */
lh_void
lh_ui_label_on_baseline(const lh_ui_label_t *self, const lh_ui_entity_event_t *event);

/**
 * @brief On ::lh_ui_entity_event_measure: grow the bounds by
 *        ::lh_ui_label_get_text_rect. Other events are ignored.
 */
lh_void
lh_ui_label_on_measure(const lh_ui_label_t *self, const lh_ui_entity_event_t *event);

/**
 * @brief The FNV-1a hash of @p text and its length in @p length: what a label
 *        compares to tell whether new text is the text it already shows.
 *
 * Two short strings that collide on both are not a case a label meets. A
 * ::lh_null @p text is the empty string.
 */
lh_u32_t
lh_ui_label_hash_text(const lh_char_t *text, lh_u32_t *length);

/**
 * @brief Fill @p self so it covers @p rect and shows @p text.
 *
 * @p text is not copied. ::lh_null shows nothing. Style starts as ::lh_null,
 * so nothing is drawn until a style with a font and a text color is set.
 */
lh_void
lh_ui_label_init(lh_ui_label_t *self, lh_ui_rect_t rect, const lh_char_t *text);

/**
 * @brief The entity @p self embeds (through its container).
 */
lh_ui_entity_t *
lh_ui_label_as_entity(lh_ui_label_t *self);

/**
 * @brief The container @p self embeds.
 */
lh_ui_container_t *
lh_ui_label_as_container(lh_ui_label_t *self);

/**
 * @brief The string @p self shows, or ::lh_null when it has none.
 */
const lh_char_t *
lh_ui_label_get_text(const lh_ui_label_t *self);

/** The ink of the current text, relative to the label's own origin, so it stays right
   when the label moves before its text changes again. */
lh_void
lh_ui_label_get_ink_reach(lh_ui_label_t *self, lh_ui_rect_t *out);

/**
 * @brief Point @p self at @p text. @p text is not copied.
 *
 * ::lh_null clears the text. The same bytes again record nothing: a poll that
 * rewrites a cell with the number it already shows does not ask the window to
 * paint it. The bytes are told apart by a hash, because the caller may
 * overwrite the buffer @p self already points at before this call, and the
 * previous bytes are gone by then.
 */
lh_void
lh_ui_label_set_text_v(lh_ui_label_t *self, const lh_char_t *text);

/**
 * @brief Forget the remembered ink of @p self.
 *
 * A field edits its buffer in place and never calls ::lh_ui_label_set_text_v,
 * because the label already points at that buffer. The next draw measures
 * again.
 */
lh_void
lh_ui_label_drop_metrics(lh_ui_label_t *self);

/**
 * @brief Font of the style of @p self, or ::lh_null (no style or no font).
 */
const lh_ui_font_t *
lh_ui_label_get_font(const lh_ui_label_t *self);

/**
 * @brief Solid text color of the style of @p self, or ::lh_null.
 */
const lh_ui_color_t *
lh_ui_label_get_text_color(const lh_ui_label_t *self);

/**
 * @brief Where the first line starts: the top-left of the rect of @p self,
 *        moved in by the left and top style padding (::lh_ui_entity_get_padding).
 */
lh_void
lh_ui_label_get_text_origin(const lh_ui_label_t *self, lh_ui_point_t *out);

/** How wide the lines of the text may be: the padded box when the style wraps, and 0
   -- no width, lines end at line breaks only -- when it does not. */
lh_ui_scalar_t
lh_ui_label_get_wrap_width(const lh_ui_label_t *self);

/** Where each line of a wrapped text sits inside the widest: the alignment the text
   is placed by. */
lh_ui_text_align_h_t
lh_ui_label_get_line_align(const lh_ui_label_t *self);

/** True when @p a and @p b keep the same distances off every edge. */
lh_bool_t
lh_ui_label_same_insets(const lh_ui_insets_t *a, const lh_ui_insets_t *b);

/** @p self with what its text measures worked out: from the metrics fields when
   nothing they were worked out from has changed, else measured again and kept.

   Measuring is laying the text out glyph by glyph, wrapped or not, and every frame
   asks for it several times over -- whether the label shows at all, where its text
   starts, where its line is, what its ink covers -- of every label, drawn or not.
   It is the same answer each time until the text (::lh_ui_label_set_text_v drops it),
   the font, the box, the padding, the alignment or the wrapping width changes, and
   those are what is compared here. The label is const to its callers because none
   of this is what it looks like; only the remembering writes. */
const lh_ui_label_t *
lh_ui_label_measured(const lh_ui_label_t *self);

/**
 * @brief First ink row of the text of @p self below the top of its first line
 *        box (::lh_ui_text_get_ink_top_wrapped), measured once with its size.
 */
lh_ui_scalar_t
lh_ui_label_get_ink_top(const lh_ui_label_t *self);

/**
 * @brief The rect the text covers (unscrolled); empty without text or font.
 */
lh_void
lh_ui_label_get_text_rect(const lh_ui_label_t *self, lh_ui_rect_t *out);

/**
 * @brief True when there is a canvas, text, a font and a text color.
 */
lh_bool_t
lh_ui_label_can_draw_text(const lh_ui_label_t *self, const lh_ui_canvas_t *canvas);

/** The longest start of @p text that fits in @p room with an ellipsis after it, written
   into @p out with the ellipsis, or `lh_bool_false` when the whole line fits and is to
   be drawn as it is.

   The ellipsis is the font's own `…` when it has one and three dots when it does not:
   a box of a glyph nobody baked is worse than three full stops. The cut is between
   code points, never inside one, and a start that would not fit in @p out is cut
   shorter rather than written past it. */
lh_bool_t
lh_ui_label_shorten(const lh_ui_font_t *font, const lh_char_t *text, lh_s32_t room, lh_char_t *out,
                    lh_u32_t size);

/**
 * @brief Draw the text of @p self inside ::lh_ui_entity_push_children (scroll
 *        offset and rect clip), then pop.
 */
lh_void
lh_ui_label_draw_text(const lh_ui_label_t *self, lh_ui_canvas_t *canvas);

/**
 * @brief The container the label is built on (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_container_t *
lh_ui_label_get_container(const lh_ui_label_t *self);

/**
 * @brief The container the label is built on, to change in place (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_container_t *
lh_ui_label_get_container_mut(lh_ui_label_t *self);

/**
 * @brief Store the container the label is built on (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @param container The new value.
 */
lh_void
lh_ui_label_set_container(lh_ui_label_t *self, lh_ui_container_t container);

/**
 * @brief Whether the cached text metrics are current (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_ui_label_is_metrics_ready(const lh_ui_label_t *self);

/**
 * @brief Store whether the cached text metrics are current (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @param metrics_ready The new value.
 */
lh_void
lh_ui_label_set_metrics_ready(lh_ui_label_t *self, lh_bool_t metrics_ready);

/**
 * @brief Whether the text hash is current (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_ui_label_is_text_hashed(const lh_ui_label_t *self);

/**
 * @brief Store whether the text hash is current (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @param text_hashed The new value.
 */
lh_void
lh_ui_label_set_text_hashed(lh_ui_label_t *self, lh_bool_t text_hashed);

/**
 * @brief The hash of the text the metrics were measured for (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_ui_label_get_text_hash(const lh_ui_label_t *self);

/**
 * @brief Store the hash of the text the metrics were measured for (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @param text_hash The new value.
 */
lh_void
lh_ui_label_set_text_hash(lh_ui_label_t *self, lh_u32_t text_hash);

/**
 * @brief The length of the text the metrics were measured for (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 */
lh_u32_t
lh_ui_label_get_text_length(const lh_ui_label_t *self);

/**
 * @brief Store the length of the text the metrics were measured for (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @param text_length The new value.
 */
lh_void
lh_ui_label_set_text_length(lh_ui_label_t *self, lh_u32_t text_length);

/**
 * @brief The rectangle the text was last drawn in (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_rect_t *
lh_ui_label_get_text_noted(const lh_ui_label_t *self);

/**
 * @brief The rectangle the text was last drawn in, to change in place (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_rect_t *
lh_ui_label_get_text_noted_mut(lh_ui_label_t *self);

/**
 * @brief Store the rectangle the text was last drawn in (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @param text_noted The new value.
 */
lh_void
lh_ui_label_set_text_noted(lh_ui_label_t *self, lh_ui_rect_t text_noted);

/**
 * @brief The font the metrics were measured with (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 */
const struct lh_ui_font *
lh_ui_label_get_metrics_font(const lh_ui_label_t *self);

/**
 * @brief Store the font the metrics were measured with (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @param metrics_font The new value.
 */
lh_void
lh_ui_label_set_metrics_font(lh_ui_label_t *self, const struct lh_ui_font * metrics_font);

/**
 * @brief The rectangle the metrics were measured in (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_rect_t *
lh_ui_label_get_metrics_rect(const lh_ui_label_t *self);

/**
 * @brief The rectangle the metrics were measured in, to change in place (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_rect_t *
lh_ui_label_get_metrics_rect_mut(lh_ui_label_t *self);

/**
 * @brief Store the rectangle the metrics were measured in (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @param metrics_rect The new value.
 */
lh_void
lh_ui_label_set_metrics_rect(lh_ui_label_t *self, lh_ui_rect_t metrics_rect);

/**
 * @brief The padding the metrics were measured with (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_insets_t *
lh_ui_label_get_metrics_padding(const lh_ui_label_t *self);

/**
 * @brief The padding the metrics were measured with, to change in place (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_insets_t *
lh_ui_label_get_metrics_padding_mut(lh_ui_label_t *self);

/**
 * @brief Store the padding the metrics were measured with (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @param metrics_padding The new value.
 */
lh_void
lh_ui_label_set_metrics_padding(lh_ui_label_t *self, lh_ui_insets_t metrics_padding);

/**
 * @brief The horizontal alignment the metrics were measured with (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 */
lh_ui_text_align_h_t
lh_ui_label_get_metrics_align_h(const lh_ui_label_t *self);

/**
 * @brief Store the horizontal alignment the metrics were measured with (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @param metrics_align_h The new value.
 */
lh_void
lh_ui_label_set_metrics_align_h(lh_ui_label_t *self, lh_ui_text_align_h_t metrics_align_h);

/**
 * @brief The vertical alignment the metrics were measured with (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 */
lh_ui_text_align_v_t
lh_ui_label_get_metrics_align_v(const lh_ui_label_t *self);

/**
 * @brief Store the vertical alignment the metrics were measured with (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @param metrics_align_v The new value.
 */
lh_void
lh_ui_label_set_metrics_align_v(lh_ui_label_t *self, lh_ui_text_align_v_t metrics_align_v);

/**
 * @brief Where the text starts (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_point_t *
lh_ui_label_get_metrics_origin(const lh_ui_label_t *self);

/**
 * @brief Where the text starts, to change in place (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_point_t *
lh_ui_label_get_metrics_origin_mut(lh_ui_label_t *self);

/**
 * @brief Store where the text starts (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @param metrics_origin The new value.
 */
lh_void
lh_ui_label_set_metrics_origin(lh_ui_label_t *self, lh_ui_point_t metrics_origin);

/**
 * @brief The top of the ink of the text (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_label_get_metrics_ink_top(const lh_ui_label_t *self);

/**
 * @brief Store the top of the ink of the text (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @param metrics_ink_top The new value.
 */
lh_void
lh_ui_label_set_metrics_ink_top(lh_ui_label_t *self, lh_ui_scalar_t metrics_ink_top);

/**
 * @brief The width the text was wrapped to (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 */
lh_ui_scalar_t
lh_ui_label_get_metrics_wrap(const lh_ui_label_t *self);

/**
 * @brief Store the width the text was wrapped to (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @param metrics_wrap The new value.
 */
lh_void
lh_ui_label_set_metrics_wrap(lh_ui_label_t *self, lh_ui_scalar_t metrics_wrap);

/**
 * @brief The rectangle the ink of the text covers (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_rect_t *
lh_ui_label_get_metrics_ink(const lh_ui_label_t *self);

/**
 * @brief The rectangle the ink of the text covers, to change in place (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_rect_t *
lh_ui_label_get_metrics_ink_mut(lh_ui_label_t *self);

/**
 * @brief Store the rectangle the ink of the text covers (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @param metrics_ink The new value.
 */
lh_void
lh_ui_label_set_metrics_ink(lh_ui_label_t *self, lh_ui_rect_t metrics_ink);

/**
 * @brief The size of the text (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_size_t *
lh_ui_label_get_metrics_size(const lh_ui_label_t *self);

/**
 * @brief The size of the text, to change in place (level 1).
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_size_t *
lh_ui_label_get_metrics_size_mut(lh_ui_label_t *self);

/**
 * @brief Store the size of the text (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @param metrics_size The new value.
 */
lh_void
lh_ui_label_set_metrics_size(lh_ui_label_t *self, lh_ui_size_t metrics_size);

/**
 * @brief Store the text the label shows (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The label; checked by ::lh_assert_runtime_ref.
 * @param text The new value.
 */
lh_void
lh_ui_label_set_text(lh_ui_label_t *self, const lh_char_t * text);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_LABEL_H */
