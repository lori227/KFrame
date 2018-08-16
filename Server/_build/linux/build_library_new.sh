#!/bin/bash

function pause() {
  if [ "$#" -eq 0 ]
  then
    PROMPT="Press any key . . ."
  else
    PROMPT="$1"
  fi

  read -n1 -r -p "$PROMPT"
}

#-----------------------------------------------------
#color function
## blue to echo 
function blue(){
    echo -e "\033[35m[ $1 ]\033[0m"
}

## green to echo 
function green(){
    echo -e "\033[32m[ $1 ]\033[0m"
}

## Error to warning with blink
function bred(){
    echo -e "\033[31m\033[01m\033[05m[ $1 ]\033[0m"
}

## Error to warning with blink
function byellow(){
    echo -e "\033[33m\033[01m\033[05m[ $1 ]\033[0m"
}

## Error
function red(){
    echo -e "\033[31m\033[01m[ $1 ]\033[0m"
}

## warning
function yellow(){
    echo -e "\033[33m\033[01m[ $1 ]\033[0m"
}

#-----------------------------------------------------

cd ../../KFLibrary

#-----------------------------------------------------
blue "start building google protobuf"

cd google/protobuf
bash ./autogen.sh
bash ./configure --enable-shared=yes --prefix=/usr/local/
make clean
make -j 4
cp -R -f src/.libs/*.so* ../../../_lib/linux/3rd/
cd ../../

blue "end building google protobuf"

#pause
#-----------------------------------------------------
blue "start building json"

cd json

if [ -d "build" ]; then
	rm -rf build
fi

mkdir build && cd build
cmake -G "Unix Makefiles" -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=OFF -DCMAKE_BUILD_TYPE=release ../
make -j 4
cp -R -f src/lib_json/*.so* ../../../_lib/linux/3rd/
cd ../../

blue "end building json"

#pause
#-----------------------------------------------------
blue "start building libiconv"

cd libiconv/linux/source
chmod 777 configure
./configure --enable-shared=yes --prefix=/usr/local/
make -j 4
cp -R -f lib/*.so* ../../../../_lib/linux/3rd/
cp -R -f lib/.libs/*.so* ../../../../_lib/linux/3rd/
cd ../../../

blue "end building libiconv"

#pause
#-----------------------------------------------------
blue "start building libuv"

cd libuv
bash ./autogen.sh
bash ./configure --prefix=/usr/local/
make -j 4
cp -R -f .libs/*.so* ../../_lib/linux/3rd/
cd ../

blue "end building libuv"

#pause
#-----------------------------------------------------
blue "start building luaplus"

cd luaplus/projects
make -j 4
cp -R -f bin/Debug/*.a ../../../_lib/linux/3rd/
cp -R -f bin/Release/*.a ../../../_lib/linux/3rd/
cd ../../

blue "end building luaplus"

#pause
#-----------------------------------------------------
blue "start building redis"

cd redis/linux/deps/hiredis
make -j 4
cp -R -f *.so ../../../../../_lib/linux/3rd/
cp -R -f *.a ../../../../../_lib/linux/3rd/
cd ../../../../

blue "end building redis"

#pause
#-----------------------------------------------------
blue "start building zlib"

cd zlib
bash ./configure --prefix=/usr/local
make -j 4
cp -R -f *.so* ../../_lib/linux/3rd/
cd ../

blue "end building zlib"

#pause
#-----------------------------------------------------
blue "start building libgo"

cd libgo
mkdir -p build
cd build
cmake ..
cp -R -f *.so* ../../../_lib/linux/3rd/
cp -R -f *.a* ../../../_lib/linux/3rd/
cd ../../

blue "end building libgo"

#pause
#-----------------------------------------------------

blue "start building poco"

cd poco
chmod -R 755 *
bash ./configure --prefix=/usr/local --config=Linux --cflags=-fPIC --static --no-tests --no-samples --omit=PageCompiler --include-path=../../openssl/include,../../../mysql/include --library-path=../../_lib/linux/3rd
make -j 4
cp -R -f lib/Linux/x86_64/*.a ../../_lib/linux/3rd/
cd ../../

blue "start building poco"

#pause
#-----------------------------------------------------
green "all libraries build finished"

