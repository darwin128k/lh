/**
 * @file sink.h
 * @brief ::lh_gl_view_t as an input sink: ::lh_gl_view_sink.
 *
 * The input path is a chain of ::lh_input_sink_fn: a window
 * (::lh_os_window_set_on_input), the web host (the `input` of
 * `lh_os_web_host_hooks_t`) or a queue (::lh_input_queue_drain) hands each
 * event to one. ::lh_gl_view_sink is the view at the end of that chain: it has
 * the sink's shape and takes the view as `self`.
 *
 * With a recognizer in front (::lh_os_window_set_recognizer,
 * ::lh_input_recognizer_feed) the view gets every raw event and the gestures
 * made of them. It acts on the raw ones and ignores the synthesized ones
 * (::lh_gl_view_input), because it makes its own click from press and release:
 * a click is never acted on twice. What the recognizer does change is a
 * vertical wheel with Shift held, which reaches the view already turned
 * sideways when the recognizer's config says so (the default does).
 *
 * A focus loss needs no call of its own: the `focus_out` event goes down the
 * same chain, the recognizer cancels the pointers it held and the view lets go
 * of every press (::lh_gl_view_cancel).
 *
 * Example usage:
 * @code{.c}
 * lh_input_recognizer_init(lh_addr_of(recognizer), lh_null);         // default config
 * lh_os_window_set_on_input(lh_addr_of(window), lh_gl_view_sink, lh_addr_of(view));
 * lh_os_window_set_recognizer(lh_addr_of(window), lh_addr_of(recognizer)); // fed, ticked
 * @endcode
 *
 * Requires ::LH_LIBRARY_OPTION_GL.
 */

#ifndef LH_GL_VIEW_SINK_H
#define LH_GL_VIEW_SINK_H

#include <lh/config.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/input/event/ptr.h>
#include <lh/core/self.h>

#if !LH_LIBRARY_OPTION_GL
#    error "lh/gl/view/sink.h requires LH_LIBRARY_OPTION_GL"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief An ::lh_input_sink_fn: @p event to ::lh_gl_view_input on @p self, an
 *        ::lh_gl_view_t.
 *
 * What ::lh_gl_view_input answers is dropped, as a sink returns nothing: what
 * the event changed is in ::lh_gl_view_has_damage and ::lh_gl_view_take_damage.
 *
 * @param self  The view (an ::lh_gl_view_t), the sink's context; checked by
 *              ::lh_gl_view_input on the paths that use it.
 * @param event The event; it lives only for the call. Checked by
 *              ::lh_assert_runtime_ref.
 */
lh_void
lh_gl_view_sink(lh_self_ptr self, const lh_input_event_ptr event);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_VIEW_SINK_H */
