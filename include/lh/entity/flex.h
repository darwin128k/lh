/**
 * @file flex.h
 * @brief A container that places its direct children (CSS flex).
 *
 * ::lh_entity_flex_t starts with ::lh_entity_container_t. The container
 * holds the children; this class moves them. Create one with
 * ::lh_entity_flex_class, or derive a widget from that class when the
 * widget itself is the flex box.
 *
 * A child keeps the size it was given. Size 0 on an axis is automatic:
 * the line may stretch it, and a flex container with no size of its own
 * hugs its children. Grow, shrink, basis, align-self and order are kept
 * on this container, one record per child. The defaults are the CSS ones:
 * grow 0, shrink 1, basis auto, align-self auto, order 0.
 *
 * Names follow CSS. `row-gap` is the gap between lines, `column-gap` the
 * gap between items in a row; a column swaps those two. Padding is the
 * inset of the content box.
 *
 * Layout runs from ::lh_entity_screen_render, and ::lh_entity_flex_layout
 * runs it for one container. Hidden children are skipped. There is no
 * min/max size and no baseline alignment.
 */

#ifndef LH_ENTITY_FLEX_H
#define LH_ENTITY_FLEX_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/container.h>
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

/**
 * @struct lh_entity_flex_spec
 * @brief How one child takes space inside its flex container.
 */
struct lh_entity_flex_spec
{
    lh_entity_t *entity;
    lh_int_t grow;
    lh_int_t shrink;
    lh_int_t basis;
    lh_int_t align;
    lh_int_t order;
};

/**
 * @struct lh_entity_flex
 * @brief A ::lh_entity_container_t that places its children.
 *
 * `content_width` and `content_height` are what ::lh_entity_flex_content last
 * measured, and the tag beside each of them is the layout pass that measured
 * it, zero being no pass at all. They exist because measuring a hugging
 * child means measuring its own children, so without them a container nested
 * `n` deep is walked once by every level above it, on top of the walk that
 * level makes for itself. The tag is what makes the reuse sound: layout
 * places a container's children only after every measurement that can reach it has
 * run, because ::lh_entity_flex_layout_tree visits parents first. So the
 * sizes a measurement reads are the ones the pass started with, for the whole
 * pass, and a stale entry is one whose tag is not the current pass. Each
 * axis is tagged apart, because a pass that measured one axis has no
 * measurement of the other to hand back. Both entries belong to a layout
 * pass and to nothing else, so a measurement taken outside one is not kept.
 */
struct lh_entity_flex
{
    lh_entity_container_t container;
    lh_int_t direction;
    lh_int_t wrap;
    lh_int_t justify;
    lh_int_t align_items;
    lh_int_t align_content;
    lh_int_t row_gap;
    lh_int_t column_gap;
    lh_int_t pad_top;
    lh_int_t pad_right;
    lh_int_t pad_bottom;
    lh_int_t pad_left;
    lh_bool_t hug_width;
    lh_bool_t hug_height;
    lh_int_t content_width;
    lh_int_t content_height;
    lh_int_t content_width_tag;
    lh_int_t content_height_tag;
    lh_int_t item_count;
    struct lh_entity_flex_spec item[LH_ENTITY_FLEX_LIMIT];
};
typedef struct lh_entity_flex lh_entity_flex_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_flex_t, derived from ::lh_entity_container_class.
 *
 * A new flex container is a row, does not wrap, packs at the start, and
 * stretches on the cross axis.
 */
extern const lh_entity_class_t lh_entity_flex_class;

/**
 * @brief ::LH_ENTITY_FLEX_ROW and the three other directions.
 */
lh_void
lh_entity_flex_set_direction(lh_entity_flex_t *self, lh_int_t direction);

lh_int_t
lh_entity_flex_get_direction(const lh_entity_flex_t *self);

/**
 * @brief Whether a ::LH_ENTITY_FLEX_DIRECTION lays its children out along the
 *        x axis. The two reversed directions do; the two column ones do not.
 */
lh_bool_t
lh_entity_flex_horizontal(lh_int_t direction);

/**
 * @brief ::LH_ENTITY_FLEX_NOWRAP, ::LH_ENTITY_FLEX_WRAP or
 *        ::LH_ENTITY_FLEX_WRAP_REVERSE.
 */
lh_void
lh_entity_flex_set_wrap(lh_entity_flex_t *self, lh_int_t wrap);

