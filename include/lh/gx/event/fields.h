/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_event_t.
 */

#ifndef LH_GX_EVENT_FIELDS_H
#define LH_GX_EVENT_FIELDS_H

/**
 * @def lh_gx_event_fields(kind_type, point_type, key_type, code_type)
 * @brief What happened, and where or with what.
 *
 * `kind` is what happened (::lh_gx_event_kind_t); `at` is where the pointer is,
 * in screen coordinates — the same as an entity's rectangle on the screen
 * (::lh_gx_entity_get_screen_rect), so a class compares the two directly.
 * `key` is the key of a key event (::lh_gx_key_none for every other kind);
 * `code` is the Unicode code point of a text event (`0` for every other kind).
 *
 * @param kind_type  Type of the kind (::lh_gx_event_kind_t).
 * @param point_type Type of the point (::lh_gx_point_t).
 * @param key_type   Type of the key (::lh_gx_key_t).
 * @param code_type  Type of the code point (::lh_u32_t).
 */
#define lh_gx_event_fields(kind_type, point_type, key_type, code_type)                             \
    kind_type kind;                                                                                \
    point_type at;                                                                                 \
    key_type key;                                                                                  \
    code_type code

#endif /* LH_GX_EVENT_FIELDS_H */
