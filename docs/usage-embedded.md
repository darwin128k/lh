# lh on a microcontroller (no OS)

A walk through the hot spots of the `H:\Projects\f4ve-bare` firmware
(STM32F407VET6 + 3.2″ ILI9341 TFT, 16-bit FSMC). Each section is one place
where the firmware touches lh: what happens there and what to repeat in your
own project.

Overview and option matrix: [usage.md](usage.md).

---

## 1. Build: the firmware's `CMakeLists.txt`

Everything lh takes from libc and the OS by default is turned off **before**
`add_subdirectory`:

```cmake
if (F4VE_USE_LH)
    set(LH_BUILD_DOCS  OFF CACHE BOOL "" FORCE)
    set(LH_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(LH_BUILD_BENCH OFF CACHE BOOL "" FORCE)

    # No OS: no files, sockets or windows
    set(LH_LIBRARY_OPTION_OS        OFF CACHE BOOL "" FORCE)
    set(LH_LIBRARY_OPTION_OS_WINDOW OFF CACHE BOOL "" FORCE)

    # Software canvas + present sinks (lh_ui_display_t)
    set(LH_LIBRARY_OPTION_UI              ON CACHE BOOL "" FORCE)
    # Roboto 16 px / 4bpp — text on the TFT
    set(LH_LIBRARY_OPTION_UI_FONT_ROBOTO  ON CACHE BOOL "" FORCE)

    # No TLS, no default malloc/abort/stdio — the host wires them
    set(LH_LIBRARY_OPTION_THREAD_LOCAL                 OFF CACHE BOOL "" FORCE)
    set(LH_LIBRARY_OPTION_MEMORY_ALLOCATOR_USE_STDLIB  OFF CACHE BOOL "" FORCE)
    set(LH_LIBRARY_OPTION_RUNTIME_TERMINATE_USE_STDLIB OFF CACHE BOOL "" FORCE)
    set(LH_LIBRARY_OPTION_RUNTIME_LOGGER_USE_STDLIB    OFF CACHE BOOL "" FORCE)
    set(LH_LIBRARY_OPTION_RUNTIME_IO_USE_STDLIB        OFF CACHE BOOL "" FORCE)

    set(LH_LIBRARY_OPTION_LTO OFF CACHE STRING "" FORCE)
    # Bare metal: no loader to fix up .got
    set(LH_POSITION_INDEPENDENT_CODE OFF CACHE BOOL "" FORCE)

    add_subdirectory(lib/lh EXCLUDE_FROM_ALL)
    target_compile_definitions(f4ve_firmware PRIVATE F4VE_USE_LH=1)
    target_link_libraries(f4ve_firmware PRIVATE lh)
endif ()
```

Why:

- **`LH_POSITION_INDEPENDENT_CODE=OFF`** is the main trap. With PIC the
  runtime globals (writer, terminate, …) are read through `.got`, which
  nothing copies on bare metal — so they come out `NULL`. Besides the option,
  the toolchain file (`cmake/arm-none-eabi.cmake`) passes `-fno-pic -fno-pie`.
- **`LTO=OFF`** — an archive of machine code links with any
  `arm-none-eabi-gcc`, no LTO plugin required.
- **`*_USE_STDLIB=OFF`** — otherwise `malloc`, `abort` and `stdout` are pulled
  into the firmware. The host installs its own handlers instead (section 2).

Toolchain:

```cmake
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfloat-abi=soft")
set(CMAKE_C_FLAGS_INIT "${CPU_FLAGS} -fno-pic -fno-pie -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${CPU_FLAGS} -fno-pic -fno-pie -Wl,--gc-sections -specs=nano.specs -specs=nosys.specs")
```

`-ffunction-sections` + `--gc-sections` drop everything unused from lh — you
pay only for what you call.

Build:

```bat
cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

---

## 2. Runtime: `src/app.c`, `src/board.c`

With `*_USE_STDLIB=OFF` the runtime has no output and no fatal exit. The
first thing the application does is wire them:

```c
#include <lh/null.h>
#include <lh/runtime/terminate.h>
#include <lh/runtime/writer.h>

static void
host_terminate(void)
{
    extern void HardFault_Handler(void);
    HardFault_Handler();          /* failed contract → into the debugger */
}

