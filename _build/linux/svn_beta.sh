#!/bin/bash

configpath=/home/server/project/chess/trunk/Resource/config
serverpath=/data/chess/beta/config

svn up $configpath
cp -a -f $configpath/*.xml $serverpath/
cp -a -rf $configpath/Beta/* $serverpath/

