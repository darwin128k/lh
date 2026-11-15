/**
 * @file fields.h
 * @brief Member fields of ::lh_os_input_sample_t.
 */

#ifndef LH_OS_INPUT_SAMPLE_FIELDS_H
#define LH_OS_INPUT_SAMPLE_FIELDS_H

/**
 * @def lh_os_input_sample_fields
 * @brief One contact a driver read in this frame:
 *
 * - `contact` — the driver's own id, stable while the finger is down;
 * - `x`, `y` — client position, Q24.8.
 *
 * A contact that is in the frame is down. One the next frame omits went up.
 *
 * @param contact_id_type ::lh_os_input_contact_id_t.
 * @param coord_type      ::lh_os_input_coord_t.
 */
#define lh_os_input_sample_fields(contact_id_type, coord_type)                                     \
    contact_id_type contact;                                                                       \
    coord_type x;                                                                                  \
    coord_type y

#endif /* LH_OS_INPUT_SAMPLE_FIELDS_H */
