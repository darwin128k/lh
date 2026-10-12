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
`lh_ui_decl_t` rows, the styles, places and flows they point at, the theme colours by
name, and accessors by `#id`.

It reads CSS the way the web writes it -- as much of it as `lh` can draw -- and refuses
the rest with the line and the reason. One top-level rule with a tag and an `#id` is the
screen, and nesting is its tree; every other top-level rule styles it, with the cascade
worked out at generation time:

```css
:root { --accent: #2e6be6; --ink: #d2d8e2; }

.btn        { height: 28px; padding: 0 12px; border-radius: 6px; background: var(--accent); color: white; }
.btn:hover  { background: #4a82ef; }
.btn:active { background: #2457bd; }

div#app { width: 480px; height: 320px; padding: 12px; gap: 10px; color: var(--ink);
    header#top { display: flex; align-items: center; height: 32px; gap: 8px;
        h1#title       { flex: 1; text: "Devices"; }
        button#add.btn { width: 90px; text: "Add device"; }
    }
    div#list { flex: 1; border: 1px solid #2a313b; border-radius: 8px; overflow-y: auto;
        checkbox#c1 { text: "Sensor A"; checked: true; }
    }
    footer#bottom { display: flex; height: 28px; gap: 8px;
        input#name    { flex: 1; value: "New device"; maxlength: 32; }
        button#ok.btn { width: 80px; text: "OK"; }
    }
}
```

What it reads, in short (the module docstring has the whole list):

| | |
|---|---|
| states | `:hover`, `:active`, `:checked` and `transition: background 150ms ease-out` between them |
| selectors | tags (`box` or `div`, `label` or `p` / `h1` / `span`, `img`, `checkbox`, `input`, ...), `#id`, `.class`, ` ` and `>`, lists, `:hover` `:active` `:checked` |
| values | `px` `rem` `em` `%`, every CSS colour form, `var()` with fallbacks, `!important`, `inherit` / `initial` |
| layout | `display: flex / block / grid / none`, `flex-wrap`, `justify-content` incl. `space-*`, `align-items` / `-self`, `gap`, `flex` with weights and `flex-basis`, `margin` (incl. `auto` both ways), `min-` / `max-` sizes, `padding`, `aspect-ratio`, `position: absolute / fixed` with `left` / `right` / `top` / `bottom` / `inset`, `z-index` |
| look | `background` (colour or `linear-` / `radial-gradient()`), `color`, `border`, `border-radius`, `box-shadow`, `opacity`, `visibility`, `font`, `text-align`, `vertical-align`, `text-transform`, `text-overflow: ellipsis`, `white-space` (`normal` wraps at the box; one line is the default here, unlike CSS), `\A` line breaks |

What `lh` has no widget for is built out of what it has: a `button` with `text`, a
`checkbox` / `radio` / `switch` with a caption, a `progress` bar, `overflow: auto` (a
frame and a scroll bar), and rows placed by hand over a flow. The generated rows are
named after the one the sheet wrote: `#c1__text`, `#list__vbar`.

What a stylesheet does not carry is what the application loads or writes in C: an
image's mask, click callbacks (set through the generated `<prefix>button` /
`<prefix>toggle` accessors), and fonts -- `font: bold 14px Inter` names
`<prefix>font_inter_14_bold`, which the header declares and the application defines, so
a font nobody baked is a link error and not a caption in some other face.

```sh
python3 scripts/css2decl.py ui/window.css src/ui --prefix cfg_ui_
python3 scripts/css2decl.py ui/window.css src/ui --preview shot.png --build build --watch
```

| Flag | Meaning |
|------|---------|
| `--prefix` | the C name every generated symbol hangs off: `cfg_ui_decls`, `cfg_ui_colour_ink`, `cfg_ui_init`, `cfg_ui_find`, `cfg_ui_label` |
| `--preview` | also paint the screen into this PNG, with lh's own software canvas |
| `--build` | the lh build directory the preview links against (`liblh.a`, `include/lh/config.h`) |
| `--cc` | the compiler for the preview (default `$CC`, else `cc`) |
| `--watch` | paint the preview again every time the sheet is saved |

`--prefix` is an option and not a constant **because this script is not one
application's**. It must end in `_` -- every generated name is the prefix followed by a
name that does not start with one, so `cfg_ui_` gives `cfg_ui_decls` and a prefix without
the underscore would give `cfg_uidecls`.

Two checks keep it honest. `css2decl-selftest.py` runs a hundred-odd sheets that must be
refused (with the word that says why), accepted, or written a particular way. And the
CTest target `lh_test_css2decl` generates three sheets under `test/src/lh/ui/` at build
time, compiles them as C, builds the trees and checks them -- including that the first
`lh_ui_layout_apply` moves **no** row, because the generator works layouts out with the
engine's own arithmetic.

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
