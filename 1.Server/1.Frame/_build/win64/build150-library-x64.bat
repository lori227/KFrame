cd ../../

mkdir _lib
mkdir _lib\win64

mkdir _bin
mkdir _bin\win64
:================================================================

cd KFLibrary
:================================================================

: openssl

copy /y openssl\lib\*.lib ..\_lib\win64\
copy /y openssl\bin\*.dll ..\_bin\win64\
:================================================================

:mysql

copy /y mysql\lib\*.lib ..\_lib\win64\
copy /y mysql\lib\*.dll ..\_bin\win64\
:================================================================

:  build protobuf

cd google/protobuf/vsprojects
call build150-protobuf-x64.bat

cd ../../../
:================================================================

:  build log4cxx

cd log4cxx/log4cxx
call build150-log4cxx-x64.bat

cd ../../../
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

:  build json

cd json/makefiles/msvc2010
call build150-json-x64.bat

cd ../../../
:================================================================

:  build poco

cd poco
call build150-poco-x64.bat

cd ../
:================================================================