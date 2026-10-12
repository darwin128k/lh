/**
 * @file fields.h
 * @brief Member fields of ::lh_os_serial_t.
 */

#ifndef LH_OS_SERIAL_FIELDS_H
#define LH_OS_SERIAL_FIELDS_H

/**
 * @def lh_os_serial_fields(handle_type)
 * @brief The port's handle; ::LH_OS_SYSTEM_SERIAL_HANDLE_INVALID while closed.
 *
 * @param handle_type ::lh_os_system_serial_handle_t.
 */
#define lh_os_serial_fields(handle_type) handle_type handle

#endif /* LH_OS_SERIAL_FIELDS_H */
