/**
 * @file fields.h
 * @brief Member fields of ::lh_os_wake_t.
 */

#ifndef LH_OS_WAKE_FIELDS_H
#define LH_OS_WAKE_FIELDS_H

/**
 * @def lh_os_wake_fields(handle_type)
 * @brief Read and write endpoints owned by a pollable wake signal.
 * @param handle_type Native socket handle type.
 */
#define lh_os_wake_fields(handle_type)                                                             \
    handle_type reader;                                                                            \
    handle_type writer

#endif /* LH_OS_WAKE_FIELDS_H */
