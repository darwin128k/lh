@echo off
setlocal EnableExtensions

set "TARGET_OS=vista"
set "CONFIG=Release"
set "BUILD_DIR=build\windows-vista"
set "GENERATOR=MinGW Makefiles"
set "C_COMPILER="
set "CXX_COMPILER="
set "TARGET="
set "SHARED=ON"
set "DOCS=OFF"
set "TESTS=OFF"
set "CLEAN=0"
set "RUN_TESTS=0"
set "PARALLEL=1"

:parse
if "%~1"=="" goto after_parse
if /I "%~1"=="--help" goto usage
if /I "%~1"=="-h" goto usage
if /I "%~1"=="--target-os" (
    set "TARGET_OS=%~2"
    shift
    shift
    goto parse
)
if /I "%~1"=="--config" (
    set "CONFIG=%~2"
    shift
    shift
    goto parse
)
if /I "%~1"=="--build-dir" (
    set "BUILD_DIR=%~2"
    shift
    shift
    goto parse
)
if /I "%~1"=="--generator" (
    set "GENERATOR=%~2"
    shift
    shift
    goto parse
)
if /I "%~1"=="--cc" (
    set "C_COMPILER=%~2"
    shift
    shift
    goto parse
)
if /I "%~1"=="--cxx" (
    set "CXX_COMPILER=%~2"
    shift
    shift
    goto parse
)
if /I "%~1"=="--target" (
    set "TARGET=%~2"
    shift
    shift
    goto parse
)
if /I "%~1"=="--shared" (
    set "SHARED=ON"
    shift
    goto parse
)
if /I "%~1"=="--static" (
    set "SHARED=OFF"
    shift
    goto parse
)
if /I "%~1"=="--docs" (
    set "DOCS=ON"
    shift
    goto parse
)
if /I "%~1"=="--no-docs" (
    set "DOCS=OFF"
    shift
    goto parse
)
if /I "%~1"=="--tests" (
    set "TESTS=ON"
    shift
    goto parse
)
if /I "%~1"=="--no-tests" (
    set "TESTS=OFF"
    shift
    goto parse
)
if /I "%~1"=="--clean" (
    set "CLEAN=1"
    shift
    goto parse
)
if /I "%~1"=="--run-tests" (
    set "RUN_TESTS=1"
    shift
    goto parse
)
if /I "%~1"=="--parallel" (
    set "PARALLEL=%~2"
    shift
    shift
    goto parse
)

echo Unknown option: %~1
echo.
goto usage_error

:after_parse
if /I "%TARGET_OS%"=="xp" (
    set "WINVER_HEX=0x0501"
    set "SYSTEM_VERSION=5.1"
    if "%BUILD_DIR%"=="build\windows-vista" set "BUILD_DIR=build\windows-xp"
    goto target_ok
)
if /I "%TARGET_OS%"=="vista" (
    set "WINVER_HEX=0x0600"
    set "SYSTEM_VERSION=6.0"
    goto target_ok
)

echo Unsupported target OS: %TARGET_OS%
echo Use: xp or vista
exit /b 2

:target_ok
if /I not "%CONFIG%"=="Debug" if /I not "%CONFIG%"=="Release" if /I not "%CONFIG%"=="RelWithDebInfo" if /I not "%CONFIG%"=="MinSizeRel" (
    echo Unsupported config: %CONFIG%
    exit /b 2
)

if "%RUN_TESTS%"=="1" if /I not "%TESTS%"=="ON" (
    echo --run-tests requires --tests.
    exit /b 2
)

if exist "%CD%\CMakeLists.txt" (
    set "REPO_ROOT=%CD%"
) else (
    pushd "%~dp0.." >nul
    if errorlevel 1 exit /b 1
    set "REPO_ROOT=%CD%"
    popd
)
pushd "%REPO_ROOT%" >nul
if errorlevel 1 exit /b 1
for %%I in ("%BUILD_DIR%") do set "BUILD_PATH=%%~fI"
popd

