#! /bin/sh 

chmod 777 4.protoc
export LD_LIBRARY_PATH=/usr/local/lib

for file in `ls *.proto`  
do  
  echo $file  
./4.protoc -I=./ --cpp_out=dllexport_decl=LIBPROTOC_EXPORT:./ $file
done  

\cp *.pb.* ../../1.Server/KFContrib/KFProtocol/
rm -f *.pb.*