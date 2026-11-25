/**
 * @file id.h
 * @brief The system's own name for one touch contact or pen:
 *        ::lh_input_contact_id_t.
 */

#ifndef LH_INPUT_CONTACT_ID_H
#define LH_INPUT_CONTACT_ID_H

#include <lh/core/numeric/fixed/types.h>

/**
 * @typedef lh_input_contact_id_t
 * @brief The id a system gives a contact for as long as it lasts: a Win32
 *        pointer id, an X11 touch id, an evdev slot. Any value; it is not a
 *        pointer id (::lh_input_pointer_id_t), and ::lh_input_contacts_t
 *        is what turns one into the other.
 */
typedef lh_u32_t lh_input_contact_id_t;

#endif /* LH_INPUT_CONTACT_ID_H */
