mkdir -p ../../_bin/lib
mkdir -p ../../_bin/bin/linux/release

cd ../../_resource/proto
#sh 2.linux.sh
cd -

mkdir -p .build
cd .build
cmake ../../../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release 
make -j 6
cd ../

cp -a -f ../../_lib/linux/3rd/*.so* ../../_bin/lib/
