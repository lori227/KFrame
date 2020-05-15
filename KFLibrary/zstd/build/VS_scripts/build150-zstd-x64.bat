
:  build zstd

call build.VS2017.cmd
copy /y .\bin\Debug\x64\libzstd_static.lib ..\..\..\..\_lib\win64\debug\3rd\libzstdd.lib
copy /y .\bin\Release\x64\libzstd_static.lib ..\..\..\..\_lib\win64\release\3rd\libzstd.lib
