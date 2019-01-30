

chmod 777 *.sh
./configure --prefix=/usr/local

make clean
make
make install

#\cp /usr/local/zlib/lib/libz* ../../_lib/linux
