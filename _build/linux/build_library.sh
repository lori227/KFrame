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
yum install -y gcc-c++
yum install -y dos2unix
yum install -y autoconf automake libtool
#-----------------------------------------------------

cd ../../KFLibrary
libpath=_lib/linux/3rd/
#-----------------------------------------------------
blue "start building zlib"

cd zlib
bash ./configure --prefix=/usr/local
make -j 4
cp -R -f libz.a ../../$libpath
cp -R -f libz.so* ../../$libpath
cd ../

blue "end building zlib"
#-----------------------------------------------------
blue "start building lz4"

cd lz4
make clean
make -j 4
cp -R -f ./lib/liblz4.a ../../$libpath
cp -R -f ./lib/liblz4.so* ../../$libpath
cd ../

blue "end building lz4"
#pause
#-----------------------------------------------------
blue "start building libunwind"

cd libunwind
bash ./configure --prefix=/usr/local
make clean
make -j 4
make install
cd ../

blue "end building libunwind"

#-----------------------------------------------------
blue "start building tcmalloc"

cd tcmalloc
bash ./autogen.sh
bash ./configure --prefix=/usr/local
make clean
make -j 4
make install
cp -R -f /usr/local/lib/libtcmalloc_minimal.* ../../$libpath
cd ../

blue "end building tcmalloc"
#-----------------------------------------------------
blue "start building google protobuf"

cd google/protobuf
bash ./autogen.sh
bash ./configure --enable-shared=yes --prefix=/usr/local/
make clean
make -j 4
make install
cp -R -f src/.libs/libprotobuf.so* ../../../$libpath
cp -R -f src/protoc ../../../_resource/proto/4.protoc
cd ../../

blue "end building google protobuf"

#pause
#-----------------------------------------------------
blue "start building libiconv"

cd libiconv/linux/source
chmod 777 configure
./configure --enable-shared=yes --prefix=/usr/local/
make -j 4
cp -R -f lib/*.so* ../../../../$libpath
cp -R -f lib/.libs/*.so* ../../../../$libpath
cd ../../../

blue "end building libiconv"

#pause
#-----------------------------------------------------
blue "start building libuv"

cd libuv
bash ./autogen.sh
bash ./configure --prefix=/usr/local/
make -j 4
cp -R -f .libs/*.so* ../../$libpath
cd ../

blue "end building libuv"

#pause
#-----------------------------------------------------
blue "start building luaplus"

cd luaplus/projects
make
cp -R -f bin/Debug/*.a ../../../$libpath
cp -R -f bin/Release/*.a ../../../$libpath
cd ../../

blue "end building luaplus"

#pause
#-----------------------------------------------------
blue "start building redis"

cd redis/linux/deps/hiredis
make clean
make -j 4
make install
cp -R -f /usr/local/lib/libhiredis.* ../../../../../$libpath
cd ../../../../

blue "end building redis"

#pause

#-----------------------------------------------------
blue "start building openssl"

cd openssl
rm -f makefile
bash config
make clean
make -j 4
cp -R -f ./libssl.a ../../$libpath
cp -R -f ./libssl.so* ../../$libpath
cp -R -f ./libcrypto.a ../../$libpath
cp -R -f ./libcrypto.so* ../../$libpath
cd ../

blue "end building openssl"

#pause

#-----------------------------------------------------
blue "start building poco"

cd poco
chmod -R 755 *
bash ./configure --prefix=/usr/local --config=Linux --cflags=-fPIC --static --no-tests --no-samples --omit=PageCompiler,Data/ODBC --include-path=../../openssl/include,../../../mysql/include --library-path=../../_lib/linux/3rd/,./lib/,../lib/
make -j 4
cp -R -f lib/Linux/x86_64/*.a ../../$libpath
cd ../

blue "end building poco"

#pause
#-----------------------------------------------------
green "all libraries build finished"

