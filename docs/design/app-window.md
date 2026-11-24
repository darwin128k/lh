# The application and its windows

Status: the application and its windows run on contracts (owner and mavis,
2026-11-17). `lh_os_window_t` and `lh_os_app_t` are the same code with an
operating system and without one; below them are two tables a backend fills —
the system, a board, a test (section 8). What is left is the title and the
target — the pixels the window shows — and that belongs to the drawing part,
not to `lh/os/`.

## 1. What the owner wants

- An application contains windows. The first window opened into it is the
  main one.
- A window can own modal children. A modal child disables its parent until it
  closes.
- A window is not bound to anything when it is created: it exists, and the
  application takes it later.
- The same abstraction has to work on any operating system — Windows, X11,
  AppKit — and on a board with none.
- Everything the window system can do natively — the caption, the size, the
  frame — must stay native where there is an operating system.

## 2. What the tree already has

| brick | what it does |
|---|---|
| `lh/os/app.h` | `lh_os_app_t`: the window list (the first linked window is the main one), the timer group, the quit flag, and `lh_os_app_run` — the message pump. |
| `lh/os/window.h` | `lh_os_window_t`: the window. It exists without an application (`lh_os_window_init`); `lh_os_window_open` links it into an application and shows it; `lh_os_window_open_modal` makes it a modal child and disables the parent; `lh_os_window_unlink` takes it out. The callbacks: close, input, paint, resize, tick, zone. |
| `lh/os/system/surface.h` | The off-screen surface: a DIB on Windows, an image on X11 and AppKit. |
| `lh/gl/present/sink.h` | The present target: a panel, a window, the web player. Portable as it is. |
| `lh/gl/display.h` | The display of a microcontroller. |

So the model the owner describes **is** the model of the tree: an application
that owns windows, a main window, modal children, a window that exists before
it belongs to an application. None of that has to be designed again.

## 3. What was wrong, and what it is now

`lh_os_window_t` carried the portable half and the native half in one struct:
the handle (`lh_os_window_get_handle`), the paint device context
(`lh_os_window_get_paint_dc`), the paint rectangle, the frame, the corner, the
placement. A board with no operating system has none of those — and the header
refused to compile without `LH_LIBRARY_OPTION_OS`, as did `lh/os/app.h`.

So on a board there was no application and no window, only the display and the
present sink.

**Done (2026-11-17).** The struct was split first (section 7), and then the
split itself was replaced by a contract (section 8): `lh_os_window_t` holds what
is the application's, the backend keeps one opaque pointer per window, and
neither `lh/os/window.h` nor `lh/os/app.h` needs an operating system. The
window's place — where it is and how big it is — is the application's, and the
application can always ask.

## 4. The split

Two structs, one window.

- **`lh_os_window_t`** — the portable half. What the application touches: the
  place (position and size), the title, the children, and the callbacks. It
  needs no operating system.
- **`lh_os_window_native_t`** (`lh/os/window/native.h`) — the native half: the
  handle, the device context, the frame, the placement, the invalidation. It
  needs `LH_LIBRARY_OPTION_OS` and `LH_LIBRARY_OPTION_OS_WINDOW`.

The native half is *optional*. A window without one is a window on a board: a
rectangle of the application's own pixels, with a title the application draws
itself, because there is no system to draw a caption.

## 5. The application

`lh_os_app_t` is portable: the window list (only with an operating system), the
timers, the quit flag, and **a step**.

The step is the whole application in one call — the timers that are due, and
then the work. On an operating system `lh_os_app_run` is the pump that calls
it; on a board, the step is called by rail's loop (`lh_rail_loop_pass`), or by
`main` on a board with no threads at all. This is the same shape rail already
has: a loop that is never started is driven the same way by its owner.

**Done (2026-11-17).** `lh_os_app_step(app, now)` fires the timers and calls
the step; the step's call has the shape of a rail loop's step
(`lh_os_app_step_fn`), so one can stand in for the other. `lh/os/app.h` no
longer requires an operating system, and `lh/os/app.c` is built everywhere.

The first window opened is the main one, and closing it ends the application —
that rule does not change.

