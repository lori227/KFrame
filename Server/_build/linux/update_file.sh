if [ ! -n "$2" ]; then
    	echo "please input file name!"
    	exit 0
fi
echo $2

# 更新资源
cd ../../../
\cp -f ./Resource/config/* ./Bin/config/
cd Bin/tool/gcm/builder/
chmod 777 gcm_build
./gcm_build -t 2 -b $1 -f $2

cd ../../../../../../../
