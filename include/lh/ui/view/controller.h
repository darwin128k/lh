/**
 * @file controller.h
 * @brief ::lh_ui_view_t as the handler of an ::lh_os_controller_t:
 *        ::lh_ui_view_controller_handle and ::lh_ui_view_controller_cancel.
 *
 * The controller (`lh/os/controller.h`) knows nothing of the UI. These two have
 * the shapes of its handlers (::lh_os_controller_handlers_handle_fn,
 * ::lh_os_controller_handlers_cancel_fn) and take the view as `self`, the
 * controller's context.
 *
 * With ::lh_os_controller_flags_gestures the view gets every raw event and the
 * gestures the recognizer makes of them. It acts on the raw ones and ignores the
 * synthesized ones (::lh_ui_view_input), because it makes its own click from
 * press and release: a click is never acted on twice. What the recognizer does
 * change is a vertical wheel with Shift held, which reaches the view already
 * turned sideways when the recognizer's config says so (the default does).
 *
 * Example usage:
 * @code{.c}
 * lh_os_controller_init(lh_addr_of(controller), lh_ui_view_controller_handle, // events
 *                       lh_addr_of(view), lh_null);                           // to the view
 * lh_os_controller_set_cancel_cb(lh_addr_of(controller), lh_ui_view_controller_cancel);
 * @endcode
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_VIEW_CONTROLLER_H
#define LH_UI_VIEW_CONTROLLER_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/input/event/ptr.h>
#include <lh/self.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/view/controller.h requires LH_LIBRARY_OPTION_UI"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief An ::lh_os_controller_handlers_handle_fn: @p event to
 *        ::lh_ui_view_input on @p self, an ::lh_ui_view_t.
 *
 * @return What ::lh_ui_view_input returned: true when the view acted on
 *         @p event, so ::lh_ui_view_take_damage has something to repaint.
 */
lh_bool_t
lh_ui_view_controller_handle(lh_self_ptr self, const lh_os_input_event_ptr event);

/**
 * @brief An ::lh_os_controller_handlers_cancel_fn: ::lh_ui_view_cancel on
 *        @p self, an ::lh_ui_view_t.
 */
lh_void
lh_ui_view_controller_cancel(lh_self_ptr self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_VIEW_CONTROLLER_H */
