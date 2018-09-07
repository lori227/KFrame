/*
Navicat MySQL Data Transfer

Source Server         : 192.168.1.9
Source Server Version : 50640
Source Host           : 192.168.1.9:3306
Source Database       : kframe

Target Server Type    : MYSQL
Target Server Version : 50640
File Encoding         : 65001

Date: 2018-09-07 17:01:11
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `deploy`
-- ----------------------------
DROP TABLE IF EXISTS `deploy`;
CREATE TABLE `deploy` (
  `appid` varchar(50) NOT NULL COMMENT '服务器id',
  `appchannel` int(10) NOT NULL DEFAULT '0',
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
  PRIMARY KEY (`appid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of deploy
-- ----------------------------
INSERT INTO `deploy` VALUES ('101.0.11.1', '101', 'data', 'master', '2.0', '1', '1', '0', '17643', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.111.1', '101', 'log', 'master', '1.0', '1', '1', '0', '17654', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.112.1', '101', 'log', 'proxy', '1.0', '1', '1', '0', '17666', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.113.1', '101', 'log', 'shard', '1.0', '1', '1', '0', '17680', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.12.1', '101', 'data', 'proxy', '2.0', '1', '1', '0', '17693', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.121.1', '101', 'dir', 'master', '2.0', '1', '1', '0', '17708', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.122.1', '101', 'dir', 'proxy', '2.0', '1', '1', '0', '17718', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.123.1', '101', 'dir', 'shard', '2.0', '1', '1', '0', '17729', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.13.1', '101', 'data', 'shard', '2.0', '1', '1', '0', '17742', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.21.1', '101', 'public', 'master', '2.0', '1', '1', '0', '17759', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.22.1', '101', 'public', 'proxy', '2.0', '1', '1', '0', '17773', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.23.1', '101', 'public', 'shard', '2.0', '1', '1', '0', '17787', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.31.1', '101', 'route', 'master', '2.0', '1', '1', '0', '17803', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.32.1', '101', 'route', 'proxy', '2.0', '1', '1', '0', '17817', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.33.1', '101', 'route', 'shard', '2.0', '1', '1', '0', '17832', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.4.1', '101', 'auth', 'auth', '2.0', '1', '1', '0', '17846', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.41.1', '101', 'match', 'master', '2.0', '1', '1', '0', '17861', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.42.1', '101', 'match', 'proxy', '2.0', '1', '1', '0', '17874', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.43.1', '101', 'match', 'shard', '2.0', '1', '1', '0', '17888', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.51.1', '101', 'battle', 'master', '2.0', '1', '1', '0', '17901', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.52.1', '101', 'battle', 'proxy', '2.0', '1', '1', '0', '17916', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.53.1', '101', 'battle', 'shard', '2.0', '1', '1', '0', '17929', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.61.1', '101', 'mail', 'master', '2.0', '1', '1', '0', '17944', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.62.1', '101', 'mail', 'proxy', '2.0', '1', '1', '0', '17960', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.63.1', '101', 'mail', 'shard', '2.0', '1', '1', '0', '17973', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.71.1', '101', 'relation', 'master', '2.0', '1', '1', '0', '17988', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.72.1', '101', 'relation', 'proxy', '2.0', '1', '1', '0', '18002', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.73.1', '101', 'relation', 'shard', '2.0', '1', '1', '0', '18016', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.81.1', '101', 'group', 'master', '2.0', '1', '1', '0', '18030', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.82.1', '101', 'group', 'proxy', '2.0', '1', '1', '0', '18044', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.83.1', '101', 'group', 'shard', '2.0', '1', '1', '0', '18058', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.91.1', '101', 'rank', 'master', '2.0', '1', '1', '0', '18071', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.92.1', '101', 'rank', 'proxy', '2.0', '1', '1', '0', '18086', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.0.93.1', '101', 'rank', 'shard', '2.0', '1', '1', '0', '18100', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.1.241.1', '101', 'zone', 'master', '2.0', '1', '1', '0', '18114', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.1.242.1', '101', 'zone', 'world', '2.0', '1', '1', '0', '18127', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.1.243.1', '101', 'zone', 'gate', '2.0', '1', '1', '0', '18144', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.1.244.1', '101', 'zone', 'login', '2.0', '1', '1', '0', '18160', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.1.245.1', '101', 'zone', 'game', '2.0', '1', '1', '0', '18178', '1536303014', '101.0.3.1', '192.168.1.9', 'sgame_internal_svn_75_201809071326');
INSERT INTO `deploy` VALUES ('101.3.241.1', '101', 'zone', 'master', '2.0', '1', '1', '0', '7218', '1536138941', '101.0.3.227', '192.168.10.230', '');
INSERT INTO `deploy` VALUES ('101.3.242.1', '101', 'zone', 'world', '2.0', '1', '1', '0', '7228', '1536138941', '101.0.3.227', '192.168.10.230', '');
INSERT INTO `deploy` VALUES ('101.3.243.1', '101', 'zone', 'gate', '2.0', '1', '1', '0', '7241', '1536138941', '101.0.3.227', '192.168.10.230', '');
INSERT INTO `deploy` VALUES ('101.3.244.1', '101', 'zone', 'login', '2.0', '1', '1', '0', '7251', '1536138941', '101.0.3.227', '192.168.10.230', '');
INSERT INTO `deploy` VALUES ('101.3.245.1', '101', 'zone', 'game', '2.0', '1', '1', '0', '7261', '1536138941', '101.0.3.227', '192.168.10.230', '');
