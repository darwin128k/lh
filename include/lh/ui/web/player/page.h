/**
 * @file page.h
 * @brief The Canvas2D player document: built as an HTML tree and a JavaScript
 *        tree, then written out.
 *
 * The page the web player serves on `GET /` is a small document (two
 * canvases and a style) and one script that fetches frames, draws them and
 * sends input back. Both are built as trees - the markup with
 * ::lh_web_xml_node_t (written in the HTML dialect), the script with
 * ::lh_web_js_node_t - and only the writers turn them into text: names are
 * validated, strings escaped and parentheses placed by precedence. No part of
 * the page is held as code text.
 *
 * The script is a list of top-level statements, each made by its own
 * `lh_ui_web_player_page_create_<part>` brick (a new tree the caller owns and
 * frees with ::lh_web_js_node_destroy, or hands to a parent).
 * ::lh_ui_web_player_page_create_script puts them in order;
 * ::lh_ui_web_player_page_try_write writes the whole page.
 *
 * The player builds the page once, into its own buffer, when it starts
 * (::lh_ui_web_player_page_build from ::lh_ui_web_player_init) and serves those
 * bytes on every request.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_WEB_PLAYER_PAGE_H
#define LH_UI_WEB_PLAYER_PAGE_H

#include <lh/os/key.h>
#include <lh/core/web/xml/node.h>
#include <lh/core/web/js/node.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/player/page.h requires LH_LIBRARY_OPTION_UI"
#endif

/* ── keys ────────────────────────────────────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The browser's `KeyboardEvent.key` for @p key (`"Tab"`, `" "`,
 *        `"ArrowLeft"`, ...): static text, not to be freed. Empty for
 *        ::lh_os_key_other and for a value outside ::lh_os_key_t.
 */
lh_str_view_t
lh_ui_web_player_page_get_key_name(lh_os_key_t key);

/**
 * @brief Whether @p key is a modifier (Shift, Control, Alt): held with other
 *        keys, so the player leaves the browser's default action to it.
 */
lh_bool_t
lh_ui_web_player_page_is_modifier(lh_os_key_t key);

/* ── document ────────────────────────────────────────────────────────────── */

/** A new `<head>`: charset, viewport, title and the style. The caller owns it. */
lh_web_xml_node_t *lh_ui_web_player_page_create_head(lh_void);

/**
 * @brief A new `<body>`: the player canvas `#c`, the 3D canvas `#v3d` and a
 *        `<script>` holding a line break and @p script as raw text (copied).
 *        @p script must come from ::lh_web_js_writer_try_write, which keeps
 *        `</` out of the text. The caller owns the result.
 */
lh_web_xml_node_t *
lh_ui_web_player_page_create_body(lh_str_view_t script);

/**
 * @brief A new object expression `{Tab:1,Enter:2,...}`: every key's browser
 *        name (::lh_ui_web_player_page_get_key_name) to its ::lh_os_key_t
 *        number, so the numbers on the wire are the enum's own. Only the
 *        modifiers when @p modifiers_only. The caller owns the result.
 */
lh_web_js_node_t *
lh_ui_web_player_page_create_key_table(lh_bool_t modifiers_only);

/** A new `<html lang="ru">` with the head and the body of @p script. The caller owns it. */
lh_web_xml_node_t *
lh_ui_web_player_page_create_html(lh_str_view_t script);

/**
 * @brief Append `<!doctype html>` and the `<html>` of @p script to @p out: one
 *        document of the two, written by ::lh_web_xml_writer_try_write in the
 *        HTML dialect.
 *
 * @return ::lh_bool_false when the writer rejects the tree.
 */
lh_bool_t
lh_ui_web_player_page_try_write_document(lh_str_t *out, lh_str_view_t script);

/** A new `var keys={...}` with every key (::lh_ui_web_player_page_create_key_table). */
lh_web_js_node_t *lh_ui_web_player_page_create_keys(lh_void);

/** A new `var mods={...}` with the modifiers only (::lh_ui_web_player_page_create_key_table). */
lh_web_js_node_t *lh_ui_web_player_page_create_mods(lh_void);