void
f4ve_app_run(void)
{
    lh_runtime_terminate_set(host_terminate);
    lh_runtime_writer_init(f4ve_usart1_writer_cb, lh_null);
    /* ... */
}
```

The writer is a plain `lh_io_writer_write_cb`: take a buffer, return how many
bytes were written.

```c
lh_ssize_t
f4ve_usart1_writer_cb(lh_self_ptr self, const lh_ptr buf, lh_usize_t size)
{
    const lh_byte_t *p = lh_cast_static(const lh_byte_t *, buf);
    lh_usize_t i;

    (void)self;
    for (i = 0; i < size; ++i)
    {
        if (f4ve_usart1_putc(p[i]) != 0)
        {
            return lh_cast_static(lh_ssize_t, i);
        }
    }
    return lh_cast_static(lh_ssize_t, size);
}
```

From then on `lh_runtime_writer_print("tick %u\r\n", n)` works — lh's own
printf engine, no newlib `printf`.

What else can be wired the same way:

| What | Call | When |
|---|---|---|
| Allocator | `lh_memory_allocator_set(lh_runtime_allocator(), …)` or `-DLH_LIBRARY_OPTION_MEMORY_ALLOCATOR_DEFAULT_ALLOC=pvPortMalloc` | `lh_array_t`, `lh_str_t`, anything that grows |
| Logger | `lh_logger_set(lh_runtime_logger(), …)` | `lh_logger_info` and friends |
| Reader | `lh_io_reader_set(lh_runtime_reader(), …)` | input from a UART |

The firmware installs no allocator: the UI and display run on static buffers.

---

## 3. Display: `src/display.c` — `lh_ui_display_t`

`lh_ui_display_t` (`lh/ui/display.h`) packages "a software canvas over an
application-owned buffer + a table of present sinks". The application owns
the buffer; on an MCU it is a **strip** — part of the screen, not the whole
frame.

```c
#include <lh/ui/display.h>
#include <lh/ui/pixmap/format.h>
#include <lh/ui/present/sink.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>

/* 320 × 48 RGB565 ≈ 30 KB — about 1/5 of the panel, like LVGL PARTIAL */
static uint16_t s_fb[F4VE_TFT_WIDTH * F4VE_FB_STRIP_H];
static uint16_t s_strip_y;
static lh_ui_display_t s_disp;
static lh_ui_present_sink_t s_sink;

void
f4ve_display_init(void)
{
    lh_ui_size_t panel;

    lh_ui_size_init(lh_addr_of(panel), lh_ui_scalar(F4VE_TFT_WIDTH), lh_ui_scalar(F4VE_TFT_HEIGHT));

    lh_ui_display_init(lh_addr_of(s_disp));
    lh_ui_display_set_panel(lh_addr_of(s_disp), panel);
    lh_ui_display_set_format(lh_addr_of(s_disp), lh_ui_pixmap_format_rgb565);
    lh_ui_display_set_strip_height(lh_addr_of(s_disp), lh_ui_scalar(F4VE_FB_STRIP_H));
    lh_ui_display_set_buffer(lh_addr_of(s_disp), (lh_byte_ptr)s_fb,
                             (lh_s32_t)(F4VE_TFT_WIDTH * 2u));   /* stride in bytes */

    lh_ui_present_sink_init(lh_addr_of(s_sink), "f4ve", f4ve_lh_present, lh_addr_of(s_strip_y));
    lh_ui_display_set_sink(lh_addr_of(s_disp), 0u, lh_addr_of(s_sink));
}
```

Working with a strip — move the strip window, draw, present:

```c
void
f4ve_display_set_strip_y(uint16_t y0)
{
    s_strip_y = y0;                                         /* for the sink */
    lh_ui_display_set_strip_y(lh_addr_of(s_disp), lh_ui_scalar(y0));
}

void
f4ve_display_present(void)
{
    lh_ui_display_present(lh_addr_of(s_disp), lh_null);    /* NULL = whole strip */
}
```

Rules:

- **Format.** On a panel with a 16-bit bus use `rgb565` — the buffer goes to
  the panel unconverted. On the desktop it is the other way round (see
  [usage-desktop.md](usage-desktop.md)).
- **Strip height** — 1/10 to 1/5 of the panel. `0` = a full-screen buffer.
- **The buffer is neither copied nor freed** — it is the application's static.
- You can keep several sinks (`index` < `LH_UI_PRESENT_SINKS_MAX`) and toggle
  them with `lh_ui_display_set_sink_enabled`. One frame goes to every enabled
  sink.

---

## 4. Present sink: `src/lh_present.c`

A sink is an `lh_ui_present_fn`: "show these rects of this pixmap". The
`drawn` coordinates are **strip-local**, so the sink gets the strip offset
through `context`.

```c
lh_void
f4ve_lh_present(lh_ptr context, const lh_ui_pixmap_t *pixels, const lh_ui_rects_t *drawn)
{
    uint16_t strip_y = context != lh_null ? *(const uint16_t *)context : 0;
    lh_u32_t i, n;

    if (pixels == lh_null)
        return;

    /* Empty / NULL drawn = the whole pixmap */
    if (drawn == lh_null || lh_ui_rects_is_empty(drawn))
    {
        flush_rect(pixels, 0, 0, lh_ui_pixmap_get_width(pixels),
                   lh_ui_pixmap_get_height(pixels), strip_y);
        return;
    }

    n = lh_ui_rects_get_count(drawn);
    for (i = 0; i < n; ++i)
    {
        const lh_ui_rect_t *r = lh_ui_rects_get(drawn, i);
        const lh_ui_point_t *o = lh_ui_rect_get_origin(r);
        const lh_ui_size_t *s = lh_ui_rect_get_size(r);

        flush_rect(pixels, lh_ui_point_get_x(o), lh_ui_point_get_y(o),
                   lh_ui_size_get_width(s), lh_ui_size_get_height(s), strip_y);
    }
}
```

`flush_rect` takes the address of the first row with
`lh_ui_pixmap_get_address(pixels, x, y)` and hands it to the panel driver
(`f4ve_tft_flush`) with a stride of `lh_ui_pixmap_get_width(pixels)` pixels.
Everything that knows the hardware (MADCTL, mirroring, the ILI9341 write
window) stays in the driver — **lh never names a concrete peripheral**.

Plugging in your own display is exactly one such function.

---

## 5. Text: `src/tft.c` — Roboto without the UI tree

The font can be used directly, without a canvas or widgets
(`LH_LIBRARY_OPTION_UI_FONT_ROBOTO=ON` works even with `UI=OFF`):

```c
#include <lh/ui/font/roboto.h>
#include <lh/ui/mask.h>

