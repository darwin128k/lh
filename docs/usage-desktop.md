# lh in a desktop application

A walk through `H:\Projects\configurator` (target `configurator`) — a Modbus
configurator with a Win32 GDI window, a UI tree generated from CSS, and device
polling on a timer. A minimal window without a UI tree is
[`example/window_stats.c`](../example/window_stats.c).

Overview and option matrix: [usage.md](usage.md).
The same code in a browser: [usage-web.md](usage-web.md).

---

## 1. Build

The default options suit the desktop: OS, window, UI, Roboto and the stdlib
defaults are all on. All that is left is adding the library:

```cmake
cmake_minimum_required(VERSION 3.12)
project(my_app LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# One Windows API level for lh and the app: lh reads LH_OS_WINDOWS_VERSION (default 0x0501, XP)
# and defines WINVER / _WIN32_WINNT from it. Don't add your own WINVER definitions
# before add_subdirectory -- lh's sources would get two different values.
set(LH_OS_WINDOWS_VERSION "0x0501" CACHE STRING "Windows API level for lh and my_app")

set(LH_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/lh" CACHE PATH "Path to lh")
add_subdirectory("${LH_DIR}" "${CMAKE_BINARY_DIR}/lh" EXCLUDE_FROM_ALL)

add_executable(my_app src/main.c)
target_link_libraries(my_app PRIVATE lh)   # brings user32, gdi32, ws2_32
if (WIN32)
    target_compile_definitions(my_app PRIVATE WINVER=${LH_OS_WINDOWS_VERSION} _WIN32_WINNT=${LH_OS_WINDOWS_VERSION})
endif ()
```

Worth copying from `configurator/CMakeLists.txt`:

- **`-O2` for lh even in Debug** — a software-rendered UI at `-O0` is
  noticeably slower, and you usually debug the application, not the library:

  ```cmake
  if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
      target_compile_options(lh PRIVATE
          $<$<NOT:$<CONFIG:Release,RelWithDebInfo,MinSizeRel>>:-O2>)
  endif ()
  ```

- **No `include_directories` pointing at lh** — `target_link_libraries`
  already puts the generated `config.h` first.
- Windows exist on Win32 and X11 (libX11 is opened with `dlopen` at run time,
  so no development package is needed to build). macOS has no window backend
  yet.

---

## 2. Skeleton: app, window, loop

```c
#include <lh/os/app.h>
#include <lh/os/window.h>
#include <lh/util/addr.h>

int
main(void)
{
    lh_os_app_t app;
    lh_os_window_t window;
    static struct my_main self;      /* static: the state can be large */

    lh_os_app_init(lh_addr_of(app));
    lh_os_window_init(lh_addr_of(window));

    if (!my_main_init(lh_addr_of(self)))   /* canvas, view, tree — sections 3–4 */
        return 1;

    lh_os_window_set_on_paint  (lh_addr_of(window), on_paint,   lh_addr_of(self));
    lh_os_window_set_on_resize (lh_addr_of(window), on_resize,  lh_addr_of(self));
    lh_os_window_set_on_close  (lh_addr_of(window), on_close,   lh_addr_of(self));
    lh_os_window_set_on_input  (lh_addr_of(window), on_input,   lh_addr_of(self)); /* section 5 */
    lh_os_window_set_on_tick   (lh_addr_of(window), on_tick,    lh_addr_of(self), 20U); /* ms */

    if (!lh_os_window_open(lh_addr_of(app), lh_addr_of(window), "My app", 980, 620))
        return 1;

    lh_os_app_run(lh_addr_of(app));   /* until lh_os_app_quit */

    lh_os_window_deinit(lh_addr_of(window));
    lh_os_app_deinit(lh_addr_of(app));
    return 0;
}

static lh_void
on_close(lh_os_window_t *window, lh_os_window_close_reason_t reason, lh_ptr context)
{
    (void)reason; (void)context;
    lh_os_app_quit(lh_os_window_get_app(window));
}
```

Lessons from configurator:

- **The main struct is `static`.** configurator's holds a ~1 MB Modbus session;
  on the stack that is an overflow, and it surfaces as an exception inside the
  window procedure — i.e. it looks like a drawing bug.
