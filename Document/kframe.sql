/*
Navicat MySQL Data Transfer

Source Server         : 192.168.1.9
Source Server Version : 50640
Source Host           : 192.168.1.9:3306
Source Database       : kframe

Target Server Type    : MYSQL
Target Server Version : 50640
File Encoding         : 65001

Date: 2018-08-14 18:19:49
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
  `startup` int(10) unsigned NOT NULL COMMENT '是否已经关闭',
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
INSERT INTO `deploy` VALUES ('100.0.11.1', 'data', 'master', '1', '0', '17204', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.111.1', 'log', 'master', '1', '0', '17211', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.112.1', 'log', 'proxy', '1', '0', '17219', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.113.1', 'log', 'shard', '1', '0', '17227', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.12.1', 'data', 'proxy', '1', '0', '17235', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.13.1', 'data', 'shard', '1', '0', '17242', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.21.1', 'public', 'master', '1', '0', '17261', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.22.1', 'public', 'proxy', '1', '0', '17272', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.23.1', 'public', 'shard', '1', '0', '17290', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.31.1', 'route', 'master', '1', '0', '17300', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.32.1', 'route', 'proxy', '1', '0', '17307', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.33.1', 'route', 'shard', '1', '0', '17326', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.4.1', 'auth', 'auth', '1', '0', '17940', '1534240738', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.41.1', 'match', 'master', '1', '0', '17357', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.42.1', 'match', 'proxy', '1', '0', '17366', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.43.1', 'match', 'shard', '1', '0', '17379', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.5.1', 'platform', 'platform', '1', '0', '17785', '1534240328', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.5.2', 'platform', 'platform', '1', '0', '17796', '1534240328', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.51.1', 'battle', 'master', '1', '0', '17414', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.52.1', 'battle', 'proxy', '1', '0', '17427', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.53.1', 'battle', 'shard', '1', '0', '17449', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.61.1', 'mail', 'master', '1', '0', '17461', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.62.1', 'mail', 'proxy', '1', '0', '17478', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.63.1', 'mail', 'shard', '1', '0', '17491', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.71.1', 'relation', 'master', '1', '0', '17508', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.72.1', 'relation', 'proxy', '1', '0', '17524', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.73.1', 'relation', 'shard', '1', '0', '17533', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.81.1', 'group', 'master', '1', '0', '17545', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.82.1', 'group', 'proxy', '1', '0', '17558', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.83.1', 'group', 'shard', '1', '0', '17571', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.91.1', 'rank', 'master', '1', '0', '17584', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.92.1', 'rank', 'proxy', '1', '0', '17601', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.0.93.1', 'rank', 'shard', '1', '0', '17614', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.1.241.1', 'zone', 'master', '1', '0', '17624', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.1.242.1', 'zone', 'world', '1', '0', '17643', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.1.243.1', 'zone', 'gate', '1', '0', '17650', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.1.244.1', 'zone', 'login', '1', '0', '17664', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.1.245.1', 'zone', 'proxy', '1', '0', '17682', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.1.246.1', 'zone', 'game', '1', '0', '17700', '1534240169', '100.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('100.3.241.1', 'zone', 'master', '1', '0', '0', '1534241867', '100.0.3.227', '192.168.10.230');
INSERT INTO `deploy` VALUES ('100.3.242.1', 'zone', 'world', '1', '0', '0', '1534241867', '100.0.3.227', '192.168.10.230');
INSERT INTO `deploy` VALUES ('100.3.243.1', 'zone', 'gate', '1', '0', '0', '1534241867', '100.0.3.227', '192.168.10.230');
INSERT INTO `deploy` VALUES ('100.3.244.1', 'zone', 'login', '1', '0', '0', '1534241867', '100.0.3.227', '192.168.10.230');
INSERT INTO `deploy` VALUES ('100.3.245.1', 'zone', 'proxy', '1', '0', '0', '1534241867', '100.0.3.227', '192.168.10.230');
INSERT INTO `deploy` VALUES ('100.3.245.6', 'zone', 'game', '1', '0', '0', '1534241867', '100.0.3.227', '192.168.10.230');

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
  `logtype` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '日志类型 0 不打日志 1 本地日志  2远程日志',
  `service` varchar(50) NOT NULL DEFAULT '' COMMENT '服务类型  cluster 集群服务  zone 游戏分区',
  `ftpid` int(11) NOT NULL DEFAULT '1' COMMENT 'ftpid',
  `deploypath` varchar(100) NOT NULL DEFAULT '/data/' COMMENT '部署路径',
  PRIMARY KEY (`appname`,`apptype`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of launch
-- ----------------------------
INSERT INTO `launch` VALUES ('auth', 'auth', 'auth', 'authserver', './setting/auth.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('battle', 'master', 'battle', 'battleserver', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('battle', 'proxy', 'battle', 'battleserver', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('battle', 'shard', 'battle', 'battleserver', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('data', 'master', 'data', 'dataserver', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('data', 'proxy', 'data', 'dataserver', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('data', 'shard', 'data', 'dataserver', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('group', 'master', 'group', 'groupserver', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('group', 'proxy', 'group', 'groupserver', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('group', 'shard', 'group', 'groupserver', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('guild', 'master', 'guild', 'guildserver', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('guild', 'proxy', 'guild', 'guildserver', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('guild', 'shard', 'guild', 'guildserver', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('log', 'master', 'log', 'logserver', './setting/master.startup', '1', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('log', 'proxy', 'log', 'logserver', './setting/proxy.startup', '1', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('log', 'shard', 'log', 'logserver', './setting/shard.startup', '1', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('mail', 'master', 'mail', 'mailserver', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('mail', 'proxy', 'mail', 'mailserver', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('mail', 'shard', 'mail', 'mailserver', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('match', 'master', 'match', 'matchserver', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('match', 'proxy', 'match', 'matchserver', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('match', 'shard', 'match', 'matchserver', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('platform', 'platform', 'platform', 'platformserver', './setting/platform.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('public', 'master', 'public', 'publicserver', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('public', 'proxy', 'public', 'publicserver', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('public', 'shard', 'public', 'publicserver', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('rank', 'master', 'rank', 'rankserver', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('rank', 'proxy', 'rank', 'rankserver', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('rank', 'shard', 'rank', 'rankserver', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('relation', 'master', 'relation', 'relationserver', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('relation', 'proxy', 'relation', 'relationserver', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('relation', 'shard', 'relation', 'relationserver', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('route', 'master', 'route', 'routeserver', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('route', 'proxy', 'route', 'routeserver', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('route', 'shard', 'route', 'routeserver', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'game', 'zone', 'zoneserver', './setting/game.startup', '2', 'zone', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'gate', 'zone', 'zoneserver', './setting/gate.startup', '2', 'zone', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'login', 'zone', 'zoneserver', './setting/login.startup', '2', 'zone', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'master', 'zone', 'zoneserver', './setting/master.startup', '2', 'zone', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'proxy', 'zone', 'zoneserver', './setting/proxy.startup', '2', 'zone', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'world', 'zone', 'zoneserver', './setting/world.startup', '2', 'zone', '1', '../');

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
INSERT INTO `machine` VALUES ('内网测试机', '192.168.1.9', '192.168.1.9', '4', '8', '16', '320', '100.0.3.1', '1', '12001');
INSERT INTO `machine` VALUES ('小兵测试机', '192.168.10.230', '192.168.10.230', '8', '16', '16', '100', '100.0.3.227', '1', '12001');
