/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macros for ::lh_input_contacts_t.
 */

#ifndef LH_INPUT_CONTACTS_INITIALIZER_H
#define LH_INPUT_CONTACTS_INITIALIZER_H

#include <lh/core/bool.h>
#include <lh/core/initializer.h>
#include <lh/core/input/device/kind.h>

/**
 * @def lh_input_contacts_initializer(contacts, device_kinds, taken)
 * @brief Produces a brace-enclosed initializer for ::lh_input_contacts_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_input_contacts_fields; each argument is the brace initializer of one
 * array.
 *
 * Example usage:
 * @code{.c}
 * static lh_input_contacts_t contacts = lh_input_contacts_initializer(
 *     lh_initializer(0U, 7U),                         // contacts
 *     lh_initializer(lh_input_device_kind_none,
 *                    lh_input_device_kind_touch),  // device_kinds
 *     lh_initializer(lh_bool_false, lh_bool_true));   // taken
 * @endcode
 *
 * @see lh_initializer
 * @see lh_input_contacts_t
 */
#define lh_input_contacts_initializer(contacts, device_kinds, taken)                               \
    lh_initializer(contacts, device_kinds, taken)

/**
 * @def lh_input_contacts_empty_initializer()
 * @brief No contact known: what ::lh_input_contacts_init leaves.
 *
 * Example usage:
 * @code{.c}
 * static lh_input_contacts_t contacts = lh_input_contacts_empty_initializer();
 * @endcode
 *
 * @see lh_input_contacts_initializer
 */
#define lh_input_contacts_empty_initializer()                                                      \
    lh_input_contacts_initializer(lh_initializer(0U), lh_initializer(lh_input_device_kind_none),   \
                                  lh_initializer(lh_bool_false))

#endif /* LH_INPUT_CONTACTS_INITIALIZER_H */
