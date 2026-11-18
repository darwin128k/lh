/**
 * @file page.c
 * @brief Implementation of `lh/gx/web/page.h`.
 */

#include <lh/gx/web/page.h>

#include <lh/gx/backend/web/player.h>

/* The transport, line by line; the size of the array less its NUL is its length. */
static const lh_char_t m_gx_web_page_script[] =
    "/* lh gx web page: the frames come in over the websocket, the pointer goes out. */\n"
    "function lhGxPoint(canvas, e) {\n"
    "  const r = canvas.getBoundingClientRect();\n"
    "  return Math.round((e.clientX - r.left) * canvas.width / r.width) + \" \" +\n"
    "    Math.round((e.clientY - r.top) * canvas.height / r.height);\n"
    "}\n"
    "function lhGxConnect(canvas, path) {\n"
    "  const ctx = canvas.getContext(\"2d\");\n"
    "  let ws = null, down = false;\n"
    "  function send(line) {\n"
    "    if (ws !== null && ws.readyState === 1) ws.send(line);\n"
    "  }\n"
    "  function open() {\n"
    "    ws = new WebSocket((location.protocol === \"https:\" ? \"wss://\" : \"ws://\") + "
    "location.host + path);\n"
    "    ws.onmessage = function (m) { lhGxPlay(ctx, JSON.parse(m.data)); };\n"
    "    ws.onclose = function () { setTimeout(open, 1000); };\n"
    "  }\n"
    "  canvas.addEventListener(\"pointerdown\", function (e) {\n"
    "    down = true;\n"
    "    canvas.setPointerCapture(e.pointerId);\n"
    "    send(\"p \" + lhGxPoint(canvas, e));\n"
    "  });\n"
    "  canvas.addEventListener(\"pointermove\", function (e) {\n"
    "    if (down) send(\"m \" + lhGxPoint(canvas, e));\n"
    "  });\n"
    "  canvas.addEventListener(\"pointerup\", function (e) {\n"
    "    if (down) { down = false; send(\"r \" + lhGxPoint(canvas, e)); }\n"
    "  });\n"
    "  canvas.addEventListener(\"pointercancel\", function () {\n"
    "    if (down) { down = false; send(\"c\"); }\n"
    "  });\n"
    "  open();\n"
    "}\n";

/* The document around the canvas and the scripts. */
static const lh_char_t m_gx_web_page_head[] =
    "<!doctype html>\n"
    "<html><head><meta charset=\"utf-8\"><title>lh gx</title>\n"
    "<style>html,body{margin:0;background:#202020}"
    "canvas{display:block;margin:auto;touch-action:none}</style>\n"
    "</head><body><canvas id=\"lh\" width=\"";
static const lh_char_t m_gx_web_page_between[] = "\" height=\"";
static const lh_char_t m_gx_web_page_open[] = "\"></canvas>\n<script>\n";
static const lh_char_t m_gx_web_page_start[] =
    "lhGxConnect(document.getElementById(\"lh\"), \"" LH_GX_WEB_PAGE_WS_PATH "\");\n";
static const lh_char_t m_gx_web_page_tail[] = "</script></body></html>\n";

lh_str_view_t
lh_gx_web_page_get_script(lh_void)
{
    lh_str_view_t script;

    lh_str_view_init_by_size(lh_addr_of(script), m_gx_web_page_script,
                             lh_math_sub_one(sizeof(m_gx_web_page_script)));
    return script;
}

lh_void
lh_gx_web_page_write(lh_str_t *out, lh_uint_t width, lh_uint_t height)
{
    lh_str_append(out, m_gx_web_page_head, lh_math_sub_one(sizeof(m_gx_web_page_head)));
    lh_str_append_uint(out, width);
    lh_str_append(out, m_gx_web_page_between, lh_math_sub_one(sizeof(m_gx_web_page_between)));
    lh_str_append_uint(out, height);
    lh_str_append(out, m_gx_web_page_open, lh_math_sub_one(sizeof(m_gx_web_page_open)));
    lh_str_append_view(out, lh_gx_backend_web_player_get_script());
    lh_str_append_view(out, lh_gx_web_page_get_script());
    lh_str_append(out, m_gx_web_page_start, lh_math_sub_one(sizeof(m_gx_web_page_start)));
    lh_str_append(out, m_gx_web_page_tail, lh_math_sub_one(sizeof(m_gx_web_page_tail)));
}
