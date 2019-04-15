/*
Navicat MySQL Data Transfer

Source Server         : 192.168.1.9
Source Server Version : 50640
Source Host           : 192.168.1.9:3306
Source Database       : kframe

Target Server Type    : MYSQL
Target Server Version : 50640
File Encoding         : 65001

Date: 2018-08-30 11:21:12
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `deploy`
-- ----------------------------
DROP TABLE IF EXISTS `deploy`;
CREATE TABLE `deploy` (
  `appid` varchar(50) NOT NULL COMMENT '服务器id',
  `appname` varchar(50) NOT NULL COMMENT '服务器名字',
  `apptype` varchar(50) NOT NULL COMMENT '服务器类型',
  `logtype` varchar(50) NOT NULL DEFAULT '1.0' COMMENT 'x.y x 0 不打日志 1 本地日志  2远程日志 y 日志级别',
  `startup` int(10) unsigned NOT NULL COMMENT '是否已经关闭',
  `debug` int(10) NOT NULL DEFAULT '0' COMMENT '1=debug 0 release',
  `shutdown` int(10) NOT NULL DEFAULT '0',
  `process` int(10) unsigned NOT NULL COMMENT '进程id',
  `time` bigint(20) unsigned NOT NULL COMMENT '状态时间( 启动或者关闭 )',
  `agentid` varchar(50) NOT NULL COMMENT '部署agentid',
  `localip` varchar(50) NOT NULL COMMENT '局域网ip',
  PRIMARY KEY (`appid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of deploy
-- ----------------------------
INSERT INTO `deploy` VALUES ('101.0.11.1', 'data', 'master', '2.0', '1', '0', '0', '64840', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.111.1', 'log', 'master', '1.0', '1', '0', '0', '64850', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.112.1', 'log', 'proxy', '1.0', '1', '0', '0', '64865', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.113.1', 'log', 'shard', '1.0', '1', '0', '0', '64883', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.12.1', 'data', 'proxy', '2.0', '1', '0', '0', '64915', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.121.1', 'dir', 'master', '2.0', '1', '0', '0', '64929', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.122.1', 'dir', 'proxy', '2.0', '1', '0', '0', '64939', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.123.1', 'dir', 'shard', '2.0', '1', '0', '0', '64950', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.13.1', 'data', 'shard', '2.0', '1', '0', '0', '64965', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.21.1', 'public', 'master', '2.0', '1', '0', '0', '64980', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.22.1', 'public', 'proxy', '2.0', '1', '0', '0', '64993', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.23.1', 'public', 'shard', '2.0', '1', '0', '0', '65006', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.31.1', 'route', 'master', '2.0', '1', '0', '0', '65021', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.32.1', 'route', 'proxy', '2.0', '1', '0', '0', '65035', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.33.1', 'route', 'shard', '2.0', '1', '0', '0', '65051', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.4.1', 'auth', 'auth', '2.0', '1', '0', '0', '65064', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.41.1', 'match', 'master', '2.0', '1', '0', '0', '65080', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.42.1', 'match', 'proxy', '2.0', '1', '0', '0', '65093', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.43.1', 'match', 'shard', '2.0', '1', '0', '0', '65107', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.51.1', 'battle', 'master', '2.0', '1', '0', '0', '65122', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.52.1', 'battle', 'proxy', '2.0', '1', '0', '0', '65135', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.53.1', 'battle', 'shard', '2.0', '1', '0', '0', '65151', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.61.1', 'mail', 'master', '2.0', '1', '0', '0', '65166', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.62.1', 'mail', 'proxy', '2.0', '1', '0', '0', '65182', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.63.1', 'mail', 'shard', '2.0', '1', '0', '0', '65196', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.71.1', 'relation', 'master', '2.0', '1', '0', '0', '65210', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.72.1', 'relation', 'proxy', '2.0', '1', '0', '0', '65224', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.73.1', 'relation', 'shard', '2.0', '1', '0', '0', '65237', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.81.1', 'group', 'master', '2.0', '1', '0', '0', '65252', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.82.1', 'group', 'proxy', '2.0', '1', '0', '0', '65266', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.83.1', 'group', 'shard', '2.0', '1', '0', '0', '65280', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.91.1', 'rank', 'master', '2.0', '1', '0', '0', '65293', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.92.1', 'rank', 'proxy', '2.0', '1', '0', '0', '65307', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.93.1', 'rank', 'shard', '2.0', '1', '0', '0', '65321', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.1.241.1', 'zone', 'master', '2.0', '1', '0', '0', '65335', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.1.242.1', 'zone', 'world', '2.0', '1', '0', '0', '65349', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.1.243.1', 'zone', 'gate', '2.0', '1', '0', '0', '65366', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.1.244.1', 'zone', 'login', '2.0', '1', '0', '0', '65379', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.1.245.1', 'zone', 'game', '2.0', '1', '0', '0', '65393', '1535548810', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.3.241.1', 'zone', 'master', '2.0', '1', '0', '0', '2012', '1535595180', '101.0.3.227', '192.168.10.230');
INSERT INTO `deploy` VALUES ('101.3.242.1', 'zone', 'world', '2.0', '1', '0', '0', '2022', '1535595180', '101.0.3.227', '192.168.10.230');
INSERT INTO `deploy` VALUES ('101.3.243.1', 'zone', 'gate', '2.0', '1', '0', '0', '2035', '1535595180', '101.0.3.227', '192.168.10.230');
INSERT INTO `deploy` VALUES ('101.3.244.1', 'zone', 'login', '2.0', '1', '0', '0', '2045', '1535595180', '101.0.3.227', '192.168.10.230');
INSERT INTO `deploy` VALUES ('101.3.245.1', 'zone', 'game', '2.0', '1', '0', '0', '2055', '1535595180', '101.0.3.227', '192.168.10.230');

-- ----------------------------
-- Table structure for `launch`
-- ----------------------------
DROP TABLE IF EXISTS `launch`;
CREATE TABLE `launch` (
  `appname` varchar(50) NOT NULL DEFAULT '' COMMENT '进程类名',
  `apptype` varchar(50) NOT NULL DEFAULT '' COMMENT '进程类型',
  `apppath` varchar(50) NOT NULL DEFAULT '' COMMENT '程序目录',
  `appfile` varchar(50) NOT NULL DEFAULT '' COMMENT '进程文件名',
  `appconfig` varchar(100) NOT NULL DEFAULT '' COMMENT '配置路径',
  `service` varchar(50) NOT NULL DEFAULT '' COMMENT '服务类型  cluster 集群服务  zone 游戏分区',
  `ftpid` int(11) NOT NULL DEFAULT '1' COMMENT 'ftpid',
  `deploypath` varchar(100) NOT NULL DEFAULT '/data/' COMMENT '部署路径',
  PRIMARY KEY (`appname`,`apptype`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of launch
-- ----------------------------
INSERT INTO `launch` VALUES ('auth', 'auth', 'auth', 'authserver', './startup/auth.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('battle', 'master', 'battle', 'battleserver', './startup/battle.master.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('battle', 'proxy', 'battle', 'battleserver', './startup/battle.proxy.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('battle', 'shard', 'battle', 'battleserver', './startup/battle.shard.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('data', 'master', 'data', 'dataserver', './startup/data.master.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('data', 'proxy', 'data', 'dataserver', './startup/data.proxy.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('data', 'shard', 'data', 'dataserver', './startup/data.shard.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('dir', 'master', 'dir', 'dirserver', './startup/dir.master.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('dir', 'proxy', 'dir', 'dirserver', './startup/dir.proxy.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('dir', 'shard', 'dir', 'dirserver', './startup/dir.shard.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('group', 'master', 'group', 'groupserver', './startup/group.master.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('group', 'proxy', 'group', 'groupserver', './startup/group.proxy.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('group', 'shard', 'group', 'groupserver', './startup/group.shard.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('guild', 'master', 'guild', 'guildserver', './startup/guild.master.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('guild', 'proxy', 'guild', 'guildserver', './startup/guild.proxy.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('guild', 'shard', 'guild', 'guildserver', './startup/guild.shard.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('log', 'master', 'log', 'logserver', './startup/log.master.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('log', 'proxy', 'log', 'logserver', './startup/log.proxy.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('log', 'shard', 'log', 'logserver', './startup/log.shard.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('mail', 'master', 'mail', 'mailserver', './startup/mail.master.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('mail', 'proxy', 'mail', 'mailserver', './startup/mail.proxy.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('mail', 'shard', 'mail', 'mailserver', './startup/mail.shard.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('match', 'master', 'match', 'matchserver', './startup/match.master.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('match', 'proxy', 'match', 'matchserver', './startup/match.proxy.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('match', 'shard', 'match', 'matchserver', './startup/match.shard.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('public', 'master', 'public', 'publicserver', './startup/public.master.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('public', 'proxy', 'public', 'publicserver', './startup/public.proxy.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('public', 'shard', 'public', 'publicserver', './startup/public.shard.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('rank', 'master', 'rank', 'rankserver', './startup/rank.master.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('rank', 'proxy', 'rank', 'rankserver', './startup/rank.proxy.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('rank', 'shard', 'rank', 'rankserver', './startup/rank.shard.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('relation', 'master', 'relation', 'relationserver', './startup/relation.master.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('relation', 'proxy', 'relation', 'relationserver', './startup/relation.proxy.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('relation', 'shard', 'relation', 'relationserver', './startup/relation.shard.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('route', 'master', 'route', 'routeserver', './startup/route.master.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('route', 'proxy', 'route', 'routeserver', './startup/route.proxy.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('route', 'shard', 'route', 'routeserver', './startup/route.shard.startup', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'game', 'zone', 'zoneserver', './startup/zone.game.startup', 'zone', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'gate', 'zone', 'zoneserver', './startup/zone.gate.startup', 'zone', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'login', 'zone', 'zoneserver', './startup/zone.login.startup', 'zone', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'master', 'zone', 'zoneserver', './startup/zone.master.startup', 'zone', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'world', 'zone', 'zoneserver', './startup/zone.world.startup', 'zone', '1', '../');

-- ----------------------------
-- Table structure for `machine`
-- ----------------------------
DROP TABLE IF EXISTS `machine`;
CREATE TABLE `machine` (
  `name` varchar(50) NOT NULL DEFAULT '' COMMENT '机器名',
  `localip` varchar(50) NOT NULL DEFAULT '' COMMENT '局域网ip',
  `interanetip` varchar(50) NOT NULL COMMENT '外网ip',
  `cpu` int(10) NOT NULL DEFAULT '8',
  `thread` int(10) NOT NULL DEFAULT '16',
  `memory` int(10) NOT NULL DEFAULT '16',
  `harddisk` int(10) NOT NULL DEFAULT '100',
  `agentid` varchar(50) NOT NULL DEFAULT '0' COMMENT '开启的agentid',
  `status` int(10) NOT NULL DEFAULT '0' COMMENT '状态 0 agent断开连接 1 agent保持连接',
  `port` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`localip`),
  KEY `ip` (`localip`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of machine
-- ----------------------------
INSERT INTO `machine` VALUES ('内网测试机', '192.168.1.9', '192.168.1.9', '4', '8', '16', '320', '101.0.3.1', '1', '12052');
INSERT INTO `machine` VALUES ('小兵测试机', '192.168.10.230', '192.168.10.230', '8', '16', '16', '100', '101.0.3.227', '1', '12001');

-- ----------------------------
-- Table structure for `version`
-- ----------------------------
DROP TABLE IF EXISTS `version`;
CREATE TABLE `version` (
  `version_time` datetime NOT NULL,
  `version_name` varchar(128) NOT NULL,
  `version_url` varchar(512) NOT NULL,
  `version_md5` varchar(32) NOT NULL,
  PRIMARY KEY (`version_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of version
-- ----------------------------
INSERT INTO `version` VALUES ('2018-08-29 12:13:16', 'sgame_svn_9216_201808291212.tar.gz', 'http://version.leiwu.com/upload/sgame_svn_9216_201808291212.tar.gz', 'b0c792e23567a983df2d2b7337a5eb70');
INSERT INTO `version` VALUES ('2018-08-29 16:20:27', 'sgame_svn_9267_201808291619.tar.gz', 'http://version.leiwu.com/upload/sgame_svn_9267_201808291619.tar.gz', 'f8b291c558236a29f790df7aaebbfc3d');
INSERT INTO `version` VALUES ('2018-08-29 16:47:09', 'sgame_svn_9273_201808291646.tar.gz', 'http://version.leiwu.com/upload/sgame_svn_9273_201808291646.tar.gz', '00fdbe8502e4f61504fcebe1a02d634d');
INSERT INTO `version` VALUES ('2018-08-29 19:13:05', 'sgame_svn_9296_201808291912.tar.gz', 'http://version.leiwu.com/upload/sgame_svn_9296_201808291912.tar.gz', '9786cb6718a7baaed1b96eeb25537cfe');
INSERT INTO `version` VALUES ('2018-08-29 20:43:04', 'sgame_svn_9332_201808292042.tar.gz', 'http://version.leiwu.com/upload/sgame_svn_9332_201808292042.tar.gz', '7107e3537bf95585ef9233a1587e520e');
INSERT INTO `version` VALUES ('2018-08-29 21:18:31', 'sgame_svn_9340_201808292117.tar.gz', 'http://version.leiwu.com/upload/sgame_svn_9340_201808292117.tar.gz', '56ca6ecbda43778686dd00bd27f7fb01');
