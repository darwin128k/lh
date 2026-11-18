/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_field_t.
 */

#ifndef LH_GX_FIELD_FIELDS_H
#define LH_GX_FIELD_FIELDS_H

/**
 * @def lh_gx_field_fields(entity_type, style_type, char_type, size_type, offset_type, field_fn)
 * @brief The four entities of a text field, its three looks, the text and where
 *        in it the cursor is, its reactions and its state.
 *
 * - `box` — the field itself: a box of ::lh_gx_field_class that takes the
 *   pointer and the keys; its content points back at the field;
 * - `view` — the box less its padding: it draws nothing and cuts the text and
 *   the cursor to itself, so a long text scrolls inside it;
 * - `text` — a text (::lh_gx_text_class) whose content is the buffer;
 * - `caret` — the cursor, a thin box shown only while the field has the
 *   focus;
 * - `style` — the box's fill and border and the text's ink and font;
 *   `focus_style` — the box's look while it has the focus; `caret_style` —
 *   the cursor's;
 * - `buffer`, `capacity` — the application's bytes and how many there are,
 *   the `0` included; `length` — how many bytes of text it holds;
 * - `cursor` — the byte offset the cursor stands at, always between two
 *   characters; `scroll` — how many pixels of the text are scrolled off the
 *   left of the view;
 * - `on_change`, `on_submit`, `context` — the application's reactions and
 *   their data;
 * - `focused` — whether the field has the focus.
 *
 * None of the pointers owns: the styles are a theme's, the buffer, the
 * reactions and their context are the application's.
 *
 * @param entity_type Type of an entity (::lh_gx_entity_t).
 * @param style_type  Type of a style (::lh_gx_style_t).
 * @param char_type   Type of a byte of text (::lh_char_t).
 * @param size_type   Type of a size and an offset (::lh_usize_t).
 * @param offset_type Type of a scroll (::lh_s32_t).
 * @param field_fn    Type of a reaction (::lh_gx_field_fn).
 */
#define lh_gx_field_fields(entity_type, style_type, char_type, size_type, offset_type, field_fn)   \
    entity_type box;                                                                               \
    entity_type view;                                                                              \
    entity_type text;                                                                              \
    entity_type caret;                                                                             \
    const style_type *style;                                                                       \
    const style_type *focus_style;                                                                 \
    const style_type *caret_style;                                                                 \
    char_type *buffer;                                                                             \
    size_type capacity;                                                                            \
    size_type length;                                                                              \
    size_type cursor;                                                                              \
    offset_type scroll;                                                                            \
    field_fn *on_change;                                                                           \
    field_fn *on_submit;                                                                           \
    lh_ptr context;                                                                                \
    lh_bool_t focused

#endif /* LH_GX_FIELD_FIELDS_H */
