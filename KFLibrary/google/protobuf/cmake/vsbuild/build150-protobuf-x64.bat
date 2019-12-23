
:  build protobuf

"%VS150COMNTOOLS%..\IDE\Devenv" protobuf.sln /rebuild "Debug|X64" /project libprotobuf
"%VS150COMNTOOLS%..\IDE\Devenv" protobuf.sln /rebuild "Debug|X64" /project libprotoc
copy /y .\Debug\libprotobufd.lib ..\..\..\..\..\_lib\win64\debug\3rd\
copy /y .\Debug\libprotocd.lib ..\..\..\..\..\_lib\win64\debug\3rd\


"%VS150COMNTOOLS%..\IDE\Devenv" protobuf.sln /rebuild "Release|X64" /project libprotobuf
"%VS150COMNTOOLS%..\IDE\Devenv" protobuf.sln /rebuild "Release|X64" /project libprotoc
copy /y .\Release\libprotobuf.lib ..\..\..\..\..\_lib\win64\release\3rd\
copy /y .\Release\libprotoc.lib ..\..\..\..\..\_lib\win64\Release\3rd\
