/**
 * @file flex.h
 * @brief CSS flex layout for any entity (the `display: flex` of a box).
 *
 * Turn it on for a parent. The parent then places its direct children along
 * one axis and, when asked, wraps them onto the next line. A child keeps the
 * size it was given. Size 0 on an axis means that axis is automatic, the way
 * `width: auto` does: the line may stretch it, and a flex parent with no size
 * of its own hugs its children.
 *
 * Names follow CSS. `row-gap` is the gap between lines, `column-gap` the gap
 * between items in a row; a column swaps those two. Padding is the inset of
 * the content box. An item's grow, shrink, basis, align-self and order live
 * on the child, and the defaults are the CSS ones: grow 0, shrink 1, basis
 * auto, align-self auto, order 0.
 *
 * Layout runs from ::lh_entity_screen_render, and ::lh_entity_flex_layout
 * runs it for one container. Hidden children are skipped (`display: none`).
 * There is no min/max size and no baseline alignment.
 */

#ifndef LH_ENTITY_FLEX_H
#define LH_ENTITY_FLEX_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/numeric/types.h>
#include <lh/void.h>

/**
 * @def LH_ENTITY_FLEX_ROW
 * @brief Main axis left to right. The CSS initial value.
 */
#define LH_ENTITY_FLEX_ROW 0

/**
 * @def LH_ENTITY_FLEX_ROW_REVERSE
 * @brief Main axis right to left.
 */
#define LH_ENTITY_FLEX_ROW_REVERSE 1

/**
 * @def LH_ENTITY_FLEX_COLUMN
 * @brief Main axis top to bottom.
 */
#define LH_ENTITY_FLEX_COLUMN 2

/**
 * @def LH_ENTITY_FLEX_COLUMN_REVERSE
 * @brief Main axis bottom to top.
 */
#define LH_ENTITY_FLEX_COLUMN_REVERSE 3

/**
 * @def LH_ENTITY_FLEX_NOWRAP
 * @brief One line. The CSS initial value.
 */
#define LH_ENTITY_FLEX_NOWRAP 0

/**
 * @def LH_ENTITY_FLEX_WRAP
 * @brief Extra items move to the next line.
 */
#define LH_ENTITY_FLEX_WRAP 1

/**
 * @def LH_ENTITY_FLEX_WRAP_REVERSE
 * @brief Extra items move to the next line, and the lines run backward.
 */
#define LH_ENTITY_FLEX_WRAP_REVERSE 2

/**
 * @def LH_ENTITY_FLEX_START
 * @brief Pack against the start edge (`flex-start`).
 */
#define LH_ENTITY_FLEX_START 0

/**
 * @def LH_ENTITY_FLEX_END
 * @brief Pack against the end edge (`flex-end`).
 */
#define LH_ENTITY_FLEX_END 1

/**
 * @def LH_ENTITY_FLEX_CENTER
 * @brief Pack in the middle.
 */
#define LH_ENTITY_FLEX_CENTER 2

/**
 * @def LH_ENTITY_FLEX_SPACE_BETWEEN
 * @brief First item at the start, last at the end, equal gaps between.
 */
#define LH_ENTITY_FLEX_SPACE_BETWEEN 3

/**
 * @def LH_ENTITY_FLEX_SPACE_AROUND
 * @brief Equal space around each item (half of that at each end).
 */
#define LH_ENTITY_FLEX_SPACE_AROUND 4

/**
 * @def LH_ENTITY_FLEX_SPACE_EVENLY
 * @brief Equal space between items and at both ends.
 */
#define LH_ENTITY_FLEX_SPACE_EVENLY 5

/**
 * @def LH_ENTITY_FLEX_STRETCH
 * @brief Cross size follows the line (`align-items` / `align-content`).
 *        The CSS initial value for both.
 */
#define LH_ENTITY_FLEX_STRETCH 6

/**
 * @def LH_ENTITY_FLEX_AUTO
 * @brief Use the container's align, or the child's own size as its basis.
 */
#define LH_ENTITY_FLEX_AUTO 7

/**
 * @def LH_ENTITY_FLEX_LIMIT
 * @brief Most children one container lays out. Further children stay put.
 */
#define LH_ENTITY_FLEX_LIMIT 64

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Turn flex on or off for @p self. On uses the CSS initial values.
 */
lh_void
lh_entity_flex_set_on(lh_entity_t *self, lh_bool_t on);

/**
 * @brief True when @p self lays its children out.
 */
