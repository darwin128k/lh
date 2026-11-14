/**
 * @file window.h
 * @brief A native window displaying an LH entity tree: ::lh_ui_window_t.
 *
 * Composes the existing OS window, software screen, canvas, view and controller.
 * The caller owns the root and its children; this object owns the five hosting
 * parts. Initialize it with the root, open it in an ::lh_os_app_t and run that
 * application's message loop. Deinitialize it before destroying the entity tree
 * or the OS application. No platform calls or application state are needed here.
 *
 * Input is routed by the standard window/controller/view path. Each timer turn
 * runs the optional application update hook, advances the view and requests a
 * repaint when needed. An OS paint draws a complete frame, including exposure
 * and resize; the existing screen backend presents it. Coordinates are native
 * pixels, without scaling. Closing follows the OS application's existing rules.
 *
 * The native window is accessible for caption/frame configuration. Its input
 * controller, paint, resize and tick slots belong to this composition; replacing
 * them also replaces that part of the hosting contract. Use the update hook for
 * changing application values and widget properties.
 *
 * Typical hosting of an existing root (allocator already configured):
 * @code{.c}
 * lh_os_app_init(&app);
 * lh_ui_window_init(&window, &root);
 * if (lh_ui_window_try_open(&window, &app, "Example", 480, 320))
 * {
 *     lh_os_app_run(&app);
 * }
 * lh_ui_window_deinit(&window);
 * lh_os_app_deinit(&app);
 * @endcode
 * Include lh/os/app.h for the application lifecycle above.
 *
 * Requires UI, OS and OS_WINDOW. Configure the runtime allocator before use.
 */

#ifndef LH_UI_WINDOW_H
#define LH_UI_WINDOW_H

#include <lh/os/window.h>
#include <lh/os/render/backend/gdi.h>
#include <lh/ui/view.h>
#include <lh/ui/window/fields.h>

/**
 * @struct lh_ui_window
 * @brief Owned native host for a borrowed entity tree.
 */
