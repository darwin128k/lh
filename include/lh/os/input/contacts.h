/**
 * @file contacts.h
 * @brief Which system contact is which pointer id: ::lh_os_input_contacts_t.
 *
 * A system names a touch contact or a pen by an id of its own
 * (::lh_os_input_contact_id_t), any number, for as long as it lasts. An event
 * names it by a pointer id (::lh_os_input_pointer_id_t), and a recognizer or a
 * view keeps one slot per pointer id below ::LH_OS_INPUT_POINTER_MAX. This table
 * is between them: a contact going down takes the lowest free id from `1` up
 * (`0` is the mouse's), keeps it while it moves, and gives it back when it goes up
 * or is cancelled. A contact that finds every id taken gets none, and its events
 * are the backend's to drop.
 *
 * No heap, no locks: one table per input source, read and written by its thread.
 * Needs no ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_INPUT_CONTACTS_H
#define LH_OS_INPUT_CONTACTS_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/os/input/contact/id.h>
#include <lh/os/input/contacts/fields.h>
#include <lh/os/input/contacts/ptr.h>
#include <lh/os/input/device/kind.h>
#include <lh/os/input/pointer/id.h>

/**
 * @struct lh_os_input_contacts
 * @brief The pointer id each live contact of one source goes by.
 */
struct lh_os_input_contacts
{
    /** Members: ::lh_os_input_contacts_fields. */
    lh_os_input_contacts_fields(lh_os_input_contact_id_t, lh_os_input_device_kind_t);
};
/** @copybrief lh_os_input_contacts */
typedef struct lh_os_input_contacts lh_os_input_contacts_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief No contact known (::lh_os_input_contacts_empty_initializer).
 */
lh_void
lh_os_input_contacts_init(lh_os_input_contacts_t *self);

/**
 * @brief The pointer id @p contact goes by, into @p out.
 *
 * @return False, @p out untouched, for a contact @p self does not know: one it
 *         never saw go down, or one that found no free id.
 */
lh_bool_t
lh_os_input_contacts_find(const lh_os_input_contacts_ptr self, lh_os_input_contact_id_t contact,
                          lh_os_input_pointer_id_t *out);

/**
 * @brief The pointer id of @p contact, a @p device_kind going down, into @p out:
 *        the one it already has, else the lowest free one from `1` up.
 *
 * @return False, @p out untouched, when every id from `1` to
 *         ::LH_OS_INPUT_POINTER_MAX `- 1` is taken.
 */
lh_bool_t
lh_os_input_contacts_take(lh_os_input_contacts_t *self, lh_os_input_contact_id_t contact,
                          lh_os_input_device_kind_t device_kind, lh_os_input_pointer_id_t *out);

/**
 * @brief What the contact behind @p pointer is; ::lh_os_input_device_kind_none for
 *        an id no contact has.
 */
lh_os_input_device_kind_t
lh_os_input_contacts_get_device_kind(const lh_os_input_contacts_ptr self,
                                     lh_os_input_pointer_id_t pointer);

/**
 * @brief How many contacts have an id now.
 */
lh_u32_t
lh_os_input_contacts_get_count(const lh_os_input_contacts_ptr self);

/**
 * @brief The contact behind @p pointer is over: the id is free for the next one.
 *        Nothing for an id no contact has.
 */
lh_void
lh_os_input_contacts_release(lh_os_input_contacts_t *self, lh_os_input_pointer_id_t pointer);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_INPUT_CONTACTS_H */
