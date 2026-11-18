/**
 * @file web.h
 * @brief A gx screen served to browsers: ::lh_gx_web_t.
 *
 * The composition that puts a gx tree in a browser, built from bricks that
 * each stand on their own:
 *
 * - the renderer (`lh/gx/render.h`) draws the screen through the web backend
 *   (`lh/gx/backend/web.h`), so a frame is the JSON of the canvas calls for
 *   what changed;
 * - the page (`lh/gx/web/page.h`) holds a canvas, the player that replays the
 *   frames and the script that sends the pointer and the keys back;
 * - the lines (`lh/gx/web/input.h`) drive an ::lh_gx_pointer_t and an
 *   ::lh_gx_focus_t: a press focuses what takes keys under it, and the keys go
 *   to the entity with the focus; so a
 *   press in the browser reaches the entity under it as on any other screen;
 * - the host (`lh/os/web/host.h`) is the transport: it serves ::lh_gx_web_get_app_mut,
 *   which answers `GET /` with the page, takes the websocket on
 *   ::LH_GX_WEB_PAGE_WS_PATH, hands the lines in and asks for frames.
 *
 * A frame carries only what changed since the last one. When a browser joins
 * that holds nothing (a new tab, a reconnect), the host asks for a frame that
 * stands on its own and the whole screen is drawn again. A frame that draws
 * the same area twice gives the same pixels as long as the screen's tree is
 * opaque where it is drawn (a background under everything): the renderer
 * paints an area from the first entity that covers it.
 *
 * Threads: the host calls the application on its own thread — the page, the
 * lines, the frames, the tick — so the tree is touched there and only there.
 * An entity's reaction (a button's click) runs on that thread too.
 *
 * Ownership: the record is the caller's and must not move after
 * ::lh_gx_web_init (its members point at each other). The frame and the page
 * are strings it owns, freed by ::lh_gx_web_deinit. The entities attached to
 * the screen are the caller's.
 *
 * Example usage:
 * @code{.c}
 * static lh_gx_web_t web;
 *
 * lh_gx_web_init(&web, 320, 240);
 * lh_gx_entity_attach(lh_gx_web_get_screen_mut(&web), &background);
 * lh_gx_entity_attach(&background, lh_gx_button_get_box_mut(&ok));
 * lh_os_web_host_run(lh_gx_web_get_app_mut(&web), 8080);   // does not return
 * @endcode
 */

#ifndef LH_GX_WEB_H
#define LH_GX_WEB_H

#include <lh/gx/backend/web.h>
#include <lh/gx/focus.h>
#include <lh/gx/pointer.h>
#include <lh/gx/render.h>
#include <lh/gx/web/fields.h>
#include <lh/os/web/app.h>

/**
 * @struct lh_gx_web
 * @brief A gx screen served to browsers. Members: ::lh_gx_web_fields.
 */
