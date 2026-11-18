/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_tap_t.
 */

#ifndef LH_GX_TAP_FIELDS_H
#define LH_GX_TAP_FIELDS_H

/**
 * @def lh_gx_tap_fields(bool_type)
 * @brief The state of a tap: whether a press on the entity is in progress.
 *
 * @param bool_type Type of the flag (::lh_bool_t).
 */
#define lh_gx_tap_fields(bool_type) bool_type pressed

#endif /* LH_GX_TAP_FIELDS_H */