## 6. How a window works without an operating system

The honest answer: **there is no window in the operating system's sense.**
There is

- the application's own pixels — a portable pixmap, not a DIB;
- the present sink that sends the changed rectangles to the panel.

So the portable window is a rectangle of that pixmap. The title is a string the
application draws itself. The resize is the application telling the window its
new size, because nobody else will. The paint is the application calling its
own paint callback with its own pixmap.

Which means: **the pixels cannot belong to `lh/os/`.** A pixmap is the
drawing part's type, so `lh/os/` would have to depend on it to name the type
of the pixels it shows — the wrong way round. So:

- the **window** owns no pixels in either case — it is a place, a title, a
  set of children and a set of callbacks;
- the **target** owns the pixels: on a desktop it is the operating system's
  surface behind the native window, on a board it is the application's own
  buffer — both the same thing to the drawing part, a pixmap, a view of memory
  the application provides;
- the **application** hands the target to the drawing part, and the drawing
  part hands back the damage.

The present sink takes the pixels and the damage, so it belongs to the drawing
part — it is the thing that shows them, and it is portable as it is
today.

## 7. The split, field by field

`lh_os_window_t` held both halves. Here is where each field went, and what
moved with it.

**To the native window** (`lh/os/window/native.h`):

| field | why it is the system's |
|---|---|
| `handle` | the system's own window |
| `paint_dc`, `paint_left`, `paint_top`, `paint_right`, `paint_bottom` | the system's paint device and the region it asked for |
| `frame`, `corner` | whose frame the system draws, and the cut of it |
| `placement` | where the system opens the window |
| `maximized` | the system's state |
| `on_zone`, `on_zone_context` | the system's hit test: move, resize, or leave it |
| `pressed_buttons` | what the system's input left held |

**To the portable window** (`lh/os/window.h`):

| field | why it is the application's |
|---|---|
| `app`, `parent`, `children`, `link`, `modal`, `closing` | the tree the application owns |
| `on_resize`, `on_paint`, `on_input`, `on_close`, `on_tick` and their contexts | the application's reactions |
| `controller`, `controller_armed` | the input recognizer, which is not the system's |
| `tick_ms` | the period the **application** asked its timer for — the system's timer is the system's, the period is the ask |
| **the place** (new): `x`, `y`, `width`, `height` | where the window is and how big it is — the size lived only in the system, and the application could not ask for one |

**One deviation from the plan above.** `tick_ms` stayed in the portable window,
and `on_tick` with it: the period is the application's ask, and the ask is what
the portable window keeps. Only the arming of the system's timer is the
system's, and that is inside the `#if LH_LIBRARY_OPTION_OS_WINDOW` part of
`lh_os_window_set_on_tick`.

The cut is one change: the fields move, the functions that act on them move with
them, and the callers — the backends and gl — follow.

**Superseded the same day by section 8.** The native half drew the line with
`#if`. Under the contract the only field that is truly the system's — the
handle — is the system backend's own state behind `lh_os_window_get_native`, and
everything else in the table above went back up to the portable window:
`frame`, `corner` and `placement` are the application's asks, `maximized`,
`on_zone`, `pressed_buttons` and the paint target are the window's, whoever
fills them. `lh/os/window/native.h` is gone.

## 8. The contract

*Set by the owner, 2026-11-17:* **LH is contracts.** LH says what a window
and an application need from below, as a table of calls; a backend — Win32,
X11, AppKit, a board with no system, a test — fills that table, and the
window and the application work the same on every one of them. Whether there
is an operating system is the backend's business, not LH's.

### 8.1 What was wrong

The split in section 7 drew the line with `#if`, not with a contract:

- `src/lh/os/window.c` calls `lh_os_system_window_*` directly, and the link
  picks Win32, X11 or AppKit. There is nothing a backend *fills*; there is a
  set of functions a backend *is*.
- Without `LH_LIBRARY_OPTION_OS_WINDOW` that code is cut out, and nothing
  takes its place. On a board nobody opens a window, nobody calls its paint,
  input, tick or close, and the application has no window list at all — the
  portable window is a struct of callbacks nobody calls.
