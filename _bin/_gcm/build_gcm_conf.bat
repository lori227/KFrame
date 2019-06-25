echo "generate version..."

if not exist ..\config ( mkdir ..\config )
xcopy /y ..\..\..\Resource\config\*.xml ..\config\
xcopy /y ..\..\..\Resource\config\server\*.xml ..\config\

if not exist ..\script ( mkdir ..\script )
xcopy /S /y ..\..\..\Resource\script\* ..\script\

cd builder
gcm_build.exe -m debug -b develop -c 1.0 -z 1
cd ..