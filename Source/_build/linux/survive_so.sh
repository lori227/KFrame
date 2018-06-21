#!/bin/sh

deploypath=~/Survive/Source/_bin/so
binpath=/usr/local/lib

mkdir -p $deplypath
cp -f $binpath/*.so* $deploypath/

basepath=$(cd `dirname $0`; pwd)
cd $deploypath
git add -f *
git commit -m "deploy file update" *
git push

cd $basepath


