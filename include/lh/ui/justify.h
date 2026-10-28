/**
 * @file justify.h
 * @brief ::lh_ui_justify_t — where the leftover room goes along the flow.
 */

#ifndef LH_UI_JUSTIFY_H
#define LH_UI_JUSTIFY_H

/**
 * @enum lh_ui_justify
 * @brief Where the leftover room goes along the flow.
 *
 * Only asked when no child took it: a child that fills (::lh_ui_place_size_fill)
 * gets what is left, and this is what centres or ends the row when none does —
 * a button with a picture and no text, for instance.
 *
 * The `space_*` three hand it out **between** the children instead, the way CSS's
 * `justify-content` does: `between` puts it all between them and none at the ends
 * (one child is at the start), `around` gives each child the same room on both of
 * its sides, so the ends have half of what is between, and `evenly` makes every
 * space the same, the ends included. The gap is still kept between children, and
 * the leftover comes on top of it.
 */
typedef enum lh_ui_justify
{
    lh_ui_justify_start = 0,
    lh_ui_justify_center,
    lh_ui_justify_end,
    lh_ui_justify_space_between,
    lh_ui_justify_space_around,
    lh_ui_justify_space_evenly
} lh_ui_justify_t; /**< Where the leftover room goes along the flow. */

#endif /* LH_UI_JUSTIFY_H */