- **Set the timer before `open`** — the period is kept and armed on open.
- **`on_tick` is the only thing that comes back on a schedule.** Device
  polling, animations and `lh_ui_view_tick` belong there, not in `on_paint`.
- **One pass per call.** A callback that loops "while there is work" stops
  the message pump: the window cannot paint, take a click, or be closed.
- **All input is one callback.** `lh_os_window_set_on_input` takes every mouse,
  wheel, key, text and focus event as one `lh_os_input_event_t`; there are no
  per-event setters. A click is not an event of the window: the view makes it
  from press and release (section 5).

---

## 3. Canvas: the GDI backend and the view

`lh_os_render_backend_gdi` draws with the software renderer into a DIB
section the size of the client area and shows it with one `BitBlt`.

```c
#include <lh/os/render/backend/gdi.h>
#include <lh/ui/canvas.h>
#include <lh/ui/view.h>

struct my_main
{
    lh_os_render_backend_gdi_context_t gdi;
    lh_ui_canvas_t canvas;
    lh_ui_view_t view;
    /* tree, application state ... */
};

static lh_bool_t
my_main_init(struct my_main *self)
{
    lh_os_render_backend_gdi_context_init(lh_addr_of(self->gdi));
    lh_ui_canvas_init(lh_addr_of(self->canvas), lh_addr_of(lh_os_render_backend_gdi),
                      lh_addr_of(self->gdi));
    lh_ui_view_init(lh_addr_of(self->view));
    lh_ui_view_set_canvas(lh_addr_of(self->view), lh_addr_of(self->canvas));
    lh_ui_view_set_clear(lh_addr_of(self->view), lh_addr_of(s_colour_bg));  /* background */

    return my_ui_build(self);   /* section 4 */
}
```

Painting — the DC and the paint rect are valid **only inside `on_paint`**:

```c
static lh_void
on_paint(lh_os_window_t *window, lh_ptr context)
{
    struct my_main *self = lh_ptr_rcast(struct my_main, context);
    int l, t, r, b;
    lh_ui_rect_t damage;

    my_refresh(self);   /* update texts/styles from data — no polling here */

    lh_os_window_get_paint_rect(window, &l, &t, &r, &b);
    lh_ui_rect_init(lh_addr_of(damage), l, t, r - l, b - t);

    lh_os_render_backend_gdi_context_set_hwnd(lh_addr_of(self->gdi), lh_os_window_get_handle(window));
    lh_os_render_backend_gdi_context_set_hdc (lh_addr_of(self->gdi), lh_os_window_get_paint_dc(window));
    lh_ui_view_draw(lh_addr_of(self->view), lh_addr_of(damage));
    lh_os_render_backend_gdi_context_set_hdc (lh_addr_of(self->gdi), lh_null);
}
```

To repaint only what changed, take the damage from the view and invalidate
that rect of the window:

```c
lh_ui_rect_t damage;
if (lh_ui_view_take_damage(lh_addr_of(self->view), lh_addr_of(damage)))
    lh_os_window_invalidate_rect(window, x0, y0, x1, y1);   /* from damage */
```

**Buffer format.** ARGB8888 by default, and on the desktop that is the right
choice: RGB565 draws faster, but a 32-bit DC widens every pixel on present
and the whole frame ends up slower (measurements in the comment on
`lh_os_render_backend_gdi_context_get_format`). RGB565 is for MCUs.

---

## 4. The UI tree

Two ways, and they can be mixed.

### 4.1. By hand

