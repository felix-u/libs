@echo off

setlocal
set name=name

for %%a in (%*) do set "%%a=1"
if not "%msvc%"=="1" if not "%clang%"=="1" set msvc=1
if not "%release%"=="1"                    set debug=1

set dir_deps=..\deps
set include_paths=-I%dir_deps%

set cl_common=cl -nologo -FC -diagnostics:column -std:c11 -MT %include_paths%
set clang_common=clang -pedantic -Wno-microsoft -std=c11 -MT %include_paths%
set cl_link=-link -incremental:no user32.lib d3d11.lib dxguid.lib dxgi.lib d3dcompiler.lib
set clang_link=-luser32 -ld3d11 -ldxguid -ldxgi -ld3dcompiler
set cl_debug=%cl_common% -W4 -WX -Z7 -DBUILD_DEBUG=1 -fsanitize=address
set clang_debug=%clang_common% ^
    -Wall -Werror -Wextra -Wshadow -Wconversion -Wdouble-promotion ^
    -Wno-unused-function -Wno-deprecated-declarations -fno-strict-aliasing ^
    -g3 -fsanitize=address,undefined -fsanitize-trap -DBUILD_DEBUG=1
set cl_out=-out:
set clang_out=-o

if "%msvc%"=="1"  set compile_debug=%cl_debug%
if "%msvc%"=="1"  set compile_link=%cl_link%
if "%msvc%"=="1"  set compile_out=%cl_out%
if "%clang%"=="1" set compile_debug=%clang_debug%
if "%clang%"=="1" set compile_link=%clang_link%
if "%clang%"=="1" set compile_out=%clang_out%

if not exist build mkdir build
pushd build

if exist %name%.pdb del %name%.pdb
%compile_debug% ..\src\main.c %compile_link% %compile_out%%name%.exe

popd
