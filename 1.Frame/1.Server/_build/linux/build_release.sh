mkdir -p ../../_bin/lib
mkdir -p ../../_bin/bin/release

cd ../../../Resource/protocol
sh linux.sh
cd -

cmake ../../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release 
make -j 6
cp -a -f ../../_lib/linux/3rd/*.so* ../../_bin/lib/