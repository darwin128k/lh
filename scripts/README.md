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

## Modern Windows

Use the PowerShell wrapper:

```powershell
.\scripts\build-windows.ps1 -Config Release -RunTests
```

If local policy blocks unsigned scripts:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build-windows.ps1 -Config Release -RunTests
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