- `lh_os_app_run` calls `lh_os_system_window_pump_wait` directly, so the loop
  of an application exists only on an operating system — and it does not even
  call the step.
- A window cannot be tested without a real window of the system: the tests
  without an operating system are 8, with one 209.

The model to follow is in the tree already: `lh_gl_canvas_backend_t` (a table
of slots, optional slots with a stated fallback, the context held beside it)
and `lh_gl_present_sink_t` (a panel, GDI and the web player behind one call).

### 8.2 Two contracts

**`lh_os_window_backend_t` — what a window needs from below** (calls down).
Every slot takes the backend's context, and most take the window. Built from
what `window.c` called before:

| slot | the system backend | required — and without it |
|---|---|---|
| `open(context, window, title, width, height, owner)` → bool | `lh_os_system_window_open` | yes |
| `close(context, window)` | `_close` | yes |
| `invalidate(context, window)` | `_invalidate` | yes |
| `invalidate_rect(context, window, l, t, r, b)` | `_invalidate_rect` | yes |
| `now(context)` → tick (`lh_timer_clock_fn`) | `lh_os_tick_ms` | yes: the controller's deadlines are timed by it |
| `set_timer(context, window, ms)` (`0` stops it) | `_set_timer` | no: the window has no timer of its own; whoever drives the backend calls `lh_os_window_on_native_tick` |
| `set_enabled(context, window, enabled)` | `_set_enabled` | no: a modal child does not disable its parent |
| `minimize`, `set_maximized` | `_minimize`, `_set_maximized` | no: the call does nothing |
| `set_corner_radius` | `_set_corner_radius` | no: the radius is kept for the next `open` |
| `get_client_size`, `get_position` | `_get_client_size`, `_get_position` | no: the window answers with its place |
| `get_paint_region`, `now_us` | `_get_paint_region`, `lh_os_tick_us` | no: frame stats record the request unknown and the time `0` |

There is no handle in the contract. A backend keeps its own state per window
behind one opaque pointer the window carries (`lh_os_window_get_native`,
`lh_os_window_set_native`); the window never reads it, and ::lh_null means the
window is not open. For the system backend that pointer is the handle itself
(`lh_os_window_backend_system_get_handle`).

What the native half held and is not the system's went up to the portable
window, because it is the application's **ask**, read by the backend in `open`:
`frame`, `corner`, `placement`. `pressed_buttons` is input tracking, which is
the window's whatever fed the input, and the paint target is what the backend
hands the paint (`lh_os_window_get_paint_target`).

**Up-calls — what a backend tells the window.** These exist already and are
portable: `lh_os_window_on_native_paint`, `_on_native_resize`,
`_on_native_input`, `_on_native_tick`, `_on_native_destroy`,
`lh_os_window_zone_at`. They stop being under `#if`: a board's touch driver
reports input through the same call as Win32 does. The paint up-call carries
the damage (today the system's paint region, read back through
`lh_os_system_window_get_paint_region`) and an opaque target, the drawing part's
pixmap.

**`lh_os_app_backend_t` — what an application's loop needs** (calls down):

| slot | today | on a board |
|---|---|---|
| `now(context)` → tick | `lh_os_tick_ms` | rail's base clock |
| `wait(context, timeout)` → quit or not | `lh_os_system_window_pump_wait` | rail's queue, or sleep until the interrupt |
| `wake(context)` | `lh_os_system_window_post_quit` | rail's signal |

With it `lh_os_app_run` is one portable loop — `now`, `lh_os_app_step`,
`wait` until the next timer — and the window list stops being under `#if`:
an application holds its windows everywhere.

### 8.3 Who fills them

- **LH ships the system backend as a brick**: one table over the existing
  `lh_os_system_window_*`, which keep holding only the platform's own calls
  (section 0.2 of AGENTS.md). Win32, X11 and AppKit are not three tables: the
  link already picks one set of `lh_os_system_window_*`, and the table over it
  is portable. These are the only files built under
  `LH_LIBRARY_OPTION_OS_WINDOW` (`src/lh/os/window/backend/system.c`,
  `src/lh/os/app/backend/system.c`), and a table is filled into the host's own
  memory (`lh_os_window_backend_system_init`, `lh_os_app_backend_system_init`).
