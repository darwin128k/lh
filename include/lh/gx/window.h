/**
 * @file window.h
 * @brief A gx screen in a desktop window: ::lh_gx_window_t.
 *
 * The composition that puts a gx tree in a window of the operating system,
 * built on the contracts on both sides of it:
 *
 * - the window is the system's (`lh/os/window.h`), opened through the window
 *   backend the application installed — Win32, X11, AppKit or a test's;
 * - the pixels are a system surface the window's size
 *   (`lh/os/system/surface.h`), and the software backend (`lh/gx/backend/sw.h`)
 *   draws into them;
 * - the window fills a present table (`lh/gx/present.h`): a frame drawn asks
 *   the system to repaint only the rectangles that changed, and a paint copies
 *   the surface — clipped by the system to what it asked for;
 * - the window's input is translated (`lh/gx/window/input.h`) and delivered
 *   (`lh/gx/input.h`) to a pointer and a focus.
 *
 * A frame is drawn on the window's tick and right after an input event, so a
 * press shows at once; a paint only copies pixels already drawn, so a window
 * uncovered by another is repainted without drawing anything. A resize makes a
 * new surface and draws the whole screen again.
 *
 * Threads: the window's callbacks run on the application's thread, so the tree
 * is touched there and only there, reactions included.
 *
 * Ownership: the record is the caller's and must not move after
 * ::lh_gx_window_init; ::lh_gx_window_deinit releases the surface and the
 * window. The entities attached to the screen are the caller's.
 */

#ifndef LH_GX_WINDOW_H
#define LH_GX_WINDOW_H

#include <lh/gx/backend/sw.h>
#include <lh/gx/focus.h>
#include <lh/gx/pointer.h>
#include <lh/gx/present.h>
#include <lh/gx/window/fields.h>
#include <lh/os/system/surface.h>
#include <lh/os/window.h>

/**
 * @def LH_GX_WINDOW_FRAME_MS
 * @brief How often the window looks for a change to draw, in milliseconds.
 */
#define LH_GX_WINDOW_FRAME_MS 16U

/**
 * @struct lh_gx_window
 * @brief A gx screen in a window. Members: ::lh_gx_window_fields.
 */
struct lh_gx_window
{
    /** Members: ::lh_gx_window_fields. */
    lh_gx_window_fields(lh_os_window_t, lh_os_system_surface_handle_t, lh_memory_bounds_t,
                        lh_gx_pixmap_t, lh_gx_backend_sw_t, lh_gx_canvas_t, lh_gx_render_t,
                        lh_gx_present_t, lh_gx_pointer_t, lh_gx_focus_t);
};
/** @copybrief lh_gx_window */
typedef struct lh_gx_window lh_gx_window_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The system's window (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_os_window_t *
lh_gx_window_get_window_mut(lh_gx_window_t *self);

/**
 * @brief The system's surface the pixels live in (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @return The handle, ::LH_OS_SYSTEM_SURFACE_HANDLE_INVALID before the window is
 *         open.
 */
lh_os_system_surface_handle_t
lh_gx_window_get_surface(const lh_gx_window_t *self);

/**
 * @brief Store the surface (level 1); nothing is drawn or released.
 *
 * @param self    The window; checked by ::lh_assert_runtime_ref.
 * @param surface The handle.
 */
lh_void
lh_gx_window_set_surface(lh_gx_window_t *self, lh_os_system_surface_handle_t surface);

/**
 * @brief The bounds of the surface's pixels (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_memory_bounds_t *
lh_gx_window_get_bits_mut(lh_gx_window_t *self);

/**
 * @brief The surface's pixels as gx sees them (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_pixmap_t *
lh_gx_window_get_pixmap(const lh_gx_window_t *self);

/**
 * @brief The surface's pixels, to set up (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_pixmap_t *
lh_gx_window_get_pixmap_mut(lh_gx_window_t *self);

/**
 * @brief The software backend drawing into the pixels (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_backend_sw_t *
lh_gx_window_get_sw_mut(lh_gx_window_t *self);

/**
 * @brief The backend's canvas table (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_canvas_t *
lh_gx_window_get_canvas_mut(lh_gx_window_t *self);

/**
 * @brief The renderer of the screen (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_render_t *
lh_gx_window_get_render(const lh_gx_window_t *self);

/**
 * @brief The renderer, to draw with (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_render_t *
lh_gx_window_get_render_mut(lh_gx_window_t *self);

/**
 * @brief The window's present table (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_present_t *
lh_gx_window_get_present(const lh_gx_window_t *self);

/**
 * @brief The window's present table, to fill (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_present_t *
lh_gx_window_get_present_mut(lh_gx_window_t *self);

/**
 * @brief The pointer the window's input drives (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_pointer_t *
lh_gx_window_get_pointer_mut(lh_gx_window_t *self);

/**
 * @brief Where the window's keys go (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_focus_t *
lh_gx_window_get_focus(const lh_gx_window_t *self);

/**
 * @brief Where the window's keys go, to move (level 1).
 *
 * @param self The window; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_focus_t *
lh_gx_window_get_focus_mut(lh_gx_window_t *self);

/**
 * @brief The screen: the root the caller attaches its entities to.
 *
 * @param self The window; must not be null.
 * @return The renderer's screen, never ::lh_null.
 */