```c
#include <lh/ui/button.h>
#include <lh/ui/color.h>
#include <lh/ui/entity.h>
#include <lh/ui/font/roboto.h>
#include <lh/ui/label.h>
#include <lh/ui/paint.h>
#include <lh/ui/style.h>

static lh_ui_color_t s_ink, s_card;
static lh_ui_paint_t s_p_ink, s_p_card;
static lh_ui_style_t s_text, s_btn;

static lh_ui_entity_t s_root;
static lh_ui_label_t  s_title;
static lh_ui_button_t s_ok;

static lh_void
on_ok(lh_ui_button_t *button, lh_ptr context)
{
    (void)button;
    /* the click has already ended; the button shows its resting style */
}

static lh_bool_t
my_ui_build(struct my_main *self)
{
    lh_ui_rect_t r;

    lh_ui_color_init_hex(&s_ink, 0x1B2430);
    lh_ui_color_init_hex(&s_card, 0xFFFFFF);
    lh_ui_paint_init_color(&s_p_ink, &s_ink);    /* a paint copies the colour */
    lh_ui_paint_init_color(&s_p_card, &s_card);

    lh_ui_style_init(&s_text);
    lh_ui_style_set_font(&s_text, &lh_ui_font_roboto);
    lh_ui_style_set_text(&s_text, &s_p_ink);

    lh_ui_style_init(&s_btn);
    lh_ui_style_set_fill(&s_btn, &s_p_card);

    lh_ui_rect_init(&r, 0, 0, 980, 620);
    lh_ui_entity_init(&s_root, r);

    lh_ui_rect_init(&r, 24, 24, 400, 28);
    lh_ui_label_init(&s_title, r, "Hello, lh");
    lh_ui_entity_set_style(lh_ui_label_as_entity(&s_title), &s_text);
    lh_ui_entity_add_child(&s_root, lh_ui_label_as_entity(&s_title));

    lh_ui_rect_init(&r, 24, 72, 120, 36);
    lh_ui_button_init(&s_ok, r);
    lh_ui_button_set_style(&s_ok, &s_btn);
    lh_ui_button_set_on_click(&s_ok, on_ok, self);
    lh_ui_entity_add_child(&s_root, lh_ui_button_as_entity(&s_ok));

    lh_ui_view_set_root(lh_addr_of(self->view), &s_root);
    return lh_bool_true;
}
```

**Lifetimes.** A widget does **not copy** styles, paints, flows or text — it
keeps the address. Everything the tree points at must outlive the tree
(statics or fields of the main struct). Colour is the exception:
`lh_ui_paint_init_color` copies the four channels, so the colour must be
filled in **before** that call.

### 4.2. From CSS (as configurator does)

The screen is described by a stylesheet, `ui/configurator.css`, and
`lib/lh/scripts/css2decl.py` turns it into an `lh_ui_decl_t` table:

```sh
python3 lib/lh/scripts/css2decl.py ui/my.css src/my/ui --prefix my_ui_
```

It writes `ui_generated.c/.h` with:

| Name | What |
|---|---|
| `my_ui_decls[]`, `my_ui_decl_count` | the rows of the tree |
| `my_ui_names[]` | the `#id` of each row |
| `my_ui_colour_*` | colours from `:root` (`--accent-dim` → `my_ui_colour_accent_dim`) |
| `my_ui_init()` | fills colours/styles/flows — **call before the build** |
| `my_ui_label(nodes, "title")`, `my_ui_button(…)`, … | a row by `#id`, with a kind check |

Building the tree into a static pool:

```c
#include <lh/ui/decl.h>
#include "ui/ui_generated.h"

#define MY_UI_ROWS 48
static lh_byte_t       s_pool[LH_UI_DECL_NODE_MAX * MY_UI_ROWS];
static lh_ui_entity_t *s_nodes[MY_UI_ROWS];

static lh_bool_t
my_ui_build(struct my_main *self)
{
    lh_ui_entity_t *root = lh_null;
    lh_ui_decl_fault_t fault;

    if (my_ui_decl_count > MY_UI_ROWS || !my_ui_init())
        return lh_bool_false;
    if (!lh_ui_decl_build(my_ui_decls, my_ui_decl_count, s_nodes,
                          s_pool, (lh_u32_t)sizeof(s_pool), &root, &fault))
    {
        /* fault.index is the row, fault.code the reason */
        return lh_bool_false;
    }
    lh_ui_view_set_root(lh_addr_of(self->view), root);

    lh_ui_button_set_on_click(my_ui_button(s_nodes, "ok"), on_ok, self);
    return lh_bool_true;
}
```

The generated files are **checked in** — a build needs no Python.
configurator offers a target that regenerates them, and a dependency so an
edited stylesheet is never silently ignored:

```cmake
set_source_files_properties(src/my/ui/ui_generated.c PROPERTIES OBJECT_DEPENDS "${CMAKE_SOURCE_DIR}/ui/my.css")

find_package(Python3 COMPONENTS Interpreter QUIET)
if (Python3_Interpreter_FOUND)
    add_custom_target(my_ui
        COMMAND ${Python3_EXECUTABLE} "${LH_DIR}/scripts/css2decl.py" ui/my.css src/my/ui --prefix my_ui_
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" VERBATIM)
endif ()
```

