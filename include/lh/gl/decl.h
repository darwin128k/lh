/**
 * @file decl.h
 * @brief A tree written as a table: ::lh_gl_decl_t.
 *
 * The same tree an app builds by hand, written as rows instead of as calls. There
 * is no parser and no markup language: a declaration is a `const` array of
 * ::lh_gl_decl_t that the compiler puts in flash, and ::lh_gl_decl_build places
 * one widget per row into memory the app owns and wires parent to child.
 *
 * **A declaration describes the tree, not the materials.** Styles, texts, masks,
 * layouts and click callbacks are ordinary C that the app already has, and a row
 * points at them — that is deliberate. A markup language would have to invent a
 * spelling for a fill, a shadow and a radius, and then a parser for it; here a
 * style stays what it already is, a call to ::lh_gl_style_assign_fill, and the thing
 * being declared is the one part that is painful to write by hand: who is a child
 * of whom, where it sits, which flow places it.
 *
 * **The app owns the memory.** ::lh_gl_decl_build takes a byte pool and a stride,
 * because `lh` does not own an allocator: a table of rows is data, and the widgets
 * it makes are the app's. Nothing here allocates, parses or frees.
 *
 * The contract is that a tree from a declaration is **indistinguishable** from the
 * same tree written by hand: same rects, same classes, same damage, same picture.
 * That is what the tests compare, and it is the only claim this file makes.
 *
 * A row's parent must be a **lower index**: a child cannot exist before the thing
 * it is a child of. ::lh_gl_decl_build refuses anything else and says which row.
 */

#ifndef LH_GL_DECL_H
#define LH_GL_DECL_H

#include <lh/gl/image.h>
#include <lh/gl/input.h>
#include <lh/gl/scrollbar.h>
#include <lh/gl/toggle.h>
#include <lh/gl/decl/kind.h>
#include <lh/gl/decl/fault.h>
#include <lh/gl/decl/fields.h>

/**
 * @brief @p parent of a row that is a root: no parent, and exactly one row of a
 *        declaration may say it.
 */
#define LH_GL_DECL_ROOT (lh_cast_static(lh_s32_t, (-1)))

/**
 * @struct lh_gl_decl
 * @brief One row: one widget, where it sits, who holds it, and what it is made
 *        of.
 *
 * The fields above `as` are the same for every kind and mean the same thing. What
 * is under `as` depends on the kind, so a button row never carries a mask and a
 * label row never carries a callback. Nothing here is owned except the node the
 * build places: styles, layouts, texts, masks, tints and callbacks all outlive it
 * by the app's own rules, exactly as they do when the tree is written by hand.
 */
