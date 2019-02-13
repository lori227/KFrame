#! /bin/sh 

chmod 777 __protoc
export LD_LIBRARY_PATH=/usr/local/lib

for file in `ls *.proto`  
do  
  echo $file  
  bash 4.protoc -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ $file
done  

\cp *.pb.* ../KFContrib/KFProtocol/
rm -f *.pb.*