

cd ../../KFLibrary

cd google/protobuf
chmod -R 777 *.sh
./setup.sh
cd ../../

cd json
chmod -R 777 *.sh
./setup.sh
cd ..

cd libiconv/linux
chmod -R 777 *.sh
./setup.sh
cd ../../

cd libuv
chmod -R 777 *.sh
./setup.sh
cd ..


#cd log4cxx
#chmod -R 777 *.sh
#./setup.sh
#cd ..

cd luaplus/projects
make clean
make
./setup.sh
cd ../../

cd redis/linux
chmod -R 777 *.sh
./setup.sh
cd ../../

cd zlib
chmod -R 777 *.sh
./setup.sh
cd ..


cd poco
chmod -R 777 *.sh
./setup.sh
cd ..



ln -sf /usr/local/lib/libiconv.so.2 /usr/lib/libiconv.so.2
