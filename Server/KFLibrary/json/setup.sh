
rm -f CMakeCache.txt
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=release -DBUILD_STATIC_LIBS=OFF -DBUILD_SHARED_LIBS=ON -DINCLUDE_INSTALL_DIR=/usr/local/include  -DCMAKE_INSTALL_LIBDIR=/usr/local/lib


make clean
make 
make install

#\cp /usr/local/lib64/libjsoncpp.a /usr/local/lib/libjsoncpp.a
#\cp ./src/lib_json/libjsoncpp.a ../../_lib/linux
