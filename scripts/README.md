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
plus its public `<name>_bits` / `<name>_advances` arrays; `--header` also
writes a header declaring them, `--notice` adds a licence line. The built-in
Roboto (`src/lh/ui/font/roboto.c`, `include/lh/ui/font/roboto.h`) is made
with:

```sh
python scripts/font.py --font fonts/Roboto-Regular.ttf --size 16 --bpp 4 --range 32-126 \
    --name lh_ui_font_roboto \
    --notice "Roboto is Copyright 2011 Google Inc. and licensed under the Apache License 2.0." \
    --header include/lh/ui/font/roboto.h --include lh/ui/font/roboto.h -o src/lh/ui/font/roboto.c
```

## Linux

Use the POSIX shell wrapper:

```sh
./scripts/build-linux.sh --config Release --run-tests
```

More options:

```sh
./scripts/build-linux.sh --help
```
