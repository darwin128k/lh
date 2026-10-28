/**
 * @file align.h
 * @brief ::lh_ui_place_align_t — where a child sits across the flow.
 */

#ifndef LH_UI_LAYOUT_PLACE_ALIGN_H
#define LH_UI_LAYOUT_PLACE_ALIGN_H

/**
 * @enum lh_ui_place_align
 * @brief Where a child sits across the flow.
 *
 * `fill` takes the whole cross size of the content box; `start`, `center` and
 * `end` place a child of its own size at the start, in the middle or at the end.
 *
 * `baseline` is `center` for anything that has no line to stand on and the one
 * answer for everything that has: a row of an icon and a caption is one line of
 * text, and two things centred in the same room are only the same line by
 * accident. A child answers its own baseline
 * (::lh_ui_entity_get_baseline) and every one of them in the row is put down so
 * its baseline lands on the same row — the **deepest** of theirs, which is the row
 * a child of the same box and size would have been centred at anyway, so the row
 * only ever lifts a child up to meet a deeper one and never pushes one below the
 * middle of the room it is in. A child with no baseline is centred, and a
 * **vertical** flow has no cross row to line up and falls back to `center` for
 * everybody, because a baseline runs down a picture and not across one.
 */
typedef enum lh_ui_place_align
{
    lh_ui_place_align_start = 0,
    lh_ui_place_align_center,
    lh_ui_place_align_end,
    lh_ui_place_align_fill,
    lh_ui_place_align_baseline
} lh_ui_place_align_t; /**< Where a child sits across the flow. */

#endif /* LH_UI_LAYOUT_PLACE_ALIGN_H */