struct lh_gl_decl
{
    /** Members: ::lh_gl_decl_fields. */
    lh_gl_decl_fields(lh_gl_decl_kind_t, lh_gl_scalar_t, lh_gl_style_t, lh_gl_place_t,
                      lh_gl_layout_t);
};
/** @copybrief lh_gl_decl */
typedef struct lh_gl_decl lh_gl_decl_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief What this row builds (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
lh_gl_decl_kind_t
lh_gl_decl_get_kind(const lh_gl_decl_t *self);

/**
 * @brief Store what this row builds (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_kind(lh_gl_decl_t *self, lh_gl_decl_kind_t value);

/**
 * @brief The left of the rect, in the parent's space (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
lh_gl_scalar_t
lh_gl_decl_get_x(const lh_gl_decl_t *self);

/**
 * @brief Store the left of the rect, in the parent's space (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_x(lh_gl_decl_t *self, lh_gl_scalar_t value);

/**
 * @brief The top of the rect (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
lh_gl_scalar_t
lh_gl_decl_get_y(const lh_gl_decl_t *self);

/**
 * @brief Store the top of the rect (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_y(lh_gl_decl_t *self, lh_gl_scalar_t value);

/**
 * @brief The width of the rect (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
lh_gl_scalar_t
lh_gl_decl_get_w(const lh_gl_decl_t *self);

/**
 * @brief Store the width of the rect (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_w(lh_gl_decl_t *self, lh_gl_scalar_t value);

/**
 * @brief The height of the rect (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
lh_gl_scalar_t
lh_gl_decl_get_h(const lh_gl_decl_t *self);

/**
 * @brief Store the height of the rect (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_h(lh_gl_decl_t *self, lh_gl_scalar_t value);

/**
 * @brief The row that holds this one, or ::LH_GL_DECL_ROOT (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gl_decl_get_parent(const lh_gl_decl_t *self);

/**
 * @brief Store the row that holds this one, or ::LH_GL_DECL_ROOT (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_parent(lh_gl_decl_t *self, lh_s32_t value);

/**
 * @brief The look of the row (not owned, may be ::lh_null) (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
const lh_gl_style_t *
lh_gl_decl_get_style(const lh_gl_decl_t *self);

/**
 * @brief Store the look of the row (not owned, may be ::lh_null) (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_style(lh_gl_decl_t *self, const lh_gl_style_t *value);

/**
 * @brief What the row wants a flow to give it (not owned, may be ::lh_null) (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
const lh_gl_place_t *
lh_gl_decl_get_place(const lh_gl_decl_t *self);

/**
 * @brief Store what the row wants a flow to give it (not owned, may be ::lh_null) (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_place(lh_gl_decl_t *self, const lh_gl_place_t *value);

/**
 * @brief The flow that places the row's children (not owned, may be ::lh_null) (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
const lh_gl_layout_t *
lh_gl_decl_get_layout(const lh_gl_decl_t *self);

/**
 * @brief Store the flow that places the row's children (not owned, may be ::lh_null) (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_layout(lh_gl_decl_t *self, const lh_gl_layout_t *value);

/**
 * @brief Whether the row is built hidden (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_gl_decl_is_hidden(const lh_gl_decl_t *self);

/**
 * @brief Store whether the row is built hidden (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_hidden(lh_gl_decl_t *self, lh_bool_t value);

/**
 * @brief The caption of a label row (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
const lh_char_t *
lh_gl_decl_get_label_text(const lh_gl_decl_t *self);

/**
 * @brief Store the caption of a label row (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_label_text(lh_gl_decl_t *self, const lh_char_t *value);

/**
 * @brief The look of a button row while the pointer is on it (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
const lh_gl_style_t *
lh_gl_decl_get_button_hot_style(const lh_gl_decl_t *self);

/**
 * @brief Store the look of a button row while the pointer is on it (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_button_hot_style(lh_gl_decl_t *self, const lh_gl_style_t *value);

/**
 * @brief What a click on a button row means (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
lh_gl_button_on_click_cb
lh_gl_decl_get_button_on_click(const lh_gl_decl_t *self);

/**
 * @brief Store what a click on a button row means (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_button_on_click(lh_gl_decl_t *self, lh_gl_button_on_click_cb value);

/**
 * @brief What a button row hands its click callback (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_gl_decl_get_button_click_context(const lh_gl_decl_t *self);

/**
 * @brief Store what a button row hands its click callback (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_button_click_context(lh_gl_decl_t *self, lh_ptr value);

/**
 * @brief The resting look of a toggle row while off (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
const lh_gl_style_t *
lh_gl_decl_get_toggle_off_style(const lh_gl_decl_t *self);

/**
 * @brief Store the resting look of a toggle row while off (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_toggle_off_style(lh_gl_decl_t *self, const lh_gl_style_t *value);

/**
 * @brief The pointer look of a toggle row while off (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
const lh_gl_style_t *
lh_gl_decl_get_toggle_off_hot_style(const lh_gl_decl_t *self);

/**
 * @brief Store the pointer look of a toggle row while off (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_toggle_off_hot_style(lh_gl_decl_t *self, const lh_gl_style_t *value);

/**
 * @brief The resting look of a toggle row while on (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
const lh_gl_style_t *
lh_gl_decl_get_toggle_on_style(const lh_gl_decl_t *self);

/**
 * @brief Store the resting look of a toggle row while on (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_toggle_on_style(lh_gl_decl_t *self, const lh_gl_style_t *value);

/**
 * @brief The pointer look of a toggle row while on (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
const lh_gl_style_t *
lh_gl_decl_get_toggle_on_hot_style(const lh_gl_decl_t *self);

/**
 * @brief Store the pointer look of a toggle row while on (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_toggle_on_hot_style(lh_gl_decl_t *self, const lh_gl_style_t *value);

/**
 * @brief Whether a toggle row starts on (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_gl_decl_is_toggle_checked(const lh_gl_decl_t *self);

/**
 * @brief Store whether a toggle row starts on (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_toggle_checked(lh_gl_decl_t *self, lh_bool_t value);

/**
 * @brief What an image row shows (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
const lh_gl_mask_t *
lh_gl_decl_get_image_mask(const lh_gl_decl_t *self);

/**
 * @brief Store what an image row shows (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_image_mask(lh_gl_decl_t *self, const lh_gl_mask_t *value);

/**
 * @brief The colour an image row shows it in (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
const lh_gl_color_t *
lh_gl_decl_get_image_tint(const lh_gl_decl_t *self);

/**
 * @brief Store the colour an image row shows it in (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_image_tint(lh_gl_decl_t *self, const lh_gl_color_t *value);

/**
 * @brief The row of the container a scrollbar row scrolls (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gl_decl_get_scrollbar_box(const lh_gl_decl_t *self);

/**
 * @brief Store the row of the container a scrollbar row scrolls (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_scrollbar_box(lh_gl_decl_t *self, lh_s32_t value);

/**
 * @brief The axis a scrollbar row scrolls on (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
lh_gl_axis_t
lh_gl_decl_get_scrollbar_axis(const lh_gl_decl_t *self);

/**
 * @brief Store the axis a scrollbar row scrolls on (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_scrollbar_axis(lh_gl_decl_t *self, lh_gl_axis_t value);

/**
 * @brief When a scrollbar row is shown (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
lh_gl_scrollbar_mode_t
lh_gl_decl_get_scrollbar_mode(const lh_gl_decl_t *self);

/**
 * @brief Store when a scrollbar row is shown (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_scrollbar_mode(lh_gl_decl_t *self, lh_gl_scrollbar_mode_t value);

/**
 * @brief The buffer an input row edits (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
lh_char_t *
lh_gl_decl_get_input_buffer(const lh_gl_decl_t *self);

/**
 * @brief Store the buffer an input row edits (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_input_buffer(lh_gl_decl_t *self, lh_char_t *value);

/**
 * @brief The bytes that may be typed into an input row (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
lh_u16_t
lh_gl_decl_get_input_capacity(const lh_gl_decl_t *self);

/**
 * @brief Store the bytes that may be typed into an input row (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_input_capacity(lh_gl_decl_t *self, lh_u16_t value);

/**
 * @brief What an input row starts with (level 1).
 *
 * @param self The row; checked by ::lh_assert_runtime_ref.
 */
const lh_char_t *
lh_gl_decl_get_input_text(const lh_gl_decl_t *self);

/**
 * @brief Store what an input row starts with (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self  The row; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_gl_decl_set_input_text(lh_gl_decl_t *self, const lh_char_t *value);

/** Record @p code at row @p index in @p fault, when the caller asked for one. */
lh_void
lh_gl_decl_fault(lh_gl_decl_fault_t *fault, lh_gl_decl_error_t code, lh_u32_t index);

/** Bytes the widget of @p kind takes in the pool. */
lh_u32_t
lh_gl_decl_size_of(lh_gl_decl_kind_t kind);

/** Everything a row can be wrong on its own, before a single widget is placed: what
   it is, whether a slot can hold it, and what it points at. A build that stopped
   half way through would leave a tree that looks finished and is not. */
lh_gl_decl_error_t
lh_gl_decl_check(const lh_gl_decl_t *decls, lh_u32_t index);

/**
 * @brief The rect of @p self -- `x`, `y`, `w`, `h` -- into @p out (level 2).
 *
 * @param self The row; checked by its getters.
 * @param out  Receives the rect.
 */
lh_void
lh_gl_decl_get_rect(const lh_gl_decl_t *self, lh_gl_rect_t *out);

/**
 * @brief What every kind of row says the same way, onto the @p entity its widget
 *        is: the style (but for a button and a toggle, which aim the entity at
 *        their own style so a press composes on top of the one showing), the
 *        place, the flow of a container, and hidden.
 *
 * @param entity The widget the row built.
 * @param row    The row.
 */
lh_void
lh_gl_decl_apply_row(lh_gl_entity_t *entity, const lh_gl_decl_t *row);

/**
 * @brief Wire every placed row of @p decls to its parent, and give back the root.
 *
 * @param decls The rows.
 * @param count How many.
 * @param nodes The widget of every row (::lh_gl_decl_try_place).
 * @return The widget of the row that is the root.
 */
lh_gl_entity_t *
lh_gl_decl_link(const lh_gl_decl_t *decls, lh_u32_t count, lh_gl_entity_t **nodes);

/**
 * @brief Place a bare entity: the rect and nothing else.
 *
 * Reads only the fields every row has and the block of `as` of its own kind; the
 * style and the place of the entity are ::lh_gl_decl_apply_row's.
 *
 * @param row  The row; its kind is the one this places.
 * @param slot Room for the widget, at least ::lh_gl_decl_size_of bytes, suitably
 *             aligned; the widget is initialised in it.
 * @return The entity of the widget in @p slot.
 */
lh_gl_entity_t *
lh_gl_decl_place_entity(const lh_gl_decl_t *row, lh_byte_ptr slot);

/**
 * @brief Place a container: the rect; its flow is applied with the rest of the row.
 *
 * Reads only the fields every row has and the block of `as` of its own kind; the
 * style and the place of the entity are ::lh_gl_decl_apply_row's.
 *
 * @param row  The row; its kind is the one this places.
 * @param slot Room for the widget, at least ::lh_gl_decl_size_of bytes, suitably
 *             aligned; the widget is initialised in it.
 * @return The entity of the widget in @p slot.
 */
lh_gl_entity_t *
lh_gl_decl_place_container(const lh_gl_decl_t *row, lh_byte_ptr slot);

/**
 * @brief Place a label: the rect and its caption.
 *
 * Reads only the fields every row has and the block of `as` of its own kind; the
 * style and the place of the entity are ::lh_gl_decl_apply_row's.
 *
 * @param row  The row; its kind is the one this places.
 * @param slot Room for the widget, at least ::lh_gl_decl_size_of bytes, suitably
 *             aligned; the widget is initialised in it.
 * @return The entity of the widget in @p slot.
 */
lh_gl_entity_t *
lh_gl_decl_place_label(const lh_gl_decl_t *row, lh_byte_ptr slot);

/**
 * @brief Place a button: the rect, its resting and pointer looks, and its click.
 *
 * Reads only the fields every row has and the block of `as` of its own kind; the
 * style and the place of the entity are ::lh_gl_decl_apply_row's.
 *
 * @param row  The row; its kind is the one this places.
 * @param slot Room for the widget, at least ::lh_gl_decl_size_of bytes, suitably
 *             aligned; the widget is initialised in it.
 * @return The entity of the widget in @p slot.
 */
lh_gl_entity_t *
lh_gl_decl_place_button(const lh_gl_decl_t *row, lh_byte_ptr slot);

/**
 * @brief Place a toggle: the rect, its four looks and the state it starts in.
 *
 * Reads only the fields every row has and the block of `as` of its own kind; the
 * style and the place of the entity are ::lh_gl_decl_apply_row's.
 *
 * @param row  The row; its kind is the one this places.
 * @param slot Room for the widget, at least ::lh_gl_decl_size_of bytes, suitably
 *             aligned; the widget is initialised in it.
 * @return The entity of the widget in @p slot.
 */
lh_gl_entity_t *
lh_gl_decl_place_toggle(const lh_gl_decl_t *row, lh_byte_ptr slot);

/**
 * @brief Place an image: the rect, its mask, and its tint when the row names one.
 *
 * Reads only the fields every row has and the block of `as` of its own kind; the
 * style and the place of the entity are ::lh_gl_decl_apply_row's.
 *
 * @param row  The row; its kind is the one this places.
 * @param slot Room for the widget, at least ::lh_gl_decl_size_of bytes, suitably
 *             aligned; the widget is initialised in it.
 * @return The entity of the widget in @p slot.
 */
lh_gl_entity_t *
lh_gl_decl_place_image(const lh_gl_decl_t *row, lh_byte_ptr slot);

/**
 * @brief Place a scrollbar: the rect, its axis and the container it scrolls (already placed in
 * `nodes`), and its mode when the row names one.
 *
 * Reads only the fields every row has and the block of `as` of its own kind; the
 * style and the place of the entity are ::lh_gl_decl_apply_row's.
 *
 * @param row  The row; its kind is the one this places.
 * @param slot Room for the widget, at least ::lh_gl_decl_size_of bytes, suitably
 *             aligned; the widget is initialised in it.
 * @param nodes The rows placed so far, by row: the container the bar scrolls is one of them
 *              (::lh_gl_decl_check put it lower).
 * @return The entity of the widget in @p slot.
 */
lh_gl_entity_t *
lh_gl_decl_place_scrollbar(const lh_gl_decl_t *row, lh_byte_ptr slot, lh_gl_entity_t **nodes);

/**
 * @brief Place an input: the rect, its buffer and capacity, and the text it starts with.
 *
 * Reads only the fields every row has and the block of `as` of its own kind; the
 * style and the place of the entity are ::lh_gl_decl_apply_row's.
 *
 * @param row  The row; its kind is the one this places.
 * @param slot Room for the widget, at least ::lh_gl_decl_size_of bytes, suitably
 *             aligned; the widget is initialised in it.
 * @return The entity of the widget in @p slot.
 */
lh_gl_entity_t *
lh_gl_decl_place_input(const lh_gl_decl_t *row, lh_byte_ptr slot);

/**
 * @brief Check every row of @p decls on its own (::lh_gl_decl_check) and the
 *        table as a whole, and say how many bytes its widgets take.
 *
 * Nothing is written but @p bytes and @p fault. The table is refused at its first
 * row that is wrong, then when it has no root or more than one.
 *
 * @param decls The rows; checked by the caller.
 * @param count How many.
 * @param bytes Receives the bytes the widgets take, laid end to end
 *              (::lh_gl_decl_size_of); checked by ::lh_assert_runtime_ref.
 * @param fault Receives the row and the reason when the table is refused; may be
 *              ::lh_null.
 * @return ::lh_bool_true when every row and the table are right.
 */
lh_bool_t
lh_gl_decl_try_measure(const lh_gl_decl_t *decls, lh_u32_t count, lh_u32_t *bytes,
                       lh_gl_decl_fault_t *fault);

/**
 * @brief One row into one slot: the component its kind names and that kind's own
 *        fields, and nothing of any other kind (::lh_gl_decl_place_entity ...
 *        ::lh_gl_decl_place_input). The style and the place are the caller's,
 *        because they are the same whatever the widget is.
 *
 * @param row   The row.
 * @param slot  Room for the widget (::lh_gl_decl_size_of bytes).
 * @param nodes The rows placed so far, by row (a scrollbar reads its container).
 * @return The entity of the widget, or ::lh_null for a kind that is not one.
 */
lh_gl_entity_t *
lh_gl_decl_place_row(const lh_gl_decl_t *row, lh_byte_ptr slot, lh_gl_entity_t **nodes);

/**
 * @brief Place every row of @p decls in @p storage, end to end
 *        (::lh_gl_decl_place_row, ::lh_gl_decl_apply_row), each into
 *        `nodes[row]`. Nothing is wired.
 *
 * A scrollbar is built around the container it scrolls: one naming a row that is
 * not a container is refused before its widget is touched.
 *
 * @param decls   The rows, measured (::lh_gl_decl_try_measure).
 * @param count   How many.
 * @param nodes   Receives the widget of every row.
 * @param storage At least the bytes the measure gave.
 * @param fault   Receives the row and the reason when one is refused; may be
 *                ::lh_null.
 * @return ::lh_bool_true when every row was placed.
 */
lh_bool_t
lh_gl_decl_try_place(const lh_gl_decl_t *decls, lh_u32_t count, lh_gl_entity_t **nodes,
                     lh_byte_ptr storage, lh_gl_decl_fault_t *fault);

/**
 * @brief Build every row of @p decls into @p storage and wire parent to child.
 *
 * @param decls   Rows, in any kind. Not owned; may be a static table.
 * @param count   How many rows.
 * @param nodes   Receives the ::lh_gl_entity_t of every row, so a caller can reach
 *                a child later (a scrollbar's container, a row to press). Not owned.
 * @param storage Bytes the widgets are placed in. Not owned, not freed. Rows are
 *                laid **densely**: row N starts where row N-1 ended, sized by the
 *                widget that row builds. There is no stride to pass and therefore
 *                no way to pass one by mistake — the two numbers a caller could
 *                mix up are exactly the two that cost a silent overrun.
 * @param bytes   Size of @p storage. A pool too small for the rows is refused
 *                rather than written past.
 * @param root    Receives the row that is a root, the tree the view takes.
 * @param fault   Receives the row and the reason, when the build refuses. May be
 *                ::lh_null when the app does not want to know.
 *
 * @return ::lh_bool_true when every row was built. A refused build says which row,
 *         and nothing is wired until every row is true, so there is never a
 *         half-built tree that looks finished.
 */
lh_bool_t
lh_gl_decl_build(const lh_gl_decl_t *decls, lh_u32_t count, lh_gl_entity_t **nodes,
                 lh_byte_ptr storage, lh_u32_t bytes, lh_gl_entity_t **root,
                 lh_gl_decl_fault_t *fault);

LH_COMPILER_EXTERN_C_END

/**
 * @brief Bytes one row takes when it is any widget at all: the widest node, so an
 *        app that wants a static pool of a known row count can size it without
 *        measuring anything. Dense packing usually needs less.
 */
#define LH_GL_DECL_NODE_MAX                                                                        \
    (sizeof(lh_gl_toggle_t) > sizeof(lh_gl_input_t) ? sizeof(lh_gl_toggle_t)                       \
                                                    : sizeof(lh_gl_input_t))

#endif /* LH_GL_DECL_H */
