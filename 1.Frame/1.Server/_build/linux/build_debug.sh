mkdir -p ../../../Bin/lib/debug
mkdir -p ../../../Bin/bin/debug

cd ../../../Resource/protocol
sh linux.sh
cd -

cmake ../../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
make -j 6
cp -a -f ../../_lib/linux/3rd/*.so* ../../../Bin/lib/debug
