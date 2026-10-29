/**
 * @file fn.h
 * @brief Synchronous event handler function type.
 *
 * A handler runs on the publishing thread, inside ::lh_os_module_event_bus_publish, and may call
 * back into the bus (see there for what is allowed).
 */

#ifndef LH_OS_MODULE_EVENT_FN_H
#define LH_OS_MODULE_EVENT_FN_H

#include <lh/os/module/event/result.h>
#include <lh/compiler/extern/c.h>
#include <lh/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Handle borrowed payload on the publishing thread.
 * @param name Canonical name owned by the bus; valid until bus deinit.
 * @param payload Borrowed publisher-defined data, never copied by the bus.
 * @param context Subscription context, borrowed until its disposal callback runs.
 * @return Ordered delivery result. Only stop interrupts remaining callbacks.
 */
typedef lh_os_module_event_result_t(lh_os_module_event_fn)(const lh_char_t *name, lh_ptr payload,
                                                           lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_MODULE_EVENT_FN_H */
