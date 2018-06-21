#! /bin/sh -  
for file in `ls *.proto`  
do  
  echo $file  
  __protoc -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:../ $file
done  
