/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_canvas_web_atlas_t.
 */

#ifndef LH_UI_CANVAS_WEB_ATLAS_FIELDS_H
#define LH_UI_CANVAS_WEB_ATLAS_FIELDS_H

/**
 * @def lh_ui_canvas_web_atlas_fields(count_type, side_type, hash_type, px_type, slot_type, max,
 *                                    slots)
 * @brief Every distinct glyph cover sent so far, by id, with a hash index over them:
 *
 * - `n` — covers held;
 * - `w`, `h`, `len`, `hash`, `px` — width, height, bytes, content hash and the
 *   coverage bytes (owned) of each cover;
 * - `slot` — open addressing by content hash: id + 1, `0` for an empty slot.
 */
#define lh_ui_canvas_web_atlas_fields(count_type, side_type, hash_type, px_type, slot_type, max,   \
                                      slots)                                                       \
    count_type n;                                                                                  \
    side_type w[max];                                                                              \
    side_type h[max];                                                                              \
    count_type len[max];                                                                           \
    hash_type hash[max];                                                                           \
    px_type px[max];                                                                               \
    slot_type slot[slots]

#endif /* LH_UI_CANVAS_WEB_ATLAS_FIELDS_H */
