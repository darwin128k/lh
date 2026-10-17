# Using lh: three targets

The same library builds for three very different targets. What changes between
them is not the application code but **the set of `LH_LIBRARY_OPTION_*`** and
**where the pixels go**: a TFT panel, a window, or a browser.

| Target | Reference project | Guide |
|---|---|---|
| Microcontroller, no OS (STM32F407 + ILI9341) | `H:\Projects\f4ve-bare` | [usage-embedded.md](usage-embedded.md) |
| Desktop window (Win32 GDI, X11) | `H:\Projects\configurator`, target `configurator` | [usage-desktop.md](usage-desktop.md) |
| Web: the same UI tree in a browser | `H:\Projects\configurator`, target `cfg_web` | [usage-web.md](usage-web.md) |

---

## 1. The model

```
   application           lh/ui                          where pixels go
 ┌────────────┐   ┌───────────────┐   ┌──────────────────────────────────────────┐
 │ entity     │──▶│ lh_ui_view_t  │──▶│ lh_ui_canvas_t + backend                 │
 │ tree       │   │ (draw, press, │   │  • lh_ui_canvas_backend_sw  → MCU pixmap │
 │ (decl/CSS) │   │  key, wheel)  │   │  • lh_os_render_backend_gdi → window     │
 └────────────┘   └───────────────┘   │  • lh_ui_canvas_backend_web → JSON       │
                                      └──────────────────────────────────────────┘
```

- **`lh_ui_view_t`** (`lh/ui/view.h`) — an entity tree bound to a canvas. It
  draws (`lh_ui_view_draw`), takes input (`press` / `move` / `release` /
  `wheel` / `key` / `text`, or one `lh_os_input_event_t` through
  `lh_ui_view_input`) and accumulates damage. It needs no OS.
- **Input** (`lh/os/input/`) — one event type for every input, one sink, a
  ring queue and a gesture recognizer; also no OS. A desktop window hands its
  input to one sink (`lh_os_window_set_on_input`), the web host to its `input`
  hook, an MCU's touch driver builds the same events itself.
- **`lh_ui_canvas_t`** + **backend** — *how* to draw. The software backend
  writes into a pixel buffer; the GDI backend is that same software renderer
  plus one `BitBlt` into the window; the web backend records primitives as
  JSON.
- **Present sink** (`lh/ui/present/sink.h`) — *where to show* a buffer that is
  already drawn. Used where the application owns the buffer (MCU): a TFT, a
  log, a second screen.

The application code (tree, click handlers, timer) is **one** for all three
targets — configurator builds both `configurator` and `cfg_web` from the same
`main.c`.

---

## 2. Adding lh with CMake

lh is a static library only and is added as a subdirectory:

```cmake
# Turn off what the host does not need, BEFORE add_subdirectory
set(LH_BUILD_DOCS  OFF CACHE BOOL "" FORCE)
set(LH_BUILD_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(lib/lh EXCLUDE_FROM_ALL)
target_link_libraries(my_app PRIVATE lh)
```

`target_link_libraries` brings the include paths (the generated `lh/config.h`
directory comes **first**) and the system libraries of the OS layer
(`ws2_32`, `user32`, `gdi32`, `dl`). Do not add your own
`include_directories` pointing at lh — it can pick up someone else's
`config.h`.

---

## 3. Option matrix by target

| Option | MCU (f4ve-bare) | Desktop | Web |
|---|---|---|---|
| `LH_LIBRARY_OPTION_OS` | **OFF** | ON | ON (sockets) |
| `LH_LIBRARY_OPTION_OS_WINDOW` | **OFF** | ON | not needed¹ |
| `LH_LIBRARY_OPTION_UI` | ON | ON | ON |
| `LH_LIBRARY_OPTION_UI_FONT_ROBOTO` | ON | ON | ON |
| `LH_LIBRARY_OPTION_THREAD_LOCAL` | **OFF** | ON | ON |
| `LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB` | **OFF** | ON | ON |
| `LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB` | **OFF** | ON | ON |
| `LH_LIBRARY_OPTION_RUNTIME_LOGGER_USE_STDLIB` | **OFF** | ON | ON |
| `LH_LIBRARY_OPTION_RUNTIME_IO_USE_STDLIB` | **OFF** | ON | ON |
| `LH_LIBRARY_OPTION_LTO` | **OFF** | FAT (default) | FAT |
| `LH_POSITION_INDEPENDENT_CODE` | **OFF** | ON | ON |

¹ The web host (`lh/ui/web/host.h`) does not need a window, but configurator
builds both targets from one build tree, so the window is on there anyway.

Everything turned off in the MCU column **must be wired by the host**: the
fatal exit (`lh_runtime_terminate_set`), output (`lh_runtime_writer_init`),
and the allocator (`lh_memory_allocator_set`) if anything allocates.

---

## 4. Common mistakes

| Symptom | Cause | Fix |
|---|---|---|
| On an MCU the runtime writer / terminate is `NULL`; crash on the first `print` | PIC: `.got` is never copied, globals are read through an empty table | `LH_POSITION_INDEPENDENT_CODE=OFF` and `-fno-pic -fno-pie` in the toolchain file |
| The linker wants `malloc` / `abort` / `stdout` in firmware | stdlib defaults are on | Turn off the matching `*_USE_STDLIB` options (table above) |
| `lh_os_window_open` is undefined at link time | Platform without a window backend (macOS) | Windows exist for Win32 and X11 only |
| Window "hangs" and stops repainting | A long loop inside `on_tick` / `on_paint` | One pass per call; nothing blocking in callbacks |
| Russian / non-ASCII text cut short | `snprintf` cuts UTF-8 in the middle of a code point | Size text buffers in bytes, ×2 for Cyrillic |
