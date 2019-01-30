

for /f "delims=" %%a in ('dir /b/a-d/oN *.proto') do __protoc.exe -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:../ %%a