/*
Navicat MySQL Data Transfer

Source Server         : 192.168.1.9
Source Server Version : 50640
Source Host           : 192.168.1.9:3306
Source Database       : kframe

Target Server Type    : MYSQL
Target Server Version : 50640
File Encoding         : 65001

Date: 2018-08-29 20:26:05
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
