/*
Navicat MySQL Data Transfer

Source Server         : 192.168.1.9
Source Server Version : 50640
Source Host           : 192.168.1.9:3306
Source Database       : kframe

Target Server Type    : MYSQL
Target Server Version : 50640
File Encoding         : 65001

Date: 2018-09-07 17:01:18
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
