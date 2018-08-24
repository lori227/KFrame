echo "generate version..."

xcopy /y /e ..\..\..\Resource\config ..\..\config\

cd builder
gcm_build.exe -m debug