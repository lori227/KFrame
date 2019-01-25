#!/bin/sh


cd source
chmod 777 configure
#./autogen.sh
./configure --prefix=/usr/local/


make clean

make
make install
cd ../

#mkdir -p ../../_lib/linux
#\cp /usr/local/libuv/lib/libiconv.* ../../_lib/linux/