lh_bool_t
lh_entity_flex_get_on(const lh_entity_t *self);

/**
 * @brief ::LH_ENTITY_FLEX_ROW and the three other directions.
 */
lh_void
lh_entity_flex_set_direction(lh_entity_t *self, lh_int_t direction);

lh_int_t
lh_entity_flex_get_direction(const lh_entity_t *self);

/**
 * @brief ::LH_ENTITY_FLEX_NOWRAP, ::LH_ENTITY_FLEX_WRAP or
 *        ::LH_ENTITY_FLEX_WRAP_REVERSE.
 */
lh_void
lh_entity_flex_set_wrap(lh_entity_t *self, lh_int_t wrap);

lh_int_t
lh_entity_flex_get_wrap(const lh_entity_t *self);

/**
 * @brief `justify-content`: start, end, center, or one of the space modes.
 */
lh_void
lh_entity_flex_set_justify(lh_entity_t *self, lh_int_t justify);

lh_int_t
lh_entity_flex_get_justify(const lh_entity_t *self);

/**
 * @brief `align-items` for the cross axis.
 */
lh_void
lh_entity_flex_set_align(lh_entity_t *self, lh_int_t align);

lh_int_t
lh_entity_flex_get_align(const lh_entity_t *self);

/**
 * @brief `align-content` when there is more than one line.
 */
lh_void
lh_entity_flex_set_content(lh_entity_t *self, lh_int_t align);

lh_int_t
lh_entity_flex_get_content(const lh_entity_t *self);

/**
 * @brief One gap, in pixels, for both axes (`gap`).
 */
lh_void
lh_entity_flex_set_gap(lh_entity_t *self, lh_int_t gap);

/**
 * @brief `row-gap` and `column-gap`, in pixels.
 */
lh_void
lh_entity_flex_set_gaps(lh_entity_t *self, lh_int_t row_gap, lh_int_t column_gap);

lh_int_t
lh_entity_flex_get_row_gap(const lh_entity_t *self);

lh_int_t
lh_entity_flex_get_column_gap(const lh_entity_t *self);

/**
 * @brief The same padding, in pixels, on every side.
 */
lh_void
lh_entity_flex_set_pad(lh_entity_t *self, lh_int_t pad);

/**
 * @brief Padding of the content box, in pixels: top, right, bottom, left.
 */
lh_void
lh_entity_flex_set_padding(lh_entity_t *self, lh_int_t top, lh_int_t right, lh_int_t bottom,
                           lh_int_t left);

/**
 * @brief `flex-grow`. Zero, the initial value, does not take free space.
 */
lh_void
lh_entity_flex_item_set_grow(lh_entity_t *self, lh_int_t grow);

lh_int_t
lh_entity_flex_item_get_grow(const lh_entity_t *self);

/**
 * @brief `flex-shrink`. One, the initial value, lets the item get smaller.
 */
lh_void
lh_entity_flex_item_set_shrink(lh_entity_t *self, lh_int_t shrink);

lh_int_t
lh_entity_flex_item_get_shrink(const lh_entity_t *self);

/**
 * @brief `flex-basis` in pixels, or ::LH_ENTITY_FLEX_AUTO for the item's size.
 */
lh_void
lh_entity_flex_item_set_basis(lh_entity_t *self, lh_int_t basis);

lh_int_t
lh_entity_flex_item_get_basis(const lh_entity_t *self);

/**
 * @brief `align-self`, or ::LH_ENTITY_FLEX_AUTO to use the container.
 */
lh_void
lh_entity_flex_item_set_align(lh_entity_t *self, lh_int_t align);

lh_int_t
lh_entity_flex_item_get_align(const lh_entity_t *self);

/**
 * @brief `order`. Lower comes first. Zero is the initial value.
 */
lh_void
lh_entity_flex_item_set_order(lh_entity_t *self, lh_int_t order);

lh_int_t
lh_entity_flex_item_get_order(const lh_entity_t *self);

/**
 * @brief Place the direct children of @p self. No change when flex is off.
 */
lh_void
lh_entity_flex_layout(lh_entity_t *self);

/**
 * @brief Lay out @p root and every flex container under it, parents first.
 */
lh_void
lh_entity_flex_layout_tree(lh_entity_t *root);

/**
 * @brief A box size written by the caller is a definite size (`width: 100px`).
 *        A size written by layout is not. ::lh_entity_2d_set_size calls this.
 */
lh_void
lh_entity_flex_note_size(const lh_entity_2d_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_FLEX_H */
