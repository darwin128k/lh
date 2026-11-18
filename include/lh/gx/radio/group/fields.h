/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_radio_group_t.
 */

#ifndef LH_GX_RADIO_GROUP_FIELDS_H
#define LH_GX_RADIO_GROUP_FIELDS_H

/**
 * @def lh_gx_radio_group_fields(radio_type, change_fn)
 * @brief Which radio of the group is chosen, and the application's reaction.
 *
 * - `selected` — the radio chosen, or ::lh_null for none; not owned;
 * - `on_change`, `context` — the application's reaction and its data.
 *
 * @param radio_type Type of a radio (::lh_gx_radio_t).
 * @param change_fn  Type of the reaction (::lh_gx_radio_group_change_fn).
 */
#define lh_gx_radio_group_fields(radio_type, change_fn)                                            \
    radio_type *selected;                                                                          \
    change_fn *on_change;                                                                          \
    lh_ptr context

#endif /* LH_GX_RADIO_GROUP_FIELDS_H */
