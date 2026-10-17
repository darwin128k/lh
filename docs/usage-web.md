# lh in a browser: the web host

The same UI tree as the desktop window — but the browser paints the pixels.
A walk through the `cfg_web` target of `H:\Projects\configurator`: it is built
from **the same `main.c`** as `configurator`, with one flag.

Read [usage-desktop.md](usage-desktop.md) first — the tree, styles and input
handlers are the same.

---

## 1. How it works

```
 application                    lh/ui/web/host.c                   browser
 ┌────────────┐  draw   ┌──────────────────────────┐  HTTP / WS  ┌─────────────┐
 │lh_ui_view_t│────────▶│ lh_ui_canvas_backend_web │────────────▶│ Canvas2D    │
 │ + hooks    │◀────────│ → JSON ops               │◀────────────│ player      │
 └────────────┘ press,  └──────────────────────────┘ clicks,     └─────────────┘
               wheel…                                 resize
```

- `lh_ui_canvas_backend_web` (`lh/ui/canvas/web.h`) records a frame's
  primitives as JSON. Glyph covers are interned: each glyph crosses the wire
  once per client.
- `lh_ui_web_host_run` (`lh/ui/web/host.h`) listens on `127.0.0.1:<port>`:
  - `GET /` — the Canvas2D HTML player (built by `lh_ui_web_page_build`);
  - `GET /ui.json` — one frame;
  - `ws://…/ws` — pushes a frame **only when the tree changed**;
  - presses, releases, moves while a button is down, wheel, keys (down and
    up), text and the tab size come back: the input as one
    `lh_os_input_event_t` each, the size through the `resize` hook.
- No Python, Node or external web server — everything runs in the process.
- Several tabs can watch one process.

Input travels as one text line per event over the WebSocket (with the socket
down, the player sends the same numbers as an HTTP query instead):

| Line | HTTP fallback | Event (to the `input` hook, else `lh_ui_view_input`) |
|---|---|---|
| `p x y` / `r x y` | `/ui/press`, `/ui/release` `?x=&y=` | `pointer_down` / `pointer_up`, primary button |
| `m x y` | `/ui/move?x=&y=` | `pointer_move`, primary held (sent only while a button is down, at most once per animation frame) |
| `w x y dx dy` | `/ui/wheel?x=&y=&dx=&dy=` | `wheel`, `dx` / `dy` notches × 120, positive up / left |
| `k key pressed` | `/ui/key?k=&p=` | `key_down` / `key_up`; `pressed` is `1` down, `0` up |
| `t code` | `/ui/text?c=` | `text` |
| `z w h` | `/ui/size?w=&h=` | `resize` hook (layout mode), no event |

`/ui/click?x=&y=` is a `pointer_down` and a `pointer_up` at one point.

`key` is an `lh_os_key_t` (`lh/os/key.h`); the player's key table is written
from that enum when the page is built, so the numbers cannot drift. A key
that also types (Space) sends its `t` line after the `k` line, as the desktop
does. A `k key` line without the flag (an older player) is a key going down.

Requires `LH_LIBRARY_OPTION_UI=ON` and `LH_LIBRARY_OPTION_OS=ON` (sockets).
The host does not need a window (`OS_WINDOW`).

---

## 2. Build: a second target from the same source

```cmake
add_executable(my_app "src/main.c")
target_link_libraries(my_app PRIVATE lh)

add_executable(my_web "src/main.c")
target_compile_definitions(my_web PRIVATE MY_WEB_HOST=1)
target_link_libraries(my_web PRIVATE lh)
```

In `main.c`:

```c
#ifndef MY_WEB_HOST
#    define MY_WEB_HOST 0
#endif
```

---

## 3. Hooks: the same handlers, `window = NULL`

The web host knows nothing about `lh_os_window_t`. Its `input` hook has the
signature of a window's sink (`lh_os_input_sink_cb`), so the desktop `on_input`
plugs in as it is; only the tick and the resize need thin wrappers — **which is
why they must survive `window == lh_null`**:

```c
#if MY_WEB_HOST
#include <lh/ui/web/host.h>

static lh_void web_tick  (lh_ptr c)               { on_tick(lh_null, c); }
static lh_void web_resize(int w, int h, lh_ptr c) { on_resize(lh_null, w, h, c); }
#endif
```

Notes:

- In the web build `on_resize` lays the tree out again but does **not** call
  `lh_os_window_invalidate` — damage on the view is enough for the next frame.
