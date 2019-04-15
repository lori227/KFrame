
:  build protobuf

"%VS150COMNTOOLS%..\IDE\Devenv" protobuf.sln /build "Debug|X64" /project libprotobuf
copy /y .\Debug\libprotobufd.lib ..\..\..\..\..\_lib\win64\debug\3rd\


"%VS150COMNTOOLS%..\IDE\Devenv" protobuf.sln /build "Release|X64" /project libprotobuf
copy /y .\Release\libprotobuf.lib ..\..\..\..\..\_lib\win64\release\3rd\
