@echo off

set compiler=zig cc -std=c99 -pedantic ^
    -Wall -Werror -Wextra -Wshadow -Wconversion -Wdouble-promotion ^
    -Wno-unused-function -Wno-sign-conversion -fno-strict-aliasing ^
    -g3 -fsanitize=undefined -fsanitize-trap -DDEBUG

%compiler% -o name.exe src\main.c
