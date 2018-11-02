/*
Navicat MySQL Data Transfer

Source Server         : #001-小兵研发192.168.10.230
Source Server Version : 50722
Source Host           : 192.168.10.230:3306
Source Database       : kfdeploy

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2018-10-29 18:32:21
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `deploy`
-- ----------------------------
DROP TABLE IF EXISTS `deploy`;
CREATE TABLE `deploy` (
  `appid` varchar(50) NOT NULL COMMENT '服务器id',
  `service` varchar(50) NOT NULL DEFAULT '1.2',
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
  `version` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`appid`,`service`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of deploy
-- ----------------------------
INSERT INTO `deploy` VALUES ('1.0.11.1', '1.1', 'data', 'master', '2.1', '1', '1', '0', '23488', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.11.1', '1.2', 'data', 'master', '2.1', '1', '1', '0', '23488', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.111.1', '1.1', 'log', 'master', '1.1', '1', '1', '0', '23498', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.111.1', '1.2', 'log', 'master', '1.1', '1', '1', '0', '23498', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.112.1', '1.1', 'log', 'proxy', '1.1', '1', '1', '0', '23510', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.112.1', '1.2', 'log', 'proxy', '1.1', '1', '1', '0', '23510', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.113.1', '1.1', 'log', 'shard', '1.1', '1', '1', '0', '23522', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.113.1', '1.2', 'log', 'shard', '1.1', '1', '1', '0', '23522', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.12.1', '1.1', 'data', 'proxy', '2.1', '1', '1', '0', '23533', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.12.1', '1.2', 'data', 'proxy', '2.1', '1', '1', '0', '23533', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.121.1', '1.1', 'dir', 'master', '2.1', '1', '1', '0', '23545', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.121.1', '1.2', 'dir', 'master', '2.1', '1', '1', '0', '23545', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.122.1', '1.1', 'dir', 'proxy', '2.1', '1', '1', '0', '23555', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.122.1', '1.2', 'dir', 'proxy', '2.1', '1', '1', '0', '23555', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.123.1', '1.1', 'dir', 'shard', '2.1', '1', '1', '0', '23566', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.123.1', '1.2', 'dir', 'shard', '2.1', '1', '1', '0', '23566', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.13.1', '1.1', 'data', 'shard', '2.1', '1', '1', '0', '23581', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.13.1', '1.2', 'data', 'shard', '2.1', '1', '1', '0', '23581', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.21.1', '1.1', 'public', 'master', '2.1', '1', '1', '0', '23592', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.21.1', '1.2', 'public', 'master', '2.1', '1', '1', '0', '23592', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.22.1', '1.1', 'public', 'proxy', '2.1', '1', '1', '0', '23604', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.22.1', '1.2', 'public', 'proxy', '2.1', '1', '1', '0', '23604', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.23.1', '1.1', 'public', 'shard', '2.1', '1', '1', '0', '23615', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.23.1', '1.2', 'public', 'shard', '2.1', '1', '1', '0', '23615', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.31.1', '1.1', 'route', 'master', '2.1', '1', '1', '0', '23626', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.31.1', '1.2', 'route', 'master', '2.1', '1', '1', '0', '23626', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.32.1', '1.1', 'route', 'proxy', '2.1', '1', '1', '0', '23638', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.32.1', '1.2', 'route', 'proxy', '2.1', '1', '1', '0', '23638', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.33.1', '1.1', 'route', 'shard', '2.1', '1', '1', '0', '23649', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.33.1', '1.2', 'route', 'shard', '2.1', '1', '1', '0', '23649', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.4.1', '1.1', 'auth', 'auth', '2.1', '1', '1', '0', '23661', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.4.1', '1.2', 'auth', 'auth', '2.1', '1', '1', '0', '23661', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.41.1', '1.1', 'match', 'master', '2.1', '1', '1', '0', '23674', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.41.1', '1.2', 'match', 'master', '2.1', '1', '1', '0', '23674', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.42.1', '1.1', 'match', 'proxy', '2.1', '1', '1', '0', '23685', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.42.1', '1.2', 'match', 'proxy', '2.1', '1', '1', '0', '23685', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.43.1', '1.1', 'match', 'shard', '2.1', '1', '1', '0', '23697', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.43.1', '1.2', 'match', 'shard', '2.1', '1', '1', '0', '23697', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.51.1', '1.1', 'battle', 'master', '2.1', '1', '1', '0', '23710', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.51.1', '1.2', 'battle', 'master', '2.1', '1', '1', '0', '23710', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.52.1', '1.1', 'battle', 'proxy', '2.1', '1', '1', '0', '23722', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.52.1', '1.2', 'battle', 'proxy', '2.1', '1', '1', '0', '23722', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.53.1', '1.1', 'battle', 'shard', '2.1', '1', '1', '0', '23733', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.53.1', '1.2', 'battle', 'shard', '2.1', '1', '1', '0', '23733', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.61.1', '1.1', 'mail', 'master', '2.1', '1', '1', '0', '23744', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.61.1', '1.2', 'mail', 'master', '2.1', '1', '1', '0', '23744', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.62.1', '1.1', 'mail', 'proxy', '2.1', '1', '1', '0', '23756', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.62.1', '1.2', 'mail', 'proxy', '2.1', '1', '1', '0', '23756', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.63.1', '1.1', 'mail', 'shard', '2.1', '1', '1', '0', '23767', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.63.1', '1.2', 'mail', 'shard', '2.1', '1', '1', '0', '23767', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.71.1', '1.1', 'relation', 'master', '2.1', '1', '1', '0', '23778', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.71.1', '1.2', 'relation', 'master', '2.1', '1', '1', '0', '23778', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.72.1', '1.1', 'relation', 'proxy', '2.1', '1', '1', '0', '23790', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.72.1', '1.2', 'relation', 'proxy', '2.1', '1', '1', '0', '23790', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.73.1', '1.1', 'relation', 'shard', '2.1', '1', '1', '0', '23801', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.73.1', '1.2', 'relation', 'shard', '2.1', '1', '1', '0', '23801', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.81.1', '1.1', 'group', 'master', '2.1', '1', '1', '0', '23813', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.81.1', '1.2', 'group', 'master', '2.1', '1', '1', '0', '23813', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.82.1', '1.1', 'group', 'proxy', '2.1', '1', '1', '0', '23824', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.82.1', '1.2', 'group', 'proxy', '2.1', '1', '1', '0', '23824', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.83.1', '1.1', 'group', 'shard', '2.1', '1', '1', '0', '23836', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.83.1', '1.2', 'group', 'shard', '2.1', '1', '1', '0', '23836', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.91.1', '1.1', 'rank', 'master', '2.1', '1', '1', '0', '23847', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.91.1', '1.2', 'rank', 'master', '2.1', '1', '1', '0', '23847', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.92.1', '1.1', 'rank', 'proxy', '2.1', '1', '1', '0', '23859', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.92.1', '1.2', 'rank', 'proxy', '2.1', '1', '1', '0', '23859', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.93.1', '1.1', 'rank', 'shard', '2.1', '1', '1', '0', '23870', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.0.93.1', '1.2', 'rank', 'shard', '2.1', '1', '1', '0', '23870', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.1.241.1', '1.1', 'zone', 'master', '2.1', '1', '1', '0', '23882', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.1.241.1', '1.2', 'zone', 'master', '2.1', '1', '1', '0', '23882', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.1.242.1', '1.1', 'zone', 'world', '2.1', '1', '1', '0', '23893', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.1.242.1', '1.2', 'zone', 'world', '2.1', '1', '1', '0', '23893', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.1.243.1', '1.1', 'zone', 'gate', '2.1', '1', '1', '0', '23908', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.1.243.1', '1.2', 'zone', 'gate', '2.1', '1', '1', '0', '23908', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.1.244.1', '1.1', 'zone', 'login', '2.1', '1', '1', '0', '23919', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.1.244.1', '1.2', 'zone', 'login', '2.1', '1', '1', '0', '23919', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.1.245.1', '1.1', 'zone', 'game', '2.1', '1', '1', '0', '23930', '1540808547', '1.0.3.1', '192.168.10.230', 'sgame_develop_315_201810291815');
INSERT INTO `deploy` VALUES ('1.1.245.1', '1.2', 'zone', 'game', '2.1', '1', '1', '0', '23930', '1540808547', '1.0.3.1', '192.168.1.9', 'sgame_develop_315_201810291815');

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
INSERT INTO `launch` VALUES ('auth', 'auth', '', 'bin/KFStartup', './startup/auth.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('battle', 'master', '', 'bin/KFStartup', './startup/battle.master.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('battle', 'proxy', '', 'bin/KFStartup', './startup/battle.proxy.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('battle', 'shard', '', 'bin/KFStartup', './startup/battle.shard.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('data', 'master', '', 'bin/KFStartup', './startup/data.master.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('data', 'proxy', '', 'bin/KFStartup', './startup/data.proxy.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('data', 'shard', '', 'bin/KFStartup', './startup/data.shard.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('dir', 'master', '', 'bin/KFStartup', './startup/dir.master.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('dir', 'proxy', '', 'bin/KFStartup', './startup/dir.proxy.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('dir', 'shard', '', 'bin/KFStartup', './startup/dir.shard.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('group', 'master', '', 'bin/KFStartup', './startup/group.master.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('group', 'proxy', '', 'bin/KFStartup', './startup/group.proxy.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('group', 'shard', '', 'bin/KFStartup', './startup/group.shard.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('guild', 'master', '', 'bin/KFStartup', './startup/guild.master.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('guild', 'proxy', '', 'bin/KFStartup', './startup/guild.proxy.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('guild', 'shard', '', 'bin/KFStartup', './startup/guild.shard.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('log', 'master', '', 'bin/KFStartup', './startup/log.master.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('log', 'proxy', '', 'bin/KFStartup', './startup/log.proxy.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('log', 'shard', '', 'bin/KFStartup', './startup/log.shard.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('mail', 'master', '', 'bin/KFStartup', './startup/mail.master.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('mail', 'proxy', '', 'bin/KFStartup', './startup/mail.proxy.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('mail', 'shard', '', 'bin/KFStartup', './startup/mail.shard.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('match', 'master', '', 'bin/KFStartup', './startup/match.master.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('match', 'proxy', '', 'bin/KFStartup', './startup/match.proxy.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('match', 'shard', '', 'bin/KFStartup', './startup/match.shard.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('public', 'master', '', 'bin/KFStartup', './startup/public.master.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('public', 'proxy', '', 'bin/KFStartup', './startup/public.proxy.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('public', 'shard', '', 'bin/KFStartup', './startup/public.shard.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('rank', 'master', '', 'bin/KFStartup', './startup/rank.master.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('rank', 'proxy', '', 'bin/KFStartup', './startup/rank.proxy.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('rank', 'shard', '', 'bin/KFStartup', './startup/rank.shard.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('relation', 'master', '', 'bin/KFStartup', './startup/relation.master.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('relation', 'proxy', '', 'bin/KFStartup', './startup/relation.proxy.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('relation', 'shard', '', 'bin/KFStartup', './startup/relation.shard.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('route', 'master', '', 'bin/KFStartup', './startup/route.master.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('route', 'proxy', '', 'bin/KFStartup', './startup/route.proxy.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('route', 'shard', '', 'bin/KFStartup', './startup/route.shard.startup', 'cluster', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('zone', 'game', '', 'bin/KFStartup', './startup/zone.game.startup', 'zone', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('zone', 'gate', '', 'bin/KFStartup', './startup/zone.gate.startup', 'zone', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('zone', 'login', '', 'bin/KFStartup', './startup/zone.login.startup', 'zone', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('zone', 'master', '', 'bin/KFStartup', './startup/zone.master.startup', 'zone', '1', '/data/sgserver');
INSERT INTO `launch` VALUES ('zone', 'world', '', 'bin/KFStartup', './startup/zone.world.startup', 'zone', '1', '/data/sgserver');

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
INSERT INTO `machine` VALUES ('内网测试机', '192.168.1.9', '192.168.1.9', '4', '8', '16', '320', '1.0.3.1', '1', '12074');
INSERT INTO `machine` VALUES ('研发测试机', '192.168.10.230', '192.168.10.230', '8', '16', '16', '100', '1.0.3.1', '1', '12028');
