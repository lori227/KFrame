
:  build zlib


"%VS150COMNTOOLS%..\IDE\Devenv" zlibvc.sln /build "Debug|X64"
copy /y .\x64\ZlibStatDebug\zlibstat.lib ..\..\..\..\..\_lib\win64\debug\3rd\zlibd.lib


"%VS150COMNTOOLS%..\IDE\Devenv" zlibvc.sln /build "Release|X64"
copy /y .\x64\ZlibStatRelease\zlibstat.lib ..\..\..\..\..\_lib\win64\release\3rd\zlib.lib
