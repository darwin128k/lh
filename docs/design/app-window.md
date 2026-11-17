# The application and its windows

Status: design in progress (owner and mavis, 2026-11-17). The model below is
what the tree already does; what is missing is that the window is still the
operating system's window, so nothing of it works on a board without one.

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

## 3. The gap: the window is the operating system's window

`lh_os_window_t` carries the portable half and the native half in one struct:
the handle (`lh_os_window_get_handle`), the paint device context
(`lh_os_window_get_paint_dc`), the paint rectangle, the frame, the corner, the
placement. A board with no operating system has none of those — and the header
refuses to compile without `LH_LIBRARY_OPTION_OS`, as does `lh/os/app.h`.

So on a board there is no application and no window, only the display and the
present sink. That is the part that does not answer the question yet.

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

`lh_os_app_t` becomes portable too: the window list, the timers, the quit
flag, and **a step**. The step is the whole application in one call — take
the events, draw what changed, return.

- On an operating system, `lh_os_app_run` is the pump that calls the step.
- On a board, the step is called by rail's loop (`lh_rail_loop_pass`), or by
  `main` on a board with no threads at all. This is the same shape rail
  already has: a loop that is never started is driven the same way by its
  owner.

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
drawing part's type, so `lh/os/` would have to depend on gx to name the type
of the pixels it shows — the wrong way round. So:

- the **window** owns no pixels in either case — it is a place, a title, a
  set of children and a set of callbacks;
- the **target** owns the pixels: on a desktop it is the operating system's
  surface behind the native window, on a board it is the application's own
  pixmap;
- the **application** hands the target to the drawing part, and the drawing
  part hands back the damage.

The present sink takes the pixels and the damage, so it belongs to the drawing
part (gx) — it is the thing that shows them, and it is portable as it is
today.

## 7. The contract

| | with an operating system | without one |
|---|---|---|
| the caption | the system draws it | the application draws it |
| the size | the system resizes and says so | the application says so |
| the paint | the system calls the paint callback with its device context | the application calls it with its pixmap |
| the present | the system | the present sink |
| the input | the controller, fed by the system | the controller, fed by rail's queue |
| the close | the system says | the application says |

What is the same on both sides: the window list, the main window, the modal
children, the timers, the quit flag, the step.

## 8. The work

| | what | who |
|---|---|---|
| A | The portable half: split `lh_os_window_t` into the window and its native half, make `lh_os_app_t` portable with a step, and settle where the target lives. | together |
| B | The window side: the native window, the caption, the resize, the frame — on Windows, X11 and AppKit. | owner |
| C | The engine: gx draws into the target and says what changed. | together |

The order matters for one reason only: gx draws into the target, so the shape
of the target has to be settled before the engine is written, and the native
window does not have to be settled before gx starts.

## 9. The debt this page creates

Until the split is made, `lh/os/window.h` and `lh/os/app.h` require an
operating system, and the portable half of a window does not exist as a brick
of its own. That is the debt the split pays, and it is listed in
`docs/cleanup.md`.
