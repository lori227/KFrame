#!/bin/sh

respath=../../../Bin
deploypath=/data/deploy

mkdir -p $deploypath
mkdir -p $deploypath/lib
mkdir -p $deploypath/bin
mkdir -p $deploypath/setting
mkdir -p $deploypath/startup

#shell
cp -f ./run.deploy.agent.sh $deploypath/
cp -f ./run.deploy.server.sh $deploypath/
cp -f ./kill.deploy.sh $deploypath/

#setting
cp -f  $respath/setting/ip.address $deploypath/setting/
cp -f  $respath/setting/bus.relation $deploypath/setting/
cp -f  $respath/setting/server.network $deploypath/setting/
cp -f  $respath/setting/mysql.address $deploypath/setting/
cp -f  $respath/setting/ftp.address $deploypath/setting/

#startup
cp -f  $respath/startup/deploy.server.startup $deploypath/startup/
cp -f  $respath/startup/deploy.agent.startup $deploypath/startup/

#lib
cp -f $respath/lib/* $deploypath/lib/

#bin
cp -f $respath/bin/KFStartup $deploypath/bin/deployserver
cp -f $respath/bin/KFStartupd $deploypath/bin/deployserverd

cp -f $respath/bin/KFBus.so $deploypath/bin/
cp -f $respath/bin/KFBusd.so $deploypath/bin/
cp -f $respath/bin/KFIpAddress.so $deploypath/bin/
cp -f $respath/bin/KFIpAddressd.so $deploypath/bin/
cp -f $respath/bin/KFTimer.so $deploypath/bin/
cp -f $respath/bin/KFTimerd.so $deploypath/bin/
cp -f $respath/bin/KFHttpClient.so $deploypath/bin/
cp -f $respath/bin/KFHttpClientd.so $deploypath/bin/
cp -f $respath/bin/KFHttpServer.so $deploypath/bin/
cp -f $respath/bin/KFHttpServerd.so $deploypath/bin/
cp -f $respath/bin/KFTcpServer.so $deploypath/bin/
cp -f $respath/bin/KFTcpServerd.so $deploypath/bin/
cp -f $respath/bin/KFTcpClient.so $deploypath/bin/
cp -f $respath/bin/KFTcpClientd.so $deploypath/bin/
cp -f $respath/bin/KFMessage.so $deploypath/bin/
cp -f $respath/bin/KFMessaged.so $deploypath/bin/
cp -f $respath/bin/KFConfig.so $deploypath/bin/
cp -f $respath/bin/KFConfigd.so $deploypath/bin/
cp -f $respath/bin/KFDeployAgent.so $deploypath/bin/
cp -f $respath/bin/KFDeployAgentd.so $deploypath/bin/
cp -f $respath/bin/KFDeployServer.so $deploypath/bin/
cp -f $respath/bin/KFDeployServerd.so $deploypath/bin/
cp -f $respath/bin/KFMySQL.so $deploypath/bin/
cp -f $respath/bin/KFMySQLd.so $deploypath/bin/
cp -f $respath/bin/KFSchedule.so $deploypath/bin/
cp -f $respath/bin/KFScheduled.so $deploypath/bin/
cp -f $respath/bin/KFFtp.so $deploypath/bin/
cp -f $respath/bin/KFFtpd.so $deploypath/bin/