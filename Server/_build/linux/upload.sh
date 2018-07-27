#!/bin/sh

if [ ! -n "$4" ]; then
    echo "please input params"
    echo "1 appname like : auth"
    echo "2 vesion pathname : test"
    echo "3 ftpid : 1"
    echo "4 upload url : http://127.0.0.1:9998/upload"
    exit 0
fi

if [ "$1" = "-" ]; then
	for file in `ls *_upload.sh`
	do
		echo $file 
		sh $file $2 $3 $4
	done
else
	sh "$1"_upload.sh $2 $3 $4
fi

