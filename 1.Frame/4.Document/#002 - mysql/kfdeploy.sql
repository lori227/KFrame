/*
Navicat MySQL Data Transfer

Source Server         : #100 - 小兵部署
Source Server Version : 50722
Source Host           : 47.104.228.135:3306
Source Database       : kfdeploy

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2019-02-15 15:47:49
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
INSERT INTO `1_1_deploy` VALUES ('/data/sgserver', '', 'bin/KFStartup', 'auth', 'auth', '1.21.0.1', '1.2', '1.0', '172.31.192.96', '1', '1', '0', '12803', '1550212921', '1.12.0.1', 'sgserver_develop_1.0.46.de8fad8.tar.gz');

-- ----------------------------
-- Table structure for `agent`
-- ----------------------------
DROP TABLE IF EXISTS `agent`;
CREATE TABLE `agent` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `localip` varchar(50) NOT NULL DEFAULT '',
  `strappid` varchar(50) NOT NULL DEFAULT '',
  `appid` bigint(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `ip` (`localip`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of agent
-- ----------------------------
INSERT INTO `agent` VALUES ('1', '172.31.192.96', '1.12.0.1', '281474977497089');
INSERT INTO `agent` VALUES ('2', '192.168.79.26', '1.12.0.2', '562949954207745');

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
INSERT INTO `machine` VALUES ('小兵测试机', '192.168.1.9', '192.168.1.9', '4', '8', '16', '320', '1.0.3.1', '1', '12074');

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
INSERT INTO `version` VALUES ('2019-02-14 11:02:00', 'sgserver_develop_1.0.45.22b150d.tar.gz', 'http://172.31.192.96/upload/sgserver_develop_1.0.45.22b150d.tar.gz', '573d808ac3bc0038eaff92f4d4cdf0df');
INSERT INTO `version` VALUES ('2019-02-14 13:34:26', 'sgserver_develop_1.0.45.a0abbc7.tar.gz', 'http://172.31.192.96/upload/sgserver_develop_1.0.45.a0abbc7.tar.gz', '19f6a4a2d0a9532b65d355b99b945241');
INSERT INTO `version` VALUES ('2019-02-14 11:28:48', 'sgserver_develop_1.0.45.cef3745.tar.gz', 'http://172.31.192.96/upload/sgserver_develop_1.0.45.cef3745.tar.gz', 'd0c98caa95318f218da90eef3fd14de1');
INSERT INTO `version` VALUES ('2019-02-15 14:05:52', 'sgserver_develop_1.0.46.de8fad8.tar.gz', 'http://172.31.192.96:8080/upload/sgserver_develop_1.0.46.de8fad8.tar.gz', '37cfab6f283664192cf73d6e59e99d66');
