/**
 * @file fn.h
 * @brief Input sink function type: ::lh_input_sink_fn.
 *
 * Not a pointer type by itself. ::lh_input_sink_cb is the pointer.
 */

#ifndef LH_INPUT_SINK_FN_H
#define LH_INPUT_SINK_FN_H

#include <lh/core/input/event/ptr.h>
#include <lh/core/self.h>

/**
 * @typedef lh_input_sink_fn
 * @brief Function type `lh_void(lh_self_ptr, const lh_input_event_ptr)`:
 *        takes one ::lh_input_event_t.
 *
 * The one callback of the input path: a window hands its events to one
 * (::lh_os_window_set_on_input), a queue drains into one
 * (::lh_input_queue_drain) and the recognizer passes events and gestures on
 * to one (::lh_input_recognizer_feed).
 *
 * @p self is the object the sink runs on: the context it was registered or
 * called with (a window's `on_input_context`, the `context` of a drain or of
 * the recognizer, the web host's `ctx`), which the sink casts to its own
 * struct. There is no separate context. @p event lives only for the call;
 * copy it to keep it.
 */
typedef lh_void(lh_input_sink_fn)(lh_self_ptr self, const lh_input_event_ptr event);

#endif /* LH_INPUT_SINK_FN_H */
