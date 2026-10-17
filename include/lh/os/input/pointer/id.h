/**
 * @file id.h
 * @brief Which pointer an event is about: ::lh_os_input_pointer_id_t.
 */

#ifndef LH_OS_INPUT_POINTER_ID_H
#define LH_OS_INPUT_POINTER_ID_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_os_input_pointer_id_t
 * @brief Which pointer an event is about (::lh_os_input_event_get_pointer): `0`
 *        for the mouse, a touch contact or a pen from `1` on. Ids at or above
 *        ::LH_OS_INPUT_POINTER_MAX get no gestures and no view session.
 */
typedef lh_u8_t lh_os_input_pointer_id_t;

#endif /* LH_OS_INPUT_POINTER_ID_H */
