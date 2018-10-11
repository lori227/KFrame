mkdir -p ../../../Bin/lib
mkdir -p ../../../Bin/bin

cmake ../../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release 
make -j 6
cp -a -f ../../_lib/linux/3rd/*.so* ../../../Bin/lib
