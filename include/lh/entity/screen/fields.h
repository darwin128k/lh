/**
 * @file fields.h
 * @brief Member fields ::lh_entity_screen_t adds to a rectangle.
 */

#ifndef LH_ENTITY_SCREEN_FIELDS_H
#define LH_ENTITY_SCREEN_FIELDS_H

#ifndef LH_ENTITY_SCREEN_DIRTY_MAX
/**
 * @def LH_ENTITY_SCREEN_DIRTY_MAX
 * @brief How many separate areas a screen keeps for redrawing before it
 *        merges them all into one. Can be overridden before including.
 */
#    define LH_ENTITY_SCREEN_DIRTY_MAX 16
#endif /* LH_ENTITY_SCREEN_DIRTY_MAX */

/**
 * @def lh_entity_screen_fields(area_type, count_type)
 * @brief The screen areas waiting to be redrawn: `dirty[0 .. dirty_count)`.
 *        Expanded after ::lh_entity_rect_fields.
 *
 * @param area_type  Type of one area (::lh_math_rect_t).
 * @param count_type Type of `dirty_count` (::lh_usize_t).
 */
#define lh_entity_screen_fields(area_type, count_type)                                             \
    area_type dirty[LH_ENTITY_SCREEN_DIRTY_MAX];                                                   \
    count_type dirty_count

#endif /* LH_ENTITY_SCREEN_FIELDS_H */
