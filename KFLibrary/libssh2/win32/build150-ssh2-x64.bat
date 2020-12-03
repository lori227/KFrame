
"%VS150COMNTOOLS%..\IDE\Devenv" libssh2.sln /rebuild "OpenSSL LIB Debug|X64" /project libssh2
copy /y .\Debug_lib\libssh2d.lib ..\..\..\_lib\win64\debug\3rd\libssh2d.lib


"%VS150COMNTOOLS%..\IDE\Devenv" libssh2.sln /rebuild "OpenSSL LIB Release|X64" /project libssh2
copy /y .\Release_lib\libssh2.lib ..\..\..\_lib\win64\release\3rd\libssh2.lib