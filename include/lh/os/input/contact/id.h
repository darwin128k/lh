/**
 * @file id.h
 * @brief The system's own name for one touch contact or pen:
 *        ::lh_os_input_contact_id_t.
 */

#ifndef LH_OS_INPUT_CONTACT_ID_H
#define LH_OS_INPUT_CONTACT_ID_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_os_input_contact_id_t
 * @brief The id a system gives a contact for as long as it lasts: a Win32
 *        pointer id, an X11 touch id, an evdev slot. Any value; it is not a
 *        pointer id (::lh_os_input_pointer_id_t), and ::lh_os_input_contacts_t
 *        is what turns one into the other.
 */
typedef lh_u32_t lh_os_input_contact_id_t;

#endif /* LH_OS_INPUT_CONTACT_ID_H */
