/**
 * @file place.h
 * @brief How one child wants to be placed by its parent's flow: ::lh_ui_place_t.
 *
 * The flow (::lh_ui_layout_t) says which way children run and how much room is
 * left over; this says what **this** child wants, and it travels with the child
 * because "how wide am I" is a fact about the child, not about whoever happens
 * to hold it. Two questions, two answers:
 *
 * - along the flow: `fixed` (::lh_ui_place_size_fixed, the length given here),
 *   `fill` (a share of what is left, by weight), or `wrap` (ask the child for its content,
 *   ::lh_ui_entity_get_content_bounds). A child with no content measures empty,
 *   so **`wrap` is what makes an empty label collapse** — and once it is gone,
 *   the leftover room goes where ::lh_ui_layout_t says, which is how a button
 *   with no text ends up with its picture in the middle.
 * - across the flow: ::lh_ui_place_align_start / _center / _end / _fill. Only
 *   `fill` needs a length from the parent; the rest are placed by what is left.
 *
 * Around those two, a **margin** the flow keeps clear on each side of the child,
 * and a **minimum** and a **maximum** it will not be sized past. They are what
 * CSS calls `margin`, `min-width` and `max-height`, and they are here because a
 * stylesheet says them about a child and not about the flow it is in.
 *
 * Not owned by anything: it is a value the entity carries, set through
 * ::lh_ui_entity_set_place. An entity that says nothing keeps the rect it was
 * given, because a flow only runs for a parent that has one.
 */

#ifndef LH_UI_LAYOUT_PLACE_H
#define LH_UI_LAYOUT_PLACE_H

#include <lh/ui/insets.h>
#include <lh/ui/layout/place/fields.h>
#include <lh/ui/layout/place/size.h>
#include <lh/ui/layout/place/align.h>

/**
 * @struct lh_ui_place
 * @brief Room along the flow and a place across it.
 */
struct lh_ui_place
{
    /** Members: ::lh_ui_place_fields. */
    lh_ui_place_fields(lh_ui_place_size_t, lh_ui_scalar_t, lh_ui_place_align_t, lh_ui_insets_t,
                       lh_ui_size_t);
};
/** @copybrief lh_ui_place */
typedef struct lh_ui_place lh_ui_place_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p size_mode @p size of the flow, at ::lh_ui_place_align_start, with no
 *        margin and no minimum or maximum.
 */
lh_void
lh_ui_place_init(lh_ui_place_t *self, lh_ui_place_size_t size_mode, lh_ui_scalar_t size);

/**
 * @brief @p self with room along the flow of @p size_mode and length @p size.
 */
lh_void
lh_ui_place_set_sizing(lh_ui_place_t *self, lh_ui_place_size_t size_mode, lh_ui_scalar_t size);

/**
 * @brief How much room @p self takes along the flow.
 */
lh_ui_place_size_t
lh_ui_place_get_size_mode(const lh_ui_place_t *self);

/**
 * @brief The length @p self takes when it is ::lh_ui_place_size_fixed, and its
 *        weight when it is ::lh_ui_place_size_fill.
 */
lh_ui_scalar_t
lh_ui_place_get_size(const lh_ui_place_t *self);

/**
 * @brief Put @p self across the flow at @p align.
 */
lh_void
lh_ui_place_set_align(lh_ui_place_t *self, lh_ui_place_align_t align);

/**
 * @brief Where @p self sits across the flow.
 */
lh_ui_place_align_t
lh_ui_place_get_align(const lh_ui_place_t *self);

/**
 * @brief Keep @p margin clear around @p self in its parent's flow (copied).
 *
 * Along the flow the margin is room the child takes on either side of its own
 * length; across it, it narrows the room the child is placed in, so a child that
 * fills is that much smaller and one that is centred is centred in what is left.
 */
lh_void
lh_ui_place_assign_margin(lh_ui_place_t *self, const lh_ui_insets_t *margin);

/**
 * @brief The room @p self keeps clear around it.
 */
const lh_ui_insets_t *
lh_ui_place_get_margin(const lh_ui_place_t *self);

/**
 * @brief The least @p self is sized to by a flow, on each axis (0: no least).
 */
lh_void
lh_ui_place_set_min_size(lh_ui_place_t *self, lh_ui_scalar_t width, lh_ui_scalar_t height);

/**
 * @brief The least @p self is sized to.
 */
lh_void
lh_ui_place_copy_min(const lh_ui_place_t *self, lh_ui_size_t *out);

/**
 * @brief The most @p self is sized to by a flow, on each axis (0: no most).
 *
 * A child held at its maximum keeps the room it gave up empty: the others are not
 * given it again. That is one pass where CSS takes as many as it needs, and it is
 * the same picture whenever the limit is not met.
 */
lh_void
lh_ui_place_set_max_size(lh_ui_place_t *self, lh_ui_scalar_t width, lh_ui_scalar_t height);

/**
 * @brief The most @p self is sized to.
 */
lh_void
lh_ui_place_copy_max(const lh_ui_place_t *self, lh_ui_size_t *out);

/**
 * @brief @p length along @p axis, held between the minimum and the maximum of
 *        @p self on that axis.
 */
lh_ui_scalar_t
lh_ui_place_clamp(const lh_ui_place_t *self, lh_ui_axis_t axis, lh_ui_scalar_t length);

/**
 * @brief Store how the size of the entity is chosen (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The placement; checked by ::lh_assert_runtime_ref.
 * @param size_mode The new value.
 */
lh_void
lh_ui_place_set_size_mode(lh_ui_place_t *self, lh_ui_place_size_t size_mode);

/**
 * @brief The margin around the entity, to change in place (level 1).
 *
 * @param self The placement; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_insets_t *
lh_ui_place_get_margin_mut(lh_ui_place_t *self);

/**
 * @brief The smallest size the entity takes, to change in place (level 1).
 *
 * @param self The placement; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_size_t *
lh_ui_place_get_min_mut(lh_ui_place_t *self);

/**
 * @brief The largest size the entity takes, to change in place (level 1).
 *
 * @param self The placement; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_size_t *
lh_ui_place_get_max_mut(lh_ui_place_t *self);

/**
 * @brief Store the size the size mode works with (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The placement; checked by ::lh_assert_runtime_ref.
 * @param size The new value.
 */
lh_void
lh_ui_place_set_size(lh_ui_place_t *self, lh_ui_scalar_t size);

/**
 * @brief The smallest size the entity takes (level 1).
 *
 * @param self The placement; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_size_t *
lh_ui_place_get_min(const lh_ui_place_t *self);

/**
 * @brief Store the smallest size the entity takes (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The placement; checked by ::lh_assert_runtime_ref.
 * @param min The new value.
 */
lh_void
lh_ui_place_set_min(lh_ui_place_t *self, lh_ui_size_t min);

/**
 * @brief The largest size the entity takes (level 1).
 *
 * @param self The placement; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_size_t *
lh_ui_place_get_max(const lh_ui_place_t *self);

/**
 * @brief Store the largest size the entity takes (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The placement; checked by ::lh_assert_runtime_ref.
 * @param max The new value.
 */
lh_void
lh_ui_place_set_max(lh_ui_place_t *self, lh_ui_size_t max);

/**
 * @brief Store the margin around the entity (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The placement; checked by ::lh_assert_runtime_ref.
 * @param margin The new value.
 */
lh_void
lh_ui_place_set_margin(lh_ui_place_t *self, lh_ui_insets_t margin);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_LAYOUT_PLACE_H */