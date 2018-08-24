/*
Navicat MySQL Data Transfer

Source Server         : 外网测试
Source Server Version : 50723
Source Host           : 123.206.102.191:3306
Source Database       : kframe

Target Server Type    : MYSQL
Target Server Version : 50723
File Encoding         : 65001

Date: 2018-08-23 16:46:46
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
  `logtype` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '日志类型 0 不打日志 1 本地日志  2远程日志',
  `service` varchar(50) NOT NULL DEFAULT '' COMMENT '服务类型  cluster 集群服务  zone 游戏分区',
  `ftpid` int(11) NOT NULL DEFAULT '1' COMMENT 'ftpid',
  `deploypath` varchar(100) NOT NULL DEFAULT '/data/' COMMENT '部署路径',
  PRIMARY KEY (`appname`,`apptype`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of launch
-- ----------------------------
INSERT INTO `launch` VALUES ('auth', 'auth', 'auth', 'authserver', './startup/auth.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('battle', 'master', 'battle', 'battleserver', './startup/battle.master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('battle', 'proxy', 'battle', 'battleserver', './startup/battle.proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('battle', 'shard', 'battle', 'battleserver', './startup/battle.shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('data', 'master', 'data', 'dataserver', './startup/data.master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('data', 'proxy', 'data', 'dataserver', './startup/data.proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('data', 'shard', 'data', 'dataserver', './startup/data.shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('dir', 'master', 'dir', 'dirserver', './startup/dir.master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('dir', 'proxy', 'dir', 'dirserver', './startup/dir.proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('dir', 'shard', 'dir', 'dirserver', './startup/dir.shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('group', 'master', 'group', 'groupserver', './startup/group.master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('group', 'proxy', 'group', 'groupserver', './startup/group.proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('group', 'shard', 'group', 'groupserver', './startup/group.shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('guild', 'master', 'guild', 'guildserver', './startup/guild.master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('guild', 'proxy', 'guild', 'guildserver', './startup/guild.proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('guild', 'shard', 'guild', 'guildserver', './startup/guild.shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('log', 'master', 'log', 'logserver', './startup/log.master.startup', '1', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('log', 'proxy', 'log', 'logserver', './startup/log.proxy.startup', '1', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('log', 'shard', 'log', 'logserver', './startup/log.shard.startup', '1', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('mail', 'master', 'mail', 'mailserver', './startup/mail.master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('mail', 'proxy', 'mail', 'mailserver', './startup/mail.proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('mail', 'shard', 'mail', 'mailserver', './startup/mail.shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('match', 'master', 'match', 'matchserver', './startup/match.master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('match', 'proxy', 'match', 'matchserver', './startup/match.proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('match', 'shard', 'match', 'matchserver', './startup/match.shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('public', 'master', 'public', 'publicserver', './startup/public.master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('public', 'proxy', 'public', 'publicserver', './startup/public.proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('public', 'shard', 'public', 'publicserver', './startup/public.shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('rank', 'master', 'rank', 'rankserver', './startup/rank.master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('rank', 'proxy', 'rank', 'rankserver', './startup/rank.proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('rank', 'shard', 'rank', 'rankserver', './startup/rank.shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('relation', 'master', 'relation', 'relationserver', './startup/relation.master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('relation', 'proxy', 'relation', 'relationserver', './startup/relation.proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('relation', 'shard', 'relation', 'relationserver', './startup/relation.shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('route', 'master', 'route', 'routeserver', './startup/route.master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('route', 'proxy', 'route', 'routeserver', './startup/route.proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('route', 'shard', 'route', 'routeserver', './startup/route.shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'game', 'zone', 'zoneserver', './startup/zone.game.startup', '2', 'zone', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'gate', 'zone', 'zoneserver', './startup/zone.gate.startup', '2', 'zone', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'login', 'zone', 'zoneserver', './startup/zone.login.startup', '2', 'zone', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'master', 'zone', 'zoneserver', './startup/zone.master.startup', '2', 'zone', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'world', 'zone', 'zoneserver', './startup/zone.world.startup', '2', 'zone', '1', '../');
