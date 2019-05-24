if [ ! -n "$2" ]; then
    	echo "please input plugin name!"
    	exit 0
fi
echo $2

# 更新资源
cd ../../_bin/_gcm/builder/
chmod 777 gcm_build
./gcm_build -t 4 -b $1 -r $2 -f $3

cd ../../../../../