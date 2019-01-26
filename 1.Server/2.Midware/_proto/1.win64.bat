@echo off

for /f "delims=" %%a in ('dir /b/a-d/oN *.proto') do 3.protoc.exe -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ %%a

xcopy *.pb.* ..\KFContrib\KFProtocol\ /y

del *.pb.*

cd ..\KFContrib\KFProtocol

"%VS150COMNTOOLS%..\IDE\Devenv" KFProtocol.vcxproj /build "Debug|X64"
"%VS150COMNTOOLS%..\IDE\Devenv" KFProtocol.vcxproj /build "Release|X64"
cd ..\..\_proto
