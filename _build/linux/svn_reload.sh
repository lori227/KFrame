#!/bin/bash

configpath=/home/server/project/chess/trunk/Resource/config
serverpath=/data/chess/develop/config

svn up $configpath
cp -a -f $configpath/*.xml $serverpath/
cp -a -rf $configpath/server/* $serverpath/