/* ── script parts: each returns a new statement the caller owns ──────────── */

/** A new `function rgba(c)`: a CSS `rgba(...)` colour from a `[r,g,b,a]` byte quad. */
lh_web_js_node_t *lh_ui_web_player_page_create_rgba(lh_void);

/** A new `var atlas,tints,...`: the frame, atlas and tint cache state. */
lh_web_js_node_t *lh_ui_web_player_page_create_state(lh_void);

/** A new `var scaleMode,...`: `fit` or `layout` from the page URL, and the last reported size. */
lh_web_js_node_t *lh_ui_web_player_page_create_scale_mode(lh_void);

/** A new `function hexv(c)`: the value of one hex digit char code. */
lh_web_js_node_t *lh_ui_web_player_page_create_hexv(lh_void);

/** A new `function decodeCover(w,h,cv)`: a canvas with the alpha mask of a hex coverage string. */
lh_web_js_node_t *lh_ui_web_player_page_create_decode_cover(lh_void);

/** A new `function ingest(d)`: decode the coverage masks a frame brings into `atlas`. */
lh_web_js_node_t *lh_ui_web_player_page_create_ingest(lh_void);

/** A new `function tint(a,c)`: a mask canvas filled with one colour. */
lh_web_js_node_t *lh_ui_web_player_page_create_tint(lh_void);

/** A new `function glyph(op)`: the tinted canvas for a `fill_mask` op, cached in `tints`. */
lh_web_js_node_t *lh_ui_web_player_page_create_glyph(lh_void);

/** A new `function round(x,r,rad)`: a rounded rectangle path, plain when `roundRect` is missing. */
lh_web_js_node_t *lh_ui_web_player_page_create_round(lh_void);

/** A new `function pane3d(d)`: place or hide the WebGL canvas over the 3D viewport. */
lh_web_js_node_t *lh_ui_web_player_page_create_pane3d(lh_void);

/** A new `function fit()`: size the canvas to the frame and the scale mode; the 2D context. */
lh_web_js_node_t *lh_ui_web_player_page_create_fit(lh_void);

/** A new `function play(d)`: draw a frame's ops. */
lh_web_js_node_t *lh_ui_web_player_page_create_play(lh_void);

/** A new `function sized(d)`: whether a frame matches the size last reported in layout mode. */
lh_web_js_node_t *lh_ui_web_player_page_create_sized(lh_void);

/** A new `function show(d)`: take a frame and draw it on the next animation frame. */
lh_web_js_node_t *lh_ui_web_player_page_create_show(lh_void);

/** A new `function pos(e)`: a mouse event in frame coordinates. */
lh_web_js_node_t *lh_ui_web_player_page_create_pos(lh_void);

/** A new `function viewSize()`: the window's inner size, at least 1x1. */
lh_web_js_node_t *lh_ui_web_player_page_create_view_size(lh_void);

/** A new `function reportSize(force)`: send the window size to the host in layout mode. */
lh_web_js_node_t *lh_ui_web_player_page_create_report_size(lh_void);

/** A new `window.onresize=...`: report the size again, or redraw the last frame. */
lh_web_js_node_t *lh_ui_web_player_page_create_on_resize(lh_void);

/** A new `var busy,rev`: the frame request in flight and the last seen revision. */
lh_web_js_node_t *lh_ui_web_player_page_create_fetch_state(lh_void);

/** A new `function load()`: fetch `/ui.json` and show it, one request at a time. */
lh_web_js_node_t *lh_ui_web_player_page_create_load(lh_void);

/** A new `function poll()`: load the frame again when `/ui.rev` changes. */
lh_web_js_node_t *lh_ui_web_player_page_create_poll(lh_void);

/** A new `function emit(line)`: send an input line over the socket, or as an HTTP request. */
lh_web_js_node_t *lh_ui_web_player_page_create_emit(lh_void);

/** A new `var c=document.getElementById('c')`: the player canvas. */
lh_web_js_node_t *lh_ui_web_player_page_create_canvas(lh_void);

/** A new `var held,mv,mvRaf`: the held button and the pending move. */
lh_web_js_node_t *lh_ui_web_player_page_create_pointer_state(lh_void);

