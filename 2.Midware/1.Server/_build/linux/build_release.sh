mkdir -p ../../_bin/lib
mkdir -p ../../_bin/bin/release

cp -a -f ../../../../1.Frame/9.Template/1.Server/_bin/bin/release ../../_bin/bin/

cd ../../../3.Resource/proto
sh linux.sh
cd -

cmake ../../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release 
make -j 6
cp -a -f ../../_lib/linux/3rd/*.so* ../../_bin/lib
