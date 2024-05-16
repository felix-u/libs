@echo off

set name=template

set cl_common=cl -nologo -std:c11 ..\src\main.c

set cl_link=-link -incremental:no

set cl_debug=%cl_common% -W4 -WX -Z7 -DBUILD_DEBUG -fsanitize=address

if not exist build mkdir build
pushd build

if exist %name%.pdb del %name%.pdb
%cl_debug% %cl_link% -out:%name%.exe

popd
