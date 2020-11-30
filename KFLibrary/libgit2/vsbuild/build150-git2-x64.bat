
:  build libuv

"%VS150COMNTOOLS%..\IDE\Devenv" libgit2.sln /rebuild "Debug|X64" /project git2
copy /y .\Debug\git2.lib ..\..\..\_lib\win64\debug\3rd\git2d.lib


"%VS150COMNTOOLS%..\IDE\Devenv" libgit2.sln /rebuild "Release|X64" /project git2
copy /y .\Release\git2.lib ..\..\..\_lib\win64\release\3rd\git2.lib