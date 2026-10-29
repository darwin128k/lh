/**
 * @file cb.h
 * @brief ::lh_console_sink_cb — pointer to an ::lh_console_sink_fn.
 */

#ifndef LH_CONSOLE_SINK_CB_H
#define LH_CONSOLE_SINK_CB_H

#include <lh/console/sink/fn.h>
#include <lh/util/ptr.h>

/** @brief Pointer to the sink a reply goes to; ::lh_null drops the reply. */
#define lh_console_sink_cb lh_ptr_of(lh_console_sink_fn)

#endif /* LH_CONSOLE_SINK_CB_H */
