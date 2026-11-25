/**
 * @file window.h
 * @brief A native window displaying an LH entity tree: ::lh_gl_window_t.
 *
 * Composes the existing OS window, software screen, canvas, view and controller.
 * The caller owns the root and its children; this object owns the five hosting
 * parts. Initialize it with the root, open it in an ::lh_os_app_t and run that
 * application's message loop. Deinitialize it before destroying the entity tree
 * or the OS application. No platform calls or application state are needed here.
 *
 * Input is routed by the standard window/controller/view path. Each timer turn
 * runs the optional application update hook, advances the view and requests a
 * repaint of what changed. An OS paint draws the rectangle the system asks for
 * -- the widgets that changed, an exposure, a resize -- and the existing screen
 * backend presents it. A change is handed to the system as its own rectangle
 * (::lh_gl_window_invalidate_damage), so the system's request is what changed
 * and what was uncovered, and nothing else is drawn. Coordinates are native
 * pixels, without scaling. Closing follows the OS application's existing rules.
 *
 * The native window is accessible for caption/frame configuration. Its input
 * controller, paint, resize and tick slots belong to this composition; replacing
 * them also replaces that part of the hosting contract. Use the update hook for
 * changing application values and widget properties.
 *
 * Typical hosting of an existing root (allocator already configured). The
 * application runs on two contracts the host installs; on a desktop the system
 * fills both (`lh/os/app/backend/system.h`, `lh/os/window/backend/system.h`):
 * @code{.c}
 * static lh_os_app_backend_t loop;
 * static lh_os_window_backend_t windows;
 *
 * lh_os_app_init(&app);
 * lh_os_app_backend_system_init(&loop);
 * lh_os_window_backend_system_init(&windows);
 * lh_os_app_install_backend(&app, &loop, lh_null);
 * lh_os_app_install_window_backend(&app, &windows, lh_null);
 * lh_gl_window_init(&window, &root);
 * if (lh_gl_window_try_open(&window, &app, "Example", 480, 320))
 * {
 *     lh_os_app_run(&app);
 * }
 * lh_gl_window_deinit(&window);
 * lh_os_app_deinit(&app);
 * @endcode
 * Include lh/os/app.h for the application lifecycle above.
 *
 * Requires UI, OS and OS_WINDOW. Configure the runtime allocator before use.
 */

#ifndef LH_GL_WINDOW_H
#define LH_GL_WINDOW_H

#include <lh/os/window.h>
#include <lh/os/render/backend/gdi.h>
#include <lh/gl/view.h>
#include <lh/gl/window/fields.h>

/**
 * @struct lh_gl_window
 * @brief Owned native host for a borrowed entity tree.
 */
