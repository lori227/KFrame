
:  build poco

call build64_vs150.cmd

copy /y .\lib64\*.lib ..\..\_lib\win64\3rd\*.lib