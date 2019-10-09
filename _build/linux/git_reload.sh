#!/bin/bash

configpath=/home/server/project/chess/trunk/Resource/config
serverpath=/data/chess/config

cd $configpath
git pull
cp -a -f $configpath/*.xml $serverpath/
cp -a -rf $configpath/server/* $serverpath/
cd -