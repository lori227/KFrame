
:  build libuv

"%VS150COMNTOOLS%..\IDE\Devenv" uv.sln /build "Debug|X64" /project libuv
copy /y .\Debug\lib\libuv.lib ..\..\_lib\win64\debug\3rd\libuvd.lib


"%VS150COMNTOOLS%..\IDE\Devenv" uv.sln /build "Release|X64" /project libuv
copy /y .\Release\lib\libuv.lib ..\..\_lib\win64\release\3rd\libuv.lib
