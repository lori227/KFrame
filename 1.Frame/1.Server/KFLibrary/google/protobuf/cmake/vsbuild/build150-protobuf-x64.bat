
:  build protobuf

"%VS150COMNTOOLS%..\IDE\Devenv" protobuf.sln /build "Debug|X64" /project libprotobuf
copy /y .\x64\Debug\libprotobufd.lib ..\..\..\..\..\_lib\win64\3rd\


"%VS150COMNTOOLS%..\IDE\Devenv" protobuf.sln /build "Release|X64" /project libprotobuf
copy /y .\x64\Release\libprotobuf.lib ..\..\..\..\..\_lib\win64\3rd\