const lh_ui_font_t *font = lh_addr_of(lh_ui_font_roboto);
lh_ui_mask_t glyph;
int32_t advance = lh_ui_font_get_advance(font, code);

if (lh_ui_font_get_glyph(font, code, lh_addr_of(glyph)))
{
    int32_t gw = lh_ui_mask_get_width(lh_addr_of(glyph));
    int32_t gh = lh_ui_mask_get_height(lh_addr_of(glyph));
    /* origin_y is the top of the line box; ink starts at ascent + glyph top */
    int32_t gy0 = origin_y + lh_ui_font_get_ascent(font) + lh_ui_font_get_top(font, code);

    for (row = 0; row < gh; ++row)
        for (col = 0; col < gw; ++col)
        {
            uint8_t cover = lh_ui_mask_get_coverage(lh_addr_of(glyph), col, row);
            if (cover)
                put_pixel(origin_x + col, gy0 + row, blend565(fg, bg, cover));
        }
}
x += advance;
```

`cover` is 0…255 coverage; anti-aliasing is a blend of `fg` and `bg`. This is
the cheapest way to decent text on an MCU.

---

## 6. Next step: the UI tree on an MCU

Today f4ve-bare draws its interface with the `tft.c` functions, while
`lh_ui_display_t` is wired and ready. To draw with an lh widget tree (the same
one the desktop uses), bind an `lh_ui_view_t` to the display's canvas:

```c
static lh_ui_view_t s_view;

lh_ui_view_init(lh_addr_of(s_view));
lh_ui_view_set_canvas(lh_addr_of(s_view), lh_ui_display_get_canvas(lh_addr_of(s_disp)));
lh_ui_view_set_root(lh_addr_of(s_view), root);   /* the tree — usage-desktop.md, section 4 */

/* once per strip: */
for (y = 0; y < F4VE_TFT_HEIGHT; y += F4VE_FB_STRIP_H)
{
    f4ve_display_set_strip_y(y);
    lh_ui_view_draw(lh_addr_of(s_view), lh_null);
    lh_ui_display_present(lh_addr_of(s_disp), lh_null);
}
```

XPT2046 touch goes into the same tree as input events (`lh/os/input/`, no OS
needed): the touch interrupt builds a `pointer_down` / `pointer_move` /
`pointer_up` with `lh_os_input_event_init_pointer` (device kind `touch`,
panel coordinates via `lh_os_input_coord_from_int`) and pushes it into an
`lh_os_input_queue_t`; the main loop drains the queue into a sink that calls
`lh_ui_view_input`. A single-touch panel is pointer `0`; a multi-touch one
gives each contact its own id below `LH_OS_INPUT_POINTER_MAX`, and the view
keeps a press session for each. `lh_ui_view_press` / `lh_ui_view_release`
(pointer `0`) and `lh_ui_view_press_pointer` / `_release_pointer` still work
directly. `lh_ui_view_tick` from a ~60 Hz timer drives kinetic scrolling;
for long press or double tap, feed the events through an
`lh_os_input_recognizer_t` and call `lh_os_input_recognizer_update` from the
same timer.

A wheel or rotary encoder sends `wheel` events in 1/120 notch
(`LH_OS_INPUT_WHEEL_NOTCH` a detent). With `LH_LIBRARY_OPTION_OS=OFF` there
is no system to ask how far a notch goes, so `lh_ui_view_input` scrolls
`LH_UI_VIEW_WHEEL_STEP` content units a notch: 40, the OS layer's own
default `LH_OS_INPUT_WHEEL_STEP_DEFAULT`. Both are `#ifndef`; define
`LH_UI_VIEW_WHEEL_STEP` for the build (e.g. `-DLH_UI_VIEW_WHEEL_STEP=20` in
the compile definitions) to change it. A build with the OS layer on a
platform without a kernel for it (STM32 and the like) gets the same 40 from
`lh_os_input_get_wheel_step`.

---

## Checklist for a new MCU project

1. A toolchain file with `-fno-pic -fno-pie`, `-ffunction-sections`, `--gc-sections`.
2. The `set(... FORCE)` block from section 1 before `add_subdirectory(lib/lh)`.
3. `lh_runtime_terminate_set` + `lh_runtime_writer_init` at the start of `main`.
4. A static strip buffer + `lh_ui_display_t` in the panel's format.
5. One `lh_ui_present_fn` → the panel driver.
6. An allocator and a logger if needed.
