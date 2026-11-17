/**
 * @file fields.h
 * @brief Macro for declaring the rects of ::lh_gx_damage_t and how many are in use.
 */

#ifndef LH_GX_DAMAGE_FIELDS_H
#define LH_GX_DAMAGE_FIELDS_H

/**
 * @def lh_gx_damage_fields(rect_type, count_type)
 * @brief The rects of ::lh_gx_damage_t, and how many of them are in use.
 *
 * `rects` is the fixed table and is never reallocated, so a drawing path can
 * keep a region on its own stack and add to it per primitive. Entries from
 * `count` on hold nothing after ::lh_gx_damage_init.
 *
 * @param rect_type  ::lh_gx_rect_t.
 * @param count_type Unsigned type of the used count.
 */
#define lh_gx_damage_fields(rect_type, count_type)                                                  \
    rect_type rects[LH_GX_DAMAGE_MAX];                                                              \
    count_type count

#endif /* LH_GX_DAMAGE_FIELDS_H */
