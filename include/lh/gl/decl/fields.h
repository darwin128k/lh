/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_decl_t.
 */

#ifndef LH_GL_DECL_FIELDS_H
#define LH_GL_DECL_FIELDS_H

/**
 * @def lh_gl_decl_fields(kind_type, scalar_type, style_type, place_type, layout_type)
 * @brief One row: one widget, where it sits, who holds it, and what it is made of.
 *
 * - `kind` -- what this row builds;
 * - `x`, `y`, `w`, `h` -- the rect, in the parent's space (absolute, as everywhere);
 * - `parent` -- the row that holds this one, or ::LH_GL_DECL_ROOT; must be lower;
 * - `style` -- the look (not owned, may be ::lh_null);
 * - `place` -- what the row wants a flow to give it (not owned, may be ::lh_null);
 * - `layout` -- the flow that places the row's children (not owned, may be
 *   ::lh_null: no flow, and its children stay where they were put). One field for
 *   every kind, because a flow belongs to a **container** and a button, a toggle
 *   and a caption all are one; a row whose widget is not a container ignores it;
 * - `as` -- what the row carries, by kind: only the block that matches `kind` is
 *   read. `label.text` the caption; `button.hot_style`, `on_click`,
 *   `click_context` the pointer look and the click; `toggle.off_style`,
 *   `off_hot_style`, `on_style`, `on_hot_style` its four looks and `checked` the
 *   state it starts in; `image.mask` what it shows and `tint` the colour (white
 *   when ::lh_null); `scrollbar.box` the row of the container it scrolls (lower
 *   than this one), `axis` and `mode`; `input.buffer` (at least `capacity` + 1
 *   bytes, outliving the input), `capacity` and `text` what it starts with
 *   (copied in);
 * - `hidden` -- built hidden: it takes no room in its parent's flow and is not
 *   drawn until the app shows it (CSS's `display: none`). Last, so that a table
 *   written before it was here -- every field in order, no names -- is still a
 *   table of rows that are shown.
 *
 * The order is the order a table is written in, field after field: a generated
 * table (`scripts/css2decl.py`) names no field but the block of `as` it fills.
 *
 * @param kind_type   ::lh_gl_decl_kind_t.
 * @param scalar_type ::lh_gl_scalar_t.
 * @param style_type  ::lh_gl_style_t.
 * @param place_type  ::lh_gl_place_t.
 * @param layout_type ::lh_gl_layout_t.
 */
#define lh_gl_decl_fields(kind_type, scalar_type, style_type, place_type, layout_type)             \
    kind_type kind;                                                                                \
    scalar_type x;                                                                                 \
    scalar_type y;                                                                                 \
    scalar_type w;                                                                                 \
    scalar_type h;                                                                                 \
    lh_s32_t parent;                                                                               \
    const style_type *style;                                                                       \
    const place_type *place;                                                                       \
    const layout_type *layout;                                                                     \
    union                                                                                          \
    {                                                                                              \
        struct                                                                                     \
        {                                                                                          \
            const lh_char_t *text;                                                                 \
        } label;                                                                                   \
        struct                                                                                     \
        {                                                                                          \
            const style_type *hot_style;                                                           \
            lh_gl_button_on_click_cb on_click;                                                     \
            lh_ptr click_context;                                                                  \
        } button;                                                                                  \
        struct                                                                                     \
        {                                                                                          \
            const style_type *off_style;                                                           \
            const style_type *off_hot_style;                                                       \
            const style_type *on_style;                                                            \
            const style_type *on_hot_style;                                                        \
            lh_bool_t checked;                                                                     \
        } toggle;                                                                                  \
        struct                                                                                     \
        {                                                                                          \
            const lh_gl_mask_t *mask;                                                              \
            const lh_gl_color_t *tint;                                                             \
        } image;                                                                                   \
        struct                                                                                     \
        {                                                                                          \
            lh_s32_t box;                                                                          \
            lh_gl_axis_t axis;                                                                     \
            lh_gl_scrollbar_mode_t mode;                                                           \
        } scrollbar;                                                                               \
        struct                                                                                     \
        {                                                                                          \
            lh_char_t *buffer;                                                                     \
            lh_u16_t capacity;                                                                     \
            const lh_char_t *text;                                                                 \
        } input;                                                                                   \
    } as;                                                                                          \
    lh_bool_t hidden

#endif /* LH_GL_DECL_FIELDS_H */