What CSS cannot express (resize anchors, the pitch of repeated rows)
configurator keeps in C (`include/cfg/ir.h`) and applies in `on_resize`.

Icons come from an lh generator too: `lib/lh/scripts/gen-icons.py` (16×16,
4bpp, `lh_ui_mask_t`).

---

## 5. Input

The window delivers every input as one event (`lh/os/input/event.h`) to one
sink; the view dispatches it to entities:

```c
static lh_void
on_input(const lh_os_input_event_ptr event, lh_ptr context)
{
    struct my_main *self = lh_ptr_rcast(struct my_main, context);

    if (lh_ui_view_input(lh_addr_of(self->view), event)) /* pressed, scrolled, typed */
        my_invalidate(self);
}
```

`lh_ui_view_input` reads the event and makes the call the view needs:

| Event | View call |
|---|---|
| `pointer_down` / `pointer_up`, primary button | `lh_ui_view_press` / `lh_ui_view_release` (click, focus, drag) |
| `pointer_move` | `lh_ui_view_move` (drag-scroll the content, drag a scrollbar thumb) |
| `pointer_cancel`, `focus_out` | `lh_ui_view_cancel` (the press ends, no click) |
| `wheel` | `lh_ui_view_wheel`, `LH_UI_VIEW_WHEEL_STEP` per notch |
| `key_down` / `key_up` | `lh_ui_view_key` (Tab moves focus) |
| `text` | `lh_ui_view_text` (a character into an input) |

`on_tick` still drives `lh_ui_view_tick` (~60 Hz: scroll glide, transitions).

What an event carries:

- **Coordinates** are client pixels in Q24.8 (`lh_os_input_coord_to_int`).
- **Wheel** distances are 1/120 notch (`LH_OS_INPUT_WHEEL_NOTCH`), positive
  up / left. The backends no longer turn Shift+wheel sideways; the gesture
  recognizer does, when asked to.
- **Buttons**: `button` is the one that changed, `buttons` what is held after
  the event; `lh_os_window_get_pressed_buttons` keeps the same mask. Windows
  delivers all five mouse buttons, X11 too.
- **Keys** carry `lh_os_key_t`, the native code as `scancode`, the modifiers
  and `LH_OS_INPUT_FLAG_REPEAT` for auto-repeat (X11 no longer drops it).
- **Focus**: `focus_in` / `focus_out` when the window gains or loses the
  keyboard; a lost mouse capture on Windows is a `pointer_cancel`.

**Gestures** — click count, double click, long press, drag begin / drag / end —
come from `lh_os_input_recognizer_t` (`lh/os/input/recognizer.h`): feed it
the raw events, call `lh_os_input_recognizer_update` from `on_tick`, and it
passes every raw event on to its own sink with the gestures among them, marked
`LH_OS_INPUT_FLAG_SYNTHESIZED`. `lh_ui_view_input` ignores synthesized events,
so one sink can take both.

**Events from somewhere else** (an interrupt, a thread) go through
`lh_os_input_queue_t` (`lh/os/input/queue.h`): one producer pushes, the UI
thread drains it into the sink.

The sink has no window parameter, so the same function plugs into the web host
unchanged ([usage-web.md](usage-web.md)) — which is exactly what configurator
does.

---

## 6. Diagnostics

- **Frame stats.** `lh_ui_frame_stats_t` + `lh_os_window_set_frame_stats`
  count frames, asked vs drawn pixels, and time. See
  `example/window_stats.c` (`-DLH_BUILD_EXAMPLES=ON`).
- **Screenshot without a screen.** configurator swaps in a file canvas
  (`shot.c`, `CFG_SHOT=<path>`) — the same `lh_ui_view_draw`, a different
  backend. Swapping is save/restore: `set_canvas(tmp)` → `draw` →
  `set_canvas(original)`.
- **Scripted input.** `CFG_CLICK=x,y` and `CFG_ENTER=1` call the same
  `on_press` / `on_release` / `on_key` the window does — reproducible bugs.
- **Log.** `lh_logger_info(lh_runtime_logger(), "...")` writes to stderr by
  default.
