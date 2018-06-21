#!/bin/sh

./autogen.sh
./configure --prefix=/usr/local/


make clean

make
make install

#mkdir -p ../../_lib/linux
#\cp /usr/local/libuv/lib/libuv.* ../../_lib/linux/

