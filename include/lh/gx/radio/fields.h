/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_radio_t.
 */

#ifndef LH_GX_RADIO_FIELDS_H
#define LH_GX_RADIO_FIELDS_H

/**
 * @def lh_gx_radio_fields(checkbox_type, group_type)
 * @brief What a radio is made of: a checkbox and the group it is in.
 *
 * - `checkbox` — the entities, the looks, the state and the tap
 *   (::lh_gx_checkbox_t); its row is of ::lh_gx_radio_class and its content
 *   points back at the radio;
 * - `group` — the group (::lh_gx_radio_group_t); not owned.
 *
 * @param checkbox_type Type of the checkbox (::lh_gx_checkbox_t).
 * @param group_type    Type of the group (::lh_gx_radio_group_t).
 */
#define lh_gx_radio_fields(checkbox_type, group_type)                                              \
    checkbox_type checkbox;                                                                        \
    group_type *group

#endif /* LH_GX_RADIO_FIELDS_H */
