/**
 * @file fn.h
 * @brief Input sink function type: ::lh_os_input_sink_fn.
 *
 * Not a pointer type by itself. ::lh_os_input_sink_cb is the pointer.
 */

#ifndef LH_OS_INPUT_SINK_FN_H
#define LH_OS_INPUT_SINK_FN_H

#include <lh/os/input/event/ptr.h>
#include <lh/ptr.h>

/**
 * @typedef lh_os_input_sink_fn
 * @brief Takes one ::lh_os_input_event_t.
 *
 * The one callback of the input path: a window hands its events to one
 * (::lh_os_window_set_on_input), a queue drains into one
 * (::lh_os_input_queue_drain) and the recognizer passes events and gestures on
 * to one (::lh_os_input_recognizer_feed). @p event lives only for the call;
 * copy it to keep it. @p context is what the sink was registered with.
 */
typedef lh_void(lh_os_input_sink_fn)(const lh_os_input_event_ptr event, lh_ptr context);

#endif /* LH_OS_INPUT_SINK_FN_H */
