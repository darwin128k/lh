# gx on a microcontroller

What a gx screen costs on a Cortex-M4 with no operating system: the program of
`mcu_gx.c` — a 320×240 RGB565 panel, a background and a button with a caption
in the built-in font (Roboto), drawn through a band of 20 rows
(`lh_gx_strip_draw`) and sent to the panel's data register; then a press and a
release, and the frame that redraws the button — against `mcu_empty.c`, a
program that does nothing with the same C library.

Built 2026-11-19 with the Arm GNU Toolchain 12.2 (`arm-none-eabi-gcc`),
`MinSizeRel` (`-Os`), `-mcpu=cortex-m4 -mthumb -mfloat-abi=soft`,
`-ffunction-sections -fdata-sections`, linked with `--gc-sections`,
newlib-nano and no system calls (`cmake/toolchain/cortex-m4.cmake`). lh with
`LH_LIBRARY_OPTION_OS=OFF`, `LH_LIBRARY_OPTION_GL=ON`. No heap: every object
is static.

## Totals (`arm-none-eabi-size`, bytes)

| program | text (flash) | data | bss (RAM) |
|---|---:|---:|---:|
| `mcu_empty` | 432 | 8 | 32 |
| `mcu_gx` | 29 764 | 8 | 13 544 |
| **gx's part** | **29 332** | 0 | **13 512** |

How it grew, step by step:

| step | gx's flash | gx's RAM |
|---|---:|---:|
| the first measure | 28 156 | 13 452 |
| a radius per corner (`lh_gx_corners_t`): the box's strips, four radii a style | 28 704 | 13 488 |
| the brush (`lh_gx_brush_t`), a style's gradient and its table | 29 332 | 13 512 |

The program draws no gradient and takes the canvas that draws colours only
(`lh_gx_backend_sw_canvas_init_solid`): nothing of the gradient is called, so
none of it is linked — the brush costs 628 bytes. With the full canvas
(`lh_gx_backend_sw_canvas_init`) the same program is 36 564 bytes of flash,
6.8 KB more: the gradient, its table and sampler (4.7 KB, the radial walk's
table of 1 KB among them), the rows of a colour per pixel (1.5 KB) and the
64-bit division of libgcc (0.7 KB).

## Flash, by what it is

| part | bytes |
|---|---:|
| the font: Roboto's bitmaps, glyph table, tops and advances | 17 060 |
| lh code: the renderer, the walk, box, text, button, tap, input, strip, the brush, the pixel kernels | 11 456 |
| the program: `main`, `panel_show`, the click | 834 |

The font is more than half of it. A board that has its own font, or a smaller
one, swaps it through the font contract (`lh_gx_font_t`); the code stays at
about 11.5 KB. The kernels for the ARGB format (`lh_pixel_span_*_argb`, 584
bytes) are linked too: the software backend picks the format at run time.

## RAM, by what it is

| part | bytes |
|---|---:|
| the band: 320 × 20 RGB565 pixels | 12 800 |
| the renderer (its damage list) | 320 |
| the button (its box, caption, tap) | 124 |
| the background entity | 52 |
| three styles (44 bytes each), the font, the pixmap, the backend, the canvas, the present, the pointer, the focus | 216 |

Everything apart from the band is 712 bytes. The band is the program's choice:
10 rows halve it, a full frame (153 600 bytes) does not fit in most M4 parts,
and the picture on the panel is the same either way (`test/src/lh/gx/strip.cpp`).

## Run it

```sh
cmake -S bench/mcu -B build-mcu -G Ninja \
      -DCMAKE_TOOLCHAIN_FILE=$PWD/cmake/toolchain/cortex-m4.cmake
cmake --build build-mcu
cmake --build build-mcu --target sizes
```

The bytes by symbol: `arm-none-eabi-nm -S --size-sort -t d build-mcu/mcu_gx`.
