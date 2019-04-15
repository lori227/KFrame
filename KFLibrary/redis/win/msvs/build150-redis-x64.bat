
:  build redis

"%VS150COMNTOOLS%..\IDE\Devenv" RedisServer.sln /build "Debug|X64"
copy /y .\x64\Debug\hiredis.lib ..\..\..\..\_lib\win64\debug\3rd\hiredisd.lib
copy /y .\x64\Debug\Win32_Interop.lib ..\..\..\..\_lib\win64\debug\3rd\Win32_Interopd.lib


"%VS150COMNTOOLS%..\IDE\Devenv" RedisServer.sln /build "Release|X64"
copy /y .\x64\Release\hiredis.lib ..\..\..\..\_lib\win64\release\3rd\hiredis.lib
copy /y .\x64\Release\Win32_Interop.lib ..\..\..\..\_lib\win64\release\3rd\Win32_Interop.lib
