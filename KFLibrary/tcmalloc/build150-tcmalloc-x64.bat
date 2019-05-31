
:  build tcmalloc

"%VS150COMNTOOLS%..\IDE\Devenv" gperftools.sln /build "Debug|X64" /project libtcmalloc_minimal

copy /y .\x64\Debug\libtcmalloc_minimald.lib ..\..\_lib\win64\debug\3rd\
copy /y .\x64\Debug\libtcmalloc_minimald.dll ..\..\_lib\win64\debug\3rd\


"%VS150COMNTOOLS%..\IDE\Devenv" gperftools.sln /build "Release-Patch|X64" /project libtcmalloc_minimal

copy /y .\x64\Release-Patch\libtcmalloc_minimal.lib ..\..\_lib\win64\release\3rd\
copy /y .\x64\Release-Patch\libtcmalloc_minimal.dll ..\..\_lib\win64\release\3rd\
