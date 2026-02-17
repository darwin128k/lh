@echo off
for /r include %%i in (*.h *.hpp) do clang-format -i "%%i"
for /r src %%i in (*.c *.cpp *.cxx *.cc) do clang-format -i "%%i"