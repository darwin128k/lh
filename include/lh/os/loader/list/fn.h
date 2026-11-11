/**
 * @file fn.h
 * @brief ::lh_os_loader_list_fn — what ::lh_os_loader_list_try_read calls for
 *        each entry.
 */

#ifndef LH_OS_LOADER_LIST_FN_H
#define LH_OS_LOADER_LIST_FN_H

#include <lh/core/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Take one entry of a module list.
 *
 * @param user  The value given to ::lh_os_loader_list_try_read.
 * @param entry The entry as written, trimmed, never empty; a view into the
 *              list text, valid for the call only.
 */
typedef lh_void(lh_os_loader_list_fn)(lh_ptr user, const lh_str_view_t *entry);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOADER_LIST_FN_H */
