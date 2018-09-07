mkdir -p /data
cmake ../../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
make -j 6
cp -a -f ../../_lib/linux/3rd/*.so* ../../../Bin/lib
