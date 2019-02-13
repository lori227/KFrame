mkdir -p ../../_bin/lib
mkdir -p ../../_bin/bin/debug

cp -a -f ../../../../1.Frame/9.Template/1.Server/_bin/bin/debug ../../_bin/bin/

cd ../../../Resource/protocol
sh linux.sh
cd -

cmake ../../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
make -j 6
cp -a -f ../../_lib/linux/3rd/*.so* ../../_bin/lib
