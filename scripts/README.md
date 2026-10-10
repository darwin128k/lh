# Build Scripts

This directory contains small wrappers around the CMake build.

## Windows XP and Vista

Use these scripts from `cmd.exe`; PowerShell is not required.

Build for Windows XP:

```bat
scripts\build-windows-xp.bat --clean
```

Build for Windows Vista:

```bat
scripts\build-windows-vista.bat --clean
```

If CMake picks the wrong compiler, pass it explicitly:

```bat
scripts\build-windows-xp.bat --generator "MinGW Makefiles" --cc "C:\SysGCC\mingw64\bin\gcc.exe" --clean
```

The XP/Vista wrappers call `build-windows-legacy.bat`, which sets:

| Target | `WINVER` | `_WIN32_WINNT` | `CMAKE_SYSTEM_VERSION` |
|--------|----------|----------------|------------------------|
| XP     | `0x0501` | `0x0501`       | `5.1`                  |
| Vista  | `0x0600` | `0x0600`       | `6.0`                  |

Useful options:

```bat
scripts\build-windows-vista.bat --static --no-docs --no-tests
scripts\build-windows-vista.bat --tests --run-tests
scripts\build-windows-legacy.bat --help
```

Final XP/Vista compatibility also depends on the compiler runtime and linked
system libraries. For XP, prefer a 32-bit toolchain when the artifact must run
on ordinary XP systems.

## Fonts and images

`font.py` and `image.py` turn a font file or a picture into a C array the
program keeps in memory. The flags follow LVGL's font converter and
`LVGLImage.py`: size, bits per pixel, character range, and a color format.
The bytes are packed the same way (high bit first; straight red, green,
blue, alpha for a picture). Nothing is read from a file when the program runs.

```sh
python scripts/font.py --font Roboto.ttf --size 16 --bpp 4 --range 32-126 -o roboto_16.c
python scripts/image.py --cf ARGB8888 -o icon.c icon.png
```

`image.py` reads PNG with the standard library. `font.py` needs Pillow
(`pip install pillow`) to rasterize the TTF. It writes an `lh_ui_font_t`
over cropped `lh_ui_mask_t` glyphs (shared `bits`, plus `glyphs` / `tops` /
`advances` and the `lh_ui_font_range_t` table); `--header` also writes a
header declaring them, `--notice` adds a licence line. The built-in Roboto
(`src/lh/ui/font/roboto.c`, `include/lh/ui/font/roboto.h`) is made with:

```sh
python scripts/font.py --font fonts/Roboto-Regular.ttf --size 16 --bpp 4 --lang ru \
    --name lh_ui_font_roboto \
    --notice "Roboto is Copyright 2011 Google Inc. and licensed under the Apache License 2.0." \
    --header include/lh/ui/font/roboto.h --include lh/ui/font/roboto.h -o src/lh/ui/font/roboto.c
```

`--lang` takes a **name** rather than a range, because a range you have to remember is a
range you will get wrong quietly: the font comes out looking fine, the build is clean,
and the text that needed it simply is not drawn. The names are `ascii`, `latin`,
`cyrillic`, `punct` and `ru`, they may be listed together (`--lang latin,cyrillic`), and
a code typed with `--range` is added to them rather than losing to them. Neither flag
means `ascii`, which is what it always meant.

That is 224 glyphs in **7 ranges**, not one: a code the font has no outline
for and that is therefore dropped has a zero advance, and a run is broken at
every such hole rather than carrying an empty slot (a range promises a glyph,
so a hole in the middle of it is a promise the font cannot keep). `--range`
takes comma-separated `first-last` pairs in hex or decimal, and the script
prints the ranges it built and the codes it dropped. Giving one dense range
(`32-0x4FF`) would work and would cost about 35 KB for the same 224 glyphs:
the mask, advance and top tables would carry 1216 slots each, 90% of them
empty. Latin, Latin-1 supplement, en/em dash, numero sign and the basic
Cyrillic block is what this interface actually asks for.

`scripts/font-selftest.py` checks the names against that font and against the
checked-in `roboto.h`, because a preset that means something else produces a
wrong font rather than an error.

## Describing a screen: `css2decl.py`

`include/lh/ui/decl.h` says there is no parser and no markup language, and that a
declaration is a `const` array the compiler puts in flash. `css2decl.py` is what writes
that array: it reads a stylesheet and emits `ui_generated.c` and `ui_generated.h` --
`lh_ui_decl_t` rows, `lh_ui_style_t`, `lh_ui_insets_t`, `lh_ui_color_t` and an
`lh_cfg_ui_find` by name.

It lives **here** rather than in an application because everything it emits is `lh_ui_*`,
and because the dialect it accepts is *this library's*, not a browser's: `box#id`,
`button#id`, `label#id`, absolute `left`/`top`, `padx`, `valign`, `text-align`,
`radius`, `background`, `color`. It is not a CSS parser and will not read a stylesheet
written for one.

```sh
python3 scripts/css2decl.py ui/window.css src/ui --prefix cfg_ui_
```

| Flag | Meaning |
|------|---------|
| `--prefix` | the C name every generated symbol hangs off: `cfg_ui_decls`, `cfg_ui_colour_ink`, `cfg_ui_init`, `cfg_ui_find`, `cfg_ui_label` |

`--prefix` is an option and not a constant **because this script is not one
application's**. It must end in `_` -- every generated name is the prefix followed by a
name that does not start with one, so `cfg_ui_` gives `cfg_ui_decls` and a prefix without
the underscore would give `cfg_uidecls`.

`css2decl-selftest.py` runs the checks: that every property the grammar names is one it
emits, that a bad sheet fails with the line that is wrong rather than a traceback, and
that the emitted table satisfies the rules `decl.h` states -- a parent is a lower index,
exactly one root.

## Icons: `gen-icons.py`

`lh/ui/mask.h` says it in one sentence: *"A font glyph is a mask; so is a baked icon or
cursor."* So an icon here is not a drawing routine and not a font -- it is **coverage**,
one 0..15 number per pixel, packed high bit first, 16 on a side at 4 bpp, and
`::lh_ui_canvas_fill_mask` paints it.

```sh
python3 scripts/gen-icons.py --out src/ui/icons.c --header include/lh/ui/icons.h \
    --prefix lh_ui_icon_
python3 scripts/gen-icons.py --selftest        # 5 checks, prints the denominator first
python3 scripts/icons-preview.py               # a PNG sheet, because packing is not shape
```

The shapes are lucide's proportions drawn with distance functions and supersampled 8x;
`--prefix` is the application's, because "which icons and what to call them" is the
application's answer and the mask layout is not. `icons-preview.py` exists because a
self-test over packing says nothing about whether the thing looks like a gear.

## Linux

Use the POSIX shell wrapper:

```sh
./scripts/build-linux.sh --config Release --run-tests
```

More options:

```sh
./scripts/build-linux.sh --help
```
