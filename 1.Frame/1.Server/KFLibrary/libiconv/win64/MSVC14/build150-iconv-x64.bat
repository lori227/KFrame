
:  build iconv

"%VS150COMNTOOLS%..\IDE\Devenv" libiconv.sln /build "Debug|X64"
copy /y .\libiconv_static\x64\Debug\libiconv_a_debug.lib ..\..\..\..\_lib\win64\3rd\libiconvd.lib


"%VS150COMNTOOLS%..\IDE\Devenv" libiconv.sln /build "Release|X64"
copy /y .\libiconv_static\x64\Release\libiconv_a.lib ..\..\..\..\_lib\win64\3rd\libiconv.lib
