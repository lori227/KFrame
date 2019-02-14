mkdir -p ../../_bin/lib/
mkdir -p ../../_bin/bin/debug
mkdir -p ../../../9.Template/1.Server/_lib/linux/3rd/

cd ../../../3.Resource/proto
sh 2.linux.sh
cd -

mkdir -p .build
cd .build
cmake ../../../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
make -j 6
cd ../

mkdir -p ../../../9.Template/1.Server/_bin/bin/debug

cp -a -f ../../_lib/linux/3rd/*.so* ../../_bin/lib/
cp -a -f ../../_lib/linux/*.a ../../../9.Template/1.Server/_lib/linux/
cp -a -f ../../_lib/linux/3rd/* ../../../9.Template/1.Server/_lib/linux/3rd/
cp -a -f ../../_bin/bin/debug ../../../9.Template/1.Server/_bin/bin/
