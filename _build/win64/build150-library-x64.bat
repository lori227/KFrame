cd ../../

mkdir _lib
mkdir _lib\win64\debug
mkdir _lib\win64\release
mkdir _lib\win64\debug\3rd
mkdir _lib\win64\release\3rd

:================================================================
cd KFLibrary
:================================================================
:  build tcmalloc

cd tcmalloc
call build150-tcmalloc-x64.bat

cd ../
:================================================================
:  build protobuf

cd google/protobuf/cmake/vsbuild
call build150-protobuf-x64.bat

cd ../../../../
:================================================================

:  build libuv

cd libuv
call build150-libuv-x64.bat

cd ../
:================================================================

:  build redis

cd redis/win/msvs
call build150-redis-x64.bat

cd ../../../
:================================================================

:  build zlib

cd zlib/contrib/vstudio/vc11
call build150-zlib-x64.bat

cd ../../../../
:================================================================

:  build libiconv

cd libiconv/win64/MSVC14
call build150-iconv-x64.bat

cd ../../../
:================================================================

:  build luaplus

cd luaplus/projects
call build150-luaplus-x64.bat

cd ../../
:================================================================

:  build poco

cd poco

call build150-poco-x64.bat

cd ../
:================================================================
cd ../
mkdir _bin\bin
mkdir _bin\bin\win64
mkdir _bin\bin\win64\debug
mkdir _bin\bin\win64\release
xcopy /y _lib\win64\debug\3rd\*.dll _bin\bin\win64\debug\
xcopy /y _lib\win64\release\3rd\*.dll _bin\bin\win64\release\