call set "PREFIX_CHECK=%%BUILD_PATH:%REPO_ROOT%=%%"
if "%PREFIX_CHECK%"=="%BUILD_PATH%" (
    echo Build directory must be inside the repository: %BUILD_PATH%
    exit /b 2
)

if /I "%BUILD_PATH%"=="%REPO_ROOT%" (
    echo Build directory cannot be the repository root.
    exit /b 2
)

if "%CLEAN%"=="1" if exist "%BUILD_PATH%" (
    echo Removing %BUILD_PATH%
    rmdir /S /Q "%BUILD_PATH%"
    if errorlevel 1 exit /b 1
)

echo Configuring lh for Windows %TARGET_OS% (%CONFIG%)
set "C_FLAGS=-DWINVER=%WINVER_HEX% -D_WIN32_WINNT=%WINVER_HEX%"
if not "%C_COMPILER%"=="" set "CC=%C_COMPILER%"
if not "%CXX_COMPILER%"=="" set "CXX=%CXX_COMPILER%"

if "%GENERATOR%"=="" (
    cmake -S "%REPO_ROOT%" -B "%BUILD_PATH%" ^
        "-DCMAKE_BUILD_TYPE=%CONFIG%" ^
        "-DCMAKE_SYSTEM_VERSION=%SYSTEM_VERSION%" ^
        "-DCMAKE_C_FLAGS=%C_FLAGS%" ^
        "-DLH_BUILD_SHARED=%SHARED%" ^
        "-DLH_BUILD_DOCS=%DOCS%" ^
        "-DLH_BUILD_TESTS=%TESTS%"
) else (
    cmake -S "%REPO_ROOT%" -B "%BUILD_PATH%" -G "%GENERATOR%" ^
        "-DCMAKE_BUILD_TYPE=%CONFIG%" ^
        "-DCMAKE_SYSTEM_VERSION=%SYSTEM_VERSION%" ^
        "-DCMAKE_C_FLAGS=%C_FLAGS%" ^
        "-DLH_BUILD_SHARED=%SHARED%" ^
        "-DLH_BUILD_DOCS=%DOCS%" ^
        "-DLH_BUILD_TESTS=%TESTS%"
)
if errorlevel 1 exit /b 1

echo Building lh for Windows %TARGET_OS% (%CONFIG%)
if "%TARGET%"=="" (
    cmake --build "%BUILD_PATH%" --config "%CONFIG%" --parallel "%PARALLEL%"
) else (
    cmake --build "%BUILD_PATH%" --config "%CONFIG%" --parallel "%PARALLEL%" --target "%TARGET%"
)
if errorlevel 1 exit /b 1

if "%RUN_TESTS%"=="1" (
    echo Running tests
    ctest --test-dir "%BUILD_PATH%" -C "%CONFIG%" --output-on-failure
    if errorlevel 1 exit /b 1
)

exit /b 0

:usage_error
call :usage
exit /b 2

:usage
echo Usage: scripts\build-windows-legacy.bat [options]
echo.
echo Options:
echo   --target-os ^<xp^|vista^>   Target Windows API level (default: vista)
echo   --config ^<name^>           Debug, Release, RelWithDebInfo, or MinSizeRel
echo   --build-dir ^<path^>        Build directory inside the repo
echo   --generator ^<name^>        CMake generator, for example "MinGW Makefiles"
echo   --cc ^<path^>               C compiler passed to CMake
echo   --cxx ^<path^>              C++ compiler passed to CMake
echo   --target ^<name^>           Build a specific target
echo   --shared                    Build shared library
echo   --static                    Build static library
echo   --docs / --no-docs          Enable or disable docs target
echo   --tests / --no-tests        Enable or disable tests
echo   --clean                     Remove build directory first
echo   --run-tests                 Run ctest after build
echo   --parallel ^<n^>            Parallel build jobs
echo   -h, --help                  Show this help
exit /b 0
