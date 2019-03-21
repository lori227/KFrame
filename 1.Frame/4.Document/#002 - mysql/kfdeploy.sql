/*
Navicat MySQL Data Transfer

Source Server         : #100 - 战旗版本机
Source Server Version : 50722
Source Host           : 192.168.1.250:3306
Source Database       : kfdeploy

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2019-03-21 20:58:34
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `1_1_deploy`
-- ----------------------------
DROP TABLE IF EXISTS `1_1_deploy`;
CREATE TABLE `1_1_deploy` (
  `deploypath` varchar(100) NOT NULL DEFAULT '/data/' COMMENT '部署路径',
  `apppath` varchar(50) NOT NULL DEFAULT '' COMMENT '程序目录',
  `appfile` varchar(50) NOT NULL DEFAULT '' COMMENT '进程文件名',
  `appname` varchar(50) NOT NULL COMMENT '服务器名字',
  `apptype` varchar(50) NOT NULL COMMENT '服务器类型',
  `appid` varchar(50) NOT NULL COMMENT '服务器id',
  `service` varchar(50) NOT NULL DEFAULT '1.2',
  `logtype` varchar(50) NOT NULL DEFAULT '1.0' COMMENT 'x.y x 0 不打日志 1 本地日志  2远程日志 y 日志级别',
  `localip` varchar(50) NOT NULL DEFAULT '' COMMENT '局域网ip',
  `startup` int(10) unsigned NOT NULL DEFAULT '1' COMMENT '是否已经关闭',
  `debug` int(10) NOT NULL DEFAULT '0' COMMENT '1=debug 0 release',
  `shutdown` int(10) NOT NULL DEFAULT '0',
  `process` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '进程id',
  `time` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '状态时间( 启动或者关闭 )',
  `agentid` varchar(50) NOT NULL DEFAULT '' COMMENT '部署agentid',
  `version` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`appid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of 1_1_deploy
-- ----------------------------
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'auth', 'auth', '1.21.0.1', '1.2', '1.0', '192.168.1.250', '1', '1', '0', '12803', '1550212921', '1.12.0.1', 'sgserver_develop_1.0.46.de8fad8.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'master', '1.22.0.1', '1.2', '1.0', '192.168.1.250', '1', '1', '0', '12803', '1550212921', '1.12.0.1', 'sgserver_develop_1.0.46.de8fad8.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'proxy', '1.23.0.1', '1.2', '1.0', '192.168.1.250', '1', '1', '0', '12803', '1550212921', '1.12.0.1', 'sgserver_develop_1.0.46.de8fad8.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'master', '1.24.0.1', '1.2', '1.0', '192.168.1.250', '1', '1', '0', '12803', '1550212921', '1.12.0.1', 'sgserver_develop_1.0.46.de8fad8.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'data', 'shard', '1.31.0.1', '1.2', '1.0', '192.168.1.250', '1', '1', '0', '12803', '1550212921', '1.12.0.1', 'sgserver_develop_1.0.46.de8fad8.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'master', '1.91.1.1', '1.2', '1.0', '192.168.1.250', '1', '1', '0', '12803', '1550212921', '1.12.0.1', 'sgserver_develop_1.0.46.de8fad8.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'world', '1.92.1.1', '1.2', '1.0', '192.168.1.250', '1', '1', '0', '12803', '1550212921', '1.12.0.1', 'sgserver_develop_1.0.46.de8fad8.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'gate', '1.93.1.1', '1.2', '1.0', '192.168.1.250', '1', '1', '0', '12803', '1550212921', '1.12.0.1', 'sgserver_develop_1.0.46.de8fad8.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'login', '1.94.1.1', '1.2', '1.0', '192.168.1.250', '1', '1', '0', '12803', '1550212921', '1.12.0.1', 'sgserver_develop_1.0.46.de8fad8.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'game', '1.95.1.1', '1.2', '1.0', '192.168.1.250', '1', '1', '0', '12803', '1550212921', '1.12.0.1', 'sgserver_develop_1.0.46.de8fad8.tar.gz');

-- ----------------------------
-- Table structure for `agent`
-- ----------------------------
DROP TABLE IF EXISTS `agent`;
CREATE TABLE `agent` (
  `localip` varchar(50) NOT NULL DEFAULT '',
  `interanetip` varchar(50) NOT NULL DEFAULT '',
  `strappid` varchar(50) NOT NULL DEFAULT '',
  `serverid` varchar(50) NOT NULL DEFAULT '',
  `serverip` varchar(50) NOT NULL DEFAULT '',
  `serverport` int(10) unsigned NOT NULL DEFAULT '10000',
  `status` int(10) unsigned NOT NULL DEFAULT '0',
  `port` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`localip`),
  KEY `ip` (`localip`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of agent
-- ----------------------------
INSERT INTO `agent` VALUES ('172.31.192.96', '47.104.228.135', '1.12.0.1', '1.11.0.1', '172.31.192.96', '10000', '0', '0');

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
INSERT INTO `version` VALUES ('2019-03-21 19:54:34', 'dark_develop_1.0.80.34.tar.gz', 'http://192.168.1.250/upload/dark_develop_1.0.80.34.tar.gz', 'd36d1465c5b183f1936bdfed3c811e7b');
