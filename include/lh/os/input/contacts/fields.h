/**
 * @file fields.h
 * @brief Member fields of ::lh_os_input_contacts_t.
 */

#ifndef LH_OS_INPUT_CONTACTS_FIELDS_H
#define LH_OS_INPUT_CONTACTS_FIELDS_H

#include <lh/os/input/pointer.h>

/**
 * @def lh_os_input_contacts_fields
 * @brief One entry per pointer id, `0 ..` ::LH_OS_INPUT_POINTER_MAX `- 1`, the
 *        index being the pointer id:
 *
 * - `contacts` -- the system's id of the contact the pointer id stands for;
 * - `device_kinds` -- what the contact is (touch, pen);
 * - `taken` -- the entry is in use.
 *
 * Entry `0` is never taken: pointer `0` is the mouse's.
 *
 * @param contact_id_type  ::lh_os_input_contact_id_t.
 * @param device_kind_type ::lh_os_input_device_kind_t.
 */
#define lh_os_input_contacts_fields(contact_id_type, device_kind_type)                             \
    contact_id_type contacts[LH_OS_INPUT_POINTER_MAX];                                             \
    device_kind_type device_kinds[LH_OS_INPUT_POINTER_MAX];                                        \
    lh_bool_t taken[LH_OS_INPUT_POINTER_MAX]

#endif /* LH_OS_INPUT_CONTACTS_FIELDS_H */
