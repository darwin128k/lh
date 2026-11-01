/**
 * @file player.h
 * @brief ::lh_ui_web_player_t — the Canvas2D player as a web application.
 *
 * The player is an application: it serves its page, it answers the input the
 * page sends, it draws the tree into JSON and it pushes the frames to the
 * websocket clients. The host knows none of that — it serves whatever
 * application it is given.
 *
 * Requires ::LH_LIBRARY_OPTION_UI and ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_UI_WEB_PLAYER_H
#define LH_UI_WEB_PLAYER_H

#include <lh/config.h>
#include <lh/ui/web/app.h>
#include <lh/ui/web/host/hooks.h>
#include <lh/ui/web/player/fields.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/player.h requires LH_LIBRARY_OPTION_UI"
#endif
#if !LH_LIBRARY_OPTION_OS
#    error "lh/ui/web/player.h requires LH_LIBRARY_OPTION_OS"
#endif

/** The routes the player answers: the page, the frame, the revision and the
    seven input endpoints. */
#define LH_UI_WEB_PLAYER_ROUTES 11u
/** Player document from ::lh_ui_web_player_page_build -- room for the script to grow. */
#define LH_UI_WEB_PLAYER_PAGE_CAP (48u * 1024u)
/** Layout mode: the tab is the view. Clamp so a 1x1 or a 16k request cannot
    blow the JSON scratch or starve the host. Fit mode never resizes. */
#define LH_UI_WEB_PLAYER_SIZE_MIN 160
/** @copydoc LH_UI_WEB_PLAYER_SIZE_MIN */
#define LH_UI_WEB_PLAYER_SIZE_MAX 4096

/**
 * @struct lh_ui_web_player
 * @brief The Canvas2D player: the view it shows, the buffers it draws into
 *        and the application it is.
 */
