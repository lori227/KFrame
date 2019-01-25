@echo off

cd proto
call __cpp_class.bat
cd ../

xcopy *.pb.* ..\..\Server\KFContrib\KFProtocol\ /y

del *.pb.*

cd ../../Server/KFContrib/KFProtocol

"%VS150COMNTOOLS%..\IDE\Devenv" KFProtocol.vcxproj /build "Debug|X64"
"%VS150COMNTOOLS%..\IDE\Devenv" KFProtocol.vcxproj /build "Release|X64"
cd ../../../Resource/protocol