/** A new `function flushMove()`: send the pending move now. */
lh_web_js_node_t *lh_ui_web_player_page_create_flush_move(lh_void);

/** A new `c.onmousedown=...`: a press on the canvas. */
lh_web_js_node_t *lh_ui_web_player_page_create_on_mouse_down(lh_void);

/** A new `window.onmouseup=...`: the release of a press, wherever it happens. */
lh_web_js_node_t *lh_ui_web_player_page_create_on_mouse_up(lh_void);

/** A new `window.onmousemove=...`: a drag, at most one move per animation frame. */
lh_web_js_node_t *lh_ui_web_player_page_create_on_mouse_move(lh_void);

/** A new `c.onwheel=...`: a wheel step, Shift turning it sideways. */
lh_web_js_node_t *lh_ui_web_player_page_create_on_wheel(lh_void);

/** A new `document.onkeydown=...`: a key down, and the text it types. */
lh_web_js_node_t *lh_ui_web_player_page_create_on_key_down(lh_void);

/** A new `document.onkeyup=...`: a key up. */
lh_web_js_node_t *lh_ui_web_player_page_create_on_key_up(lh_void);

/** A new `c.focus()`: give the canvas the keyboard. */
lh_web_js_node_t *lh_ui_web_player_page_create_focus(lh_void);

/** A new `var sock,retry,pollId,retryId`: the socket and its fallback timers. */
lh_web_js_node_t *lh_ui_web_player_page_create_socket_state(lh_void);

/** A new `function stopPoll()`: stop polling. */
lh_web_js_node_t *lh_ui_web_player_page_create_stop_poll(lh_void);

/** A new `function startPoll()`: poll every 80 ms. */
lh_web_js_node_t *lh_ui_web_player_page_create_start_poll(lh_void);

/** A new `function armRetry()`: reopen the socket later, backing off up to 5 s. */
lh_web_js_node_t *lh_ui_web_player_page_create_arm_retry(lh_void);

/** A new `function openWs()`: open the socket; poll while it is down. */
lh_web_js_node_t *lh_ui_web_player_page_create_open_ws(lh_void);

/** A new the start: report the size first in layout mode, then load and open the socket. */
lh_web_js_node_t *lh_ui_web_player_page_create_start(lh_void);

/* ── script ──────────────────────────────────────────────────────────────── */

/**
 * @brief A new program with every script part in order, from
 *        ::lh_ui_web_player_page_create_rgba to
 *        ::lh_ui_web_player_page_create_start. The caller owns it.
 */
lh_web_js_node_t *lh_ui_web_player_page_create_script(lh_void);

/**
 * @brief Append the player script as text to @p out
 *        (::lh_ui_web_player_page_create_script, ::lh_web_js_writer_try_write).
 *
 * @return ::lh_bool_false when the writer rejects the tree; @p out may then
 *         hold part of the text.
 */
lh_bool_t
lh_ui_web_player_page_try_write_script(lh_str_t *out);

/* ── page ────────────────────────────────────────────────────────────────── */

/**
 * @brief Append the whole player page to @p out: the script written first,
 *        then the document around it.
 *
 * @return ::lh_bool_false when a writer rejects its tree.
 */
lh_bool_t
lh_ui_web_player_page_try_write(lh_str_t *out);

/** ::lh_ui_web_player_page_try_write, with a runtime check that it succeeded. */
lh_void
lh_ui_web_player_page_write(lh_str_t *out);

/**
 * @brief Write the player page into @p mem, followed by a NUL byte.
 *
 * @param mem     Buffer the page is written into.
 * @param cap     Size of @p mem in bytes; the page and the NUL must fit.
 * @param out_len Receives the byte length (the NUL not counted), 0 on failure.
 *                May be ::lh_null.
 *
 * @return ::lh_bool_false when @p mem is null, the buffer is too small or the
 *         page cannot be written; @p mem is then left unchanged.
 */
lh_bool_t
lh_ui_web_player_page_build(lh_byte_ptr mem, lh_usize_t cap, lh_usize_t *out_len);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_PLAYER_PAGE_H */
