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
 *   `fill` (share what is left), or `wrap` (ask the child for its content,
 *   ::lh_ui_entity_get_content_bounds). A child with no content measures empty,
 *   so **`wrap` is what makes an empty label collapse** — and once it is gone,
 *   the leftover room goes where ::lh_ui_layout_t says, which is how a button
 *   with no text ends up with its picture in the middle.
 * - across the flow: ::lh_ui_place_align_start / _center / _end / _fill. Only
 *   `fill` needs a length from the parent; the rest are placed by what is left.
 *
 * Not owned by anything: it is a value the entity carries, set through
 * ::lh_ui_entity_set_place. An entity that says nothing keeps the rect it was
 * given, because a flow only runs for a parent that has one.
 */

#ifndef LH_UI_LAYOUT_PLACE_H
#define LH_UI_LAYOUT_PLACE_H

#include <lh/compiler/extern/c.h>
#include <lh/ui/layout/place/fields.h>
#include <lh/ui/scalar.h>
#include <lh/void.h>

/**
 * @typedef lh_ui_place_size_t
 * @brief How much room a child takes along the flow.
 *
 * - `fixed` — the length it carries (::lh_ui_place_get_size).
 * - `fill` — a share of what is left over after the others.
 * - `wrap` — its own content, so nothing to say and nothing to fill.
 */
typedef enum lh_ui_place_size
{
    lh_ui_place_size_fixed = 0,
    lh_ui_place_size_fill,
    lh_ui_place_size_wrap
} lh_ui_place_size_t;

/**
 * @typedef lh_ui_place_align_t
 * @brief Where a child sits across the flow.
 *
 * `fill` takes the whole cross size of the content box; the three others place
 * a child of its own size at the start, in the middle or at the end.
 */
typedef enum lh_ui_place_align
{
    lh_ui_place_align_start = 0,
    lh_ui_place_align_center,
    lh_ui_place_align_end,
    lh_ui_place_align_fill
} lh_ui_place_align_t;

/**
 * @struct lh_ui_place
 * @typedef lh_ui_place_t
 * @brief Room along the flow and a place across it.
 */
struct lh_ui_place
{
    lh_ui_place_fields(lh_ui_place_size_t, lh_ui_scalar_t, lh_ui_place_align_t);
};
typedef struct lh_ui_place lh_ui_place_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief @p size_mode @p size of the flow, at ::lh_ui_place_align_start.
 */
lh_void
lh_ui_place_init(lh_ui_place_t *self, lh_ui_place_size_t size_mode, lh_ui_scalar_t size);

/**
 * @brief @p self with room along the flow of @p size_mode and length @p size.
 */
lh_void
lh_ui_place_set_size(lh_ui_place_t *self, lh_ui_place_size_t size_mode, lh_ui_scalar_t size);

/**
 * @brief How much room @p self takes along the flow.
 */
lh_ui_place_size_t
lh_ui_place_get_size_mode(const lh_ui_place_t *self);

/**
 * @brief The length @p self takes when it is ::lh_ui_place_size_fixed.
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

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_LAYOUT_PLACE_H */