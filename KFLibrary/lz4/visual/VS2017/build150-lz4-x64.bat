
:  build zlib


"%VS150COMNTOOLS%..\IDE\Devenv" lz4.sln /rebuild "Debug|X64" /project liblz4
copy /y .\bin\x64_Debug\liblz4_static.lib ..\..\..\..\_lib\win64\debug\3rd\liblz4d.lib


"%VS150COMNTOOLS%..\IDE\Devenv" lz4.sln /rebuild "Release|X64" /project liblz4
copy /y .\bin\x64_Release\liblz4_static.lib ..\..\..\..\_lib\win64\release\3rd\liblz4.lib
