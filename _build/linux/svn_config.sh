#!/bin/bash

configpath=/home/server/project/chess/trunk/Resource/config
serverpath=/data/chess/config/config

svn up $configpath/DesignerConfig1
cp -a -f $configpath/*.xml $serverpath/
cp -a -rf $configpath/DesignerConfig1/* $serverpath/