lh_int_t
lh_entity_flex_get_wrap(const lh_entity_flex_t *self);

/**
 * @brief `justify-content`: start, end, center, or one of the space modes.
 */
lh_void
lh_entity_flex_set_justify(lh_entity_flex_t *self, lh_int_t justify);

lh_int_t
lh_entity_flex_get_justify(const lh_entity_flex_t *self);

/**
 * @brief `align-items` for the cross axis.
 */
lh_void
lh_entity_flex_set_align(lh_entity_flex_t *self, lh_int_t align);

lh_int_t
lh_entity_flex_get_align(const lh_entity_flex_t *self);

/**
 * @brief `align-content` when there is more than one line.
 */
lh_void
lh_entity_flex_set_content(lh_entity_flex_t *self, lh_int_t align);

lh_int_t
lh_entity_flex_get_content(const lh_entity_flex_t *self);

/**
 * @brief One gap, in pixels, for both axes (`gap`).
 */
lh_void
lh_entity_flex_set_gap(lh_entity_flex_t *self, lh_int_t gap);

/**
 * @brief `row-gap` and `column-gap`, in pixels.
 */
lh_void
lh_entity_flex_set_gaps(lh_entity_flex_t *self, lh_int_t row_gap, lh_int_t column_gap);

lh_int_t
lh_entity_flex_get_row_gap(const lh_entity_flex_t *self);

lh_int_t
lh_entity_flex_get_column_gap(const lh_entity_flex_t *self);

/**
 * @brief The same padding, in pixels, on every side.
 */
lh_void
lh_entity_flex_set_pad(lh_entity_flex_t *self, lh_int_t pad);

/**
 * @brief Padding of the content box, in pixels: top, right, bottom, left.
 */
lh_void
lh_entity_flex_set_padding(lh_entity_flex_t *self, lh_int_t top, lh_int_t right, lh_int_t bottom,
                           lh_int_t left);

/**
 * @brief `flex-grow` of @p self, stored on its flex parent.
 *        Zero, the initial value, does not take free space.
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
 * @brief `flex-basis` in pixels, or a negative value for the item's size.
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
 * @brief Place the direct children of @p self. No change when @p self is
 *        not a flex container.
 */
lh_void
lh_entity_flex_layout(lh_entity_t *self);

/**
 * @brief The box of @p box along @p horizontal, rounded to whole pixels.
 */
lh_int_t
lh_entity_flex_span(const lh_entity_2d_t *box, lh_bool_t horizontal);

/**
 * @brief What @p self records about @p child, or null when the child was
 *        never given a record, which is what a child with no flex property
 *        set looks like. A record is made by the first
 *        ::lh_entity_flex_item_set_* call on the child.
 */
const struct lh_entity_flex_spec *
lh_entity_flex_spec_at(const lh_entity_flex_t *self, const lh_entity_t *child);

/**
 * @brief How much room @p node's children take along @p horizontal, its own
 *        padding included. A child with a basis takes that basis, a child
 *        that hugs that axis, or that has no size to offer, is measured by
 *        its own content, and the rest contribute their size.
 *
 * The answer is kept on @p node for the rest of the layout pass it was
 * measured in, and re-measured in the next one. Outside a layout there is no
 * pass to keep it for, so it is measured every time. Within a pass it is the
 * same for every caller, which is what makes keeping it safe to read; see
 * ::lh_entity_flex_t for why.
 */
lh_int_t
lh_entity_flex_content(const lh_entity_flex_t *node, lh_bool_t horizontal);

/**
 * @brief What @p child takes along @p horizontal as an item of its parent.
 *        A non-negative @p basis wins, a hugging or unsized flex child is
 *        measured by ::lh_entity_flex_content, and anything else answers its
 *        own size.
 */
lh_int_t
lh_entity_flex_child_span(lh_entity_t *child, lh_bool_t horizontal, lh_int_t basis);

/**
 * @brief Lay out @p root and every flex container under it, parents first.
 */
lh_void
lh_entity_flex_layout_tree(lh_entity_t *root);

/**
 * @brief A box size written by the caller is a definite size.
 *        A size written by layout is not. ::lh_entity_2d_set_size calls this,
 *        and it applies only when @p self is a flex container.
 */
lh_void
lh_entity_flex_note_size(const lh_entity_2d_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_FLEX_H */
