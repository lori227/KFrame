echo "generate version..."

md ..\lib
xcopy /y /e ..\..\Resource\config ..\config\

cd builder
gcm_build.exe -m debug -b develop -c 1
cd ..