struct lh_gl_window
{
    /** Members: ::lh_gl_window_fields. */
    lh_gl_window_fields(lh_os_window_t, lh_os_render_backend_gdi_context_t, lh_gl_canvas_t,
                        lh_gl_view_t, lh_os_controller_t, lh_os_window_on_tick_cb, lh_ptr);
};
/** @copybrief lh_gl_window */
typedef struct lh_gl_window lh_gl_window_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Read the owned native window (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed member address, never null; valid until deinitialization.
 */
const lh_os_window_t *
lh_gl_window_get_window(const lh_gl_window_t *self);

/**
 * @brief Access the owned native window for configuration (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed mutable member address, never null. Do not destroy it separately.
 */
lh_os_window_t *
lh_gl_window_get_window_mut(lh_gl_window_t *self);

/**
 * @brief Read the owned software screen context (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed member address, never null; valid until deinitialization.
 */
const lh_os_render_backend_gdi_context_t *
lh_gl_window_get_screen(const lh_gl_window_t *self);

/**
 * @brief Access the owned software screen context for configuration (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed mutable member address, never null. Do not destroy it separately.
 */
lh_os_render_backend_gdi_context_t *
lh_gl_window_get_screen_mut(lh_gl_window_t *self);

/**
 * @brief Read the owned canvas (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed member address, never null; valid until deinitialization.
 */
const lh_gl_canvas_t *
lh_gl_window_get_canvas(const lh_gl_window_t *self);

/**
 * @brief Access the owned canvas for configuration (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed mutable member address, never null. Do not destroy it separately.
 */
lh_gl_canvas_t *
lh_gl_window_get_canvas_mut(lh_gl_window_t *self);

/**
 * @brief Read the owned entity-tree view (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed member address, never null; valid until deinitialization.
 */
const lh_gl_view_t *
lh_gl_window_get_view(const lh_gl_window_t *self);

/**
 * @brief Access the owned entity-tree view for configuration (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed mutable member address, never null. Do not destroy it separately.
 */
lh_gl_view_t *
lh_gl_window_get_view_mut(lh_gl_window_t *self);

/**
 * @brief Read the owned input controller (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed member address, never null; valid until deinitialization.
 */
const lh_os_controller_t *
lh_gl_window_get_controller(const lh_gl_window_t *self);

/**
 * @brief Access the owned input controller for configuration (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed mutable member address, never null. Do not destroy it separately.
 */
lh_os_controller_t *
lh_gl_window_get_controller_mut(lh_gl_window_t *self);

/**
 * @brief Read the optional application update callback (level 1).
 * @param self Initialized composition; never null.
 * @return The callback, or null when none is installed.
 */
lh_os_window_on_tick_cb
lh_gl_window_get_on_update_fn(const lh_gl_window_t *self);

/**
 * @brief Store only the application update callback (level 1).
 * @param self Initialized composition; never null.
 * @param on_update Callback, or null to disable it; context is unchanged.
 */
lh_void
lh_gl_window_set_on_update_fn(lh_gl_window_t *self, lh_os_window_on_tick_cb on_update);

/**
 * @brief Read the application update context (level 1).
 * @param self Initialized composition; never null.
 * @return Borrowed context pointer; may be null.
 */
lh_ptr
lh_gl_window_get_on_update_context(const lh_gl_window_t *self);

/**
 * @brief Store only the application update context (level 1).
 * @param self Initialized composition; never null.
 * @param context Borrowed context; must outlive callback use. May be null.
 */
lh_void
lh_gl_window_set_on_update_context(lh_gl_window_t *self, lh_ptr context);

/**
 * @brief Set the application update callback and its context.
 *
 * Called on the window thread before view animation and repaint scheduling,
 * once per LH_GL_VIEW_FRAME_MS. The callback receives the native window and
 * context. Change widget properties here; the view records their damage.
 * @param self Initialized composition; never null.
 * @param on_update Optional callback; null disables updates.
 * @param context Borrowed callback context, valid until the hook is removed.
 */
lh_void
lh_gl_window_set_on_update(lh_gl_window_t *self, lh_os_window_on_tick_cb on_update, lh_ptr context);

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
lh_gl_window_try_open(lh_gl_window_t *self, struct lh_os_app *app, const char *title, int width,
                      int height);

/**
 * @brief Whether widget changes or canvas operations require a paint.
 * @param self Initialized composition; never null.
 * @return True for pending view damage or accumulated canvas damage. Checking
 *         does not consume either; scrolling records its damage on the canvas.
 */
lh_bool_t
lh_gl_window_needs_paint(const lh_gl_window_t *self);

/**
 * @brief The rectangle the system asks this paint for, from the native paint cycle
 *        (::lh_os_window_get_paint_rect).
 *
 * @param self Initialized composition; never null.
 * @param out  The rectangle in client pixels; never null. Untouched on false.
 * @return False outside a paint cycle, or when the system gave an empty rectangle:
 *         then the paint does not know what was asked and draws everything.
 */
lh_bool_t
lh_gl_window_try_get_paint_area(const lh_gl_window_t *self, lh_gl_rect_t *out);

/**
 * @brief Hand what changed to the system as rectangles to repaint, and forget it
 *        here.
 *
 * The view's pending damage (in client pixels) goes out a rectangle at a time
 * (::lh_os_window_invalidate_rect), each widened to whole pixels; one wholly
 * inside @p painting is skipped, because the paint that is running draws it. The
 * canvas's damage recorded between frames -- a scroll's -- is in the space of the
 * rectangles that moved, not of the window, so it asks for the whole window
 * (::lh_os_window_invalidate). Both are cleared: the system keeps the request
 * until its paint, so nothing that changed is lost between the update and the
 * paint.
 *
 * @param self     Initialized composition; never null.
 * @param painting What the running paint draws, or ::lh_null outside a paint.
 */
lh_void
lh_gl_window_invalidate_damage(lh_gl_window_t *self, const lh_gl_rect_t *painting);

/**
 * @brief Cancel in-progress input and request a full paint after resizing.
 * @param window The owned native window.
 * @param width New client width in native pixels.
 * @param height New client height in native pixels.
 * @param context Its initialized lh_gl_window_t; never null.
 */
lh_void
lh_gl_window_on_resize(lh_os_window_t *window, int width, int height, lh_ptr context);

/**
 * @brief Checked try_open; runtime failure if native creation fails.
 * @param self Initialized composition with no open native window.
 * @param app Initialized application, borrowed while the window is open.
 * @param title Native title, never null.
 * @param width Positive client width in pixels.
 * @param height Positive client height in pixels.
 */
lh_void
lh_gl_window_open(lh_gl_window_t *self, struct lh_os_app *app, const char *title, int width,
                  int height);

/**
 * @brief Run the optional update, view animation and repaint scheduling once.
 * @param self Initialized composition; called on its window thread.
 *
 * The installed native timer calls this automatically; do not tick it twice.
 */
lh_void
lh_gl_window_update(lh_gl_window_t *self);

/**
 * @brief Render the borrowed entity tree into the native paint target.
 * @param self Initialized composition, inside its native window's paint callback.
 *
 * Draws the rectangle the system asks for (::lh_gl_window_try_get_paint_area) --
 * the whole frame when it does not say. A change made since the last update is
 * handed to the system first (::lh_gl_window_invalidate_damage) unless this paint
 * covers it, so it is drawn by the next paint instead of being dropped.
 */
lh_void
lh_gl_window_paint(lh_gl_window_t *self);

/**
 * @brief Close the native window, release its surface and detach the entity tree.
 * @param self Initialized composition; use exactly once before reinitializing.
 *
 * Cancels active input before detaching the root. Does not destroy the borrowed
 * root or application. The root no longer points to this view afterwards.
 */
lh_void
lh_gl_window_deinit(lh_gl_window_t *self);

/**
 * @brief Native tick callback delegating to update.
 * @param window The owned native window.
 * @param context Its initialized lh_gl_window_t; never null.
 */
lh_void
lh_gl_window_on_tick(lh_os_window_t *window, lh_ptr context);

/**
 * @brief Native paint callback delegating to paint.
 * @param window The owned native window, during painting.
 * @param context Its initialized lh_gl_window_t; never null.
 */
lh_void
lh_gl_window_on_paint(lh_os_window_t *window, lh_ptr context);

/**
 * @brief Initialize owned hosting parts and mount a borrowed root.
 * @param self Storage for a new composition; never null, not already initialized.
 * @param root Entity tree with no parent and no other owning view. May be null
 *             for an empty view; must outlive the composition while mounted.
 *
 * No native window is opened. Pairs with deinit even if try_open fails.
 */
lh_void
lh_gl_window_init(lh_gl_window_t *self, lh_gl_entity_t *root);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_WINDOW_H */