struct lh_ui_window
{
    /** Members: ::lh_ui_window_fields. */
    lh_ui_window_fields(lh_os_window_t, lh_os_render_backend_gdi_context_t, lh_ui_canvas_t,
                        lh_ui_view_t, lh_os_controller_t, lh_os_window_on_tick_cb, lh_ptr);
};
/** @copybrief lh_ui_window */
typedef struct lh_ui_window lh_ui_window_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Read the owned native window (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed member address, never null; valid until deinitialization.
 */
const lh_os_window_t *
lh_ui_window_get_window(const lh_ui_window_t *self);

/**
 * @brief Access the owned native window for configuration (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed mutable member address, never null. Do not destroy it separately.
 */
lh_os_window_t *
lh_ui_window_get_window_mut(lh_ui_window_t *self);

/**
 * @brief Read the owned software screen context (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed member address, never null; valid until deinitialization.
 */
const lh_os_render_backend_gdi_context_t *
lh_ui_window_get_screen(const lh_ui_window_t *self);

/**
 * @brief Access the owned software screen context for configuration (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed mutable member address, never null. Do not destroy it separately.
 */
lh_os_render_backend_gdi_context_t *
lh_ui_window_get_screen_mut(lh_ui_window_t *self);

/**
 * @brief Read the owned canvas (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed member address, never null; valid until deinitialization.
 */
const lh_ui_canvas_t *
lh_ui_window_get_canvas(const lh_ui_window_t *self);

/**
 * @brief Access the owned canvas for configuration (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed mutable member address, never null. Do not destroy it separately.
 */
lh_ui_canvas_t *
lh_ui_window_get_canvas_mut(lh_ui_window_t *self);

/**
 * @brief Read the owned entity-tree view (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed member address, never null; valid until deinitialization.
 */
const lh_ui_view_t *
lh_ui_window_get_view(const lh_ui_window_t *self);

/**
 * @brief Access the owned entity-tree view for configuration (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed mutable member address, never null. Do not destroy it separately.
 */
lh_ui_view_t *
lh_ui_window_get_view_mut(lh_ui_window_t *self);

/**
 * @brief Read the owned input controller (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed member address, never null; valid until deinitialization.
 */
const lh_os_controller_t *
lh_ui_window_get_controller(const lh_ui_window_t *self);

/**
 * @brief Access the owned input controller for configuration (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed mutable member address, never null. Do not destroy it separately.
 */
lh_os_controller_t *
lh_ui_window_get_controller_mut(lh_ui_window_t *self);

/**
 * @brief Read the optional application update callback (level 1).
 * @param self Initialized composition; never null.
 * @return The callback, or null when none is installed.
 */
lh_os_window_on_tick_cb
lh_ui_window_get_on_update_fn(const lh_ui_window_t *self);

/**
 * @brief Store only the application update callback (level 1).
 * @param self Initialized composition; never null.
 * @param on_update Callback, or null to disable it; context is unchanged.
 */
lh_void
lh_ui_window_set_on_update_fn(lh_ui_window_t *self, lh_os_window_on_tick_cb on_update);

/**
 * @brief Read the application update context (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed context pointer; may be null.
 */
lh_ptr
lh_ui_window_get_on_update_context(const lh_ui_window_t *self);

/**
 * @brief Store only the application update context (level 1).
 * @param self Initialized composition; never null.
 * @param context Borrowed context; must outlive callback use. May be null.
 */
lh_void
lh_ui_window_set_on_update_context(lh_ui_window_t *self, lh_ptr context);

/**
 * @brief Set the application update callback and its context.
 *
 * Called on the window thread before view animation and repaint scheduling,
 * once per LH_UI_VIEW_FRAME_MS. The callback receives the native window and
 * context. Change widget properties here; the view records their damage.
 * @param self Initialized composition; never null.
 * @param on_update Optional callback; null disables updates.
 * @param context Borrowed callback context, valid until the hook is removed.
 */
lh_void
lh_ui_window_set_on_update(lh_ui_window_t *self, lh_os_window_on_tick_cb on_update, lh_ptr context);

/**
 * @brief Initialize owned hosting parts and mount a borrowed root.
 * @param self Storage for a new composition; never null, not already initialized.
 * @param root Entity tree with no parent and no other owning view. May be null
 *             for an empty view; must outlive the composition while mounted.
 *
 * No native window is opened. Pairs with deinit even if try_open fails.
 */
lh_void
lh_ui_window_init(lh_ui_window_t *self, lh_ui_entity_t *root);

/**
 * @brief Open the owned native window in an existing application.
 * @param self Initialized composition with no open native window.
 * @param app Borrowed initialized application; outlives the open window.
 * @param title Native window title, copied by the OS; never null.
 * @param width Requested client width in native pixels; positive.
 * @param height Requested client height in native pixels; positive.
 * @return True when opened; false on a native creation failure. On failure the
 *         composition remains initialized and can be retried or deinitialized.
 */
lh_bool_t
lh_ui_window_try_open(lh_ui_window_t *self, struct lh_os_app *app, const char *title, int width,
                      int height);

/**
 * @brief Checked try_open; runtime failure if native creation fails.
 * @param self Initialized composition with no open native window.
 * @param app Initialized application, borrowed while the window is open.
 * @param title Native title, never null.
 * @param width Positive client width in pixels.
 * @param height Positive client height in pixels.
 */
lh_void
lh_ui_window_open(lh_ui_window_t *self, struct lh_os_app *app, const char *title, int width,
                  int height);

/**
 * @brief Whether widget changes or canvas operations require a paint.
 * @param self Initialized composition; never null.
 * @return True for pending view damage or accumulated canvas damage. Checking
 *         does not consume either; scrolling records its damage on the canvas.
 */
lh_bool_t
lh_ui_window_needs_paint(const lh_ui_window_t *self);

/**
 * @brief Run the optional update, view animation and repaint scheduling once.
 * @param self Initialized composition; called on its window thread.
 *
 * The installed native timer calls this automatically; do not tick it twice.
 */
lh_void
lh_ui_window_update(lh_ui_window_t *self);

/**
 * @brief Render the borrowed entity tree into the native paint target.
 * @param self Initialized composition, inside its native window's paint callback.
 *
 * Consumes pending view damage and draws a full frame because an OS exposure
 * can request pixels outside the most recent widget change.
 */
lh_void
lh_ui_window_paint(lh_ui_window_t *self);

/**
 * @brief Native tick callback delegating to update.
 * @param window The owned native window.
 * @param context Its initialized lh_ui_window_t; never null.
 */
lh_void
lh_ui_window_on_tick(lh_os_window_t *window, lh_ptr context);

/**
 * @brief Native paint callback delegating to paint.
 * @param window The owned native window, during painting.
 * @param context Its initialized lh_ui_window_t; never null.
 */
lh_void
lh_ui_window_on_paint(lh_os_window_t *window, lh_ptr context);

/**
 * @brief Cancel in-progress input and request a full paint after resizing.
 * @param window The owned native window.
 * @param width New client width in native pixels.
 * @param height New client height in native pixels.
 * @param context Its initialized lh_ui_window_t; never null.
 */
lh_void
lh_ui_window_on_resize(lh_os_window_t *window, int width, int height, lh_ptr context);

/**
 * @brief Close the native window, release its surface and detach the entity tree.
 * @param self Initialized composition; use exactly once before reinitializing.
 *
 * Cancels active input before detaching the root. Does not destroy the borrowed
 * root or application. The root no longer points to this view afterwards.
 */
lh_void
lh_ui_window_deinit(lh_ui_window_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WINDOW_H */
