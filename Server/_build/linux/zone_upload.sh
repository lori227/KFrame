#!/bin/sh

function copyfile()
{
	file1=$path1/$2
	file2=$path2/$2
	if [ -n "$3" ]; then
		file2=$path2/$3
	fi

	needcopyfile=0
	if [ ! -f "$file1" ];then
		needcopyfile=0
	elif [ ! -f "$file2" ];then
		needcopyfile=1	
	elif [ $1 == "1" ];then
		filetime1=`stat -c %Y $file1`
        	filetime2=`stat -c %Y $file2`

        	if [ $filetime1 -gt $filetime2 ];then
                	needcopyfile=1
        	fi
	fi
	
	if [ $needcopyfile == 1 ];then
		echo "copy $file1 $file2"
		cp -f $file1 $file2
	fi
}

if [ ! -n "$3" ]; then
    echo "please input path name"
    exit 0
fi

versionpath=/kframe/$1
deploypath=/data/upload/deploy/zone/linux

mkdir -p $deploypath
mkdir -p $deploypath/setting
mkdir -p $deploypath/plugin
mkdir -p $deploypath/config
mkdir -p $deploypath/script

#setting
path1=$versionpath/setting
path2=$deploypath/setting
copyfile 1 ip.address
copyfile 1 bus.relation
copyfile 1 server.network
copyfile 1 common.startup
copyfile 1 cluster.setting
copyfile 1 zone.setting
copyfile 1 kernel.setting

path1=$versionpath/setting/zone
copyfile 1 gate.startup
copyfile 1 login.startup
copyfile 1 master.startup
copyfile 1 game.startup
copyfile 1 world.startup

#config
cp -f $versionpath/config/* $deploypath/config/

#script
cp -f $versionpath/script/* $deploypath/script/

#plugin
path1=$versionpath
path2=$deploypath
copyfile 1 KFStartup zoneserver
copyfile 1 KFStartupd zoneserverd

path1=$versionpath/plugin
path2=$deploypath/plugin
copyfile 1 KFAchieve.so
copyfile 1 KFAchieved.so
copyfile 1 KFActivity.so
copyfile 1 KFActivityd.so
copyfile 1 KFBattleClient.so
copyfile 1 KFBattleClientd.so
copyfile 1 KFBus.so
copyfile 1 KFBusd.so
copyfile 1 KFChat.so
copyfile 1 KFChatd.so
copyfile 1 KFClothes.so
copyfile 1 KFClothesd.so
copyfile 1 KFClusterClient.so
copyfile 1 KFClusterClientd.so
copyfile 1 KFCompound.so
copyfile 1 KFCompoundd.so
copyfile 1 KFConfig.so
copyfile 1 KFConfigd.so
copyfile 1 KFDataClient.so
copyfile 1 KFDataClientd.so
copyfile 1 KFDebug.so
copyfile 1 KFDebugd.so
copyfile 1 KFDeployClient.so
copyfile 1 KFDeployClientd.so
copyfile 1 KFDeployCommand.so
copyfile 1 KFDeployCommandd.so
copyfile 1 KFDisplay.so
copyfile 1 KFDisplayd.so
copyfile 1 KFEnter.so
copyfile 1 KFEnterd.so
copyfile 1 KFGame.so
copyfile 1 KFGamed.so
copyfile 1 KFGate.so
copyfile 1 KFGated.so
copyfile 1 KFGift.so
copyfile 1 KFGiftd.so
copyfile 1 KFGroupClient.so
copyfile 1 KFGroupClientd.so
copyfile 1 KFGuildClient.so
copyfile 1 KFGuildClientd.so
copyfile 1 KFHttpClient.so
copyfile 1 KFHttpClientd.so
copyfile 1 KFHttpServer.so
copyfile 1 KFHttpServerd.so
copyfile 1 KFIpAddress.so
copyfile 1 KFIpAddressd.so
copyfile 1 KFItem.so
copyfile 1 KFItemd.so
copyfile 1 KFKernel.so
copyfile 1 KFKerneld.so
copyfile 1 KFLogClient.so
copyfile 1 KFLogClientd.so
copyfile 1 KFLogin.so
copyfile 1 KFLogind.so
copyfile 1 KFLua.so
copyfile 1 KFLuad.so
copyfile 1 KFMailClient.so
copyfile 1 KFMailClientd.so
copyfile 1 KFMatchClient.so
copyfile 1 KFMatchClientd.so
copyfile 1 KFMessage.so
copyfile 1 KFMessaged.so
copyfile 1 KFOption.so
copyfile 1 KFOptiond.so
copyfile 1 KFPlayer.so
copyfile 1 KFPlayerd.so
copyfile 1 KFPublicClient.so
copyfile 1 KFPublicClientd.so
copyfile 1 KFRankClient.so
copyfile 1 KFRankClientd.so
copyfile 1 KFRelationClient.so
copyfile 1 KFRelationClientd.so
copyfile 1 KFReset.so
copyfile 1 KFResetd.so
copyfile 1 KFRouteClient.so
copyfile 1 KFRouteClientd.so
copyfile 1 KFSchedule.so
copyfile 1 KFScheduled.so
copyfile 1 KFSignIn.so
copyfile 1 KFSignInd.so
copyfile 1 KFStore.so
copyfile 1 KFStored.so
copyfile 1 KFTask.so
copyfile 1 KFTaskd.so
copyfile 1 KFTcpClient.so
copyfile 1 KFTcpClientd.so
copyfile 1 KFTcpServer.so
copyfile 1 KFTcpServerd.so
copyfile 1 KFTimer.so
copyfile 1 KFTimerd.so
copyfile 1 KFWorld.so
copyfile 1 KFWorldd.so
copyfile 1 KFZone.so
copyfile 1 KFZoned.so

#upload to ftp
curl -sd '{"ftpid":'$2',"apppath":"zone"}' $3
