/*
Navicat MySQL Data Transfer

Source Server         : #100 - 战旗版本机
Source Server Version : 50722
Source Host           : 192.168.1.250:3306
Source Database       : kfdeploy

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2019-04-16 15:48:44
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `1_2_deploy`
-- ----------------------------
DROP TABLE IF EXISTS `1_2_deploy`;
CREATE TABLE `1_2_deploy` (
  `deploypath` varchar(100) NOT NULL DEFAULT '/data/' COMMENT '部署路径',
  `apppath` varchar(50) NOT NULL DEFAULT '' COMMENT '程序目录',
  `appfile` varchar(50) NOT NULL DEFAULT 'bin/KFStartup' COMMENT '进程文件名',
  `appname` varchar(50) NOT NULL DEFAULT '' COMMENT '服务器名字',
  `apptype` varchar(50) NOT NULL DEFAULT '' COMMENT '服务器类型',
  `appid` varchar(50) NOT NULL DEFAULT '' COMMENT '服务器id',
  `service` varchar(50) NOT NULL DEFAULT '1.2',
  `logtype` varchar(50) NOT NULL DEFAULT '1.0' COMMENT 'x.y x 0 不打日志 1 本地日志  2远程日志 y 日志级别',
  `param` varchar(500) NOT NULL DEFAULT '',
  `agentid` varchar(50) NOT NULL DEFAULT '' COMMENT '部署agentid',
  `startup` int(10) unsigned NOT NULL DEFAULT '1' COMMENT '是否已经关闭',
  `debug` int(10) NOT NULL DEFAULT '0' COMMENT '1=debug 0 release',
  `shutdown` int(10) NOT NULL DEFAULT '0',
  `process` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '进程id',
  `time` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '状态时间( 启动或者关闭 )',
  `deployid` varchar(50) NOT NULL DEFAULT '',
  `version` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`appid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of 1_2_deploy
-- ----------------------------
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'auth', 'auth', '1.21.0.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '18056', '1555400500', '1.11.0.1', 'chess_develop_1.0.16.124.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'master', '1.22.0.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '18077', '1555400500', '1.11.0.1', 'chess_develop_1.0.16.124.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'proxy', '1.23.0.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '18106', '1555400500', '1.11.0.1', 'chess_develop_1.0.16.124.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'shard', '1.24.0.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '18172', '1555400500', '1.11.0.1', 'chess_develop_1.0.16.124.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'data', 'shard', '1.31.0.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '18092', '1555400500', '1.11.0.1', 'chess_develop_1.0.16.124.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'match', 'shard', '1.38.0.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '18121', '1555400500', '1.11.0.1', 'chess_develop_1.0.16.124.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'room', 'shard', '1.39.0.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '18135', '1555400500', '1.11.0.1', 'chess_develop_1.0.16.124.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'master', '1.91.1.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '18143', '1555400500', '1.11.0.1', 'chess_develop_1.0.16.124.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'world', '1.92.1.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '18163', '1555400500', '1.11.0.1', 'chess_develop_1.0.16.124.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'gate', '1.93.1.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '18197', '1555400500', '1.11.0.1', 'chess_develop_1.0.16.124.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'login', '1.94.1.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '18046', '1555400500', '1.11.0.1', 'chess_develop_1.0.16.124.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'game', '1.95.1.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '18030', '1555400500', '1.11.0.1', 'chess_develop_1.0.16.124.tar.gz');

-- ----------------------------
-- Table structure for `agent`
-- ----------------------------
DROP TABLE IF EXISTS `agent`;
CREATE TABLE `agent` (
  `localip` varchar(50) NOT NULL DEFAULT '',
  `interanetip` varchar(50) NOT NULL DEFAULT '',
  `strappid` varchar(50) NOT NULL DEFAULT '',
  `service` varchar(50) NOT NULL DEFAULT '',
  `serverid` varchar(50) NOT NULL DEFAULT '',
  `serverip` varchar(50) NOT NULL DEFAULT '',
  `serverport` int(10) unsigned NOT NULL DEFAULT '10000',
  `status` int(10) unsigned NOT NULL DEFAULT '0',
  `port` int(10) NOT NULL DEFAULT '0',
  `command` varchar(200) NOT NULL DEFAULT '',
  PRIMARY KEY (`localip`),
  KEY `ip` (`localip`) USING BTREE,
  KEY `id` (`strappid`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of agent
-- ----------------------------
INSERT INTO `agent` VALUES ('192.168.1.155', '192.168.1.155', '1.12.0.1', '2', '1.11.0.1', '192.168.1.250', '10000', '1', '12001', '');

-- ----------------------------
-- Table structure for `file`
-- ----------------------------
DROP TABLE IF EXISTS `file`;
CREATE TABLE `file` (
  `file_name` varchar(50) NOT NULL,
  `file_path` varchar(50) NOT NULL,
  `file_url` varchar(200) NOT NULL,
  `file_time` datetime NOT NULL,
  `file_md5` varchar(50) NOT NULL,
  PRIMARY KEY (`file_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of file
-- ----------------------------

-- ----------------------------
-- Table structure for `tool`
-- ----------------------------
DROP TABLE IF EXISTS `tool`;
CREATE TABLE `tool` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `toolip` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `tool` (`toolip`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of tool
-- ----------------------------
INSERT INTO `tool` VALUES ('2', '192.168.1.47');
INSERT INTO `tool` VALUES ('1', '192.168.1.71');

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
INSERT INTO `version` VALUES ('2019-04-10 22:57:50', 'chess_develop_1.0.10.103.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.10.103.tar.gz', '65ab43742aa999c85cb52c486408c3d0');
INSERT INTO `version` VALUES ('2019-04-10 23:15:09', 'chess_develop_1.0.10.104.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.10.104.tar.gz', 'b788f590ad0cb2d8111e0f6d44e074b5');
INSERT INTO `version` VALUES ('2019-04-11 14:30:32', 'chess_develop_1.0.11.107.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.11.107.tar.gz', 'd38926a9b25ff16699e6f55009d61be9');
INSERT INTO `version` VALUES ('2019-04-11 22:13:49', 'chess_develop_1.0.11.110.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.11.110.tar.gz', '629d6c6159e4d24bd1f83e5ad2c4bae1');
INSERT INTO `version` VALUES ('2019-04-12 17:51:38', 'chess_develop_1.0.12.113.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.12.113.tar.gz', '3a786168b2aa7300abeb1be082266017');
INSERT INTO `version` VALUES ('2019-04-12 20:21:09', 'chess_develop_1.0.12.114.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.12.114.tar.gz', '699b1443febc6b140a8af49d02394356');
INSERT INTO `version` VALUES ('2019-04-13 13:14:30', 'chess_develop_1.0.13.115.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.13.115.tar.gz', 'a242e0b21599d2d344bb46002f7a36af');
INSERT INTO `version` VALUES ('2019-04-13 18:55:05', 'chess_develop_1.0.13.116.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.13.116.tar.gz', '6af7c77422e42b9bcfe9d6c3591f6c85');
INSERT INTO `version` VALUES ('2019-04-15 21:16:53', 'chess_develop_1.0.15.120.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.15.120.tar.gz', 'd339b654d65819360185b632aeddc3ec');
INSERT INTO `version` VALUES ('2019-04-16 11:14:54', 'chess_develop_1.0.16.121.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.16.121.tar.gz', '73a0530cd3bfde5aa615a8c834150469');
INSERT INTO `version` VALUES ('2019-04-16 15:39:50', 'chess_develop_1.0.16.124.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.16.124.tar.gz', '4b7702ff39be85b19fa34be93ac1ffdb');
