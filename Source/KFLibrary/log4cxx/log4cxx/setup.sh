./autogen.sh

./configure --prefix=/usr/local/log4cxx --with-apr=/usr/local/apr --with-apr-util=/usr/local/aprutil

make clean

make
make install

mkdir -p ../../../_lib/linux
\cp /usr/local/lib/liblog4cxx* ../../../_lib/linux