struct lh_ui_web_player
{
    /** Members: ::lh_ui_web_player_fields. */
    lh_ui_web_player_fields(lh_ui_view_t *, lh_uint_t, lh_str_ptr, lh_usize_t,
                            const lh_ui_web_host_hooks_t *, lh_ui_web_app_t, lh_ui_web_route_t,
                            lh_os_controller_t, lh_ui_canvas_web_t, lh_ui_web_host_hooks_t);
};
/** @copybrief lh_ui_web_player */
typedef struct lh_ui_web_player lh_ui_web_player_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The application the host serves (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 */
lh_ui_web_app_t *
lh_ui_web_player_get_app(lh_ui_web_player_t *self);

/**
 * @brief The view the player shows (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 */
lh_ui_view_t *
lh_ui_web_player_get_view(const lh_ui_web_player_t *self);

/**
 * @brief Store the tree the player shows (level 1).
 *
 * A level-1 setter: the value is stored as given.
 *
 * @param self  The player; checked by ::lh_assert_runtime_ref.
 * @param view  The new tree.
 */
lh_void
lh_ui_web_player_set_view(lh_ui_web_player_t *self, lh_ui_view_t *view);

/**
 * @brief The width of the canvas (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 */
lh_uint_t
lh_ui_web_player_get_width(const lh_ui_web_player_t *self);

/**
 * @brief Store the width of the canvas (level 1).
 *
 * A level-1 setter: the value is stored as given.
 *
 * @param self   The player; checked by ::lh_assert_runtime_ref.
 * @param width  The new width.
 */
lh_void
lh_ui_web_player_set_width(lh_ui_web_player_t *self, lh_uint_t width);

/**
 * @brief The height of the canvas (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 */
lh_uint_t
lh_ui_web_player_get_height(const lh_ui_web_player_t *self);

/**
 * @brief Store the height of the canvas (level 1).
 *
 * A level-1 setter: the value is stored as given.
 *
 * @param self    The player; checked by ::lh_assert_runtime_ref.
 * @param height  The new height.
 */
lh_void
lh_ui_web_player_set_height(lh_ui_web_player_t *self, lh_uint_t height);

/**
 * @brief The scratch one frame is drawn into (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 */
lh_str_ptr
lh_ui_web_player_get_json(const lh_ui_web_player_t *self);

/**
 * @brief Store the scratch one frame is drawn into (level 1).
 *
 * A level-1 setter: the value is stored as given.
 *
 * @param self  The player; checked by ::lh_assert_runtime_ref.
 * @param json  The new scratch.
 */
lh_void
lh_ui_web_player_set_json(lh_ui_web_player_t *self, lh_str_ptr json);

/**
 * @brief The size of the frame scratch (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_ui_web_player_get_json_cap(const lh_ui_web_player_t *self);

/**
 * @brief Store the size of the frame scratch (level 1).
 *
 * A level-1 setter: the value is stored as given.
 *
 * @param self      The player; checked by ::lh_assert_runtime_ref.
 * @param json_cap  The new size.
 */
lh_void
lh_ui_web_player_set_json_cap(lh_ui_web_player_t *self, lh_usize_t json_cap);

/**
 * @brief The buffer the player document is built into (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 */
lh_byte_ptr
lh_ui_web_player_get_page(const lh_ui_web_player_t *self);

/**
 * @brief Store the buffer the player document is built into (level 1).
 *
 * A level-1 setter: the value is stored as given.
 *
 * @param self  The player; checked by ::lh_assert_runtime_ref.
 * @param page  The new buffer.
 */
lh_void
lh_ui_web_player_set_page(lh_ui_web_player_t *self, lh_byte_ptr page);

/**
 * @brief The size of the player document buffer (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_ui_web_player_get_page_cap(const lh_ui_web_player_t *self);

/**
 * @brief Store the size of the player document buffer (level 1).
 *
 * A level-1 setter: the value is stored as given.
 *
 * @param self      The player; checked by ::lh_assert_runtime_ref.
 * @param page_cap  The new size.
 */
lh_void
lh_ui_web_player_set_page_cap(lh_ui_web_player_t *self, lh_usize_t page_cap);

/**
 * @brief How long the player document is (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_ui_web_player_get_page_len(const lh_ui_web_player_t *self);

/**
 * @brief Store how long the player document is (level 1).
 *
 * A level-1 setter: the value is stored as given.
 *
 * @param self      The player; checked by ::lh_assert_runtime_ref.
 * @param page_len  The new length.
 */
lh_void
lh_ui_web_player_set_page_len(lh_ui_web_player_t *self, lh_usize_t page_len);

/**
 * @brief The player's routes (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 */
lh_ui_web_route_t *
lh_ui_web_player_get_routes(lh_ui_web_player_t *self);

/**
 * @brief The application's hooks (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_web_host_hooks_t *
lh_ui_web_player_get_hooks(const lh_ui_web_player_t *self);

/**
 * @brief Store the application's hooks (level 1).
 *
 * A level-1 setter: the value is stored as given.
 *
 * @param self   The player; checked by ::lh_assert_runtime_ref.
 * @param hooks  The new hooks; may be ::lh_null.
 */
lh_void
lh_ui_web_player_set_hooks(lh_ui_web_player_t *self, const lh_ui_web_host_hooks_t *hooks);

/**
 * @brief The hooks the player works with (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_ui_web_host_hooks_t *
lh_ui_web_player_get_routed(const lh_ui_web_player_t *self);

/**
 * @brief The hooks the player works with, to change in place (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_web_host_hooks_t *
lh_ui_web_player_get_routed_mut(lh_ui_web_player_t *self);

/**
 * @brief The player's own view controller (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_os_controller_t *
lh_ui_web_player_get_controller(const lh_ui_web_player_t *self);

/**
 * @brief The player's own view controller, to change in place (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_os_controller_t *
lh_ui_web_player_get_controller_mut(lh_ui_web_player_t *self);

/**
 * @brief Whether the controller is the player's (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_ui_web_player_get_owns_controller(const lh_ui_web_player_t *self);

/**
 * @brief Store whether the controller is the player's (level 1).
 *
 * A level-1 setter: the value is stored as given.
 *
 * @param self             The player; checked by ::lh_assert_runtime_ref.
 * @param owns_controller  True when it is.
 */
lh_void
lh_ui_web_player_set_owns_controller(lh_ui_web_player_t *self, lh_bool_t owns_controller);

/**
 * @brief The canvas the player draws a frame with (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_ui_canvas_web_t *
lh_ui_web_player_get_web(const lh_ui_web_player_t *self);

/**
 * @brief The canvas the player draws a frame with, to change in place (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_ui_canvas_web_t *
lh_ui_web_player_get_web_mut(lh_ui_web_player_t *self);

/**
 * @brief The controller the player works with (level 1).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 */
lh_os_controller_t *
lh_ui_web_player_get_routed_controller(const lh_ui_web_player_t *self);

/**
 * @brief Give one input event to the application.
 *
 * The event goes to the hooks' input first, then to the controller — the
 * player's own, when the caller gave none.
 *
 * @param self  The player; checked by ::lh_assert_runtime_ref.
 * @param event The event.
 *
 * @return True when the tree may have changed.
 */
lh_bool_t
lh_ui_web_player_input(lh_ui_web_player_t *self, const lh_os_input_event_t *event);

/**
 * @brief Make the canvas @p w x @p h, clamped.
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 * @param w    The width the page asked for.
 * @param h    The height it asked for.
 *
 * @return True when the size changed.
 */
lh_bool_t
lh_ui_web_player_resize(lh_ui_web_player_t *self, lh_sint_t w, lh_sint_t h);

/**
 * @brief Serve the player document.
 *
 * The route handler of `/`.
 *
 * @param ctx    The player's context.
 * @param req    The request, as it came in.
 * @param len    Its length.
 * @param client The socket to answer on.
 *
 * @return ::lh_bool_true when it answered.
 */
lh_bool_t
lh_ui_web_player_serve_page(lh_ptr ctx, const lh_str_ptr req, lh_usize_t len,
                             lh_os_net_socket_t *client);

/**
 * @brief Serve the frame counter, so a page knows whether it is behind.
 *
 * The route handler of `/ui.rev`.
 *
 * @param ctx    The player's context.
 * @param req    The request, as it came in.
 * @param len    Its length.
 * @param client The socket to answer on.
 *
 * @return ::lh_bool_true when it answered.
 */
lh_bool_t
lh_ui_web_player_serve_revision(lh_ptr ctx, const lh_str_ptr req, lh_usize_t len,
                                 lh_os_net_socket_t *client);

/**
 * @brief Draw the tree into the frame scratch.
 *
 * @param self    The player; checked by ::lh_assert_runtime_ref.
 * @param glyphs  The font masks the frame uses; may be ::lh_null.
 * @param bytes   Receives the frame.
 * @param len     Receives its length.
 *
 * @return ::lh_bool_false when the frame did not fit.
 */
lh_bool_t
lh_ui_web_player_draw(lh_ui_web_player_t *self, lh_ui_canvas_web_glyphs_t *glyphs, const lh_str_ptr *bytes,
                      lh_usize_t *len);

/**
 * @brief Start a player.
 *
 * The page is built into @p page and the frames are drawn into @p json; both
 * buffers belong to the caller and must outlive the player.
 *
 * @param self     The player; checked by ::lh_assert_runtime_ref.
 * @param view     The tree the player shows.
 * @param width    The width of the canvas.
 * @param height   The height of the canvas.
 * @param json     The scratch one frame is drawn into.
 * @param json_cap Its size.
 * @param page     The buffer the player document is built into.
 * @param page_cap Its size.
 * @param hooks    The application's hooks; may be ::lh_null.
 *
 * @return ::lh_bool_false when the player document did not build.
 */
lh_bool_t
lh_ui_web_player_init(lh_ui_web_player_t *self, lh_ui_view_t *view, lh_uint_t width, lh_uint_t height,
                      lh_str_ptr json, lh_usize_t json_cap, lh_byte_ptr page, lh_usize_t page_cap,
                      const lh_ui_web_host_hooks_t *hooks);

/**
 * @brief A pointer event at view pixel (@p x, @p y).
 *
 * @param self  The player; checked by ::lh_assert_runtime_ref.
 * @param kind  The pointer event.
 * @param x     The x in view pixels.
 * @param y     The y in view pixels.
 *
 * @return True when the tree may have changed.
 */
lh_bool_t
lh_ui_web_player_pointer(lh_ui_web_player_t *self, lh_os_input_kind_t kind, lh_sint_t x, lh_sint_t y);

/**
 * @brief A wheel notch at (@p x, @p y).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 * @param x    The x in view pixels.
 * @param y    The y in view pixels.
 * @param dx   The notches to the left; 1 is left.
 * @param dy   The notches up; 1 is up.
 *
 * @return True when the tree may have changed.
 */
lh_bool_t
lh_ui_web_player_wheel(lh_ui_web_player_t *self, lh_sint_t x, lh_sint_t y, lh_sint_t dx, lh_sint_t dy);

/**
 * @brief A key going down or up.
 *
 * @param self     The player; checked by ::lh_assert_runtime_ref.
 * @param k        The key, as an ::lh_os_key_t.
 * @param pressed  True when the key goes down.
 *
 * @return True when the tree may have changed.
 */
lh_bool_t
lh_ui_web_player_key(lh_ui_web_player_t *self, lh_sint_t k, lh_bool_t pressed);

/**
 * @brief A typed code point.
 *
 * @param self  The player; checked by ::lh_assert_runtime_ref.
 * @param code  The code point; 0 is ignored.
 *
 * @return True when the tree may have changed.
 */
lh_bool_t
lh_ui_web_player_text(lh_ui_web_player_t *self, lh_u32_t code);

/**
 * @brief Serve the tree as one JSON frame.
 *
 * The route handler of `/ui.json`.
 *
 * @param ctx    The player's context.
 * @param req    The request, as it came in.
 * @param len    Its length.
 * @param client The socket to answer on.
 *
 * @return ::lh_bool_true when it answered.
 */
lh_bool_t
lh_ui_web_player_serve_frame(lh_ptr ctx, const lh_str_ptr req, lh_usize_t len,
                              lh_os_net_socket_t *client);

/**
 * @brief The frame the application pushes to the websocket clients.
 *
 * The application's frame callback.
 *
 * @param ctx    The player's context.
 * @param glyphs The font masks the frame uses.
 * @param bytes  Receives the frame.
 * @param len    Receives its length.
 */
lh_void
lh_ui_web_player_frame(lh_ptr ctx, lh_ui_canvas_web_glyphs_t *glyphs, const lh_str_ptr *bytes,
                       lh_usize_t *len);

/**
 * @brief The application's tick: the controller, the hooks and the damage.
 *
 * @param ctx The player's context.
 */
lh_void
lh_ui_web_player_tick(lh_ptr ctx);

/**
 * @brief Read one input line of the page.
 *
 * The lines are `p x y`, `r x y`, `m x y` (a move while a button is down),
 * `w x y dx dy`, `k key pressed` (`1` down, `0` up), `t code` and `z w h` (the
 * size).
 *
 * @param self The player; checked by ::lh_assert_runtime_ref.
 * @param line The line.
 * @param len  Its length.
 */
lh_void
lh_ui_web_player_event_line(lh_ui_web_player_t *self, const lh_str_ptr line, lh_usize_t len);

/**
 * @brief Serve one input of the page.
 *
 * The route handler of the `/ui/...` endpoints: the click, the press, the
 * release, the move, the wheel, the key, the text and the size.
 *
 * @param ctx    The player's context.
 * @param req    The request, as it came in.
 * @param len    Its length.
 * @param client The socket to answer on.
 *
 * @return ::lh_bool_true when it answered.
 */
lh_bool_t
lh_ui_web_player_serve_input(lh_ptr ctx, const lh_str_ptr req, lh_usize_t len,
                              lh_os_net_socket_t *client);

/**
 * @brief Read one message of a websocket client.
 *
 * The application's message callback: the page sends its input as lines.
 *
 * @param ctx  The player's context.
 * @param text The message.
 * @param len  Its length.
 */
lh_void
lh_ui_web_player_message(lh_ptr ctx, const lh_str_ptr text, lh_usize_t len);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_PLAYER_H */
