@echo off

if not exist build.exe (
    if exist build.pdb del build.pdb
    cl -nologo -FC -diagnostics:column -std:c11 -Oi build.c -Z7 -MTd -DBUILD_DEBUG -DWINDOWS_SUBSYSTEM=WINDOWS_SUBSYSTEM_CONSOLE -Isrc ^
        -link -subsystem:console -incremental:no -entry:entrypoint ^
        Kernel32.lib Ntdll.lib -Fe:build.exe
    if errorlevel 1 exit /b %errorlevel%
)

build.exe %*
