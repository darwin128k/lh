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

With a radius per corner (`lh_gx_corners_t`): the box's strips cost 548 bytes
of flash over one radius, and each style 12 bytes of RAM more (four radii, not
one); the first measure was 28 156 and 13 452.

| program | text (flash) | data | bss (RAM) |
|---|---:|---:|---:|
| `mcu_empty` | 432 | 8 | 32 |
| `mcu_gx` | 29 136 | 8 | 13 520 |
| **gx's part** | **28 704** | 0 | **13 488** |

## Flash, by what it is

| part | bytes |
|---|---:|
| the font: Roboto's bitmaps, glyph table, tops and advances | 17 060 |
| lh code: the renderer, the walk, box, text, button, tap, input, strip, the pixel kernels | 10 826 |
| the program: `main`, `panel_show`, the click | 834 |

The font is more than half of it. A board that has its own font, or a smaller
one, swaps it through the font contract (`lh_gx_font_t`); the code stays at
about 11 KB. The kernels for the ARGB format (`lh_pixel_span_*_argb`, 584
bytes) are linked too: the software backend picks the format at run time.

## RAM, by what it is

| part | bytes |
|---|---:|
| the band: 320 × 20 RGB565 pixels | 12 800 |
| the renderer (its damage list) | 320 |
| the button (its box, caption, tap) | 124 |
| the background entity | 52 |
| three styles, the font, the pixmap, the backend, the canvas, the present, the pointer, the focus | 192 |

Everything apart from the band is 688 bytes. The band is the program's choice:
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
