/**
 * @file fn.h
 * @brief Subscription context matching function type.
 *
 * A bus search borrows both pointers for one call. Matching must not mutate
 * the bus or release subscription state while the search is in progress.
 */

#ifndef LH_OS_MODULE_EVENT_MATCH_FN_H
#define LH_OS_MODULE_EVENT_MATCH_FN_H

#include <lh/bool.h>
#include <lh/ptr.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/** @brief True when borrowed subscription context matches the borrowed search key. */
typedef lh_bool_t(lh_os_module_event_match_fn)(lh_ptr context, lh_ptr key);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_MODULE_EVENT_MATCH_FN_H */
