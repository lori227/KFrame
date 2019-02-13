mkdir -p ../../_bin/lib
mkdir -p ../../_bin/bin/release

cd ../../../Resource/protocol
sh linux.sh
cd -

cmake ../../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release 
make -j 6
mkdir -p ../../../9.Template/1.Server/_bin/bin/release

cp -a -f ../../_lib/linux/3rd/*.so* ../../_bin/lib/
cp -a -f ../../_lib/linux/*.a ../../../9.Template/1.Server/_lib/linux/
cp -a -f ../../_lib/linux/3rd/*.so* ../../../9.Template/1.Server/_lib/linux/3rd/
cp -a -f ../../_bin/bin/release ../../../9.Template/1.Server/_bin/bin/
