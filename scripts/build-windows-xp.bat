@echo off
pushd "%~dp0.." >nul
if errorlevel 1 exit /b 1
call "scripts\build-windows-legacy.bat" --target-os xp %*
set "EXIT_CODE=%ERRORLEVEL%"
popd
exit /b %EXIT_CODE%
