@echo off

set currentpath=%cd%

cd %1
"%VS150COMNTOOLS%..\IDE\Devenv" KFProtocol.vcxproj /build "Debug|X64"  /project KFProtocol
"%VS150COMNTOOLS%..\IDE\Devenv" KFProtocol.vcxproj /build "Release|X64" /project KFProtocol
cd %currentpath%
