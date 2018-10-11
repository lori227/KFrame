/*
Navicat MySQL Data Transfer

Source Server         : 1.9测试服
Source Server Version : 50640
Source Host           : 192.168.1.9:3306
Source Database       : kframe

Target Server Type    : MYSQL
Target Server Version : 50640
File Encoding         : 65001

Date: 2018-09-11 17:08:04
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
  `appchannel` int(10) NOT NULL DEFAULT '0',
  `zoneid` int(10) NOT NULL DEFAULT '0' COMMENT '如果是小区, 这个是zoneid',
  `logtype` varchar(50) NOT NULL DEFAULT '1.0' COMMENT 'x.y x 0 不打日志 1 本地日志  2远程日志 y 日志级别',
  `startup` int(10) unsigned NOT NULL COMMENT '是否已经关闭',
  `debug` int(10) NOT NULL DEFAULT '0' COMMENT '1=debug 0 release',
  `shutdown` int(10) NOT NULL DEFAULT '0',
  `process` int(10) unsigned NOT NULL COMMENT '进程id',
  `time` bigint(20) unsigned NOT NULL COMMENT '状态时间( 启动或者关闭 )',
  `agentid` varchar(50) NOT NULL COMMENT '部署agentid',
  `localip` varchar(50) NOT NULL COMMENT '局域网ip',
  `version` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`appid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of deploy
-- ----------------------------
INSERT INTO `deploy` VALUES ('101.0.11.1', 'data', 'master', '101', '0', '2.0', '1', '1', '0', '36335', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.111.1', 'log', 'master', '101', '0', '1.0', '1', '1', '0', '36346', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.112.1', 'log', 'proxy', '101', '0', '1.0', '1', '1', '0', '36360', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.113.1', 'log', 'shard', '101', '0', '1.0', '1', '1', '0', '36377', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.12.1', 'data', 'proxy', '101', '0', '2.0', '1', '1', '0', '36391', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.121.1', 'dir', 'master', '101', '0', '2.0', '1', '1', '0', '36407', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.122.1', 'dir', 'proxy', '101', '0', '2.0', '1', '1', '0', '36417', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.123.1', 'dir', 'shard', '101', '0', '2.0', '1', '1', '0', '36427', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.13.1', 'data', 'shard', '101', '0', '2.0', '1', '1', '0', '36441', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.21.1', 'public', 'master', '101', '0', '2.0', '1', '1', '0', '36454', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.22.1', 'public', 'proxy', '101', '0', '2.0', '1', '1', '0', '36468', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.23.1', 'public', 'shard', '101', '0', '2.0', '1', '1', '0', '36482', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.31.1', 'route', 'master', '101', '0', '2.0', '1', '1', '0', '36496', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.32.1', 'route', 'proxy', '101', '0', '2.0', '1', '1', '0', '36509', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.33.1', 'route', 'shard', '101', '0', '2.0', '1', '1', '0', '36523', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.4.1', 'auth', 'auth', '101', '0', '2.0', '1', '1', '0', '36536', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.41.1', 'match', 'master', '101', '0', '2.0', '1', '1', '0', '36552', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.42.1', 'match', 'proxy', '101', '0', '2.0', '1', '1', '0', '36565', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.43.1', 'match', 'shard', '101', '0', '2.0', '1', '1', '0', '36579', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.51.1', 'battle', 'master', '101', '0', '2.0', '1', '1', '0', '36592', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.52.1', 'battle', 'proxy', '101', '0', '2.0', '1', '1', '0', '36605', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.53.1', 'battle', 'shard', '101', '0', '2.0', '1', '1', '0', '36621', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.61.1', 'mail', 'master', '101', '0', '2.0', '1', '1', '0', '36635', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.62.1', 'mail', 'proxy', '101', '0', '2.0', '1', '1', '0', '36652', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.63.1', 'mail', 'shard', '101', '0', '2.0', '1', '1', '0', '36665', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.71.1', 'relation', 'master', '101', '0', '2.0', '1', '1', '0', '36680', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.72.1', 'relation', 'proxy', '101', '0', '2.0', '1', '1', '0', '36693', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.73.1', 'relation', 'shard', '101', '0', '2.0', '1', '1', '0', '36706', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.81.1', 'group', 'master', '101', '0', '2.0', '1', '1', '0', '36721', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.82.1', 'group', 'proxy', '101', '0', '2.0', '1', '1', '0', '36734', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.83.1', 'group', 'shard', '101', '0', '2.0', '1', '1', '0', '36748', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.91.1', 'rank', 'master', '101', '0', '2.0', '1', '1', '0', '36761', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.92.1', 'rank', 'proxy', '101', '0', '2.0', '1', '1', '0', '36774', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.0.93.1', 'rank', 'shard', '101', '0', '2.0', '1', '1', '0', '36787', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.1.241.1', 'zone', 'master', '101', '1', '2.0', '1', '1', '0', '36802', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.1.242.1', 'zone', 'world', '101', '1', '2.0', '1', '1', '0', '36816', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.1.243.1', 'zone', 'gate', '101', '1', '2.0', '1', '1', '0', '36832', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.1.244.1', 'zone', 'login', '101', '1', '2.0', '1', '1', '0', '36846', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('101.1.245.1', 'zone', 'game', '101', '1', '2.0', '1', '1', '0', '36859', '1536560560', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_115_201809101402');

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
INSERT INTO `machine` VALUES ('内网测试机', '192.168.1.9', '192.168.1.9', '4', '8', '16', '320', '101.0.3.1', '1', '12052');
INSERT INTO `machine` VALUES ('小兵测试机', '192.168.10.230', '192.168.10.230', '8', '16', '16', '100', '101.0.3.227', '0', '12001');

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
INSERT INTO `version` VALUES ('2018-09-08 12:12:31', 'sgame_internal_svn_106_201809081210.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_106_201809081210.tar.gz', 'ad06c7930d538e658e3234ea28c43bc7');
INSERT INTO `version` VALUES ('2018-09-08 19:12:29', 'sgame_internal_svn_106_201809081910.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_106_201809081910.tar.gz', 'df53dd8974b72af558fbee5dd3fb0c3e');
INSERT INTO `version` VALUES ('2018-09-10 12:08:46', 'sgame_internal_svn_110_201809101207.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_110_201809101207.tar.gz', '7cc872c55b36a95b4493554b5ccb9066');
INSERT INTO `version` VALUES ('2018-09-10 14:04:45', 'sgame_internal_svn_115_201809101402.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_115_201809101402.tar.gz', 'c2dad829f1198e75fbe2d107b9bb4f0e');
INSERT INTO `version` VALUES ('2018-09-10 19:12:07', 'sgame_internal_svn_120_201809101910.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_120_201809101910.tar.gz', 'c29d3a577bf7b8ef60438c8e05a6107e');
INSERT INTO `version` VALUES ('2018-09-11 12:11:54', 'sgame_internal_svn_128_201809111210.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_128_201809111210.tar.gz', '8aba3d598df53686fbb13a88b8166a4d');
INSERT INTO `version` VALUES ('2018-08-31 16:30:29', 'sgame_internal_svn_20_201808311629.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_20_201808311629.tar.gz', 'f7627f84d2aa9c865e77563ad7840223');
INSERT INTO `version` VALUES ('2018-08-31 19:12:58', 'sgame_internal_svn_25_201808311912.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_25_201808311912.tar.gz', 'fc53a5146ce4a397bb53e71e1f4f4d92');
INSERT INTO `version` VALUES ('2018-09-02 09:56:56', 'sgame_internal_svn_26_201809020956.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_26_201809020956.tar.gz', 'a0b6adb4acfccb8aebe1e4161d5187ef');
INSERT INTO `version` VALUES ('2018-09-02 10:15:19', 'sgame_internal_svn_26_201809021014.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_26_201809021014.tar.gz', '53df27c745caf9c88482e6ea05edbc16');
INSERT INTO `version` VALUES ('2018-09-03 12:12:45', 'sgame_internal_svn_36_201809031212.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_36_201809031212.tar.gz', '417c8dc800f546cbf161da836af4ba0a');
INSERT INTO `version` VALUES ('2018-09-03 14:55:45', 'sgame_internal_svn_39_201809031455.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_39_201809031455.tar.gz', '91ef8c625f899af8bffbf096a543e7a0');
INSERT INTO `version` VALUES ('2018-09-03 19:12:44', 'sgame_internal_svn_39_201809031912.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_39_201809031912.tar.gz', '192ec817ff4a07ccebc9fc0d8c287058');
INSERT INTO `version` VALUES ('2018-09-04 12:12:30', 'sgame_internal_svn_40_201809041211.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_40_201809041211.tar.gz', '89ff0d0739a6bb459708e50b87bdcbf4');
INSERT INTO `version` VALUES ('2018-09-04 19:12:46', 'sgame_internal_svn_40_201809041912.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_40_201809041912.tar.gz', '51d20935e527a6c7dd0301b74db6b27d');
INSERT INTO `version` VALUES ('2018-09-05 12:12:30', 'sgame_internal_svn_41_201809051211.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_41_201809051211.tar.gz', '7015cb5420e9fbc10d17fc3d80b559be');
INSERT INTO `version` VALUES ('2018-09-05 14:40:06', 'sgame_internal_svn_48_201809051439.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_48_201809051439.tar.gz', '7885665feac3905831eaa264740819e6');
INSERT INTO `version` VALUES ('2018-09-05 15:16:26', 'sgame_internal_svn_49_201809051515.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_49_201809051515.tar.gz', '7c7a56cb4b32fe4f9fdf60915906376d');
INSERT INTO `version` VALUES ('2018-09-05 15:16:26', 'sgame_internal_svn_51_201809051657.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_51_201809051657.tar.gz', '4b91380fd43a528eab00d04d0870927e');
INSERT INTO `version` VALUES ('2018-09-05 19:27:57', 'sgame_internal_svn_55_201809051921.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_55_201809051921.tar.gz', '1a6232103f28a48390e66bab440d3dd9');
INSERT INTO `version` VALUES ('2018-09-06 12:18:48', 'sgame_internal_svn_57_201809061212.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_57_201809061212.tar.gz', '187b9cfc23ccc2964edd932ccfa85c93');
INSERT INTO `version` VALUES ('2018-09-06 19:18:28', 'sgame_internal_svn_63_201809061912.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_63_201809061912.tar.gz', 'dc86710e8457a5d135771b55eb2cee1b');
INSERT INTO `version` VALUES ('2018-09-07 11:09:32', 'sgame_internal_svn_71_201809071107.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_71_201809071107.tar.gz', 'bc23f09667f3a7d004a3723053465baa');
INSERT INTO `version` VALUES ('2018-09-07 11:34:59', 'sgame_internal_svn_73_201809071133.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_73_201809071133.tar.gz', '5908a08f7cae4cd548ed39c9635171e9');
INSERT INTO `version` VALUES ('2018-09-07 12:12:44', 'sgame_internal_svn_74_201809071210.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_74_201809071210.tar.gz', '99dcf7bf86ac3f146a59110e4967ab02');
INSERT INTO `version` VALUES ('2018-09-07 13:28:38', 'sgame_internal_svn_75_201809071326.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_75_201809071326.tar.gz', '01d7ad3cd3e11497fb839ccdf0e1b092');
INSERT INTO `version` VALUES ('2018-09-07 18:30:57', 'sgame_internal_svn_97_201809071829.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_97_201809071829.tar.gz', '5982e59cc91e6f764c2ba86dd575cd94');
INSERT INTO `version` VALUES ('2018-09-07 19:12:45', 'sgame_internal_svn_97_201809071910.tar.gz', 'http://version.leiwu.com/static/upload/version/sgame_internal_svn_97_201809071910.tar.gz', 'c99033822082216411ad8bc7de8ce271');
