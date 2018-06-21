#!/bin/sh

chmod 777 -R *.sh

cd apr
chmod 777 configure
./configure --prefix=/usr/local


make clean
make
make install
cd ..

cd apr-util
chmod 777 configure
./configure --prefix=/usr/local --with-apr=/usr/local

make clean
make
make install

cd ..

cd log4cxx
chmod 777 -R *.sh
./autogen.sh

chmod 777 configure
./configure --prefix=/usr/local --with-apr=/usr/local --with-apr-util=/usr/local

make clean

make
make install

#mkdir -p ../../../_lib/linux
#mkdir -p ../../../_bin/linux
#\cp /usr/local/log4cxx/lib/liblog4cxx* ../../../_lib/linux
#\cp /usr/local/log4cxx/lib/*.so* ../../../_bin/linux
cd ..