struct lh_gx_web
{
    /** Members: ::lh_gx_web_fields. */
    lh_gx_web_fields(lh_gx_render_t, lh_gx_canvas_t, lh_gx_backend_web_t, lh_web_json_writer_t,
                     lh_str_t, lh_gx_pointer_t, lh_gx_focus_t, lh_os_web_app_route_t,
                     lh_os_web_app_t);
};
/** @copybrief lh_gx_web */
typedef struct lh_gx_web lh_gx_web_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The renderer of the screen (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_render_t *
lh_gx_web_get_render(const lh_gx_web_t *self);

/**
 * @brief The renderer of the screen, to change (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_render_t *
lh_gx_web_get_render_mut(lh_gx_web_t *self);

/**
 * @brief The web backend's table, to fill (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_canvas_t *
lh_gx_web_get_canvas_mut(lh_gx_web_t *self);

/**
 * @brief The web backend, to write a frame with (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_backend_web_t *
lh_gx_web_get_backend_mut(lh_gx_web_t *self);

/**
 * @brief The JSON writer the backend writes through (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_web_json_writer_t *
lh_gx_web_get_writer_mut(lh_gx_web_t *self);

/**
 * @brief The text of the last frame drawn (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_str_t *
lh_gx_web_get_frame(const lh_gx_web_t *self);

/**
 * @brief The text of the frame, to write (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_str_t *
lh_gx_web_get_frame_mut(lh_gx_web_t *self);

/**
 * @brief The page every browser is given (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_str_t *
lh_gx_web_get_page(const lh_gx_web_t *self);

/**
 * @brief The page, to write (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_str_t *
lh_gx_web_get_page_mut(lh_gx_web_t *self);

/**
 * @brief The pointer the browsers' lines drive (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_pointer_t *
lh_gx_web_get_pointer(const lh_gx_web_t *self);

/**
 * @brief The pointer, to drive (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_pointer_t *
lh_gx_web_get_pointer_mut(lh_gx_web_t *self);

/**
 * @brief Where the browsers' keys go (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_focus_t *
lh_gx_web_get_focus(const lh_gx_web_t *self);

/**
 * @brief Where the keys go, to move (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_focus_t *
lh_gx_web_get_focus_mut(lh_gx_web_t *self);

/**
 * @brief The routes of the application, to fill (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @return The first of ::LH_GX_WEB_ROUTES routes, never ::lh_null.
 */
lh_os_web_app_route_t *
lh_gx_web_get_routes_mut(lh_gx_web_t *self);

/**
 * @brief What the host serves (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_os_web_app_t *
lh_gx_web_get_app(const lh_gx_web_t *self);

/**
 * @brief What the host serves, to hand to it (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_os_web_app_t *
lh_gx_web_get_app_mut(lh_gx_web_t *self);

/**
 * @brief The screen: the root the caller attaches its entities to.
 *
 * @param self The application; must not be null.
 * @return The renderer's screen, never ::lh_null.
 */
lh_gx_entity_t *
lh_gx_web_get_screen_mut(lh_gx_web_t *self);

/**
 * @brief Ask the host for a frame when the screen has something to draw:
 *        ::lh_os_web_app_bump when the renderer holds damage.
 *
 * @param self The application; must not be null.
 * @return ::lh_bool_true when a frame was asked for.
 */
lh_bool_t
lh_gx_web_bump_if_changed(lh_gx_web_t *self);

/**
 * @brief Draw one frame into the frame text: what changed, or with
 *        @p restart the whole screen.
 *
 * The text is emptied first, then holds one frame (`lh/gx/backend/web.h`).
 *
 * @param self    The application; must not be null.
 * @param restart ::lh_bool_true to draw the whole screen, for a browser that
 *                holds nothing.
 * @return How many commands the frame holds; `0` when nothing changed.
 */
lh_usize_t
lh_gx_web_draw(lh_gx_web_t *self, lh_bool_t restart);

/**
 * @brief The application's `GET /` (::lh_os_web_app_route_fn): the page.
 *
 * @param ctx     The application (::lh_gx_web_t).
 * @param request The request; borrowed.
 * @param headers Its fields; not read.
 * @param body    Its body; not read.
 * @param client  The socket to answer on.
 * @return ::lh_bool_true: the route always answers, even when sending fails.
 */
lh_bool_t
lh_gx_web_serve_page(lh_ptr ctx, const lh_http_request_t *request, const lh_http_header_t *headers,
                     const lh_str_view_t *body, lh_os_net_socket_t *client);

/**
 * @brief Free the frame and the page. The entities are the caller's and are
 *        not touched.
 *
 * @param self The application; must not be null, and the host must no longer
 *             serve it.
 */
lh_void
lh_gx_web_deinit(lh_gx_web_t *self);

/**
 * @brief Take one line from a browser (`lh/gx/web/input.h`): a pointer event
 *        goes to the pointer (::lh_gx_pointer_take) over the screen, and a
 *        press moves the focus to what it landed on (::lh_gx_focus_press); a
 *        key or a text goes to the focus (::lh_gx_focus_take). A frame is asked
 *        for when it changed something.
 *
 * @param self The application; must not be null.
 * @param text The line; not required to be NUL-terminated.
 * @param size How many characters it has.
 * @return ::lh_bool_true when the line was one of the page's lines; any
 *         other line is ignored.
 */
lh_bool_t
lh_gx_web_take_line(lh_gx_web_t *self, const lh_char_t *text, lh_usize_t size);

/**
 * @brief The application's frame (::lh_os_web_app_frame_fn): ::lh_gx_web_draw,
 *        and its text when it holds a command or @p restart asked for one.
 *
 * @param ctx     The application (::lh_gx_web_t).
 * @param restart ::lh_bool_true for a frame that stands on its own.
 * @param bytes   Receives the frame's text, owned by the application and good
 *                until the next frame; ::lh_null when there is none.
 * @param len     Receives its length; `0` when there is no frame to send.
 */
lh_void
lh_gx_web_frame(lh_ptr ctx, lh_bool_t restart, const lh_str_ptr *bytes, lh_usize_t *len);

/**
 * @brief The application's tick (::lh_os_web_app_tick_fn):
 *        ::lh_gx_web_bump_if_changed, for a change that did not come from a
 *        line.
 *
 * @param ctx The application (::lh_gx_web_t).
 */
lh_void
lh_gx_web_tick(lh_ptr ctx);

/**
 * @brief The application's message (::lh_os_web_app_message_fn):
 *        ::lh_gx_web_take_line.
 *
 * @param ctx  The application (::lh_gx_web_t).
 * @param text The line.
 * @param len  Its length.
 */
lh_void
lh_gx_web_message(lh_ptr ctx, const lh_str_ptr text, lh_usize_t len);

/**
 * @brief A @p width by @p height screen served to browsers.
 *
 * The renderer draws through the web backend, the page is written, the
 * pointer holds nothing, nothing has the focus, and the application answers `GET /`, takes the
 * websocket on ::LH_GX_WEB_PAGE_WS_PATH and has its frame, message and tick.
 * The screen is empty: the caller attaches its entities to
 * ::lh_gx_web_get_screen_mut.
 *
 * @param self   The record; must not move afterwards. Must not be null.
 * @param width  The screen's width, in pixels.
 * @param height The screen's height, in pixels.
 */
lh_void
lh_gx_web_init(lh_gx_web_t *self, lh_gx_scalar_t width, lh_gx_scalar_t height);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_WEB_H */
