mkdir -p ../../_bin/lib/
mkdir -p ../../_lib/linux/
mkdir -p ../../_bin/bin/debug

cp -a -f ../../../../1.Frame/9.Template/1.Server/_bin/bin/debug ../../_bin/bin/
cp -a -f ../../../../1.Frame/9.Template/1.Server/_lib/linux/ ../../_lib/

cd ../../../3.Resource/proto
sh 2.linux.sh
cd -

cmake ../../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
make -j 6
cp -a -f ../../_lib/linux/3rd/*.so* ../../_bin/lib
