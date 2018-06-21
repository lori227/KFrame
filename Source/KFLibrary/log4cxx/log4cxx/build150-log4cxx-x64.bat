
:  build log4cxx


call configure.bat

cd projects

"%VS150COMNTOOLS%..\IDE\Devenv" log4cxx.sln /build "Debug|X64"
copy /y .\Debug\log4cxx.lib ..\..\..\..\_lib\win64\log4cxxd.lib
copy /y .\Debug\log4cxx.dll ..\..\..\..\_bin\win64\log4cxxd.dll

"%VS150COMNTOOLS%..\IDE\Devenv" log4cxx.sln /build "Release|X64"
copy /y .\Release\log4cxx.lib ..\..\..\..\_lib\win64\log4cxx.lib
copy /y .\Release\log4cxx.dll ..\..\..\..\_bin\win64\log4cxx.dll