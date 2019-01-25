
:  build luaplus

"%VS150COMNTOOLS%..\IDE\Devenv" LuaPlus.sln /build "Debug|X64"
copy /y .\x64\Debug\LuaPlusd.lib ..\..\..\_lib\win64\3rd\LuaPlusd.lib


"%VS150COMNTOOLS%..\IDE\Devenv" LuaPlus.sln /build "Release|X64"
copy /y .\x64\Release\LuaPlus.lib ..\..\..\_lib\win64\3rd\LuaPlus.lib
