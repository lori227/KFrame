
:  build json

"%VS150COMNTOOLS%..\IDE\Devenv" jsoncpp.sln /build "Debug|X64"
copy /y x64\Debug\lib_json.lib  ..\..\..\..\_lib\win64\3rd\jsoncppd.lib


"%VS150COMNTOOLS%..\IDE\Devenv" jsoncpp.sln /build "Release|X64"
copy /y x64\Release\lib_json.lib ..\..\..\..\_lib\win64\3rd\jsoncpp.lib
