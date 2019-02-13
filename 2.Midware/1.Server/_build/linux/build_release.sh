mkdir -p ../../_bin/lib
mkdir -p ../../_lib/linux/
mkdir -p ../../_bin/bin/release

cp -a -f ../../../../1.Frame/9.Template/1.Server/_bin/bin/release ../../_bin/bin/
cp -a -f ../../../../1.Frame/9.Template/1.Server/_lib/linux/ ../../_lib/

cd ../../../3.Resource/proto
sh 2.linux.sh
cd -

mkdir -p build
cd build
cmake ../../../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release 
make -j 6
cd ../

cp -a -f ../../_lib/linux/3rd/*.so* ../../_bin/lib