lh_gx_entity_t *
lh_gx_window_get_screen_mut(lh_gx_window_t *self);

/**
 * @brief The window's present (::lh_gx_present_show_fn): ask the system to
 *        repaint each rectangle drawn.
 *
 * @param context The window (::lh_gx_window_t).
 * @param pixels  The surface's pixels; not read — the paint copies them.
 * @param at      Where they are: `(0, 0)`, the surface is the screen.
 * @param drawn   The rectangles drawn.
 */
lh_void
lh_gx_window_show(lh_ptr context, const lh_gx_pixmap_t *pixels, lh_gx_point_t at,
                  const lh_gx_damage_t *drawn);

/**
 * @brief Draw what changed into the surface and ask the system to show it
 *        (::lh_gx_present_draw); nothing without a surface.
 *
 * @param self The window; must not be null.
 * @return ::lh_bool_true when something was drawn.
 */
lh_bool_t
lh_gx_window_update(lh_gx_window_t *self);

/**
 * @brief Copy the surface to the window's paint target; the system clips the
 *        copy to what it asked to repaint. Nothing is drawn.
 *
 * @param self The window; must not be null, and in a paint.
 */
lh_void
lh_gx_window_paint(lh_gx_window_t *self);

/**
 * @brief Release the surface and the system's window. The entities are the
 *        caller's and are not touched.
 *
 * @param self The window; must not be null.
 */
lh_void
lh_gx_window_deinit(lh_gx_window_t *self);

/**
 * @brief Make the surface @p width by @p height: release the old one, create a
 *        new one, point the pixels and the backend at it, size the screen to it
 *        and mark it all to be drawn.
 *
 * @param self   The window; must not be null.
 * @param width  Pixels; at least `1`.
 * @param height Pixels; at least `1`.
 * @return ::lh_bool_false when the system gave no surface: the window then has
 *         none and draws nothing until the next resize.
 */
lh_bool_t
lh_gx_window_try_resize(lh_gx_window_t *self, lh_s32_t width, lh_s32_t height);

/**
 * @brief Take one of the system's input events: translated
 *        (::lh_gx_window_input_try_get_event), delivered (::lh_gx_input_take),
 *        and what it changed drawn at once.
 *
 * @param self  The window; must not be null.
 * @param event The system's event; must not be null.
 * @return ::lh_bool_true when the screen took it.
 */
lh_bool_t
lh_gx_window_take_input(lh_gx_window_t *self, const lh_os_input_event_t *event);

/**
 * @brief The window's tick (::lh_os_window_on_tick_fn): ::lh_gx_window_update.
 *
 * @param window  The system's window.
 * @param context The window (::lh_gx_window_t).
 */
lh_void
lh_gx_window_on_tick(lh_os_window_t *window, lh_ptr context);

/**
 * @brief The window's paint (::lh_os_window_on_paint_fn): ::lh_gx_window_paint.
 *
 * @param window  The system's window.
 * @param context The window (::lh_gx_window_t).
 */
lh_void
lh_gx_window_on_paint(lh_os_window_t *window, lh_ptr context);

/**
 * @brief The window's resize (::lh_os_window_on_resize_fn): a gesture in
 *        progress is cancelled, the surface remade (::lh_gx_window_try_resize)
 *        and the screen drawn again.
 *
 * @param window  The system's window.
 * @param width   The new client width.
 * @param height  The new client height.
 * @param context The window (::lh_gx_window_t).
 */
lh_void
lh_gx_window_on_resize(lh_os_window_t *window, int width, int height, lh_ptr context);

/**
 * @brief The window's input (::lh_os_input_sink_fn): ::lh_gx_window_take_input.
 *
 * @param self  The window (::lh_gx_window_t).
 * @param event The system's event.
 */
lh_void
lh_gx_window_on_input(lh_self_ptr self, const lh_os_input_event_ptr event);

/**
 * @brief Open the window on @p app, @p width by @p height, titled @p title, and
 *        make its surface.
 *
 * @param self   The window; initialised. Must not be null.
 * @param app    The application, with its window backend installed.
 * @param title  The title, UTF-8.
 * @param width  Client width in pixels.
 * @param height Client height in pixels.
 * @return ::lh_bool_false when the system would not open the window or give a
 *         surface.
 */
lh_bool_t
lh_gx_window_try_open(lh_gx_window_t *self, struct lh_os_app *app, const lh_char_t *title,
                      lh_s32_t width, lh_s32_t height);

/**
 * @brief A window not yet open: the renderer over the software backend, the
 *        present table, the pointer and the focus, the system window's callbacks
 *        set. The screen is empty and has no size until the window opens.
 *
 * @param self The record; must not move afterwards. Must not be null.
 */
lh_void
lh_gx_window_init(lh_gx_window_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_WINDOW_H */