- Any hook may be `lh_null`. Without `input` the host hands every event to
  `lh_ui_view_input` on the served view itself; with it, the hook gets every
  event (moves included) and passes on to `lh_ui_view_input` what the view
  should see.
- The events carry `lh_os_tick_ms` as their time and no modifiers.
- `tick` runs about every 20 ms **and between requests**, so device polling is
  not starved by HTTP.

---

## 4. `main`: branch before the window

Everything before the window opens (state, canvas, view, tree) is shared. The
branch goes right after `my_main_init`:

```c
int
main(int argc, char **argv)
{
    static struct my_main self;
    lh_os_app_t app;
    lh_os_window_t window;

    lh_os_app_init(lh_addr_of(app));
    lh_os_window_init(lh_addr_of(window));
    if (!my_main_init(lh_addr_of(self)))
        return 1;

#if MY_WEB_HOST
    {
        /* One JSON frame. 4 MB is ample for configurator. */
        static char json[4u * 1024u * 1024u];
        lh_net_port_t port = (lh_net_port_t)8765;
        lh_ui_web_host_hooks_t hooks;

        (void)app; (void)window;
        if (argc > 1)
            port = (lh_net_port_t)atoi(argv[1]);

        hooks.ctx    = lh_addr_of(self);
        hooks.tick   = web_tick;
        hooks.input  = on_input;   /* the desktop sink, unchanged */
        hooks.resize = web_resize;

        /* Does not return on success. */
        if (!lh_ui_web_host_run(lh_addr_of(self.view), 980u, 620u,
                                json, sizeof(json), port, &hooks))
        {
            lh_logger_error(lh_runtime_logger(), "listen %u failed", (unsigned)port);
            return 1;
        }
        return 0;
    }
#else
    (void)argc; (void)argv;
#endif

    /* ... desktop branch: set_on_input and the rest, lh_os_window_open, lh_os_app_run ... */
}
```

The view's canvas needs no preparation: for every frame the host binds a web
backend to the view, draws the whole frame, and **resets the canvas to
`lh_null`**. So in the web build do not draw the view with your own canvas
outside the host — between frames it has none.

Running:

```bat
build\my_web.exe          rem http://127.0.0.1:8765/
build\my_web.exe 9000     rem another port
```

---

## 5. Scaling

The mode is a query parameter of the page:

| URL | Behaviour |
|---|---|
| `http://127.0.0.1:8765/` (`scale=layout`) | The tab size becomes the view size; the app lays out again (`resize` hook). Sharp. |
| `http://127.0.0.1:8765/?scale=fit` | The fixed `width × height` frame is contain-fit into the tab. May blur. |

In both modes clicks are mapped back into view pixels.

---

## 6. No server: a frame to memory or a file

The web backend can be used on its own — e.g. to dump a frame for comparison
against a wireframe (configurator's `CFG_DUMP_WEB`):

```c
#include <lh/ui/canvas/web.h>

static char buf[4u * 1024u * 1024u];
lh_ui_canvas_web_t web;
lh_ui_canvas_t canvas;

if (lh_ui_canvas_web_open_mem(&web, buf, sizeof(buf), 980u, 620u))
{
    lh_ui_canvas_init(&canvas, lh_addr_of(lh_ui_canvas_backend_web), &web);
    lh_ui_view_set_canvas(&self->view, &canvas);
    lh_ui_view_draw(&self->view, lh_null);             /* whole frame */
    lh_ui_view_set_canvas(&self->view, &self->canvas); /* restore our own */

    if (lh_ui_canvas_web_close(&web))
        write_file(path, lh_ui_canvas_web_bytes(&web), lh_ui_canvas_web_length(&web));
}
```

---

## 7. Limitations

- Listens on `127.0.0.1` only, no TLS — a local tool, not a public server.
  `wss` is supported by the codec, but no TLS stream is built into the host.
- configurator does not wire `key` / `text` yet (`lh_null`) — keyboard input
  in the web build is wired the same way as the mouse.
- The `json` buffer holds one frame; enlarge it for a big tree (an overflow
  does not write past the end: the frame fails and `lh_ui_canvas_web_close`
  returns `lh_bool_false`).
- A 3D pane is not a canvas op but a separate backend (`lh/ui/viewport3d.h`).
- Windows XP does not need the player — it stays on the GDI window.
