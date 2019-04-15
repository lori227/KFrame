
:  build poco

call build64_vs150.cmd

xcopy /y .\lib64\*mdd.lib ..\..\_lib\win64\debug\3rd\
xcopy /y .\lib64\*md.lib ..\..\_lib\win64\release\3rd\