/**
 * @file fields.h
 * @brief Member fields of ::lh_os_mutex_t.
 */

#ifndef LH_OS_MUTEX_FIELDS_H
#define LH_OS_MUTEX_FIELDS_H

/**
 * @def lh_os_mutex_fields(handle_type)
 * @brief Native resource handle owned by the object.
 *
 * @param handle_type Native handle type (::lh_os_system_mutex_handle_t).
 */
#define lh_os_mutex_fields(handle_type) handle_type handle

#endif /* LH_OS_MUTEX_FIELDS_H */
