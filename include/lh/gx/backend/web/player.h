/**
 * @file player.h
 * @brief The browser side of the web backend: ::lh_gx_backend_web_player_get_script.
 *
 * The web backend (`lh/gx/backend/web.h`) writes the canvas calls as JSON
 * frames; the player is the script that replays them on a page's 2D canvas.
 * It is the whole browser side of drawing: it knows the canvas's calls and
 * nothing of the entities, the classes or the damage, which stay in C. A host
 * puts the script on its page (inline, or served as a file of its own) and
 * calls `lhGxPlay(ctx, frame)` with each frame it receives, `frame` being the
 * parsed JSON.
 *
 * The script defines three functions and nothing else:
 *
 * - `lhGxPlay(ctx, frame)` — every command of @c frame, in order, on the
 *   `CanvasRenderingContext2D` @c ctx: clipped to the command's clip, the
 *   context's state restored after each one. A command it does not know is
 *   skipped, so an older page survives a newer backend.
 * - `lhGxColor(k)` — the ARGB number @c k as a CSS `rgba(...)`.
 * - `lhGxFillMask(ctx, rect, k, coverage)` — the colour @c k through the hex
 *   coverage, drawn with an `OffscreenCanvas` so that the clip and the blending
 *   of @c ctx apply (`putImageData` would ignore both).
 *
 * The transport — how frames reach the page — is not the player's: a host
 * sends them as it likes (a websocket, a fetch) and hands each one in.
 *
 * Ownership: the script is constant text in the library's read-only data.
 */

#ifndef LH_GX_BACKEND_WEB_PLAYER_H
#define LH_GX_BACKEND_WEB_PLAYER_H

#include <lh/core/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The player's JavaScript source.
 *
 * @return A view of the script, without a terminating NUL in its size; it
 *         lives as long as the program and is never ::lh_null.
 */
lh_str_view_t lh_gx_backend_web_player_get_script(lh_void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_BACKEND_WEB_PLAYER_H */