- **A board** fills its own table — the panel, the touch driver, rail. LH can
  ship one for its own display (`lh/gl/display.h` + present sink) later, as
  another brick, when there is a board to need it.
- **A test** fills a fake table and checks every path of the window without a
  system — on every configuration.
- **A host** (SDL, wasm, a remote) fills its own and LH does not need to know.

The slots start empty, as the core's slots do: the host installs the table it
wants (`lh_os_app_install_window_backend`, `lh_os_app_install_backend`), and the
window takes its backend from the application it is opened into — a window
exists before it belongs to an application, and it has no backend until then.

### 8.4 What it cost

- One indirect call per window operation — open, close, invalidate. Nothing
  next to what the system does for them.
- `window.c` and `app.c` lost every `#if`. The frame stats the window used to
  record into gl's type left the window: a measurement is gl's own composition
  around the paint (`lh_gl_frame_meter_t`), reading the backend's
  `get_paint_region` and `now_us` slots, so the os layer no longer knows gl.
- `lh_os_window_native_*` is gone; `lh_os_window_get_handle` and
  `lh_os_window_get_paint_dc` became `lh_os_window_backend_system_get_handle`
  and `lh_os_window_get_paint_target`. A host installs the two tables — four
  lines, in the docs' examples and in `example/window_stats.c`.
- The tests of the window and the application run on a fake backend in every
  configuration: without an operating system they went from 8 to 48.

### 8.5 Decisions

*Decided by the owner, 2026-11-17*, and written into AGENTS.md ("Contracts")
as a rule for every part, not for the window alone:

1. **Nothing is installed by default.** The host installs the table it wants
   — the core's rule for slots. A window opened into an application with no
   window backend, or an application run with no loop backend, is a checked
   failure, not a silent window of the wrong kind.
2. **Two tables**, the window's and the loop's: a board can have a loop
   without windows (a remote that only reads buttons), and a host can have
   windows on a loop it already runs (wasm, where the browser owns the loop).

### 8.6 What stays the same

| | with an operating system | without one |
|---|---|---|
| the caption | the system draws it | the application draws it |
| the size | the system resizes and says so | the application says so |
| the paint | the backend calls the paint up-call | the same |
| the present | the system | the present sink |
| the input | the controller, fed by the backend | the same |
| the close | the backend says | the same |

The window list, the main window, the modal children, the timers, the quit
flag and the step are the same on both sides.

## 9. The work

| | what | who |
|---|---|---|
| A | The portable half: split `lh_os_window_t` into the window and its native half, make `lh_os_app_t` portable with a step, and settle where the target lives. | together |
| A2 | The contracts (section 8): `lh_os_window_backend_t` and `lh_os_app_backend_t`, the system backend as one brick over `lh_os_system_window_*`, a fake backend for the tests, and every `#if LH_LIBRARY_OPTION_OS*` out of `window.c` and `app.c`. | together — done |
| B | The window side: the native window, the caption, the resize, the frame — on Windows, X11 and AppKit. | owner |
| C | The engine draws into the target and says what changed. | together |

A is done: the struct is split, the application has its step, and the target is
the drawing part's pixmap — the application provides the memory, and the drawing part
draws into it. A2 is done too (2026-11-17): the line between the window and
the system is a contract, and B is one backend among others, not the place the
window lives. B and C are open.

The order matters for one reason only: the engine draws into the target, so the
shape of the target has to be settled before the engine is written, and the
native window does not have to be settled before the engine starts.

## 10. The debt this page paid

Until the split was made, `lh/os/window.h` and `lh/os/app.h` required an
operating system, and the portable half of a window did not exist as a brick of
its own. That debt is paid and gone from `docs/cleanup.md`.

What is left of it: the **title**. A window has no title of its own — the
system's window has the caption it was opened with, and a window on a board has
none, because the application draws its own. The title belongs to the
application, next to the place.
