/*
Navicat MySQL Data Transfer

Source Server         : #100 - 战旗版本机
Source Server Version : 50722
Source Host           : 192.168.2.31:3306
Source Database       : kfdeploy

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2019-06-12 16:48:28
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `1_1_deploy`
-- ----------------------------
DROP TABLE IF EXISTS `1_1_deploy`;
CREATE TABLE `1_1_deploy` (
  `deploypath` varchar(100) NOT NULL DEFAULT '/data/' COMMENT '部署路径',
  `apppath` varchar(50) NOT NULL DEFAULT '' COMMENT '程序目录',
  `appfile` varchar(50) NOT NULL DEFAULT 'bin/KFStartup' COMMENT '进程文件名',
  `appname` varchar(50) NOT NULL DEFAULT '' COMMENT '服务器名字',
  `apptype` varchar(50) NOT NULL DEFAULT '' COMMENT '服务器类型',
  `appid` varchar(50) NOT NULL DEFAULT '' COMMENT '服务器id',
  `service` varchar(50) NOT NULL DEFAULT '1.2',
  `net` varchar(50) NOT NULL DEFAULT '0',
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
-- Records of 1_1_deploy
-- ----------------------------
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'auth', 'auth', '21.0.1', '1.1', '1', '1.0', '', '12.0.1', '1', '1', '0', '21760', '1560179823', '11.0.1', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'master', '22.0.1', '1.1', '1', '1.0', '', '12.0.1', '1', '1', '0', '21797', '1560179823', '11.0.1', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'proxy', '23.0.1', '1.1', '1', '1.0', '', '12.0.1', '1', '1', '0', '21818', '1560179823', '11.0.1', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'shard', '24.0.1', '1.1', '1', '1.0', '', '12.0.1', '1', '1', '0', '21829', '1560179823', '11.0.1', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'data', 'shard', '31.0.1', '1.1', '1', '1.0', '', '12.0.1', '1', '1', '0', '21810', '1560179823', '11.0.1', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'public', 'shard', '32.0.1', '1.1', '1', '1.0', '', '12.0.1', '1', '1', '0', '21778', '1560179823', '11.0.1', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'match', 'shard', '38.0.1', '1.1', '1', '1.0', '', '12.0.1', '1', '1', '0', '21861', '1560179823', '11.0.1', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'room', 'shard', '39.0.1', '1.1', '1', '1.0', '', '12.0.1', '1', '1', '0', '21839', '1560179823', '11.0.1', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'master', '91.1.1', '1.1', '1', '1.0', '', '12.0.1', '1', '1', '0', '21786', '1560179823', '11.0.1', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'world', '92.1.1', '1.1', '1', '1.0', '', '12.0.1', '1', '1', '0', '21850', '1560179823', '11.0.1', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'gate', '93.1.1', '1.1', '1', '1.0', '', '12.0.1', '1', '1', '0', '21744', '1560179823', '11.0.1', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'login', '94.1.1', '1.1', '1', '1.0', '', '12.0.1', '1', '1', '0', '21752', '1560179823', '11.0.1', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'game', '95.1.1', '1.1', '1', '1.0', '', '12.0.1', '1', '1', '0', '21736', '1560179823', '11.0.1', 'chess_develop_1.0.71.259.tar.gz');

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
  `net` varchar(50) NOT NULL DEFAULT '0',
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
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'auth', 'auth', '21.0.1', '1.2', '1', '1.0', '', '12.0.2', '1', '1', '0', '21760', '1560179823', '11.0.2', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'master', '22.0.1', '1.2', '1', '1.0', '', '12.0.2', '1', '1', '0', '21797', '1560179823', '11.0.2', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'proxy', '23.0.1', '1.2', '1', '1.0', '', '12.0.2', '1', '1', '0', '21818', '1560179823', '11.0.2', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'shard', '24.0.1', '1.2', '1', '1.0', '', '12.0.2', '1', '1', '0', '21829', '1560179823', '11.0.2', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'data', 'shard', '31.0.1', '1.2', '1', '1.0', '', '12.0.2', '1', '1', '0', '21810', '1560179823', '11.0.2', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'public', 'shard', '32.0.1', '1.2', '1', '1.0', '', '12.0.2', '1', '1', '0', '21778', '1560179823', '11.0.2', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'match', 'shard', '38.0.1', '1.2', '1', '1.0', '', '12.0.2', '1', '1', '0', '21861', '1560179823', '11.0.2', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'room', 'shard', '39.0.1', '1.2', '1', '1.0', '', '12.0.2', '1', '1', '0', '21839', '1560179823', '11.0.2', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'master', '91.1.1', '1.2', '1', '1.0', '', '12.0.2', '1', '1', '0', '21786', '1560179823', '11.0.2', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'world', '92.1.1', '1.2', '1', '1.0', '', '12.0.2', '1', '1', '0', '21850', '1560179823', '11.0.2', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'gate', '93.1.1', '1.2', '1', '1.0', '', '12.0.2', '1', '1', '0', '21744', '1560179823', '11.0.2', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'login', '94.1.1', '1.2', '1', '1.0', '', '12.0.2', '1', '1', '0', '21752', '1560179823', '11.0.2', 'chess_develop_1.0.71.259.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'game', '95.1.1', '1.2', '1', '1.0', '', '12.0.2', '1', '1', '0', '21736', '1560179823', '11.0.2', 'chess_develop_1.0.71.259.tar.gz');

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
INSERT INTO `agent` VALUES ('192.168.2.30', '192.168.2.30', '12.0.2', '1.2', '11.0.2', '192.168.2.30', '10000', '1', '12008', '');
INSERT INTO `agent` VALUES ('192.168.2.31', '192.168.2.31', '12.0.1', '1.1', '11.0.1', '192.168.2.31', '10000', '0', '0', '');

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
-- Table structure for `plugin`
-- ----------------------------
DROP TABLE IF EXISTS `plugin`;
CREATE TABLE `plugin` (
  `file_name` varchar(50) NOT NULL,
  `plugin_name` varchar(50) NOT NULL DEFAULT '',
  `file_path` varchar(50) NOT NULL,
  `file_url` varchar(200) NOT NULL,
  `file_time` datetime NOT NULL,
  `file_md5` varchar(50) NOT NULL,
  PRIMARY KEY (`file_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of plugin
-- ----------------------------

-- ----------------------------
-- Table structure for `resource`
-- ----------------------------
DROP TABLE IF EXISTS `resource`;
CREATE TABLE `resource` (
  `resource_time` datetime NOT NULL,
  `resource_name` varchar(128) NOT NULL,
  `resource_url` varchar(512) NOT NULL,
  `resource_md5` varchar(32) NOT NULL,
  PRIMARY KEY (`resource_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of resource
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
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of tool
-- ----------------------------
INSERT INTO `tool` VALUES ('3', '172.28.166.193');
INSERT INTO `tool` VALUES ('4', '192.168.1.42');
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
INSERT INTO `version` VALUES ('2019-06-10 18:52:47', 'chess_develop_1.0.71.252.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.0.71.252.tar.gz', '2ad4c0137ff160c7ba1c92bdd7ba3652');
INSERT INTO `version` VALUES ('2019-06-10 22:47:09', 'chess_develop_1.0.71.256.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.0.71.256.tar.gz', '67b8496648f6526d25d731f1d037d07a');
INSERT INTO `version` VALUES ('2019-06-10 23:15:38', 'chess_develop_1.0.71.259.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.0.71.259.tar.gz', 'c8fa4f864ac1bd3934c58886a7f1decb');
