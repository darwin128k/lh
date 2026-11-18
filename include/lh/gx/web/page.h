/**
 * @file page.h
 * @brief The page the gx web application serves: a canvas and two scripts.
 *
 * The whole browser side of a gx application is one HTML document: a
 * `<canvas>` the size of the screen, the player of the web backend
 * (`lh/gx/backend/web/player.h`) that draws the frames, and the transport
 * script here that connects them to the host:
 *
 * - it opens a websocket on ::LH_GX_WEB_PAGE_WS_PATH and plays every message
 *   it gets as a frame (`lhGxPlay`), opening the socket again a second after
 *   it closes — a new connection is a new client, which gets a frame that
 *   stands on its own;
 * - it sends the pointer as the lines of `lh/gx/web/input.h`, in canvas
 *   pixels whatever size the canvas is shown at, a move only while the
 *   pointer is down, and keeps the pointer captured until it goes up.
 * - it sends the keys of the document as the key and text lines of
 *   `lh/gx/web/input.h`: a key that is one character as its code point, any
 *   other by its `KeyboardEvent.key` name; a key held with Ctrl, Alt or Meta is
 *   left to the browser, so its shortcuts still work.
 *
 * Nothing on the page knows an entity, a class or a style: those stay in C.
 *
 * Ownership: the scripts are constant text in the library's read-only data;
 * ::lh_gx_web_page_write appends to the caller's string.
 */

#ifndef LH_GX_WEB_PAGE_H
#define LH_GX_WEB_PAGE_H

#include <lh/core/str.h>

/**
 * @def LH_GX_WEB_PAGE_WS_PATH
 * @brief The path the page opens its websocket on, and the application takes
 *        the upgrades on: `/ws`.
 */
#define LH_GX_WEB_PAGE_WS_PATH "/ws"

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The transport script: `lhGxConnect(canvas, path)` and its helper.
 *
 * @return A view of the script, without a terminating NUL in its size; it
 *         lives as long as the program.
 */
lh_str_view_t lh_gx_web_page_get_script(lh_void);

/**
 * @brief Append the whole page to @p out: a @p width by @p height canvas, the
 *        player, the transport, and the call that starts them.
 *
 * @param out    The string to append to; its contents so far are kept.
 * @param width  The canvas's width, in pixels: the screen's.
 * @param height The canvas's height, in pixels.
 */
lh_void
lh_gx_web_page_write(lh_str_t *out, lh_uint_t width, lh_uint_t height);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_WEB_PAGE_H */
