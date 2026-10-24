/**
 * @file access.h
 * @brief What code may do with a range of pages: ::lh_os_memory_access_t.
 *
 * Flags; combine with `|`. Write implies read on every platform lh runs on.
 */

#ifndef LH_OS_MEMORY_ACCESS_H
#define LH_OS_MEMORY_ACCESS_H

#include <lh/byte.h>

/** No access at all. */
#define lh_os_memory_access_none 0U
/** The pages can be read. */
#define lh_os_memory_access_read 1U
/** The pages can be written (and read). */
#define lh_os_memory_access_write 2U
/** Code on the pages can run. */
#define lh_os_memory_access_execute 4U

/**
 * @typedef lh_os_memory_access_t
 * @brief A combination of `lh_os_memory_access_*` flags.
 */
typedef lh_byte_t lh_os_memory_access_t;

#endif /* LH_OS_MEMORY_ACCESS_H */
