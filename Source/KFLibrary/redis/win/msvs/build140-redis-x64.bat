
:  build redis

"%VS140COMNTOOLS%..\IDE\Devenv" RedisServer.sln /build "Debug|X64"
copy /y .\x64\Debug\hiredis.lib ..\..\..\..\_lib\win64\hiredisd.lib
copy /y .\x64\Debug\Win32_Interop.lib ..\..\..\..\_lib\win64\Win32_Interopd.lib


"%VS140COMNTOOLS%..\IDE\Devenv" RedisServer.sln /build "Release|X64"
copy /y .\x64\Release\hiredis.lib ..\..\..\..\_lib\win64\hiredis.lib
copy /y .\x64\Release\Win32_Interop.lib ..\..\..\..\_lib\win64\Win32_Interop.lib
