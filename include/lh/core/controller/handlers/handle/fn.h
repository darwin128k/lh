/**
 * @file fn.h
 * @brief Event handler function type of ::lh_controller_t:
 *        ::lh_controller_handlers_handle_fn.
 *
 * Not a pointer type by itself. ::lh_controller_handlers_handle_cb is the pointer.
 */

#ifndef LH_CONTROLLER_HANDLERS_HANDLE_FN_H
#define LH_CONTROLLER_HANDLERS_HANDLE_FN_H

#include <lh/core/bool.h>
#include <lh/core/input/event/ptr.h>
#include <lh/core/self.h>

/**
 * @typedef lh_controller_handlers_handle_fn
 * @brief Takes one event, raw or synthesized, from ::lh_controller_t.
 *
 * @p self is the controller's context (::lh_controller_get_context): the
 * object the events are for, the view of a screen (`lh_gl_view_controller_handle_event_is_handled`
 * in `lh/gl/view/controller.h`). @p event lives only for the call; copy it to
 * keep it.
 *
 * @return True when @p self acted on @p event (something to repaint, say);
 *         false for an event it has no use for.
 */
typedef lh_bool_t(lh_controller_handlers_handle_fn)(lh_self_ptr self,
                                                    const lh_input_event_ptr event);

#endif /* LH_CONTROLLER_HANDLERS_HANDLE_FN_H */
