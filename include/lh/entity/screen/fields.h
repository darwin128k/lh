/**
 * @file fields.h
 * @brief Member fields ::lh_entity_screen_t adds to a 2D entity.
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
 * @def lh_entity_screen_fields(area_type, count_type, entity_type)
 * @brief The screen areas waiting to be redrawn: `dirty[0 .. dirty_count)`.
 *        Expanded after ::lh_entity_2d_fields.
 *
 * `pressed` is the entity that received the last pointer press, so the
 * release still reaches it when the pointer has left it. Null when no
 * press is held. `focus` is the entity that receives keys. Null when
 * none.
 *
 * @param area_type   Type of one area (::lh_math_rect_t).
 * @param count_type  Type of `dirty_count` (::lh_usize_t).
 * @param entity_type Type of `pressed` and `focus` (`lh_entity_t *`).
 */
#define lh_entity_screen_fields(area_type, count_type, entity_type)                                \
    area_type dirty[LH_ENTITY_SCREEN_DIRTY_MAX];                                                   \
    count_type dirty_count;                                                                        \
    entity_type pressed;                                                                           \
    entity_type focus

#endif /* LH_ENTITY_SCREEN_FIELDS_H */
