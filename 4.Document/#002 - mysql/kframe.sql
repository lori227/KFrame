/*
Navicat MySQL Data Transfer

Source Server         : 外网中控版本
Source Server Version : 50722
Source Host           : 118.25.173.109:3306
Source Database       : kframe

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2018-10-29 09:51:14
*/

SET FOREIGN_KEY_CHECKS=0;

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
INSERT INTO `machine` VALUES ('技术封测机', '10.105.200.169', '118.25.172.173', '8', '16', '16', '100', '1.0.3.3', '0', '12001');
INSERT INTO `machine` VALUES ('渠道测试机', '10.105.51.22', '118.25.126.120', '8', '16', '16', '100', '1.0.3.2', '1', '12002');
INSERT INTO `machine` VALUES ('外网测试机', '10.105.6.78', '123.206.102.191', '4', '8', '16', '320', '1.0.3.1', '1', '12001');

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
INSERT INTO `version` VALUES ('2018-10-19 16:14:41', 'sgame_online_242_201810191614.tar.gz', 'https://version.laywoodgames.com/upload/sgame_online_242_201810191614.tar.gz', '32a40ddd594e0531e9a7fb6660e4450b');
INSERT INTO `version` VALUES ('2018-10-19 17:38:06', 'sgame_online_244_201810191737.tar.gz', 'https://version.laywoodgames.com/upload/sgame_online_244_201810191737.tar.gz', '4fb36235eb6bfbab66fcccae85dad439');
INSERT INTO `version` VALUES ('2018-10-26 16:28:31', 'sgame_online_292_201810261627.tar.gz', 'https://version.laywoodgames.com/upload/sgame_online_292_201810261627.tar.gz', '169cb4d1dc5f90c31be73b8f6caf63d3');
INSERT INTO `version` VALUES ('2018-10-26 19:50:52', 'sgame_online_296_201810261950.tar.gz', 'https://version.laywoodgames.com/upload/sgame_online_296_201810261950.tar.gz', '2ef27a578b8717a3738ea7e86a8ce88c');
INSERT INTO `version` VALUES ('2018-10-27 21:15:52', 'sgame_online_300_201810272115.tar.gz', 'https://version.laywoodgames.com/upload/sgame_online_300_201810272115.tar.gz', 'fe9ea143e69387f366b7cc7dd4455d50');
INSERT INTO `version` VALUES ('2018-09-17 19:47:43', 'sgame_online_svn_180_201809171947.tar.gz', 'https://version.laywoodgames.com/upload/sgame_online_svn_180_201809171947.tar.gz', '6957944f3fc1bc2427e3c2c1b90a58a1');
INSERT INTO `version` VALUES ('2018-09-17 19:55:53', 'sgame_online_svn_181_201809171955.tar.gz', 'https://version.laywoodgames.com/upload/sgame_online_svn_181_201809171955.tar.gz', '04ec54a55e2ee568ac0eb545c493b0d9');
INSERT INTO `version` VALUES ('2018-09-17 21:06:04', 'sgame_online_svn_183_201809172105.tar.gz', 'https://version.laywoodgames.com/upload/sgame_online_svn_183_201809172105.tar.gz', 'cd0bd9fd85a37fa9fabcda270e0670be');
INSERT INTO `version` VALUES ('2018-09-17 21:42:47', 'sgame_online_svn_185_201809172142.tar.gz', 'https://version.laywoodgames.com/upload/sgame_online_svn_185_201809172142.tar.gz', 'c48d3c4f69dfb356b42733b89de8e1d0');
INSERT INTO `version` VALUES ('2018-09-18 15:07:36', 'sgame_online_svn_187_201809181506.tar.gz', 'https://version.laywoodgames.com/upload/sgame_online_svn_187_201809181506.tar.gz', '7b4ac9b096c628ac19f49f4804a25e94');
INSERT INTO `version` VALUES ('2018-09-19 09:55:39', 'sgame_online_svn_188_201809190955.tar.gz', 'https://version.laywoodgames.com/upload/sgame_online_svn_188_201809190955.tar.gz', 'c57632c1d6146a3b5782773f537d6eb7');
INSERT INTO `version` VALUES ('2018-09-19 10:29:59', 'sgame_online_svn_190_201809191029.tar.gz', 'https://version.laywoodgames.com/upload/sgame_online_svn_190_201809191029.tar.gz', 'b02b6336b64d07f8f295d23fdb39771f');
