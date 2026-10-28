/**
 * @file size.h
 * @brief ::lh_ui_place_size_t — how much room a child takes along the flow.
 */

#ifndef LH_UI_LAYOUT_PLACE_SIZE_H
#define LH_UI_LAYOUT_PLACE_SIZE_H

/**
 * @enum lh_ui_place_size
 * @brief How much room a child takes along the flow.
 *
 * - `fixed` — the length it carries (::lh_ui_place_get_size).
 * - `fill` — a share of what is left over after the others, in proportion to its
 *   weight (::lh_ui_place_get_size; 0 weighs the same as 1, so a fill that says
 *   nothing takes an equal share, the way every fill did before there were
 *   weights).
 * - `wrap` — its own content, so nothing to say and nothing to fill.
 */
typedef enum lh_ui_place_size
{
    lh_ui_place_size_fixed = 0,
    lh_ui_place_size_fill,
    lh_ui_place_size_wrap
} lh_ui_place_size_t; /**< How much room a child takes along the flow. */

#endif /* LH_UI_LAYOUT_PLACE_SIZE_H */
