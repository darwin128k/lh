/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_entity_t.
 */

#ifndef LH_GX_ENTITY_FIELDS_H
#define LH_GX_ENTITY_FIELDS_H

/**
 * @def lh_gx_entity_fields(kind_type, flags_type, rect_type, style_type, text_type, link_type, damage_fn_type, damage_context_type, user_type)
 * @brief The members of ::lh_gx_entity_t.
 *
 * @param kind_type         ::lh_gx_entity_kind_t.
 * @param flags_type        Unsigned type of the flags.
 * @param rect_type         ::lh_gx_rect_t.
 * @param style_type        Unsigned type of the style index.
 * @param text_type         ::lh_str_view_t.
 * @param link_type         Unsigned type of a link into the tree.
 * @param damage_fn_type    ::lh_gx_entity_damage_fn.
 * @param damage_context_type The listener's own state.
 * @param user_type         The application's own pointer to its own state.
 */
#define lh_gx_entity_fields(kind_type, flags_type, rect_type, style_type, text_type,      \
                             link_type, damage_fn_type, damage_context_type, user_type)    \
    kind_type kind;                                                                       \
    flags_type flags;                                                                     \
    rect_type rect;                                                                       \
    style_type style;                                                                     \
    text_type text;                                                                       \
    link_type parent;                                                                     \
    link_type first_child;                                                                \
    link_type child_count;                                                                \
    link_type next;                                                                       \
    damage_fn_type damage_fn;                                                             \
    damage_context_type damage_context;                                                   \
    user_type user

#endif /* LH_GX_ENTITY_FIELDS_H */
