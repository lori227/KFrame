/*
Navicat MySQL Data Transfer

Source Server         : 小兵测试
Source Server Version : 50722
Source Host           : 192.168.10.230:3306
Source Database       : kframe

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2018-09-11 14:44:21
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
INSERT INTO `deploy` VALUES ('100.0.11.1', 'data', 'master', '100', '0', '2.0', '1', '1', '0', '36335', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.111.1', 'log', 'master', '100', '0', '1.0', '1', '1', '0', '36346', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.112.1', 'log', 'proxy', '100', '0', '1.0', '1', '1', '0', '36360', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.113.1', 'log', 'shard', '100', '0', '1.0', '1', '1', '0', '36377', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.12.1', 'data', 'proxy', '100', '0', '2.0', '1', '1', '0', '36391', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.121.1', 'dir', 'master', '100', '0', '2.0', '1', '1', '0', '36407', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.122.1', 'dir', 'proxy', '100', '0', '2.0', '1', '1', '0', '36417', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.123.1', 'dir', 'shard', '100', '0', '2.0', '1', '1', '0', '36427', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.13.1', 'data', 'shard', '100', '0', '2.0', '1', '1', '0', '36441', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.21.1', 'public', 'master', '100', '0', '2.0', '1', '1', '0', '36454', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.22.1', 'public', 'proxy', '100', '0', '2.0', '1', '1', '0', '36468', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.23.1', 'public', 'shard', '100', '0', '2.0', '1', '1', '0', '36482', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.31.1', 'route', 'master', '100', '0', '2.0', '1', '1', '0', '36496', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.32.1', 'route', 'proxy', '100', '0', '2.0', '1', '1', '0', '36509', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.33.1', 'route', 'shard', '100', '0', '2.0', '1', '1', '0', '36523', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.4.1', 'auth', 'auth', '100', '0', '2.0', '1', '1', '0', '36536', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.41.1', 'match', 'master', '100', '0', '2.0', '1', '1', '0', '36552', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.42.1', 'match', 'proxy', '100', '0', '2.0', '1', '1', '0', '36565', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.43.1', 'match', 'shard', '100', '0', '2.0', '1', '1', '0', '36579', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.51.1', 'battle', 'master', '100', '0', '2.0', '1', '1', '0', '36592', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.52.1', 'battle', 'proxy', '100', '0', '2.0', '1', '1', '0', '36605', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.53.1', 'battle', 'shard', '100', '0', '2.0', '1', '1', '0', '36621', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.61.1', 'mail', 'master', '100', '0', '2.0', '1', '1', '0', '36635', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.62.1', 'mail', 'proxy', '100', '0', '2.0', '1', '1', '0', '36652', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.63.1', 'mail', 'shard', '100', '0', '2.0', '1', '1', '0', '36665', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.71.1', 'relation', 'master', '100', '0', '2.0', '1', '1', '0', '36680', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.72.1', 'relation', 'proxy', '100', '0', '2.0', '1', '1', '0', '36693', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.73.1', 'relation', 'shard', '100', '0', '2.0', '1', '1', '0', '36706', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.81.1', 'group', 'master', '100', '0', '2.0', '1', '1', '0', '36721', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.82.1', 'group', 'proxy', '100', '0', '2.0', '1', '1', '0', '36734', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.83.1', 'group', 'shard', '100', '0', '2.0', '1', '1', '0', '36748', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.91.1', 'rank', 'master', '100', '0', '2.0', '1', '1', '0', '36761', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.92.1', 'rank', 'proxy', '100', '0', '2.0', '1', '1', '0', '36774', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.0.93.1', 'rank', 'shard', '100', '0', '2.0', '1', '1', '0', '36787', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.1.241.1', 'zone', 'master', '100', '1', '2.0', '1', '1', '0', '36802', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.1.242.1', 'zone', 'world', '100', '1', '2.0', '1', '1', '0', '36816', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.1.243.1', 'zone', 'gate', '100', '1', '2.0', '1', '1', '0', '36832', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.1.244.1', 'zone', 'login', '100', '1', '2.0', '1', '1', '0', '36846', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
INSERT INTO `deploy` VALUES ('100.1.245.1', 'zone', 'game', '100', '1', '2.0', '1', '1', '0', '36859', '1536560560', '100.0.3.1', '192.168.10.230', 'sgame_internal_svn_115_201809101402');
