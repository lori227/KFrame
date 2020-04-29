/*
Navicat MySQL Data Transfer

Source Server         : #100 - 战旗版本机
Source Server Version : 50722
Source Host           : 192.168.2.31:3306
Source Database       : kfdeploy

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2020-04-29 12:14:46
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `1_1_deploy`
-- ----------------------------
DROP TABLE IF EXISTS `1_1_deploy`;
CREATE TABLE `1_1_deploy` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `deploypath` varchar(100) NOT NULL DEFAULT '/data/' COMMENT '部署路径',
  `apppath` varchar(50) NOT NULL DEFAULT '' COMMENT '程序目录',
  `appfile` varchar(50) NOT NULL DEFAULT 'bin/KFStartup' COMMENT '进程文件名',
  `appname` varchar(50) NOT NULL DEFAULT '' COMMENT '服务器名字',
  `apptype` varchar(50) NOT NULL DEFAULT '' COMMENT '服务器类型',
  `appid` varchar(50) NOT NULL DEFAULT '' COMMENT '服务器id',
  `service` varchar(50) NOT NULL DEFAULT '1.2',
  `net` varchar(50) NOT NULL DEFAULT '0',
  `param` varchar(500) NOT NULL DEFAULT '',
  `agentid` varchar(50) NOT NULL DEFAULT '' COMMENT '部署agentid',
  `heartbeat` int(10) unsigned NOT NULL DEFAULT '0',
  `startup` int(10) unsigned NOT NULL DEFAULT '1' COMMENT '是否已经关闭',
  `debug` int(10) NOT NULL DEFAULT '0' COMMENT '1=debug 0 release',
  `shutdown` int(10) NOT NULL DEFAULT '0',
  `process` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '进程id',
  `time` bigint(20) unsigned NOT NULL DEFAULT '0' COMMENT '状态时间( 启动或者关闭 )',
  `deployid` varchar(50) NOT NULL DEFAULT '',
  `version` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `appid` (`appid`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=25 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of 1_1_deploy
-- ----------------------------
INSERT INTO `1_1_deploy` VALUES ('1', '/data/chess/dir', '', 'bin/KFStartup', 'dir', 'shard', '19.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '2366', '1587995754', '', 'chess_develop_1.1.393.1892.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('2', '/data/chess/auth', '', 'bin/KFStartup', 'auth', 'shard', '21.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '2329', '1587995507', '11.0.1', 'chess_develop_1.1.393.1892.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('3', '/data/chess/route', '', 'bin/KFStartup', 'route', 'master', '22.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '1146', '1587887925', '11.0.1', 'chess_develop_1.1.392.1881.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('4', '/data/chess/route', '', 'bin/KFStartup', 'route', 'proxy', '23.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '1159', '1587887925', '11.0.1', 'chess_develop_1.1.392.1881.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('5', '/data/chess/route', '', 'bin/KFStartup', 'route', 'shard', '24.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '1136', '1587887925', '11.0.1', 'chess_develop_1.1.392.1881.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('6', '/data/chess/data', '', 'bin/KFStartup', 'data', 'shard', '31.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '953', '1587887925', '11.0.1', 'chess_develop_1.1.392.1881.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('7', '/data/chess/logic', '', 'bin/KFStartup', 'logic', 'shard', '32.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '1044', '1587887925', '11.0.1', 'chess_develop_1.1.392.1881.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('8', '/data/chess/match', '', 'bin/KFStartup', 'match', 'shard', '38.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '1085', '1587887925', '11.0.1', 'chess_develop_1.1.392.1881.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('9', '/data/chess/room', '', 'bin/KFStartup', 'room', 'shard', '39.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '1190', '1587887925', '11.0.1', 'chess_develop_1.1.392.1881.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('10', '/data/chess/develop', '', 'bin/KFStartup', 'zone', 'master', '91.1.1', '1.1', '1', '', '12.0.2', '0', '1', '1', '0', '11681', '1588118782', '11.0.1', 'chess_develop_1.1.395.1902.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('11', '/data/chess/develop', '', 'bin/KFStartup', 'zone', 'world', '92.1.1', '1.1', '1', '', '12.0.2', '0', '1', '1', '0', '11667', '1588118782', '11.0.1', 'chess_develop_1.1.395.1902.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('12', '/data/chess/develop', '', 'bin/KFStartup', 'zone', 'gate', '93.1.1', '1.1', '1', '', '12.0.2', '0', '1', '1', '0', '11656', '1588118782', '11.0.1', 'chess_develop_1.1.395.1902.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('13', '/data/chess/develop', '', 'bin/KFStartup', 'zone', 'login', '94.1.1', '1.1', '1', '', '12.0.2', '0', '1', '1', '0', '11645', '1588118782', '11.0.1', 'chess_develop_1.1.395.1902.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('14', '/data/chess/develop', '', 'bin/KFStartup', 'zone', 'game', '95.1.1', '1.1', '1', '', '12.0.2', '0', '1', '1', '0', '11637', '1588118782', '11.0.1', 'chess_develop_1.1.395.1902.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('15', '/data/chess/config', '', 'bin/KFStartup', 'zone', 'master', '91.2.1', '1.1', '1', '', '12.0.2', '0', '1', '1', '0', '19391', '1588034201', '11.0.1', 'chess_config_1.1.394.1892.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('16', '/data/chess/config', '', 'bin/KFStartup', 'zone', 'world', '92.2.1', '1.1', '1', '', '12.0.2', '0', '1', '1', '0', '19377', '1588034201', '11.0.1', 'chess_config_1.1.394.1892.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('17', '/data/chess/config', '', 'bin/KFStartup', 'zone', 'gate', '93.2.1', '1.1', '1', '', '12.0.2', '0', '1', '1', '0', '19358', '1588034201', '11.0.1', 'chess_config_1.1.394.1892.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('18', '/data/chess/config', '', 'bin/KFStartup', 'zone', 'login', '94.2.1', '1.1', '1', '', '12.0.2', '0', '1', '1', '0', '19403', '1588034201', '11.0.1', 'chess_config_1.1.394.1892.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('19', '/data/chess/config', '', 'bin/KFStartup', 'zone', 'game', '95.2.1', '1.1', '1', '', '12.0.2', '0', '1', '1', '0', '19369', '1588034201', '11.0.1', 'chess_config_1.1.394.1892.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('20', '/data/chess/develop', '', 'bin/KFStartup', 'zone', 'master', '91.3.1', '1.1', '1', '', '12.0.3', '0', '1', '1', '1', '0', '1588134505', '11.0.1', 'chess_beta_1.1.289.1398.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('21', '/data/chess/develop', '', 'bin/KFStartup', 'zone', 'world', '92.3.1', '1.1', '1', '', '12.0.3', '0', '1', '1', '1', '0', '1588134505', '11.0.1', 'chess_beta_1.1.289.1398.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('22', '/data/chess/develop', '', 'bin/KFStartup', 'zone', 'gate', '93.3.1', '1.1', '1', '', '12.0.3', '0', '1', '1', '1', '0', '1588134505', '11.0.1', 'chess_beta_1.1.289.1398.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('23', '/data/chess/develop', '', 'bin/KFStartup', 'zone', 'login', '94.3.1', '1.1', '1', '', '12.0.3', '0', '1', '1', '1', '0', '1588134505', '11.0.1', 'chess_beta_1.1.289.1398.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('24', '/data/chess/develop', '', 'bin/KFStartup', 'zone', 'game', '95.3.1', '1.1', '1', '', '12.0.3', '0', '1', '1', '1', '0', '1588134505', '11.0.1', 'chess_beta_1.1.289.1398.tar.gz');

-- ----------------------------
-- Table structure for `1_1_deploy_old`
-- ----------------------------
DROP TABLE IF EXISTS `1_1_deploy_old`;
CREATE TABLE `1_1_deploy_old` (
  `deploypath` varchar(100) NOT NULL DEFAULT '/data/' COMMENT '部署路径',
  `apppath` varchar(50) NOT NULL DEFAULT '' COMMENT '程序目录',
  `appfile` varchar(50) NOT NULL DEFAULT 'bin/KFStartup' COMMENT '进程文件名',
  `appname` varchar(50) NOT NULL DEFAULT '' COMMENT '服务器名字',
  `apptype` varchar(50) NOT NULL DEFAULT '' COMMENT '服务器类型',
  `appid` varchar(50) NOT NULL DEFAULT '' COMMENT '服务器id',
  `service` varchar(50) NOT NULL DEFAULT '1.2',
  `net` varchar(50) NOT NULL DEFAULT '0',
  `param` varchar(500) NOT NULL DEFAULT '',
  `agentid` varchar(50) NOT NULL DEFAULT '' COMMENT '部署agentid',
  `heartbeat` int(10) unsigned NOT NULL DEFAULT '0',
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
-- Records of 1_1_deploy_old
-- ----------------------------
INSERT INTO `1_1_deploy_old` VALUES ('/data/chess', '', 'bin/KFStartup', 'auth', 'auth', '21.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '5240', '1577676510', '11.0.1', 'chess_develop_1.1.274.1318.tar.gz');
INSERT INTO `1_1_deploy_old` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'master', '22.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '5279', '1577676510', '11.0.1', 'chess_develop_1.1.274.1318.tar.gz');
INSERT INTO `1_1_deploy_old` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'proxy', '23.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '5305', '1577676510', '11.0.1', 'chess_develop_1.1.274.1318.tar.gz');
INSERT INTO `1_1_deploy_old` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'shard', '24.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '5318', '1577676510', '11.0.1', 'chess_develop_1.1.274.1318.tar.gz');
INSERT INTO `1_1_deploy_old` VALUES ('/data/chess', '', 'bin/KFStartup', 'data', 'shard', '31.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '5292', '1577676510', '11.0.1', 'chess_develop_1.1.274.1318.tar.gz');
INSERT INTO `1_1_deploy_old` VALUES ('/data/chess', '', 'bin/KFStartup', 'logic', 'shard', '32.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '5253', '1577676510', '11.0.1', 'chess_develop_1.1.274.1318.tar.gz');
INSERT INTO `1_1_deploy_old` VALUES ('/data/chess', '', 'bin/KFStartup', 'match', 'shard', '38.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '5357', '1577676510', '11.0.1', 'chess_develop_1.1.274.1318.tar.gz');
INSERT INTO `1_1_deploy_old` VALUES ('/data/chess', '', 'bin/KFStartup', 'room', 'shard', '39.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '5331', '1577676510', '11.0.1', 'chess_develop_1.1.274.1318.tar.gz');
INSERT INTO `1_1_deploy_old` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'master', '91.1.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '5266', '1577676510', '11.0.1', 'chess_develop_1.1.274.1318.tar.gz');
INSERT INTO `1_1_deploy_old` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'world', '92.1.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '5344', '1577676510', '11.0.1', 'chess_develop_1.1.274.1318.tar.gz');
INSERT INTO `1_1_deploy_old` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'gate', '93.1.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '5214', '1577676510', '11.0.1', 'chess_develop_1.1.274.1318.tar.gz');
INSERT INTO `1_1_deploy_old` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'login', '94.1.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '5227', '1577676510', '11.0.1', 'chess_develop_1.1.274.1318.tar.gz');
INSERT INTO `1_1_deploy_old` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'game', '95.1.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '5201', '1577676510', '11.0.1', 'chess_develop_1.1.274.1318.tar.gz');

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
  `param` varchar(500) NOT NULL DEFAULT '',
  `agentid` varchar(50) NOT NULL DEFAULT '' COMMENT '部署agentid',
  `heartbeat` int(10) unsigned NOT NULL DEFAULT '0',
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
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'auth', 'auth', '21.0.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '23573', '1577664404', '11.0.2', 'chess_config_1.1.274.1318.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'master', '22.0.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '23605', '1577664404', '11.0.2', 'chess_config_1.1.274.1318.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'proxy', '23.0.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '23630', '1577664404', '11.0.2', 'chess_config_1.1.274.1318.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'shard', '24.0.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '23638', '1577664404', '11.0.2', 'chess_config_1.1.274.1318.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'data', 'shard', '31.0.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '23618', '1577664404', '11.0.2', 'chess_config_1.1.274.1318.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'logic', 'shard', '32.0.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '23584', '1577664404', '11.0.2', 'chess_config_1.1.274.1318.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'match', 'shard', '38.0.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '23671', '1577664404', '11.0.2', 'chess_config_1.1.274.1318.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'room', 'shard', '39.0.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '23652', '1577664404', '11.0.2', 'chess_config_1.1.274.1318.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'master', '91.1.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '23597', '1577664404', '11.0.2', 'chess_config_1.1.274.1318.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'world', '92.1.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '23663', '1577664404', '11.0.2', 'chess_config_1.1.274.1318.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'gate', '93.1.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '23554', '1577664404', '11.0.2', 'chess_config_1.1.274.1318.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'login', '94.1.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '23562', '1577664404', '11.0.2', 'chess_config_1.1.274.1318.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'game', '95.1.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '23546', '1577664404', '11.0.2', 'chess_config_1.1.274.1318.tar.gz');

-- ----------------------------
-- Table structure for `1_3_deploy`
-- ----------------------------
DROP TABLE IF EXISTS `1_3_deploy`;
CREATE TABLE `1_3_deploy` (
  `deploypath` varchar(100) NOT NULL DEFAULT '/data/' COMMENT '部署路径',
  `apppath` varchar(50) NOT NULL DEFAULT '' COMMENT '程序目录',
  `appfile` varchar(50) NOT NULL DEFAULT 'bin/KFStartup' COMMENT '进程文件名',
  `appname` varchar(50) NOT NULL DEFAULT '' COMMENT '服务器名字',
  `apptype` varchar(50) NOT NULL DEFAULT '' COMMENT '服务器类型',
  `appid` varchar(50) NOT NULL DEFAULT '' COMMENT '服务器id',
  `service` varchar(50) NOT NULL DEFAULT '1.2',
  `net` varchar(50) NOT NULL DEFAULT '0',
  `param` varchar(500) NOT NULL DEFAULT '',
  `agentid` varchar(50) NOT NULL DEFAULT '' COMMENT '部署agentid',
  `heartbeat` int(10) unsigned NOT NULL DEFAULT '0',
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
-- Records of 1_3_deploy
-- ----------------------------
INSERT INTO `1_3_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'auth', 'auth', '21.0.1', '1.3', '1', '', '12.0.3', '0', '1', '1', '0', '24199', '1577528654', '11.0.3', 'chess_beta_1.1.272.1318.tar.gz');
INSERT INTO `1_3_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'master', '22.0.1', '1.3', '1', '', '12.0.3', '0', '1', '1', '0', '24226', '1577528654', '11.0.3', 'chess_beta_1.1.272.1318.tar.gz');
INSERT INTO `1_3_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'proxy', '23.0.1', '1.3', '1', '', '12.0.3', '0', '1', '1', '0', '24258', '1577528654', '11.0.3', 'chess_beta_1.1.272.1318.tar.gz');
INSERT INTO `1_3_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'shard', '24.0.1', '1.3', '1', '', '12.0.3', '0', '1', '1', '0', '24266', '1577528654', '11.0.3', 'chess_beta_1.1.272.1318.tar.gz');
INSERT INTO `1_3_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'data', 'shard', '31.0.1', '1.3', '1', '', '12.0.3', '0', '1', '1', '0', '24245', '1577528654', '11.0.3', 'chess_beta_1.1.272.1318.tar.gz');
INSERT INTO `1_3_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'logic', 'shard', '32.0.1', '1.3', '1', '', '12.0.3', '0', '1', '1', '0', '24210', '1577528654', '11.0.3', 'chess_beta_1.1.272.1318.tar.gz');
INSERT INTO `1_3_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'match', 'shard', '38.0.1', '1.3', '1', '', '12.0.3', '0', '1', '1', '0', '24300', '1577528654', '11.0.3', 'chess_beta_1.1.272.1318.tar.gz');
INSERT INTO `1_3_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'room', 'shard', '39.0.1', '1.3', '1', '', '12.0.3', '0', '1', '1', '0', '24280', '1577528654', '11.0.3', 'chess_beta_1.1.272.1318.tar.gz');
INSERT INTO `1_3_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'master', '91.1.1', '1.3', '1', '', '12.0.3', '0', '1', '1', '0', '24218', '1577528654', '11.0.3', 'chess_beta_1.1.272.1318.tar.gz');
INSERT INTO `1_3_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'world', '92.1.1', '1.3', '1', '', '12.0.3', '0', '1', '1', '0', '24292', '1577528654', '11.0.3', 'chess_beta_1.1.272.1318.tar.gz');
INSERT INTO `1_3_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'gate', '93.1.1', '1.3', '1', '', '12.0.3', '0', '1', '1', '0', '24183', '1577528654', '11.0.3', 'chess_beta_1.1.272.1318.tar.gz');
INSERT INTO `1_3_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'login', '94.1.1', '1.3', '1', '', '12.0.3', '0', '1', '1', '0', '24191', '1577528654', '11.0.3', 'chess_beta_1.1.272.1318.tar.gz');
INSERT INTO `1_3_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'game', '95.1.1', '1.3', '1', '', '12.0.3', '0', '1', '1', '0', '24175', '1577528654', '11.0.3', 'chess_beta_1.1.272.1318.tar.gz');

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
INSERT INTO `agent` VALUES ('192.168.2.30', '192.168.2.30', '12.0.1', '1.1', '11.0.1', '192.168.2.30', '10000', '1', '12001', '');
INSERT INTO `agent` VALUES ('192.168.2.31', '192.168.2.31', '12.0.2', '1.1', '11.0.1', '192.168.2.30', '10000', '1', '12001', '');
INSERT INTO `agent` VALUES ('192.168.2.32', '192.168.2.32', '12.0.3', '1.1', '11.0.1', '192.168.2.30', '10000', '1', '12001', 'wget:chess_develop_1.1.395.1904.tar.gz | zone:*:*:3');

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
INSERT INTO `resource` VALUES ('2019-08-31 14:33:24', '1.0.152.evision', 'http://192.168.2.31/upload/1.0.152.evision', '2ea256d0338db66f833b9fce1977ffd4');
INSERT INTO `resource` VALUES ('2019-09-10 14:57:47', '1.0.162.evision', 'http://192.168.2.31/upload/1.0.162.evision', 'b8b2513d1970ccdd809cc2b400590688');
INSERT INTO `resource` VALUES ('2019-09-18 15:14:14', '1.0.170.evision', 'http://192.168.2.31/upload/1.0.170.evision', 'c2cc4336b766d6956177280199df5c85');
INSERT INTO `resource` VALUES ('2019-07-22 19:15:32', 'chess_resource_1.0.112.341.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.112.341.tar.gz', '12859981c1fa01e9789792c090786aa6');
INSERT INTO `resource` VALUES ('2019-07-22 19:25:16', 'chess_resource_1.0.112.347.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.112.347.tar.gz', '8a554b9105d2e6c8cf79907724cd65c9');
INSERT INTO `resource` VALUES ('2019-07-22 19:26:08', 'chess_resource_1.0.112.348.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.112.348.tar.gz', '0013592885c5ebfff8f43c53f9b7fdd3');
INSERT INTO `resource` VALUES ('2019-07-22 19:27:01', 'chess_resource_1.0.112.349.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.112.349.tar.gz', 'd8646c6c96d5e751f3ccc04389526947');
INSERT INTO `resource` VALUES ('2019-07-22 19:28:38', 'chess_resource_1.0.112.350.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.112.350.tar.gz', 'de091ef3b799fde67364a4fa1911c5aa');
INSERT INTO `resource` VALUES ('2019-07-22 22:05:03', 'chess_resource_1.0.112.353.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.112.353.tar.gz', '4b4c0338432659621959aa886a110ba6');
INSERT INTO `resource` VALUES ('2019-07-25 17:17:25', 'chess_resource_1.0.115.385.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.115.385.tar.gz', 'd12400e7de105aa56036d1a9303fa4f0');
INSERT INTO `resource` VALUES ('2019-07-25 17:42:03', 'chess_resource_1.0.116.386.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.116.386.tar.gz', 'e8d3796598c719ffe03fa57f458cfb80');
INSERT INTO `resource` VALUES ('2019-07-27 14:29:44', 'chess_resource_1.0.117.398.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.117.398.tar.gz', 'bf9719eaaddbe716063bb70cc1a5693c');
INSERT INTO `resource` VALUES ('2019-07-29 16:34:48', 'chess_resource_1.0.119.407.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.119.407.tar.gz', '9b871c403cf4e1216d744d0fd929e777');
INSERT INTO `resource` VALUES ('2019-07-29 22:18:02', 'chess_resource_1.0.120.419.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.120.419.tar.gz', 'e52dc5819fae27b88f7975e5b5db0b29');
INSERT INTO `resource` VALUES ('2019-08-02 23:56:06', 'chess_resource_1.0.123.458.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.123.458.tar.gz', '4e0b7b6572cb30fd64368c9e3aeb093b');
INSERT INTO `resource` VALUES ('2019-08-05 14:16:09', 'chess_resource_1.0.126.472.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.126.472.tar.gz', '4c2b0b147b3f7c718382a8abbc0d0b83');
INSERT INTO `resource` VALUES ('2019-08-05 14:23:48', 'chess_resource_1.0.126.473.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.126.473.tar.gz', 'dedae38bfa094e9787d54197ad16d19f');
INSERT INTO `resource` VALUES ('2019-08-05 15:13:06', 'chess_resource_1.0.126.474.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.126.474.tar.gz', 'e501af1af7f8c84778a9a5cd76e6a0bf');
INSERT INTO `resource` VALUES ('2019-08-05 15:56:09', 'chess_resource_1.0.126.475.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.126.475.tar.gz', '29ff31f83ab6daa4be5cacb1c3ad025a');
INSERT INTO `resource` VALUES ('2019-08-05 17:17:36', 'chess_resource_1.0.126.476.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.126.476.tar.gz', 'abd82d2144eb8a580a1e1307a83ed856');
INSERT INTO `resource` VALUES ('2019-08-06 17:29:41', 'chess_resource_1.0.128.478.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.128.478.tar.gz', '6d1e968a5931560c10ec45e2fad0f516');
INSERT INTO `resource` VALUES ('2019-08-07 11:26:03', 'chess_resource_1.0.128.488.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.128.488.tar.gz', '105cd66ba978292b3645814aff452abf');
INSERT INTO `resource` VALUES ('2019-08-09 21:40:15', 'chess_resource_1.0.131.526.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.131.526.tar.gz', 'ccec4dbda3fc878876f8f372fc4e9f9a');
INSERT INTO `resource` VALUES ('2019-08-13 16:56:58', 'chess_resource_1.0.135.539.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.135.539.tar.gz', '25072cb96e7a341edd0db11cb100be74');
INSERT INTO `resource` VALUES ('2019-08-13 17:19:47', 'chess_resource_1.0.135.540.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.135.540.tar.gz', '7031c4bc70280b8e216ffdc8f662b154');
INSERT INTO `resource` VALUES ('2019-08-13 17:55:58', 'chess_resource_1.0.135.541.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.135.541.tar.gz', '34ec05823f0d0bf64fc9996888e91b23');
INSERT INTO `resource` VALUES ('2019-08-16 14:39:13', 'chess_resource_1.0.138.564.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.138.564.tar.gz', '95cf6ab60fd0b9c023c9fcf80fd15b23');
INSERT INTO `resource` VALUES ('2019-08-16 20:38:37', 'chess_resource_1.0.138.568.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.138.568.tar.gz', '6a33bf88350c9cb56bf6faf41e584744');
INSERT INTO `resource` VALUES ('2019-08-17 20:39:31', 'chess_resource_1.0.139.574.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.139.574.tar.gz', 'f6fd506c18f686ee1545a8fc1078255e');
INSERT INTO `resource` VALUES ('2019-08-17 21:42:10', 'chess_resource_1.0.139.575.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.139.575.tar.gz', 'c4de45cd0eab9b8e53067e00562f03df');
INSERT INTO `resource` VALUES ('2019-08-17 22:34:58', 'chess_resource_1.0.139.577.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.139.577.tar.gz', '0f7c29790db320a6c09a11592db16775');
INSERT INTO `resource` VALUES ('2019-08-19 21:59:31', 'chess_resource_1.0.140.585.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.140.585.tar.gz', 'a632e537c34bccdaf56820c3a8a2302b');
INSERT INTO `resource` VALUES ('2019-08-19 22:07:54', 'chess_resource_1.0.140.586.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.140.586.tar.gz', '7ea5a127f2fdc3fde254486e7846f375');
INSERT INTO `resource` VALUES ('2019-08-19 17:57:14', 'chess_resource_1.0.141.581.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.141.581.tar.gz', '7fbb3e6b188d8c8459d3e00c5ef701e3');
INSERT INTO `resource` VALUES ('2019-08-19 21:06:42', 'chess_resource_1.0.141.582.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.141.582.tar.gz', '7cdb8d72054aa2d21eff8e9e85e893a1');
INSERT INTO `resource` VALUES ('2019-08-19 21:10:58', 'chess_resource_1.0.141.583.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.141.583.tar.gz', '8398e9dc80dbc81524871ddf6f621627');
INSERT INTO `resource` VALUES ('2019-08-19 21:29:32', 'chess_resource_1.0.141.585.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.141.585.tar.gz', 'ee7048f4d61205d08c0272100ba8e952');
INSERT INTO `resource` VALUES ('2019-08-22 12:37:44', 'chess_resource_1.0.144.599.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.144.599.tar.gz', 'b55ec134ca6df5485f60b99dc4a65053');
INSERT INTO `resource` VALUES ('2019-08-22 14:49:38', 'chess_resource_1.0.144.601.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.144.601.tar.gz', 'c0f901c5221ce2ee0b8a6e0f7b4648b8');
INSERT INTO `resource` VALUES ('2019-08-22 20:21:33', 'chess_resource_1.0.144.604.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.144.604.tar.gz', 'dfde20fef9979abbc2ea8f1bfc146c6a');
INSERT INTO `resource` VALUES ('2019-08-22 20:39:45', 'chess_resource_1.0.144.605.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.144.605.tar.gz', 'dd22104bc3b0cad10f7eb0755332745e');
INSERT INTO `resource` VALUES ('2019-08-23 17:12:19', 'chess_resource_1.0.145.611.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.145.611.tar.gz', 'b478e5ed476e96796cf03a2bc18f30e9');
INSERT INTO `resource` VALUES ('2019-08-24 15:29:03', 'chess_resource_1.0.145.629.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.145.629.tar.gz', '49dbb948189a979b5739847a6b3845ad');
INSERT INTO `resource` VALUES ('2019-08-24 15:35:42', 'chess_resource_1.0.145.630.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.145.630.tar.gz', 'c4e93c96214cde1e2fbab203bcbdaeb1');
INSERT INTO `resource` VALUES ('2019-08-24 15:37:31', 'chess_resource_1.0.145.631.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.145.631.tar.gz', '046f08d3c3a00d3c5bce18ce44e1f5ea');
INSERT INTO `resource` VALUES ('2019-08-24 15:21:49', 'chess_resource_1.0.146.628.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.146.628.tar.gz', '8fac2015ac4afa26dbf4f03baffac8ad');
INSERT INTO `resource` VALUES ('2019-08-29 12:02:27', 'chess_resource_1.0.150.668.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.150.668.tar.gz', 'b7a181f66a04e04b6572dda83b78a491');
INSERT INTO `resource` VALUES ('2019-08-29 12:26:22', 'chess_resource_1.0.150.669.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.150.669.tar.gz', '920a708222d7eae03a1a0aced39ce081');
INSERT INTO `resource` VALUES ('2019-08-29 17:14:25', 'chess_resource_1.0.150.674.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.150.674.tar.gz', '7616b8e8a66d211ff9971c1ed5b96faa');
INSERT INTO `resource` VALUES ('2019-08-29 17:45:06', 'chess_resource_1.0.150.677.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.150.677.tar.gz', '7b65b624301d1753a40f76d1195bfb5a');
INSERT INTO `resource` VALUES ('2019-08-29 16:57:30', 'chess_resource_1.0.150.888.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.150.888.tar.gz', 'd5d3f45e4adcdc23815391cf95af2adf');
INSERT INTO `resource` VALUES ('2019-08-29 17:03:14', 'chess_resource_1.0.150.889.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.150.889.tar.gz', '1e6f2579cf434e316b0cdf479bf38447');
INSERT INTO `resource` VALUES ('2019-08-29 17:04:55', 'chess_resource_1.0.150.890.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.150.890.tar.gz', '50189dccd76bd85bd4be2459473e9910');
INSERT INTO `resource` VALUES ('2019-08-29 17:06:18', 'chess_resource_1.0.150.891.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.150.891.tar.gz', 'e8d52136f3bc75ebaa528bc3e9437230');
INSERT INTO `resource` VALUES ('2019-08-30 15:47:25', 'chess_resource_1.0.151.686.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.151.686.tar.gz', '6b6db8522e1f7b0764c7be705a0ea729');
INSERT INTO `resource` VALUES ('2019-08-30 15:59:35', 'chess_resource_1.0.151.687.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.151.687.tar.gz', 'a1054db7db94fc184cb798b45248fab7');
INSERT INTO `resource` VALUES ('2019-08-30 16:03:59', 'chess_resource_1.0.151.689.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.151.689.tar.gz', '6cdd3eabd1d0280f2ccf0a247a093809');
INSERT INTO `resource` VALUES ('2019-08-30 16:06:43', 'chess_resource_1.0.151.690.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.151.690.tar.gz', '64ba1f39f04ac0c1414e09edfa81362b');
INSERT INTO `resource` VALUES ('2019-08-30 16:39:09', 'chess_resource_1.0.151.691.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.151.691.tar.gz', '4a0e6ba99d2c79330954810b5fe86052');
INSERT INTO `resource` VALUES ('2019-08-30 16:46:34', 'chess_resource_1.0.151.692.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.151.692.tar.gz', 'b89d3237b7308110e09540266ea4083f');
INSERT INTO `resource` VALUES ('2019-08-30 23:55:25', 'chess_resource_1.0.151.701.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.151.701.tar.gz', '9cafce17cab9cbb97b348ae5538ba5a2');
INSERT INTO `resource` VALUES ('2019-08-30 15:40:46', 'chess_resource_1.0.151.~10.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.151.~10.tar.gz', '4b317f332a68ddffb97bbd62dfa4a647');
INSERT INTO `resource` VALUES ('2019-08-31 00:28:19', 'chess_resource_1.0.152.703.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.152.703.tar.gz', '6779b415d427d7bea2f1d9f517dc027a');
INSERT INTO `resource` VALUES ('2019-08-31 14:34:26', 'chess_resource_1.0.152.707.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.152.707.tar.gz', 'c251ce14cd690580c85f6a1a2678213c');
INSERT INTO `resource` VALUES ('2019-09-02 22:53:12', 'chess_resource_1.0.154.714.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.154.714.tar.gz', '94d7deabc0b253c229378be289b422eb');
INSERT INTO `resource` VALUES ('2019-09-03 11:43:20', 'chess_resource_1.0.155.715.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.155.715.tar.gz', 'f41370b66e95de5a0a6de567c4e64839');
INSERT INTO `resource` VALUES ('2019-09-03 15:00:35', 'chess_resource_1.0.155.718.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.155.718.tar.gz', 'c1cad836d7fd2a9cfdd2e51031165f38');
INSERT INTO `resource` VALUES ('2019-09-03 15:38:56', 'chess_resource_1.0.155.720.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.155.720.tar.gz', '3c393d19038024af6a68d8f46e77e52c');
INSERT INTO `resource` VALUES ('2019-09-03 16:55:11', 'chess_resource_1.0.155.725.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.155.725.tar.gz', '1ea45a04409d7c7e6200f1e9fb537c9e');
INSERT INTO `resource` VALUES ('2019-09-03 22:29:49', 'chess_resource_1.0.155.735.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.155.735.tar.gz', '3114253badcd709777f3fdeeea5565fc');
INSERT INTO `resource` VALUES ('2019-09-03 17:11:46', 'chess_resource_1.0.156.613.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.156.613.tar.gz', 'e02e28a195316babf5117db0b2f7cd9b');
INSERT INTO `resource` VALUES ('2019-09-03 11:44:28', 'chess_resource_1.0.156.717.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.156.717.tar.gz', '90b25f7c7042f6fae748f069d3a7a952');
INSERT INTO `resource` VALUES ('2019-09-03 15:40:29', 'chess_resource_1.0.156.722.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.156.722.tar.gz', 'fc0679c8bf8459d889541b38a1034adb');
INSERT INTO `resource` VALUES ('2019-09-04 18:42:01', 'chess_resource_1.0.156.749.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.156.749.tar.gz', 'cf63d20f3f5245d8c3997c0372ee1d45');
INSERT INTO `resource` VALUES ('2019-09-04 19:36:03', 'chess_resource_1.0.156.750.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.156.750.tar.gz', '50548ff3b8e11d83f65cc13bfb6f2531');
INSERT INTO `resource` VALUES ('2019-09-04 23:27:03', 'chess_resource_1.0.156.754.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.156.754.tar.gz', '0e7c04a347e5b28bcbeb0b9b330492bc');
INSERT INTO `resource` VALUES ('2019-09-04 23:31:35', 'chess_resource_1.0.156.755.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.156.755.tar.gz', 'cf828953300818d652a240c0980e0d1e');
INSERT INTO `resource` VALUES ('2019-09-04 23:51:17', 'chess_resource_1.0.156.756.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.156.756.tar.gz', 'f7b0fdfa240be999cd646c003694e5b2');
INSERT INTO `resource` VALUES ('2019-09-04 18:26:56', 'chess_resource_1.0.157.749.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.157.749.tar.gz', '20d427c03d6d4450c2aec6b63519e511');
INSERT INTO `resource` VALUES ('2019-09-05 12:00:08', 'chess_resource_1.0.157.758.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.157.758.tar.gz', '218caf024c3fbbde8eaf239be2bb3de8');
INSERT INTO `resource` VALUES ('2019-09-05 15:21:14', 'chess_resource_1.0.157.761.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.157.761.tar.gz', 'b053b4525cf868140cf5af6996cb737c');
INSERT INTO `resource` VALUES ('2019-09-05 16:53:03', 'chess_resource_1.0.157.762.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.157.762.tar.gz', 'd2838d396406d20b2a5d09757d84e72b');
INSERT INTO `resource` VALUES ('2019-09-05 17:13:23', 'chess_resource_1.0.157.763.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.157.763.tar.gz', '4f67101b3fac01389f966f54bbe99cd3');
INSERT INTO `resource` VALUES ('2019-09-05 17:14:09', 'chess_resource_1.0.157.764.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.157.764.tar.gz', '3a6f8cf958999a587d346759eba5019d');
INSERT INTO `resource` VALUES ('2019-09-05 17:57:27', 'chess_resource_1.0.157.766.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.157.766.tar.gz', 'efd9104585ee60ab1a1ee9fab4b10d10');
INSERT INTO `resource` VALUES ('2019-09-05 18:24:12', 'chess_resource_1.0.157.767.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.157.767.tar.gz', 'b073df6da556a92d78f047dfeb30af86');
INSERT INTO `resource` VALUES ('2019-09-05 20:24:07', 'chess_resource_1.0.157.768.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.157.768.tar.gz', '7cf365b15f693bb26b0c52fbfc431b75');
INSERT INTO `resource` VALUES ('2019-09-06 21:00:10', 'chess_resource_1.0.158.779.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.158.779.tar.gz', '125044ff4d9ba5ea65c8c49a8a50677d');
INSERT INTO `resource` VALUES ('2019-09-06 23:41:08', 'chess_resource_1.0.158.784.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.158.784.tar.gz', 'c28adcd537a3c5124b93d810752ec0fd');
INSERT INTO `resource` VALUES ('2019-09-06 23:46:47', 'chess_resource_1.0.158.785.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.158.785.tar.gz', '372044ce279def1754015071f71565bf');
INSERT INTO `resource` VALUES ('2019-09-06 23:48:46', 'chess_resource_1.0.158.786.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.158.786.tar.gz', '88f2c98e5d09ed022f1cc6341db9aab3');
INSERT INTO `resource` VALUES ('2019-09-06 23:51:04', 'chess_resource_1.0.158.787.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.158.787.tar.gz', 'd934b44c7c4c1b89e82f1f439f838b02');
INSERT INTO `resource` VALUES ('2019-09-06 23:55:06', 'chess_resource_1.0.158.788.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.158.788.tar.gz', '014f7496983e0e3c3263a659c2abc8d1');
INSERT INTO `resource` VALUES ('2019-09-06 21:14:53', 'chess_resource_1.0.159.781.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.159.781.tar.gz', '78a969861708349969308d741b5e7af4');
INSERT INTO `resource` VALUES ('2019-09-07 14:37:07', 'chess_resource_1.0.159.794.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.159.794.tar.gz', '46708a5ba016c7201972c34eb42c1377');
INSERT INTO `resource` VALUES ('2019-09-07 16:23:39', 'chess_resource_1.0.159.803.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.159.803.tar.gz', 'b848aac284822b15c3632647cfd7816a');
INSERT INTO `resource` VALUES ('2019-09-09 14:47:35', 'chess_resource_1.0.162.810.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.162.810.tar.gz', '8873cc92b8561def826cd32609934a1b');
INSERT INTO `resource` VALUES ('2019-09-10 14:58:07', 'chess_resource_1.0.162.821.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.162.821.tar.gz', '1112c61aca59eefecc5a18b9720fccbd');
INSERT INTO `resource` VALUES ('2019-09-10 21:58:14', 'chess_resource_1.0.162.826.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.162.826.tar.gz', 'd0e6960fac2f4b51ef4a5ed1fb6c9ea3');
INSERT INTO `resource` VALUES ('2019-09-11 16:09:42', 'chess_resource_1.0.163.830.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.163.830.tar.gz', '713aac44d04126f24090b4a18c175b01');
INSERT INTO `resource` VALUES ('2019-09-11 17:59:52', 'chess_resource_1.0.164.838.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.164.838.tar.gz', '5e81e3131ff35061f505fa7b842cd51a');
INSERT INTO `resource` VALUES ('2019-09-16 20:55:32', 'chess_resource_1.0.168.859.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.168.859.tar.gz', '2dcc85c6f61b0a44946692b2e91937b5');
INSERT INTO `resource` VALUES ('2019-09-16 20:58:31', 'chess_resource_1.0.168.860.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.168.860.tar.gz', '257448925f6cfcf586fb2d80973b2927');
INSERT INTO `resource` VALUES ('2019-09-16 20:59:13', 'chess_resource_1.0.168.861.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.168.861.tar.gz', '12f6ceba677fb15718844e0bd96c5fbb');
INSERT INTO `resource` VALUES ('2019-09-17 15:38:49', 'chess_resource_1.0.169.871.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.169.871.tar.gz', '6b60d9be2357ddd36071410243fd63e1');
INSERT INTO `resource` VALUES ('2019-09-17 15:58:58', 'chess_resource_1.0.169.873.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.169.873.tar.gz', '0efd56895833f9c8af311cd9e4eeee8b');
INSERT INTO `resource` VALUES ('2019-09-17 21:43:28', 'chess_resource_1.0.169.878.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.169.878.tar.gz', '5c53964e519892eae653c35e9847c180');
INSERT INTO `resource` VALUES ('2019-09-17 21:52:03', 'chess_resource_1.0.169.879.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.169.879.tar.gz', '5f208c997bf28e2d9477ec1923b0b833');
INSERT INTO `resource` VALUES ('2019-09-17 21:57:54', 'chess_resource_1.0.169.880.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.169.880.tar.gz', '5c6a6166c9ca88ded682af42d47658ea');
INSERT INTO `resource` VALUES ('2019-09-18 11:21:35', 'chess_resource_1.0.170.881.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.881.tar.gz', '47f1dd3a7fe29387723fa0190e50929f');
INSERT INTO `resource` VALUES ('2019-09-18 14:21:08', 'chess_resource_1.0.170.887.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.887.tar.gz', '2540d9dadd6d48b17604545ef131bccd');
INSERT INTO `resource` VALUES ('2019-09-18 14:22:57', 'chess_resource_1.0.170.888.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.888.tar.gz', 'b2cb50005c22e5f6a1bb3cf5f31e12a8');
INSERT INTO `resource` VALUES ('2019-09-18 14:24:49', 'chess_resource_1.0.170.889.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.889.tar.gz', '936ff44c1d3781fc20218065c35f0114');
INSERT INTO `resource` VALUES ('2019-09-18 14:27:47', 'chess_resource_1.0.170.891.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.891.tar.gz', '165189365a10e9567785112720840de8');
INSERT INTO `resource` VALUES ('2019-09-18 14:59:07', 'chess_resource_1.0.170.898.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.898.tar.gz', 'fcd4ffe982128f72bf62f44b0f3af2b0');
INSERT INTO `resource` VALUES ('2019-09-18 14:59:14', 'chess_resource_1.0.170.899.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.899.tar.gz', 'e5065bb0b988590dc032154c2de521c2');
INSERT INTO `resource` VALUES ('2019-09-18 15:02:41', 'chess_resource_1.0.170.900.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.900.tar.gz', '3e0a9d590ea6309761404dc6add4845c');
INSERT INTO `resource` VALUES ('2019-09-18 15:05:00', 'chess_resource_1.0.170.901.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.901.tar.gz', 'feaa572a32e7764ddd9e85699093e218');
INSERT INTO `resource` VALUES ('2019-09-18 15:06:15', 'chess_resource_1.0.170.902.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.902.tar.gz', '3622172a16f3dad17d1a2dae9ab5341f');
INSERT INTO `resource` VALUES ('2019-09-18 15:21:04', 'chess_resource_1.0.170.907.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.907.tar.gz', '2fe1f12de28192422113da392d7caa80');
INSERT INTO `resource` VALUES ('2019-09-18 15:22:03', 'chess_resource_1.0.170.908.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.908.tar.gz', 'e71bd2976dd5a7baf6b777561d1829a5');
INSERT INTO `resource` VALUES ('2019-09-18 15:23:48', 'chess_resource_1.0.170.909.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.909.tar.gz', '9320fe388bc89e88e8c02a69e4f76fc0');
INSERT INTO `resource` VALUES ('2019-09-18 15:48:43', 'chess_resource_1.0.170.913.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.913.tar.gz', '00363aa1a33e6247a5f14b99a0d1422b');
INSERT INTO `resource` VALUES ('2019-09-18 15:48:54', 'chess_resource_1.0.170.914.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.914.tar.gz', 'e62fc1ce8f593a615aad58e24180bd42');
INSERT INTO `resource` VALUES ('2019-09-18 15:52:01', 'chess_resource_1.0.170.915.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.915.tar.gz', '119b57e30af32a6e77d0444b4adcbb18');
INSERT INTO `resource` VALUES ('2019-09-18 15:52:23', 'chess_resource_1.0.170.916.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.916.tar.gz', 'dcb5d41698acc0531a6382e35932500d');
INSERT INTO `resource` VALUES ('2019-09-18 15:53:17', 'chess_resource_1.0.170.918.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.918.tar.gz', '3109e3fbdcba7043d9f5b6a41d8acaca');
INSERT INTO `resource` VALUES ('2019-09-18 15:53:54', 'chess_resource_1.0.170.919.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.919.tar.gz', '079d08055d7fb1b48262648ececb42d5');
INSERT INTO `resource` VALUES ('2019-09-18 15:54:05', 'chess_resource_1.0.170.920.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.920.tar.gz', 'a0b05f070c1af5b3d0e1ccdbac3655c9');
INSERT INTO `resource` VALUES ('2019-09-18 15:54:22', 'chess_resource_1.0.170.921.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.921.tar.gz', 'de5329e0f509b3362580608729cb691f');
INSERT INTO `resource` VALUES ('2019-09-18 15:58:08', 'chess_resource_1.0.170.922.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.922.tar.gz', 'e15abe35014d9782fc58d20641e2a26e');
INSERT INTO `resource` VALUES ('2019-09-18 15:58:33', 'chess_resource_1.0.170.923.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.923.tar.gz', '03d362dab566e43805515f20ff5cdaab');
INSERT INTO `resource` VALUES ('2019-09-18 15:59:12', 'chess_resource_1.0.170.924.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.924.tar.gz', '896c87c1e30a4638da85ebf65fe010cc');
INSERT INTO `resource` VALUES ('2019-09-18 15:59:40', 'chess_resource_1.0.170.925.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.925.tar.gz', 'ecb90fbfa3eedae650df9d8cd9583ae5');
INSERT INTO `resource` VALUES ('2019-09-18 16:03:33', 'chess_resource_1.0.170.926.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.926.tar.gz', 'e437127cda89a94ab987cb2f1c79bbb2');
INSERT INTO `resource` VALUES ('2019-09-18 16:03:42', 'chess_resource_1.0.170.927.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.927.tar.gz', 'ac66c0ecd59015f3d1c26ff8160be70f');
INSERT INTO `resource` VALUES ('2019-09-18 16:04:41', 'chess_resource_1.0.170.929.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.929.tar.gz', '5c9909c04a22ff991cf8f207513f9e4c');
INSERT INTO `resource` VALUES ('2019-09-18 16:15:20', 'chess_resource_1.0.170.933.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.933.tar.gz', 'bd66fa6dc9d8c8c74044040e1cc0722e');
INSERT INTO `resource` VALUES ('2019-09-18 16:17:19', 'chess_resource_1.0.170.935.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.935.tar.gz', '4ef7c76e3b19ee224110feb1d9864576');
INSERT INTO `resource` VALUES ('2019-09-18 16:18:21', 'chess_resource_1.0.170.936.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.936.tar.gz', 'f0a2fbd09321d447b8c187fcb26a1cbe');
INSERT INTO `resource` VALUES ('2019-09-18 16:18:34', 'chess_resource_1.0.170.937.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.937.tar.gz', 'c9699eea0fa4ecfdfcfe3e5654c0f72f');
INSERT INTO `resource` VALUES ('2019-09-18 16:19:30', 'chess_resource_1.0.170.938.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.938.tar.gz', 'fbc9beca7ef77d84596bbfe771b949d5');
INSERT INTO `resource` VALUES ('2019-09-18 16:19:38', 'chess_resource_1.0.170.939.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.939.tar.gz', '692b7f87a6fc67ac717d82bd01e96364');
INSERT INTO `resource` VALUES ('2019-09-18 16:19:43', 'chess_resource_1.0.170.940.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.940.tar.gz', '4952c809608916bf9422333aeb25ce1f');
INSERT INTO `resource` VALUES ('2019-09-18 16:19:53', 'chess_resource_1.0.170.941.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.941.tar.gz', '609873636a0b98980905bc74b083f757');
INSERT INTO `resource` VALUES ('2019-09-18 16:22:10', 'chess_resource_1.0.170.943.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.943.tar.gz', '11b618726b6359de14a51c8a912a76ec');
INSERT INTO `resource` VALUES ('2019-09-18 16:22:25', 'chess_resource_1.0.170.944.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.944.tar.gz', '0f438226f28df3c61c3d8c50fa36eb61');
INSERT INTO `resource` VALUES ('2019-09-18 16:29:55', 'chess_resource_1.0.170.947.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.947.tar.gz', 'b71507484a954ef4250d64ad9ee7ac2e');
INSERT INTO `resource` VALUES ('2019-09-18 16:31:57', 'chess_resource_1.0.170.948.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.948.tar.gz', 'be8ef2916f35481bcad572bc6153b4af');
INSERT INTO `resource` VALUES ('2019-09-18 16:39:45', 'chess_resource_1.0.170.953.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.953.tar.gz', '8af378ff19c0ae9587b43909a403b791');
INSERT INTO `resource` VALUES ('2019-09-18 15:47:55', 'chess_resource_1.0.170.999.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.999.tar.gz', '4f35277c098948a3e92a2864e888a2bd');
INSERT INTO `resource` VALUES ('2019-09-18 15:25:28', 'chess_resource_1.0.170.evision910.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.evision910.tar.gz', 'c9a3d5b47276af1e87287f911f7d3200');
INSERT INTO `resource` VALUES ('2019-09-18 15:36:25', 'chess_resource_1.0.170.evision912.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.evision912.tar.gz', 'e4c2d8679d52fdfac6d10c2bce59bbcb');
INSERT INTO `resource` VALUES ('2019-09-18 16:04:27', 'chess_resource_1.0.170.evision928.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.evision928.tar.gz', 'a340c26280e3a719a2a71df1855e48d2');
INSERT INTO `resource` VALUES ('2019-09-18 16:08:49', 'chess_resource_1.0.170.evision930.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.evision930.tar.gz', '1ffd4448ef967771c72f5ade4e9243f6');
INSERT INTO `resource` VALUES ('2019-09-18 16:16:17', 'chess_resource_1.0.170.evision934.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.evision934.tar.gz', 'd2d19d9d8800206ee4632f488e0e15d1');
INSERT INTO `resource` VALUES ('2019-09-18 16:34:21', 'chess_resource_1.0.170.evision951.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.evision951.tar.gz', '8d9c98e22fff228969246ea0818c5dfe');
INSERT INTO `resource` VALUES ('2019-09-18 16:46:20', 'chess_resource_1.0.170.evision954.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.evision954.tar.gz', '332e82bc3a7978737121559758e9f701');
INSERT INTO `resource` VALUES ('2019-09-18 16:28:04', 'chess_resource_1.0.170.~12.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.170.~12.tar.gz', '9b853997a7cf7ae9ace09130197820c0');
INSERT INTO `resource` VALUES ('2019-09-18 11:24:14', 'chess_resource_1.0.171.882.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.171.882.tar.gz', 'f66e74a9d9cd8cc1cdb2cb779d10b1f1');
INSERT INTO `resource` VALUES ('2019-09-19 11:29:55', 'chess_resource_1.0.171.960.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.171.960.tar.gz', '9eb02d066419f64f20a3e2d5b913fc4e');
INSERT INTO `resource` VALUES ('2019-09-19 14:00:39', 'chess_resource_1.0.171.961.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.171.961.tar.gz', 'ba2a037679dc08a0ba70b6c137b3503a');
INSERT INTO `resource` VALUES ('2019-09-19 14:01:18', 'chess_resource_1.0.171.962.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.171.962.tar.gz', 'a66e859bbfd12d3081de8cda959e76ad');
INSERT INTO `resource` VALUES ('2019-09-20 20:07:51', 'chess_resource_1.0.172.1000.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.172.1000.tar.gz', 'c1634b973fa800e3b2b84f91be618bdd');
INSERT INTO `resource` VALUES ('2019-09-20 20:44:50', 'chess_resource_1.0.172.1003.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.172.1003.tar.gz', '3bc465af3a54a962d2900ec58d2986a3');
INSERT INTO `resource` VALUES ('2019-09-20 16:13:35', 'chess_resource_1.0.172.995.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.172.995.tar.gz', '7b9220dcf50ca24948bc56891d9f0ef6');
INSERT INTO `resource` VALUES ('2019-09-20 16:36:58', 'chess_resource_1.0.172.997.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.172.997.tar.gz', '60205ad51887636010cfca2bc2bc912e');
INSERT INTO `resource` VALUES ('2019-09-20 16:37:00', 'chess_resource_1.0.172.998.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.172.998.tar.gz', '59c158b501662d242efe399f0db4f53f');
INSERT INTO `resource` VALUES ('2019-09-20 20:20:30', 'chess_resource_1.0.172.evision1001.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.172.evision1001.tar.gz', '1bf81b95a8259fa5a388d4df7fc6f0e6');
INSERT INTO `resource` VALUES ('2019-09-20 20:28:28', 'chess_resource_1.0.172.evision1002.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.172.evision1002.tar.gz', '1e91c4384400e385495e77e0457654bb');
INSERT INTO `resource` VALUES ('2019-09-21 17:35:05', 'chess_resource_1.0.173.1011.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.173.1011.tar.gz', 'c0757a0d7ba31cc0562c26ed79a3f46d');
INSERT INTO `resource` VALUES ('2019-09-21 17:50:47', 'chess_resource_1.0.173.1013.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.173.1013.tar.gz', 'db23a5750439a2526cd6974356cd954e');
INSERT INTO `resource` VALUES ('2019-09-21 17:58:09', 'chess_resource_1.0.173.1014.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.173.1014.tar.gz', '51b2c3ce370965adb6b8888678a37a2e');
INSERT INTO `resource` VALUES ('2019-09-21 17:50:41', 'chess_resource_1.0.173.evision1012.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.173.evision1012.tar.gz', '6c4815bc5c1482766b27182662840a4e');
INSERT INTO `resource` VALUES ('2019-09-23 15:46:21', 'chess_resource_1.0.175.0.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.175.0.tar.gz', 'cab51306839bdbe58c04de675af64acf');
INSERT INTO `resource` VALUES ('2019-09-23 14:15:04', 'chess_resource_1.0.175.1015.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.175.1015.tar.gz', '54802976f2341b1590d265deac665711');
INSERT INTO `resource` VALUES ('2019-09-23 14:38:27', 'chess_resource_1.0.175.1016.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.175.1016.tar.gz', '5f0d3fca02f65e4fadbe92d35a13f944');
INSERT INTO `resource` VALUES ('2019-09-23 14:54:54', 'chess_resource_1.0.175.1018.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.175.1018.tar.gz', 'd4ccd93c2c5884a45f9e36cd9999f130');
INSERT INTO `resource` VALUES ('2019-09-23 15:00:43', 'chess_resource_1.0.175.1019.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.175.1019.tar.gz', 'f38b78c74d1fae309eae00cfd08d630d');
INSERT INTO `resource` VALUES ('2019-09-23 15:14:55', 'chess_resource_1.0.175.1020.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.175.1020.tar.gz', '516172f867ce7c46d4b3fc22d847e1b4');
INSERT INTO `resource` VALUES ('2019-09-23 15:51:17', 'chess_resource_1.0.175.1021.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.175.1021.tar.gz', 'b008a1772afc030faadab60bbf2dca50');
INSERT INTO `resource` VALUES ('2019-09-23 16:05:20', 'chess_resource_1.0.175.1024.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.175.1024.tar.gz', 'cea73c2e81132340461a744fb2537116');
INSERT INTO `resource` VALUES ('2019-09-23 18:04:16', 'chess_resource_1.0.175.1031.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.175.1031.tar.gz', '46c8812efc28d07f3f44a82d9e7e387c');
INSERT INTO `resource` VALUES ('2019-09-23 14:52:15', 'chess_resource_1.0.175.evision1017.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.175.evision1017.tar.gz', '99b89c37c6efb8dad0663b476472998a');
INSERT INTO `resource` VALUES ('2019-09-23 16:12:49', 'chess_resource_1.0.175.evision1027.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.175.evision1027.tar.gz', '2c1e94482f051096d43d05d5a5de3491');
INSERT INTO `resource` VALUES ('2019-09-23 16:40:34', 'chess_resource_1.0.175.evision1029.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.175.evision1029.tar.gz', 'b04b708ca3c6d26ddf053b55f8cfde88');
INSERT INTO `resource` VALUES ('2019-09-23 15:45:02', 'chess_resource_1.0.175.~12.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.175.~12.tar.gz', 'c74f43523a0003ca48497b81bf92ff51');
INSERT INTO `resource` VALUES ('2019-09-24 18:07:14', 'chess_resource_1.0.176.evision1040.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.0.176.evision1040.tar.gz', 'fbbe7dc5dcf2c154ff0b88d012ed7036');
INSERT INTO `resource` VALUES ('2019-09-26 21:33:29', 'chess_resource_1.1.179.1062.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.1.179.1062.tar.gz', '7df6f28838c2648b9875909c7ed70b11');
INSERT INTO `resource` VALUES ('2019-11-27 16:49:31', 'chess_resource_1.1.241.992.tar.gz', 'http://192.168.2.31/upload/chess_resource_1.1.241.992.tar.gz', '2237fb0beadbb3ea82975e8958580a54');

-- ----------------------------
-- Table structure for `tool`
-- ----------------------------
DROP TABLE IF EXISTS `tool`;
CREATE TABLE `tool` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `toolip` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `tool` (`toolip`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of tool
-- ----------------------------
INSERT INTO `tool` VALUES ('3', '172.28.166.193');
INSERT INTO `tool` VALUES ('5', '192.168.1.175');
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
INSERT INTO `version` VALUES ('2019-12-30 17:10:17', 'chess_beta_1.1.274.1327.tar.gz', 'http://192.168.2.31/upload/chess_beta_1.1.274.1327.tar.gz', '31b5da1fa30651779419f899a11b7db8');
INSERT INTO `version` VALUES ('2019-12-30 20:05:03', 'chess_beta_1.1.274.1330.tar.gz', 'http://192.168.2.31/upload/chess_beta_1.1.274.1330.tar.gz', '904c1a97178cf1e2d67cc0c51b63eed7');
INSERT INTO `version` VALUES ('2019-12-31 18:12:15', 'chess_beta_1.1.275.1330.tar.gz', 'http://192.168.2.31/upload/chess_beta_1.1.275.1330.tar.gz', '9da2ebeb0c2b376f0e280e8e44f75ef4');
INSERT INTO `version` VALUES ('2020-01-06 21:07:42', 'chess_beta_1.1.281.1358.tar.gz', 'http://192.168.2.31/upload/chess_beta_1.1.281.1358.tar.gz', 'bd1e16a199e519c924178e889a45ccab');
INSERT INTO `version` VALUES ('2020-01-07 22:51:17', 'chess_beta_1.1.282.1364.tar.gz', 'http://192.168.2.31/upload/chess_beta_1.1.282.1364.tar.gz', '88ae147bb8eccb60b638bb7c974a450a');
INSERT INTO `version` VALUES ('2020-01-08 13:32:26', 'chess_beta_1.1.283.1366.tar.gz', 'http://192.168.2.31/upload/chess_beta_1.1.283.1366.tar.gz', 'adf9799a901ceed3fe0a315d0b45ca52');
INSERT INTO `version` VALUES ('2020-01-13 15:16:46', 'chess_beta_1.1.288.1388.tar.gz', 'http://192.168.2.31/upload/chess_beta_1.1.288.1388.tar.gz', 'd536a50f10774f0bb05babd7324b052f');
INSERT INTO `version` VALUES ('2020-01-13 17:43:23', 'chess_beta_1.1.288.1389.tar.gz', 'http://192.168.2.31/upload/chess_beta_1.1.288.1389.tar.gz', '07f2fbef78cb430dd8907a2fc6d7f3b0');
INSERT INTO `version` VALUES ('2020-01-14 22:02:06', 'chess_beta_1.1.289.1398.tar.gz', 'http://192.168.2.31/upload/chess_beta_1.1.289.1398.tar.gz', 'a8e9978da5da61afb8c67d1b6124952c');
INSERT INTO `version` VALUES ('2019-12-30 16:26:19', 'chess_config_1.1.274.1320.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.274.1320.tar.gz', '285eead3e59c103d72e54850e05d0429');
INSERT INTO `version` VALUES ('2019-12-30 16:36:31', 'chess_config_1.1.274.1323.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.274.1323.tar.gz', '9ee2693e468e71a646b6710f8e41c99f');
INSERT INTO `version` VALUES ('2019-12-31 08:33:32', 'chess_config_1.1.275.1330.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.275.1330.tar.gz', 'de7f3808dc8ce9fc9c35068fe35b21bd');
INSERT INTO `version` VALUES ('2020-01-01 08:33:33', 'chess_config_1.1.276.1330.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.276.1330.tar.gz', '0eae25e88182b76993d3a8594574f6da');
INSERT INTO `version` VALUES ('2020-01-02 16:15:44', 'chess_config_1.1.277.1330.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.277.1330.tar.gz', 'f2fbf8ba68512552d3e62b71059505c7');
INSERT INTO `version` VALUES ('2020-01-02 16:34:22', 'chess_config_1.1.277.1341.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.277.1341.tar.gz', 'c90df1e3e1aecfe371019eeeb2c07040');
INSERT INTO `version` VALUES ('2020-01-03 08:33:35', 'chess_config_1.1.278.1341.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.278.1341.tar.gz', '3bff1e9227384319c3b61368ab62dfa8');
INSERT INTO `version` VALUES ('2020-01-04 08:33:35', 'chess_config_1.1.279.1351.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.279.1351.tar.gz', '837e2f90271aae07788d6e6684608dea');
INSERT INTO `version` VALUES ('2020-01-05 08:33:30', 'chess_config_1.1.280.1354.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.280.1354.tar.gz', '0a7d6ae8fd6467993f0549c7bd84de39');
INSERT INTO `version` VALUES ('2020-01-06 08:33:32', 'chess_config_1.1.281.1354.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.281.1354.tar.gz', '7bdb936a5bcc8fb1d16c5c7826b5ea16');
INSERT INTO `version` VALUES ('2020-01-07 08:33:31', 'chess_config_1.1.282.1358.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.282.1358.tar.gz', '0236fb307ec1b055b6dff0cbc8d298aa');
INSERT INTO `version` VALUES ('2020-01-07 21:51:10', 'chess_config_1.1.282.1360.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.282.1360.tar.gz', 'b6b5601f3966db83b9ed3535fe03dde2');
INSERT INTO `version` VALUES ('2020-01-07 22:15:45', 'chess_config_1.1.282.1362.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.282.1362.tar.gz', '25fd557b2cf7ce073144ecc182f790fb');
INSERT INTO `version` VALUES ('2020-01-07 22:17:58', 'chess_config_1.1.282.1363.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.282.1363.tar.gz', 'f1b62e069659c3175cf67e7cdf9f9a00');
INSERT INTO `version` VALUES ('2020-01-08 08:33:27', 'chess_config_1.1.283.1364.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.283.1364.tar.gz', '35733c29533716d28981b60bf3864436');
INSERT INTO `version` VALUES ('2020-01-08 13:22:06', 'chess_config_1.1.283.1366.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.283.1366.tar.gz', '778dec86443154d79c6231c0aafb4dd8');
INSERT INTO `version` VALUES ('2020-01-09 08:33:19', 'chess_config_1.1.284.1374.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.284.1374.tar.gz', '91693ed30e059fca001dce41460c84ce');
INSERT INTO `version` VALUES ('2020-01-09 10:34:41', 'chess_config_1.1.284.1375.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.284.1375.tar.gz', 'e82f07f6fc85ac446519b7b552238936');
INSERT INTO `version` VALUES ('2020-01-10 08:33:26', 'chess_config_1.1.285.1385.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.285.1385.tar.gz', '29c33ae154bc02a787341e9d61bf12dd');
INSERT INTO `version` VALUES ('2020-01-11 08:33:21', 'chess_config_1.1.286.1385.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.286.1385.tar.gz', '31524ba8b67f0a2770c0517d8b438f49');
INSERT INTO `version` VALUES ('2020-01-12 08:33:24', 'chess_config_1.1.287.1385.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.287.1385.tar.gz', 'aa70ffabf67b9f4619e108518842a9f0');
INSERT INTO `version` VALUES ('2020-01-13 08:33:27', 'chess_config_1.1.288.1385.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.288.1385.tar.gz', 'b3982cd1b99b83247c688e8c8c125567');
INSERT INTO `version` VALUES ('2020-01-13 22:38:16', 'chess_config_1.1.288.1390.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.288.1390.tar.gz', '2a6374f6fd2753f90cfea2a7a2447607');
INSERT INTO `version` VALUES ('2020-01-14 08:33:25', 'chess_config_1.1.289.1390.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.289.1390.tar.gz', '8df204969c869979468b4489d22fad46');
INSERT INTO `version` VALUES ('2020-01-14 14:21:53', 'chess_config_1.1.289.1392.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.289.1392.tar.gz', '06977e8d1dd734c724b43f5a94edbfd3');
INSERT INTO `version` VALUES ('2020-01-14 21:11:29', 'chess_config_1.1.289.1397.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.289.1397.tar.gz', 'c6b2bda1b137b99e493116c88f72027c');
INSERT INTO `version` VALUES ('2020-01-14 22:24:21', 'chess_config_1.1.289.1399.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.289.1399.tar.gz', 'b7eed1761e9bc035549348f451e98910');
INSERT INTO `version` VALUES ('2020-01-15 08:33:28', 'chess_config_1.1.290.1399.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.290.1399.tar.gz', '08efde9e8ee6e2a02443904af2f4f87d');
INSERT INTO `version` VALUES ('2020-01-15 16:23:27', 'chess_config_1.1.290.1400.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.290.1400.tar.gz', '9eeb8139ced005be64f5c0f0b9df8692');
INSERT INTO `version` VALUES ('2020-01-15 18:03:38', 'chess_config_1.1.290.1405.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.290.1405.tar.gz', 'b1f39b77c2312783a57e37a154e8377c');
INSERT INTO `version` VALUES ('2020-01-16 08:33:31', 'chess_config_1.1.291.1406.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.291.1406.tar.gz', '9483c0ef31b2ed3bff4f0aa8058a1fff');
INSERT INTO `version` VALUES ('2020-01-16 21:53:30', 'chess_config_1.1.291.1415.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.291.1415.tar.gz', 'b85a80d49f5f50616a69c1bedacf99c5');
INSERT INTO `version` VALUES ('2020-01-17 08:33:36', 'chess_config_1.1.292.1418.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.292.1418.tar.gz', '7177efa44733ea4936b934149cc8dd79');
INSERT INTO `version` VALUES ('2020-01-17 17:47:16', 'chess_config_1.1.292.1428.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.292.1428.tar.gz', '88bb8ad77c54ca4c05acee5f9a0abdbd');
INSERT INTO `version` VALUES ('2020-01-18 08:33:35', 'chess_config_1.1.293.1428.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.293.1428.tar.gz', '92db405c2d6e3fc385e3852631e61ce7');
INSERT INTO `version` VALUES ('2020-01-18 11:08:04', 'chess_config_1.1.293.1429.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.293.1429.tar.gz', '7e06274dc7b077be68be1da6f7815452');
INSERT INTO `version` VALUES ('2020-01-18 15:43:45', 'chess_config_1.1.293.1432.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.293.1432.tar.gz', '771e4f58b8d60fe06da31d9809f51aa3');
INSERT INTO `version` VALUES ('2020-01-19 08:33:35', 'chess_config_1.1.294.1432.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.294.1432.tar.gz', '1178652cec18155d8229f29cbf1e0449');
INSERT INTO `version` VALUES ('2020-01-20 08:33:37', 'chess_config_1.1.295.1433.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.295.1433.tar.gz', 'e0fb920d2296bd9137651b078738866a');
INSERT INTO `version` VALUES ('2020-01-21 08:33:35', 'chess_config_1.1.296.1435.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.296.1435.tar.gz', '28b17e92dc9e7684efc12a131f0e06c5');
INSERT INTO `version` VALUES ('2020-01-22 08:33:34', 'chess_config_1.1.297.1435.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.297.1435.tar.gz', 'fb3516e7daa9c06aa8df354a2bdeafda');
INSERT INTO `version` VALUES ('2020-01-23 08:33:35', 'chess_config_1.1.298.1435.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.298.1435.tar.gz', '9e5295fa577d21ffb3061ac55ff401a8');
INSERT INTO `version` VALUES ('2020-01-24 08:33:37', 'chess_config_1.1.299.1435.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.299.1435.tar.gz', '39597e61f7e92d7964764ee19a3771b1');
INSERT INTO `version` VALUES ('2020-01-25 08:33:43', 'chess_config_1.1.300.1435.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.300.1435.tar.gz', '222ddf8431994492a1db8b0cac7f0275');
INSERT INTO `version` VALUES ('2020-01-26 08:33:34', 'chess_config_1.1.301.1435.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.301.1435.tar.gz', 'c747646394a4d43a1dde61a818a78b35');
INSERT INTO `version` VALUES ('2020-01-27 08:33:31', 'chess_config_1.1.302.1435.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.302.1435.tar.gz', 'afa36ba51a6a188f6e72385767c0e51b');
INSERT INTO `version` VALUES ('2020-01-28 08:33:32', 'chess_config_1.1.303.1435.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.303.1435.tar.gz', '91836f58c42787b96b195b47cbbbcc3a');
INSERT INTO `version` VALUES ('2020-01-29 08:33:35', 'chess_config_1.1.304.1435.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.304.1435.tar.gz', 'd79f676271b7de053383e1b1d9c24f3e');
INSERT INTO `version` VALUES ('2020-01-30 08:33:35', 'chess_config_1.1.305.1435.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.305.1435.tar.gz', 'dd9615476b4f2cb8a1bd6a1db2cd2c54');
INSERT INTO `version` VALUES ('2020-01-31 08:33:33', 'chess_config_1.1.306.1435.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.306.1435.tar.gz', '27cbd310c242ed641438aca95ee80c9c');
INSERT INTO `version` VALUES ('2020-02-01 08:33:35', 'chess_config_1.1.307.1435.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.307.1435.tar.gz', '0f515921c11351b7a542bf82a22b8286');
INSERT INTO `version` VALUES ('2020-02-02 08:33:35', 'chess_config_1.1.308.1435.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.308.1435.tar.gz', 'cf35283e4c3dee6f12809fbb6236ae40');
INSERT INTO `version` VALUES ('2020-02-03 15:09:22', 'chess_config_1.1.309.1435.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.309.1435.tar.gz', 'b67c664cc188e142f9596a08ad14fd63');
INSERT INTO `version` VALUES ('2020-02-04 08:33:37', 'chess_config_1.1.310.1436.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.310.1436.tar.gz', 'dcd74d5a0ade8bb5ea6ff08b80d8bb0e');
INSERT INTO `version` VALUES ('2020-02-05 08:33:36', 'chess_config_1.1.311.1440.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.311.1440.tar.gz', '445518112a2a12d060458bd5268341e2');
INSERT INTO `version` VALUES ('2020-02-06 08:33:32', 'chess_config_1.1.312.1442.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.312.1442.tar.gz', 'f28087e9aa2033ff7456d2fd7085e14e');
INSERT INTO `version` VALUES ('2020-02-07 08:33:42', 'chess_config_1.1.313.1442.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.313.1442.tar.gz', '5296688e7d2613ea30dbc00b233ba886');
INSERT INTO `version` VALUES ('2020-02-08 08:33:35', 'chess_config_1.1.314.1443.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.314.1443.tar.gz', '919b946fc128ac6fe097302f7322d7c8');
INSERT INTO `version` VALUES ('2020-02-09 08:33:38', 'chess_config_1.1.315.1443.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.315.1443.tar.gz', '4812df6a26651a151d3e02ffc42bdfdc');
INSERT INTO `version` VALUES ('2020-02-10 08:33:33', 'chess_config_1.1.316.1443.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.316.1443.tar.gz', '63e30c17b56bd68d1641659ab0dc6734');
INSERT INTO `version` VALUES ('2020-02-11 08:33:33', 'chess_config_1.1.317.1444.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.317.1444.tar.gz', '82fc95530796a973d85345576bc4e902');
INSERT INTO `version` VALUES ('2020-02-12 08:33:34', 'chess_config_1.1.318.1448.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.318.1448.tar.gz', '9f595e78e4989531a4ebdbb7237468d6');
INSERT INTO `version` VALUES ('2020-02-12 18:07:59', 'chess_config_1.1.318.1452.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.318.1452.tar.gz', '91125a7320969a681c4addb2eddc0298');
INSERT INTO `version` VALUES ('2020-02-13 08:33:37', 'chess_config_1.1.319.1454.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.319.1454.tar.gz', '0394bdfe93323943934959ca320f22cc');
INSERT INTO `version` VALUES ('2020-02-14 08:33:36', 'chess_config_1.1.320.1455.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.320.1455.tar.gz', 'b5aa55242faa298dedca4e91a31c1e66');
INSERT INTO `version` VALUES ('2020-02-15 08:33:37', 'chess_config_1.1.321.1460.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.321.1460.tar.gz', 'e7ca26dc2dfa2d5a0882a1ee9e211487');
INSERT INTO `version` VALUES ('2020-02-16 08:33:40', 'chess_config_1.1.322.1462.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.322.1462.tar.gz', '1cd611489e1c8fa7584b7e8837cb0886');
INSERT INTO `version` VALUES ('2020-02-17 08:33:38', 'chess_config_1.1.323.1465.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.323.1465.tar.gz', 'ff82d33e66d7364b416edb8dc90a98da');
INSERT INTO `version` VALUES ('2020-02-17 13:48:59', 'chess_config_1.1.323.1468.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.323.1468.tar.gz', '55014f4d1470bf3bf4971426ff7f4ab1');
INSERT INTO `version` VALUES ('2020-02-17 18:01:36', 'chess_config_1.1.323.1470.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.323.1470.tar.gz', '9e442ba8d027d21d41b06940718ea482');
INSERT INTO `version` VALUES ('2020-02-18 13:26:30', 'chess_config_1.1.324.1471.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.324.1471.tar.gz', '8172e8f3161b7192313c90dd610668fe');
INSERT INTO `version` VALUES ('2020-02-18 21:50:05', 'chess_config_1.1.324.1476.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.324.1476.tar.gz', '768143df565ea8bf44d2c5b3d325d01f');
INSERT INTO `version` VALUES ('2020-02-19 08:33:42', 'chess_config_1.1.325.1479.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.325.1479.tar.gz', '9e04395e3ad92ef85b21571e614310cc');
INSERT INTO `version` VALUES ('2020-02-20 08:33:42', 'chess_config_1.1.326.1483.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.326.1483.tar.gz', '942751fef146e41a60d53e88a520b33c');
INSERT INTO `version` VALUES ('2020-02-21 08:33:47', 'chess_config_1.1.327.1487.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.327.1487.tar.gz', '43eccd810683b709d9798b1401c088de');
INSERT INTO `version` VALUES ('2020-02-22 19:53:58', 'chess_config_1.1.328.1492.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.328.1492.tar.gz', '85239623555c5042c505e7e26f3845bb');
INSERT INTO `version` VALUES ('2020-02-22 21:21:14', 'chess_config_1.1.328.1495.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.328.1495.tar.gz', '8bdfce3fb0afbcb8a4d1b78256e16cc2');
INSERT INTO `version` VALUES ('2020-02-23 08:33:49', 'chess_config_1.1.329.1495.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.329.1495.tar.gz', '82ff0edde6c33b34ca66b471304fd97b');
INSERT INTO `version` VALUES ('2020-02-24 08:33:43', 'chess_config_1.1.330.1500.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.330.1500.tar.gz', '40f2a805d2cc54cfa1a768157c30ac8c');
INSERT INTO `version` VALUES ('2020-02-24 16:25:24', 'chess_config_1.1.330.1501.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.330.1501.tar.gz', '2308e020de47606379a669e776c9c3cb');
INSERT INTO `version` VALUES ('2020-02-25 08:33:48', 'chess_config_1.1.331.1507.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.331.1507.tar.gz', 'dcde1a896e35a7c41b95fb59224ede82');
INSERT INTO `version` VALUES ('2020-02-26 08:33:45', 'chess_config_1.1.332.1515.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.332.1515.tar.gz', 'f9b736313d68e9948aae7d98ad600ecb');
INSERT INTO `version` VALUES ('2020-02-26 11:33:49', 'chess_config_1.1.332.1517.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.332.1517.tar.gz', 'bc82eafde85b82767621ed450be93ef8');
INSERT INTO `version` VALUES ('2020-02-27 08:33:46', 'chess_config_1.1.333.1522.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.333.1522.tar.gz', 'a451662bbffc4cc48a8db97500f9b71d');
INSERT INTO `version` VALUES ('2020-02-28 08:33:48', 'chess_config_1.1.334.1533.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.334.1533.tar.gz', '2030a9de209eb42c2e306e721551f9ab');
INSERT INTO `version` VALUES ('2020-02-29 08:33:58', 'chess_config_1.1.335.1545.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.335.1545.tar.gz', 'ef1d7b82055556ab89bc3819bcb7bf11');
INSERT INTO `version` VALUES ('2020-03-01 08:34:00', 'chess_config_1.1.336.1546.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.336.1546.tar.gz', '67df0dc8cbe304b918ea577dcad85991');
INSERT INTO `version` VALUES ('2020-03-02 08:34:01', 'chess_config_1.1.337.1548.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.337.1548.tar.gz', '71380a22710c1dddba2da38267b7f732');
INSERT INTO `version` VALUES ('2020-03-02 18:05:48', 'chess_config_1.1.337.1551.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.337.1551.tar.gz', '123666bd36e2814b3b2d9c50d9dc24a7');
INSERT INTO `version` VALUES ('2020-03-03 08:34:00', 'chess_config_1.1.338.1552.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.338.1552.tar.gz', '1a83ce20124f58fb177e1fb785d18d40');
INSERT INTO `version` VALUES ('2020-03-03 20:30:43', 'chess_config_1.1.338.1554.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.338.1554.tar.gz', '1ea09f7bcb7e794b7f9e5dfc35e8d913');
INSERT INTO `version` VALUES ('2020-03-04 08:34:01', 'chess_config_1.1.339.1554.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.339.1554.tar.gz', '72d2a82e46e6dea333778194c9940384');
INSERT INTO `version` VALUES ('2020-03-05 08:33:59', 'chess_config_1.1.340.1558.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.340.1558.tar.gz', '67a506b834e77abe6fc2877c24141d02');
INSERT INTO `version` VALUES ('2020-03-06 08:34:01', 'chess_config_1.1.341.1563.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.341.1563.tar.gz', 'c29853b4d8bd96fcddcc0367e2575ab8');
INSERT INTO `version` VALUES ('2020-03-07 08:33:57', 'chess_config_1.1.342.1568.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.342.1568.tar.gz', 'a1d0f98abbad9744c534caea2356095a');
INSERT INTO `version` VALUES ('2020-03-08 08:33:55', 'chess_config_1.1.343.1571.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.343.1571.tar.gz', 'fdfa2589e647fc8b3ce665e897939848');
INSERT INTO `version` VALUES ('2020-03-09 08:34:00', 'chess_config_1.1.344.1571.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.344.1571.tar.gz', 'a0a29aa1d8462cb606983952457e3996');
INSERT INTO `version` VALUES ('2020-03-10 08:34:01', 'chess_config_1.1.345.1574.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.345.1574.tar.gz', '2cddc669c3d73fbd9c9bc0ed35602f26');
INSERT INTO `version` VALUES ('2020-03-11 08:33:59', 'chess_config_1.1.346.1574.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.346.1574.tar.gz', '2316d5510a5af1f7aebd90772aa5af60');
INSERT INTO `version` VALUES ('2020-03-12 08:34:01', 'chess_config_1.1.347.1578.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.347.1578.tar.gz', '390fab19bec0928589d4ea5b3e50072f');
INSERT INTO `version` VALUES ('2020-03-13 08:34:05', 'chess_config_1.1.348.1583.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.348.1583.tar.gz', '4e8ad7f2b49a52985b017eee3e3b562b');
INSERT INTO `version` VALUES ('2020-03-14 08:34:08', 'chess_config_1.1.349.1590.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.349.1590.tar.gz', '8e639dbca9fac1f57ddaa4b304239b2e');
INSERT INTO `version` VALUES ('2020-03-15 08:34:11', 'chess_config_1.1.350.1600.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.350.1600.tar.gz', '3eb4497bea6ea2fae74fb1fdd0824686');
INSERT INTO `version` VALUES ('2020-03-16 08:34:06', 'chess_config_1.1.351.1600.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.351.1600.tar.gz', '86cde7793c413bd0ef3d76cbc60a0dff');
INSERT INTO `version` VALUES ('2020-03-17 08:34:12', 'chess_config_1.1.352.1612.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.352.1612.tar.gz', 'ab635ad3d31bd89329a9e5634b7ceae0');
INSERT INTO `version` VALUES ('2020-03-18 08:34:04', 'chess_config_1.1.353.1616.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.353.1616.tar.gz', 'e616df5ef346eb70600aa11a94635903');
INSERT INTO `version` VALUES ('2020-03-18 18:05:33', 'chess_config_1.1.353.1618.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.353.1618.tar.gz', 'b91e0c91f513223e0c07a8697994cb81');
INSERT INTO `version` VALUES ('2020-03-19 08:34:06', 'chess_config_1.1.354.1621.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.354.1621.tar.gz', '1055a848e476abb8d768516707c28116');
INSERT INTO `version` VALUES ('2020-03-20 08:34:11', 'chess_config_1.1.355.1633.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.355.1633.tar.gz', '5162fb08cbf3a2716dac526fdd895e4e');
INSERT INTO `version` VALUES ('2020-03-21 08:34:10', 'chess_config_1.1.356.1642.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.356.1642.tar.gz', '79f2d5631e8258c1afd9f34545ea3a47');
INSERT INTO `version` VALUES ('2020-03-22 08:34:05', 'chess_config_1.1.357.1644.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.357.1644.tar.gz', '03fd62c4c99465f99894503a72a4f2f9');
INSERT INTO `version` VALUES ('2020-03-23 08:34:23', 'chess_config_1.1.358.1644.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.358.1644.tar.gz', '30b783536faba4925ce6cfdaf623e59e');
INSERT INTO `version` VALUES ('2020-03-24 08:34:24', 'chess_config_1.1.359.1657.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.359.1657.tar.gz', 'f570e39b1737b387dabb3f0652f66b8e');
INSERT INTO `version` VALUES ('2020-03-25 08:34:11', 'chess_config_1.1.360.1663.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.360.1663.tar.gz', 'a80da6e096d12a34de64464d2f674d91');
INSERT INTO `version` VALUES ('2020-03-26 08:34:22', 'chess_config_1.1.361.1679.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.361.1679.tar.gz', 'be5d50a2885c7bcc2e98c2d9664c6422');
INSERT INTO `version` VALUES ('2020-03-27 08:34:16', 'chess_config_1.1.362.1690.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.362.1690.tar.gz', '7c72fadbe00e4d599a96c3d657c5d31e');
INSERT INTO `version` VALUES ('2020-03-27 19:38:04', 'chess_config_1.1.362.1696.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.362.1696.tar.gz', '0b616d1812c9ef70fc90ddce742016e2');
INSERT INTO `version` VALUES ('2020-03-27 20:25:08', 'chess_config_1.1.362.1697.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.362.1697.tar.gz', '52542bc3c896e66857f14301f389b198');
INSERT INTO `version` VALUES ('2020-03-28 08:34:21', 'chess_config_1.1.363.1700.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.363.1700.tar.gz', '2c2691e55b0b0b22caa2f0fa6afe54c5');
INSERT INTO `version` VALUES ('2020-03-28 18:16:06', 'chess_config_1.1.363.1707.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.363.1707.tar.gz', '9f1bde8e6d394778c424d5e21673b796');
INSERT INTO `version` VALUES ('2020-03-28 18:42:03', 'chess_config_1.1.363.1708.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.363.1708.tar.gz', '8298c747f09e77c8165741bc7fd9db15');
INSERT INTO `version` VALUES ('2020-03-29 08:34:19', 'chess_config_1.1.364.1709.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.364.1709.tar.gz', 'fb7eeb3048f2b5e46fa7e3e39c8d0ac0');
INSERT INTO `version` VALUES ('2020-03-30 08:34:22', 'chess_config_1.1.365.1709.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.365.1709.tar.gz', '96599952ebe02415aaeb9af6fcfeb7bb');
INSERT INTO `version` VALUES ('2020-03-30 16:48:24', 'chess_config_1.1.365.1715.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.365.1715.tar.gz', 'c194dc46e4b14218b3547077046e660d');
INSERT INTO `version` VALUES ('2020-03-30 22:15:16', 'chess_config_1.1.365.1719.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.365.1719.tar.gz', '06c2894f306c2837702797a875444d2e');
INSERT INTO `version` VALUES ('2020-03-31 08:34:22', 'chess_config_1.1.366.1719.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.366.1719.tar.gz', '262a7cf6b7090a448c142ba8592bc860');
INSERT INTO `version` VALUES ('2020-04-01 08:34:29', 'chess_config_1.1.367.1724.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.367.1724.tar.gz', '14178b822e2048d1def6425c15ba31f8');
INSERT INTO `version` VALUES ('2020-04-02 08:34:25', 'chess_config_1.1.368.1733.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.368.1733.tar.gz', 'a88277d5282986fdd85da45746cdb8b5');
INSERT INTO `version` VALUES ('2020-04-03 08:34:24', 'chess_config_1.1.369.1736.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.369.1736.tar.gz', '38d2a948aaa3275e4c5567c880fe08aa');
INSERT INTO `version` VALUES ('2020-04-04 08:34:27', 'chess_config_1.1.370.1737.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.370.1737.tar.gz', '64da121e92bf826ed2be94df688b706f');
INSERT INTO `version` VALUES ('2020-04-05 08:34:23', 'chess_config_1.1.371.1739.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.371.1739.tar.gz', '65a9d7a550d8cc6b7663b403c7de0309');
INSERT INTO `version` VALUES ('2020-04-06 08:34:23', 'chess_config_1.1.372.1739.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.372.1739.tar.gz', 'e344bca405e3bae881551397067f5df4');
INSERT INTO `version` VALUES ('2020-04-07 08:34:23', 'chess_config_1.1.373.1739.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.373.1739.tar.gz', '73d65212140edd4abcc55d7d9ed95887');
INSERT INTO `version` VALUES ('2020-04-08 08:34:22', 'chess_config_1.1.374.1745.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.374.1745.tar.gz', '344c6d53d9129b35842ab15c3ca4e117');
INSERT INTO `version` VALUES ('2020-04-09 08:34:26', 'chess_config_1.1.375.1756.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.375.1756.tar.gz', '001b21a03bef076810d881166d3e77d4');
INSERT INTO `version` VALUES ('2020-04-10 08:34:25', 'chess_config_1.1.376.1761.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.376.1761.tar.gz', 'b994fd9867f3899367ca6a5fbd44aacf');
INSERT INTO `version` VALUES ('2020-04-11 08:34:26', 'chess_config_1.1.377.1777.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.377.1777.tar.gz', '1315c3120dead87e7cddf07b51d20f9a');
INSERT INTO `version` VALUES ('2020-04-12 08:34:27', 'chess_config_1.1.378.1780.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.378.1780.tar.gz', '02d38dab054f0fccab918b33e76f5c26');
INSERT INTO `version` VALUES ('2020-04-13 08:34:29', 'chess_config_1.1.379.1780.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.379.1780.tar.gz', '429e53b66fc62652edbee6841923d088');
INSERT INTO `version` VALUES ('2020-04-14 08:34:27', 'chess_config_1.1.380.1782.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.380.1782.tar.gz', '5eb38d5df4c969d1a6cab4b8ba312bbe');
INSERT INTO `version` VALUES ('2020-04-15 08:34:37', 'chess_config_1.1.381.1789.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.381.1789.tar.gz', '4fd0077f281787e6ae086f7d04a6269d');
INSERT INTO `version` VALUES ('2020-04-16 08:34:52', 'chess_config_1.1.382.1796.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.382.1796.tar.gz', '0099291cb6a3acce22a2af14760e44f0');
INSERT INTO `version` VALUES ('2020-04-17 08:34:45', 'chess_config_1.1.383.1813.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.383.1813.tar.gz', '6eb2961fdf1da78c622b20850db38289');
INSERT INTO `version` VALUES ('2020-04-18 08:34:36', 'chess_config_1.1.384.1828.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.384.1828.tar.gz', '7107548c639ce105ffeb33d22597b2c1');
INSERT INTO `version` VALUES ('2020-04-19 08:34:33', 'chess_config_1.1.385.1828.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.385.1828.tar.gz', 'a9c83d3f805eee26649a61e47a0d6c1e');
INSERT INTO `version` VALUES ('2020-04-20 08:34:34', 'chess_config_1.1.386.1828.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.386.1828.tar.gz', '9274cffe719d278baf21bce8ba89628e');
INSERT INTO `version` VALUES ('2020-04-20 18:10:46', 'chess_config_1.1.386.1831.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.386.1831.tar.gz', '0f9bf74dba4d81b7e8ee5171da5c44bc');
INSERT INTO `version` VALUES ('2020-04-21 08:34:36', 'chess_config_1.1.387.1834.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.387.1834.tar.gz', '4a2d41a707e33612268e4056e248ae83');
INSERT INTO `version` VALUES ('2020-04-22 08:34:40', 'chess_config_1.1.388.1843.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.388.1843.tar.gz', '62d64342373f1468bdf4c41c9b3e7530');
INSERT INTO `version` VALUES ('2020-04-23 08:34:40', 'chess_config_1.1.389.1864.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.389.1864.tar.gz', 'ef6d213f6a8fc290f3aa3e6955390860');
INSERT INTO `version` VALUES ('2020-04-24 13:17:42', 'chess_config_1.1.390.1871.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.390.1871.tar.gz', '0d004171a85a0b19eade1eee2d6f8993');
INSERT INTO `version` VALUES ('2020-04-25 08:34:43', 'chess_config_1.1.391.1876.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.391.1876.tar.gz', '5810201fdd0965ce9b6144b907c5efc9');
INSERT INTO `version` VALUES ('2020-04-26 08:34:49', 'chess_config_1.1.392.1878.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.392.1878.tar.gz', '28a6b745c7eca2715e249dcb6dc88ad8');
INSERT INTO `version` VALUES ('2020-04-26 16:21:36', 'chess_config_1.1.392.1881.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.392.1881.tar.gz', 'de363fdb3e3d7ad8bdff5182a044407d');
INSERT INTO `version` VALUES ('2020-04-27 08:34:57', 'chess_config_1.1.393.1886.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.393.1886.tar.gz', '9ddad398cfff3a546a7138d5c8b8a952');
INSERT INTO `version` VALUES ('2020-04-28 08:34:42', 'chess_config_1.1.394.1892.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.394.1892.tar.gz', 'b797f4d69017e9b539c4a57cdf9b0db0');
INSERT INTO `version` VALUES ('2019-12-30 16:09:40', 'chess_develop_1.1.274.1319.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.274.1319.tar.gz', 'a4dfc861df6eebe81f61ba5794b2ed8a');
INSERT INTO `version` VALUES ('2019-12-30 17:16:53', 'chess_develop_1.1.274.1327.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.274.1327.tar.gz', '2871acf73885730df39fa3cb2c11464b');
INSERT INTO `version` VALUES ('2019-12-30 17:26:15', 'chess_develop_1.1.274.1328.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.274.1328.tar.gz', 'b80bbe0cb947266843982bd4d4140afa');
INSERT INTO `version` VALUES ('2019-12-30 17:34:51', 'chess_develop_1.1.274.1329.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.274.1329.tar.gz', 'aa909a8f24be384ac0e9a7a116a3919c');
INSERT INTO `version` VALUES ('2019-12-31 08:03:44', 'chess_develop_1.1.275.1330.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.275.1330.tar.gz', 'be25bda38b78c142f42b821f109214c7');
INSERT INTO `version` VALUES ('2020-01-01 08:03:41', 'chess_develop_1.1.276.1330.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.276.1330.tar.gz', 'e63d9d1c44382b38c464d32e9172e22a');
INSERT INTO `version` VALUES ('2020-01-02 16:19:54', 'chess_develop_1.1.277.1330.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.277.1330.tar.gz', 'da8d0c9023f5d65cad2c7d2dc7722c99');
INSERT INTO `version` VALUES ('2020-01-02 16:29:53', 'chess_develop_1.1.277.1341.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.277.1341.tar.gz', '908218fcaeca8973615a67cc9424bb6f');
INSERT INTO `version` VALUES ('2020-01-03 08:03:37', 'chess_develop_1.1.278.1341.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.278.1341.tar.gz', '6c180404ce2c6c1724750290cec52a35');
INSERT INTO `version` VALUES ('2020-01-04 08:03:38', 'chess_develop_1.1.279.1351.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.279.1351.tar.gz', '59cf01fd0472f7e50cf36b5b285ce454');
INSERT INTO `version` VALUES ('2020-01-05 08:03:37', 'chess_develop_1.1.280.1354.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.280.1354.tar.gz', '3f79be3f38bba961957c55ea5c499626');
INSERT INTO `version` VALUES ('2020-01-06 08:03:35', 'chess_develop_1.1.281.1354.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.281.1354.tar.gz', 'ddacd98154b5acde11828e0a5c73de56');
INSERT INTO `version` VALUES ('2020-01-07 08:04:01', 'chess_develop_1.1.282.1358.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.282.1358.tar.gz', '90e0cad48d48526e229c30f4fc475ca7');
INSERT INTO `version` VALUES ('2020-01-07 22:30:08', 'chess_develop_1.1.282.1363.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.282.1363.tar.gz', '9dad663dff51b0deac2c691d3daf8858');
INSERT INTO `version` VALUES ('2020-01-08 08:03:46', 'chess_develop_1.1.283.1364.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.283.1364.tar.gz', '2f2e504dc52a54157a58e870b17d8a7d');
INSERT INTO `version` VALUES ('2020-01-08 13:14:41', 'chess_develop_1.1.283.1366.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.283.1366.tar.gz', '2ce05c5ab33aed25ed13f8dabcfcca6e');
INSERT INTO `version` VALUES ('2020-01-08 14:42:26', 'chess_develop_1.1.283.1367.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.283.1367.tar.gz', '22c9650e218a082eeb23917922447ee1');
INSERT INTO `version` VALUES ('2020-01-08 16:03:51', 'chess_develop_1.1.283.1369.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.283.1369.tar.gz', '06e5b33da5575f14e92a810e76313aab');
INSERT INTO `version` VALUES ('2020-01-09 08:03:20', 'chess_develop_1.1.284.1374.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.284.1374.tar.gz', '5e007f10fb467813f084ec2308dc06df');
INSERT INTO `version` VALUES ('2020-01-09 10:29:44', 'chess_develop_1.1.284.1375.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.284.1375.tar.gz', '64c21d7dc487a22ea613a23154664ea9');
INSERT INTO `version` VALUES ('2020-01-09 16:52:32', 'chess_develop_1.1.284.1377.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.284.1377.tar.gz', 'a06ffd2944087a6c8d725a3555248f2f');
INSERT INTO `version` VALUES ('2020-01-10 08:03:31', 'chess_develop_1.1.285.1385.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.285.1385.tar.gz', 'c76a9992bc34a784ec432763b87059c9');
INSERT INTO `version` VALUES ('2020-01-11 08:03:30', 'chess_develop_1.1.286.1385.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.286.1385.tar.gz', '5ba9e2157f78037f0d26302e80af4b4c');
INSERT INTO `version` VALUES ('2020-01-12 08:03:29', 'chess_develop_1.1.287.1385.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.287.1385.tar.gz', 'c529025187533c1d3fbba3a41c512791');
INSERT INTO `version` VALUES ('2020-01-13 08:03:28', 'chess_develop_1.1.288.1385.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.288.1385.tar.gz', 'a8a2170cc94d74a27a1be90a55b5114f');
INSERT INTO `version` VALUES ('2020-01-13 22:33:13', 'chess_develop_1.1.288.1390.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.288.1390.tar.gz', 'a79b6ccf0fc92beb5a6ac1f2e23ada31');
INSERT INTO `version` VALUES ('2020-01-14 08:03:30', 'chess_develop_1.1.289.1390.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.289.1390.tar.gz', 'd65e2c9b301354d890e94abf1b175573');
INSERT INTO `version` VALUES ('2020-01-14 14:17:26', 'chess_develop_1.1.289.1392.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.289.1392.tar.gz', 'c3c6ec08829e346b2ff143d3e36cdca7');
INSERT INTO `version` VALUES ('2020-01-14 21:07:24', 'chess_develop_1.1.289.1397.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.289.1397.tar.gz', '0d4a307ca7339cc287042b1c6404e0b0');
INSERT INTO `version` VALUES ('2020-01-14 22:19:02', 'chess_develop_1.1.289.1399.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.289.1399.tar.gz', '26547471cd423b65bef986dfd5cd7ebb');
INSERT INTO `version` VALUES ('2020-01-15 08:03:31', 'chess_develop_1.1.290.1399.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.290.1399.tar.gz', '7c19c9ab3f4e57d7b71d869086c42b5e');
INSERT INTO `version` VALUES ('2020-01-15 16:18:17', 'chess_develop_1.1.290.1400.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.290.1400.tar.gz', 'e379bdb98d275cf6472eeaf33bfdeb06');
INSERT INTO `version` VALUES ('2020-01-15 16:45:39', 'chess_develop_1.1.290.1401.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.290.1401.tar.gz', '7f9ccb9c4f69fb31431fcb2a2f4e97c9');
INSERT INTO `version` VALUES ('2020-01-15 17:59:57', 'chess_develop_1.1.290.1405.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.290.1405.tar.gz', '0f835fd4a0a370704cb846840b328250');
INSERT INTO `version` VALUES ('2020-01-16 08:03:31', 'chess_develop_1.1.291.1406.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.291.1406.tar.gz', 'd3acb09f8c01566fd619bb53b6f2662a');
INSERT INTO `version` VALUES ('2020-01-16 21:08:15', 'chess_develop_1.1.291.1411.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.291.1411.tar.gz', '251870e0804bd207089ec79fac9cd2b4');
INSERT INTO `version` VALUES ('2020-01-16 21:41:17', 'chess_develop_1.1.291.1414.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.291.1414.tar.gz', 'a750af645726e37cf539b6e6c52038a0');
INSERT INTO `version` VALUES ('2020-01-16 21:45:44', 'chess_develop_1.1.291.1415.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.291.1415.tar.gz', 'b5c9c6d073c0bc171e48d440c325de48');
INSERT INTO `version` VALUES ('2020-01-17 08:03:39', 'chess_develop_1.1.292.1418.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.292.1418.tar.gz', '0ede2ecca5da4f373e56878897742a82');
INSERT INTO `version` VALUES ('2020-01-17 12:02:36', 'chess_develop_1.1.292.1419.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.292.1419.tar.gz', '45fe334fa4df76584b2166af232483f5');
INSERT INTO `version` VALUES ('2020-01-17 13:23:54', 'chess_develop_1.1.292.1421.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.292.1421.tar.gz', '22545d1de75432f1b39805facd696d42');
INSERT INTO `version` VALUES ('2020-01-17 16:46:45', 'chess_develop_1.1.292.1426.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.292.1426.tar.gz', '4a88b61b59378722cebce6c43499a6e7');
INSERT INTO `version` VALUES ('2020-01-17 16:57:13', 'chess_develop_1.1.292.1427.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.292.1427.tar.gz', '3fb5333614054da63a8215d594800051');
INSERT INTO `version` VALUES ('2020-01-17 17:06:14', 'chess_develop_1.1.292.1428.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.292.1428.tar.gz', 'c1c6b47c077e61924070bf1d3eebfbd7');
INSERT INTO `version` VALUES ('2020-01-18 08:03:41', 'chess_develop_1.1.293.1428.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.293.1428.tar.gz', 'a9a5171218c05b2ccf0b644e3ee9921f');
INSERT INTO `version` VALUES ('2020-01-18 11:04:40', 'chess_develop_1.1.293.1429.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.293.1429.tar.gz', 'abcd5857364e2a0279d882c704ecbc44');
INSERT INTO `version` VALUES ('2020-01-18 15:35:50', 'chess_develop_1.1.293.1432.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.293.1432.tar.gz', '36cc04792b818f181a5981be0e5aaba5');
INSERT INTO `version` VALUES ('2020-01-19 08:03:42', 'chess_develop_1.1.294.1432.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.294.1432.tar.gz', '5f674e82daef104268c54903a919d77c');
INSERT INTO `version` VALUES ('2020-01-20 08:03:39', 'chess_develop_1.1.295.1433.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.295.1433.tar.gz', 'bf7c7cd41d140d9038d9c10de0607a31');
INSERT INTO `version` VALUES ('2020-01-20 16:21:32', 'chess_develop_1.1.295.1434.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.295.1434.tar.gz', 'a43a0bb8c2105fc414213cc4f238dd3f');
INSERT INTO `version` VALUES ('2020-01-20 18:03:26', 'chess_develop_1.1.295.1435.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.295.1435.tar.gz', 'bbcfc2f8051a8106c44a89d6429e6908');
INSERT INTO `version` VALUES ('2020-01-21 08:03:41', 'chess_develop_1.1.296.1435.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.296.1435.tar.gz', '688bad2a8b075cf5bf8a1b45e20031cf');
INSERT INTO `version` VALUES ('2020-01-22 08:03:39', 'chess_develop_1.1.297.1435.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.297.1435.tar.gz', '2813ea39b1572972fca319a651aae19c');
INSERT INTO `version` VALUES ('2020-01-23 08:03:39', 'chess_develop_1.1.298.1435.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.298.1435.tar.gz', '26c7837f33b11cfd8ea9193ee4db04ad');
INSERT INTO `version` VALUES ('2020-01-24 08:03:44', 'chess_develop_1.1.299.1435.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.299.1435.tar.gz', '54614f04fb3f0dbf0c04bb5bfcc2ebea');
INSERT INTO `version` VALUES ('2020-01-25 08:03:38', 'chess_develop_1.1.300.1435.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.300.1435.tar.gz', '7c5816ec55954e5f3457ff0a7111b796');
INSERT INTO `version` VALUES ('2020-01-26 08:03:41', 'chess_develop_1.1.301.1435.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.301.1435.tar.gz', 'e7239c082db4debd91cd8685735cb82c');
INSERT INTO `version` VALUES ('2020-01-27 08:03:42', 'chess_develop_1.1.302.1435.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.302.1435.tar.gz', 'fc7897345587339ac7c0b1bb0df088bc');
INSERT INTO `version` VALUES ('2020-01-28 08:03:39', 'chess_develop_1.1.303.1435.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.303.1435.tar.gz', '0d104c5950a07469f1559075f5bf5d69');
INSERT INTO `version` VALUES ('2020-01-29 08:03:37', 'chess_develop_1.1.304.1435.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.304.1435.tar.gz', 'fd5f7ad08a869fd570fd7064bbf85696');
INSERT INTO `version` VALUES ('2020-01-30 08:03:40', 'chess_develop_1.1.305.1435.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.305.1435.tar.gz', '9e291f5221f0c7674dfafc46c1f14861');
INSERT INTO `version` VALUES ('2020-01-31 08:03:40', 'chess_develop_1.1.306.1435.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.306.1435.tar.gz', '6d3e217d88dbed54df9e581d28ca9ee4');
INSERT INTO `version` VALUES ('2020-02-01 08:03:40', 'chess_develop_1.1.307.1435.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.307.1435.tar.gz', 'fd5af1d1d123355c4b171f1e5ae779a9');
INSERT INTO `version` VALUES ('2020-02-02 08:03:40', 'chess_develop_1.1.308.1435.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.308.1435.tar.gz', '104d1d7168d81449e482507c121cc0d8');
INSERT INTO `version` VALUES ('2020-02-03 08:03:42', 'chess_develop_1.1.309.1435.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.309.1435.tar.gz', '404924cdb6410d1ba3c416f1fb205e58');
INSERT INTO `version` VALUES ('2020-02-04 08:03:43', 'chess_develop_1.1.310.1436.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.310.1436.tar.gz', 'df16ab8bcec2b8263ae3c1958f8fb42d');
INSERT INTO `version` VALUES ('2020-02-04 17:12:44', 'chess_develop_1.1.310.1438.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.310.1438.tar.gz', '0596d62dff19caff8221a42678aa4d6a');
INSERT INTO `version` VALUES ('2020-02-05 08:03:40', 'chess_develop_1.1.311.1440.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.311.1440.tar.gz', 'd92f19ace719db7dff0b75a34a873551');
INSERT INTO `version` VALUES ('2020-02-05 15:33:33', 'chess_develop_1.1.311.1441.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.311.1441.tar.gz', 'bbf54dbf0d4294596d080f731c378a7f');
INSERT INTO `version` VALUES ('2020-02-05 17:01:01', 'chess_develop_1.1.311.1442.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.311.1442.tar.gz', '0697039fe5e983c6700e1c94c2c69667');
INSERT INTO `version` VALUES ('2020-02-06 08:03:45', 'chess_develop_1.1.312.1442.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.312.1442.tar.gz', '38a12190efa51cd09f58b3f83a3ede7a');
INSERT INTO `version` VALUES ('2020-02-07 08:03:41', 'chess_develop_1.1.313.1442.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.313.1442.tar.gz', '49705fd2bfcfe953ca6fa183988d38b6');
INSERT INTO `version` VALUES ('2020-02-07 15:42:14', 'chess_develop_1.1.313.1443.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.313.1443.tar.gz', '168363832dab57ccc41defed0d3cee80');
INSERT INTO `version` VALUES ('2020-02-08 08:03:40', 'chess_develop_1.1.314.1443.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.314.1443.tar.gz', '5ba313f3c14f7a78b46f98937ea713dd');
INSERT INTO `version` VALUES ('2020-02-09 08:03:41', 'chess_develop_1.1.315.1443.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.315.1443.tar.gz', 'ef2542bedb2c9842d843e018776c1db2');
INSERT INTO `version` VALUES ('2020-02-10 08:03:45', 'chess_develop_1.1.316.1443.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.316.1443.tar.gz', 'abf8a7fcdf5678f6ad95a7612d19aad6');
INSERT INTO `version` VALUES ('2020-02-11 08:03:40', 'chess_develop_1.1.317.1444.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.317.1444.tar.gz', '9fd52a7ee96074c02af4caf9219a1148');
INSERT INTO `version` VALUES ('2020-02-11 15:58:16', 'chess_develop_1.1.317.1445.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.317.1445.tar.gz', '7718cfb220fc1cbc12932be85e219949');
INSERT INTO `version` VALUES ('2020-02-11 19:38:25', 'chess_develop_1.1.317.1446.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.317.1446.tar.gz', 'aabd2625e7d4aee5fd2ce0e8337ba357');
INSERT INTO `version` VALUES ('2020-02-12 08:03:43', 'chess_develop_1.1.318.1448.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.318.1448.tar.gz', '435960561fdf84acfedbde93ed39e508');
INSERT INTO `version` VALUES ('2020-02-12 16:22:10', 'chess_develop_1.1.318.1449.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.318.1449.tar.gz', 'decaee0753da3f64a9ca360b62660375');
INSERT INTO `version` VALUES ('2020-02-12 17:13:45', 'chess_develop_1.1.318.1450.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.318.1450.tar.gz', '78d2aabd0b09a5f9aa6736e4dbed640a');
INSERT INTO `version` VALUES ('2020-02-12 18:32:05', 'chess_develop_1.1.318.1453.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.318.1453.tar.gz', '3fe2ed3f9058e7bf5c2d378dae8216bb');
INSERT INTO `version` VALUES ('2020-02-13 08:03:41', 'chess_develop_1.1.319.1454.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.319.1454.tar.gz', '4286855d20d255a7c905a6bc5db712b3');
INSERT INTO `version` VALUES ('2020-02-13 17:32:41', 'chess_develop_1.1.319.1455.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.319.1455.tar.gz', '4c6ac0a4e81bca747009fe4291be5c12');
INSERT INTO `version` VALUES ('2020-02-14 08:03:43', 'chess_develop_1.1.320.1455.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.320.1455.tar.gz', 'a580ee08e1e52fc891c999e5247fdf8c');
INSERT INTO `version` VALUES ('2020-02-14 12:03:58', 'chess_develop_1.1.320.1456.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.320.1456.tar.gz', '629dfbbb22d318debf5dd4e83ed9f560');
INSERT INTO `version` VALUES ('2020-02-14 18:56:52', 'chess_develop_1.1.320.1457.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.320.1457.tar.gz', 'a8d89c837a77b63b95f61e80b4dc2bcb');
INSERT INTO `version` VALUES ('2020-02-15 08:03:40', 'chess_develop_1.1.321.1460.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.321.1460.tar.gz', '0078200d3ef7f7288dc88e0750d6065f');
INSERT INTO `version` VALUES ('2020-02-15 12:06:28', 'chess_develop_1.1.321.1461.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.321.1461.tar.gz', '449a2636ece38d32afd1816cc5fc2ff8');
INSERT INTO `version` VALUES ('2020-02-15 13:42:38', 'chess_develop_1.1.321.1462.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.321.1462.tar.gz', 'f90d7a952e8c598f71bce11b81ac313f');
INSERT INTO `version` VALUES ('2020-02-16 08:03:42', 'chess_develop_1.1.322.1462.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.322.1462.tar.gz', '68b380a4075f7bc17c78b58d98987b07');
INSERT INTO `version` VALUES ('2020-02-16 13:51:12', 'chess_develop_1.1.322.1464.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.322.1464.tar.gz', '0b8878a48ce7a3783b1009bdc1ec8ecf');
INSERT INTO `version` VALUES ('2020-02-17 08:03:55', 'chess_develop_1.1.323.1465.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.323.1465.tar.gz', 'f5f9661076433137dcd0d244fc5db290');
INSERT INTO `version` VALUES ('2020-02-17 11:05:42', 'chess_develop_1.1.323.1467.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.323.1467.tar.gz', '274ba3f8d5ff2d2620ec0e598a497932');
INSERT INTO `version` VALUES ('2020-02-17 12:16:08', 'chess_develop_1.1.323.1468.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.323.1468.tar.gz', 'fea8f0e02a20d652da3b41d591c78f64');
INSERT INTO `version` VALUES ('2020-02-17 18:07:51', 'chess_develop_1.1.323.1470.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.323.1470.tar.gz', '2f7503e11bf8f0cf08acba3a09ebed67');
INSERT INTO `version` VALUES ('2020-02-18 15:45:06', 'chess_develop_1.1.324.1471.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.324.1471.tar.gz', 'eab2b2554465959c23be058786b19367');
INSERT INTO `version` VALUES ('2020-02-18 17:34:52', 'chess_develop_1.1.324.1473.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.324.1473.tar.gz', 'a75123c791d57f2605510fce50b402db');
INSERT INTO `version` VALUES ('2020-02-18 21:32:33', 'chess_develop_1.1.324.1476.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.324.1476.tar.gz', '8721317b5babf96d264dc5cfb21c1b0b');
INSERT INTO `version` VALUES ('2020-02-18 22:30:30', 'chess_develop_1.1.324.1477.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.324.1477.tar.gz', 'f1efe942f5f8ac0acbf05758df33d2d0');
INSERT INTO `version` VALUES ('2020-02-19 08:03:48', 'chess_develop_1.1.325.1479.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.325.1479.tar.gz', '31b0696883e645eb8909b0cd3fb8d879');
INSERT INTO `version` VALUES ('2020-02-20 08:03:48', 'chess_develop_1.1.326.1483.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.326.1483.tar.gz', 'd4ecb81c2936baad667081f449206679');
INSERT INTO `version` VALUES ('2020-02-20 15:00:35', 'chess_develop_1.1.326.1486.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.326.1486.tar.gz', 'ffa37706c92ddacab21214df72fa2743');
INSERT INTO `version` VALUES ('2020-02-21 08:03:50', 'chess_develop_1.1.327.1487.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.327.1487.tar.gz', '22a17a08bae03230cdac8d5ecc20bc12');
INSERT INTO `version` VALUES ('2020-02-21 17:11:18', 'chess_develop_1.1.327.1489.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.327.1489.tar.gz', '98f88e3cffcb373c17273bb5eb3c688a');
INSERT INTO `version` VALUES ('2020-02-21 18:30:20', 'chess_develop_1.1.327.1492.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.327.1492.tar.gz', '4ba9c00a02bf9eaca689ba51a33e3d89');
INSERT INTO `version` VALUES ('2020-02-22 19:49:24', 'chess_develop_1.1.328.1492.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.328.1492.tar.gz', 'cc44952a116d6363c274467e306a695e');
INSERT INTO `version` VALUES ('2020-02-22 20:20:42', 'chess_develop_1.1.328.1495.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.328.1495.tar.gz', '1536a97a0240dd1a744ee3457f600385');
INSERT INTO `version` VALUES ('2020-02-23 08:03:48', 'chess_develop_1.1.329.1495.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.329.1495.tar.gz', '61f8632b4e2eb06a229fda29faecf082');
INSERT INTO `version` VALUES ('2020-02-23 11:53:26', 'chess_develop_1.1.329.1496.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.329.1496.tar.gz', 'beca7316bcce668738d351f76f01bf63');
INSERT INTO `version` VALUES ('2020-02-23 14:36:47', 'chess_develop_1.1.329.1497.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.329.1497.tar.gz', '4497c31eb6dda1e75c601f98c632fcdc');
INSERT INTO `version` VALUES ('2020-02-23 15:22:57', 'chess_develop_1.1.329.1498.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.329.1498.tar.gz', 'ecec3d71af05aba5760fbaa3769c427c');
INSERT INTO `version` VALUES ('2020-02-24 08:03:52', 'chess_develop_1.1.330.1500.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.330.1500.tar.gz', 'a213c7896d4f2272e85cea33d4c1c9c7');
INSERT INTO `version` VALUES ('2020-02-24 15:41:56', 'chess_develop_1.1.330.1501.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.330.1501.tar.gz', 'd910f686496809b3ddbfc5b6eb78f7a4');
INSERT INTO `version` VALUES ('2020-02-24 21:04:50', 'chess_develop_1.1.330.1504.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.330.1504.tar.gz', '7315e7b866ba7f90dd489278c41b9f65');
INSERT INTO `version` VALUES ('2020-02-25 08:03:52', 'chess_develop_1.1.331.1507.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.331.1507.tar.gz', '94afeda14a07a32a3196ef1ab6c77d89');
INSERT INTO `version` VALUES ('2020-02-25 22:48:54', 'chess_develop_1.1.331.1515.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.331.1515.tar.gz', 'a174f164649271eb917e6c3c808b94be');
INSERT INTO `version` VALUES ('2020-02-26 08:03:55', 'chess_develop_1.1.332.1515.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.332.1515.tar.gz', '587604289195bbd16a53a7a0dd02d60b');
INSERT INTO `version` VALUES ('2020-02-26 11:26:30', 'chess_develop_1.1.332.1517.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.332.1517.tar.gz', 'd72eab80aaaad970c5b329187238bd2d');
INSERT INTO `version` VALUES ('2020-02-26 12:26:10', 'chess_develop_1.1.332.1518.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.332.1518.tar.gz', 'cc7d587ac08c429e94decfdffdd307c5');
INSERT INTO `version` VALUES ('2020-02-27 08:03:58', 'chess_develop_1.1.333.1522.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.333.1522.tar.gz', '38cb51406e74e602d7f1b740ee2d35a3');
INSERT INTO `version` VALUES ('2020-02-28 08:03:55', 'chess_develop_1.1.334.1533.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.334.1533.tar.gz', '9290789189084cf3e9ba1f0b216a38e2');
INSERT INTO `version` VALUES ('2020-02-28 11:44:46', 'chess_develop_1.1.334.1537.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.334.1537.tar.gz', 'fcdd14dbb78e6b678ab91fa6ec9a5667');
INSERT INTO `version` VALUES ('2020-02-28 15:59:41', 'chess_develop_1.1.334.1538.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.334.1538.tar.gz', '31beeac3850801a03ef78d19c4362554');
INSERT INTO `version` VALUES ('2020-02-29 15:07:38', 'chess_develop_1.1.335.1545.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.335.1545.tar.gz', '00c99ca7226da1772544adbc79978eeb');
INSERT INTO `version` VALUES ('2020-02-29 18:35:04', 'chess_develop_1.1.335.1546.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.335.1546.tar.gz', '5c9a6a87d96c59b5f7c1e2b26625ee5f');
INSERT INTO `version` VALUES ('2020-03-01 21:16:54', 'chess_develop_1.1.336.1546.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.336.1546.tar.gz', '0092ecc2a51633434a3f74322b59e5ec');
INSERT INTO `version` VALUES ('2020-03-01 23:20:58', 'chess_develop_1.1.336.1547.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.336.1547.tar.gz', 'dd48b1638beaf1d4242117702f418cf8');
INSERT INTO `version` VALUES ('2020-03-01 23:26:01', 'chess_develop_1.1.336.1548.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.336.1548.tar.gz', '6c7deacf6189f6c986f5d894e9161c36');
INSERT INTO `version` VALUES ('2020-03-02 08:03:56', 'chess_develop_1.1.337.1548.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.337.1548.tar.gz', '558f76699a9c471908a4619ea289664f');
INSERT INTO `version` VALUES ('2020-03-02 13:40:59', 'chess_develop_1.1.337.1549.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.337.1549.tar.gz', '6f5e4770c1d08bb00081eaf930963000');
INSERT INTO `version` VALUES ('2020-03-02 14:33:08', 'chess_develop_1.1.337.1550.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.337.1550.tar.gz', '1e985d8dd7e405cbbb095880b2019071');
INSERT INTO `version` VALUES ('2020-03-02 16:18:19', 'chess_develop_1.1.337.1551.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.337.1551.tar.gz', '7004e731a45c45b77aa8d34803b7dbe4');
INSERT INTO `version` VALUES ('2020-03-03 11:09:29', 'chess_develop_1.1.338.1552.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.338.1552.tar.gz', 'b3d54515d46bab82ae7e064c5974b24a');
INSERT INTO `version` VALUES ('2020-03-03 18:04:29', 'chess_develop_1.1.338.1554.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.338.1554.tar.gz', '914296a18af6744865ee48ff12e001b7');
INSERT INTO `version` VALUES ('2020-03-04 08:03:58', 'chess_develop_1.1.339.1554.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.339.1554.tar.gz', 'dc9dad86156b3d8e6b785a5e9f85dec2');
INSERT INTO `version` VALUES ('2020-03-04 14:03:38', 'chess_develop_1.1.339.1556.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.339.1556.tar.gz', '5f7e488616a89a2e8ca4b3a71febb2ac');
INSERT INTO `version` VALUES ('2020-03-05 08:03:55', 'chess_develop_1.1.340.1558.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.340.1558.tar.gz', '3b536b21dcc3ca0c4d3827efa71fd2c4');
INSERT INTO `version` VALUES ('2020-03-05 18:02:14', 'chess_develop_1.1.340.1561.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.340.1561.tar.gz', '679c1a5b1bd7efb1b665c44b745dec89');
INSERT INTO `version` VALUES ('2020-03-06 08:03:58', 'chess_develop_1.1.341.1563.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.341.1563.tar.gz', 'a93f69cbd778a55d7f5c9e0c40955ae2');
INSERT INTO `version` VALUES ('2020-03-06 18:21:58', 'chess_develop_1.1.341.1566.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.341.1566.tar.gz', 'fba36cd062fcb213d657821d24c74243');
INSERT INTO `version` VALUES ('2020-03-06 18:57:37', 'chess_develop_1.1.341.1567.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.341.1567.tar.gz', '0bce82cacf51d2acc8ce6b11e824a5ad');
INSERT INTO `version` VALUES ('2020-03-07 08:03:57', 'chess_develop_1.1.342.1568.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.342.1568.tar.gz', 'd559945a1f99fdef4b3ee625ce574b08');
INSERT INTO `version` VALUES ('2020-03-07 15:15:39', 'chess_develop_1.1.342.1569.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.342.1569.tar.gz', '3ce584a47399fef90ce482012a98ddca');
INSERT INTO `version` VALUES ('2020-03-07 15:46:55', 'chess_develop_1.1.342.1570.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.342.1570.tar.gz', '0eb2e6e706941cb16e91102dbafb14dc');
INSERT INTO `version` VALUES ('2020-03-08 08:03:53', 'chess_develop_1.1.343.1571.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.343.1571.tar.gz', 'dd99fa169367c54265d083d47460be84');
INSERT INTO `version` VALUES ('2020-03-09 08:03:56', 'chess_develop_1.1.344.1571.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.344.1571.tar.gz', '8c4e16f4f8af71648b83016d417bb937');
INSERT INTO `version` VALUES ('2020-03-09 12:23:17', 'chess_develop_1.1.344.1572.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.344.1572.tar.gz', 'df5028fc549ceaa06c6ffb8bd7d7cd69');
INSERT INTO `version` VALUES ('2020-03-09 18:18:19', 'chess_develop_1.1.344.1573.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.344.1573.tar.gz', '07a2baf53b1b210c504b66662d20e876');
INSERT INTO `version` VALUES ('2020-03-10 08:03:54', 'chess_develop_1.1.345.1574.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.345.1574.tar.gz', '4e9889d127184121687e83214fdd9f60');
INSERT INTO `version` VALUES ('2020-03-11 08:03:58', 'chess_develop_1.1.346.1574.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.346.1574.tar.gz', '6a5263c7ebffb55e91bdea9a83a5c3d2');
INSERT INTO `version` VALUES ('2020-03-11 20:13:51', 'chess_develop_1.1.346.1575.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.346.1575.tar.gz', '7ee8dcdc90ed763ca6036fe805ca091b');
INSERT INTO `version` VALUES ('2020-03-11 20:59:48', 'chess_develop_1.1.346.1576.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.346.1576.tar.gz', '2f2a7ea22aa9f7f9aef7259d5d52c35b');
INSERT INTO `version` VALUES ('2020-03-11 22:18:51', 'chess_develop_1.1.346.1578.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.346.1578.tar.gz', '45d8c53bd8f67db5db5e68e350cf6d51');
INSERT INTO `version` VALUES ('2020-03-12 08:03:54', 'chess_develop_1.1.347.1578.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.347.1578.tar.gz', '9e69c1bcb43caa01be2064eb93792825');
INSERT INTO `version` VALUES ('2020-03-12 18:17:37', 'chess_develop_1.1.347.1582.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.347.1582.tar.gz', 'bc197c4346d19f985ca169e32c2d3ce5');
INSERT INTO `version` VALUES ('2020-03-12 22:58:24', 'chess_develop_1.1.347.1583.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.347.1583.tar.gz', 'dab3c2cbf95b7a6e8248e4b41a425788');
INSERT INTO `version` VALUES ('2020-03-13 08:04:00', 'chess_develop_1.1.348.1583.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.348.1583.tar.gz', '91aa01b93ab3d2c12afc7551a3d710f5');
INSERT INTO `version` VALUES ('2020-03-13 12:35:11', 'chess_develop_1.1.348.1586.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.348.1586.tar.gz', 'e7c0f6635ac519e0faaed567c7551c1c');
INSERT INTO `version` VALUES ('2020-03-13 18:06:58', 'chess_develop_1.1.348.1589.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.348.1589.tar.gz', '0a5b84109524c26cb79d3b2ba63735d1');
INSERT INTO `version` VALUES ('2020-03-14 08:04:02', 'chess_develop_1.1.349.1590.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.349.1590.tar.gz', '23e89c20bb9e8807bc0d64982af0fbc1');
INSERT INTO `version` VALUES ('2020-03-14 10:58:47', 'chess_develop_1.1.349.1591.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.349.1591.tar.gz', 'c90232fe3155262b34cf433ae9ab0af1');
INSERT INTO `version` VALUES ('2020-03-14 15:07:23', 'chess_develop_1.1.349.1593.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.349.1593.tar.gz', 'ed3c6dbf9bdf95fbb8a55a909d949620');
INSERT INTO `version` VALUES ('2020-03-14 18:02:09', 'chess_develop_1.1.349.1594.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.349.1594.tar.gz', 'dbe11acda50108771c3b50896d413ba3');
INSERT INTO `version` VALUES ('2020-03-14 18:15:52', 'chess_develop_1.1.349.1595.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.349.1595.tar.gz', '90436f7da5aa464dbe606295901875d6');
INSERT INTO `version` VALUES ('2020-03-14 22:07:55', 'chess_develop_1.1.349.1597.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.349.1597.tar.gz', 'c1fa0278a5d8444780ca25e67b00c9dd');
INSERT INTO `version` VALUES ('2020-03-14 23:32:47', 'chess_develop_1.1.349.1599.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.349.1599.tar.gz', 'be4c1e2353a4d68808022708af6b8690');
INSERT INTO `version` VALUES ('2020-03-15 08:04:15', 'chess_develop_1.1.350.1600.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.350.1600.tar.gz', '28f05b8d02ce92769dedc94c2576dcd3');
INSERT INTO `version` VALUES ('2020-03-16 08:04:01', 'chess_develop_1.1.351.1600.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.351.1600.tar.gz', '9b3a187bd5ee3e847fea1634a23dacf9');
INSERT INTO `version` VALUES ('2020-03-16 12:05:39', 'chess_develop_1.1.351.1602.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.351.1602.tar.gz', 'e324580f8783ce129a7b0938c48a7ea2');
INSERT INTO `version` VALUES ('2020-03-16 18:07:46', 'chess_develop_1.1.351.1609.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.351.1609.tar.gz', '186fa8aa8e574ff2dcda9e5eaa46ed69');
INSERT INTO `version` VALUES ('2020-03-17 08:04:10', 'chess_develop_1.1.352.1612.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.352.1612.tar.gz', 'fcc1faeeeb56e169d9d61e987a9cd8bb');
INSERT INTO `version` VALUES ('2020-03-18 08:04:04', 'chess_develop_1.1.353.1616.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.353.1616.tar.gz', '48976ead1ebf60e4b7fa728683da9110');
INSERT INTO `version` VALUES ('2020-03-18 14:14:48', 'chess_develop_1.1.353.1617.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.353.1617.tar.gz', 'e398ede3b66483ec365de6b1d3aaab10');
INSERT INTO `version` VALUES ('2020-03-18 21:05:42', 'chess_develop_1.1.353.1620.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.353.1620.tar.gz', 'c16a2ca1e7c5ae6dab793409a0cfef68');
INSERT INTO `version` VALUES ('2020-03-18 21:51:14', 'chess_develop_1.1.353.1621.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.353.1621.tar.gz', '11d1f96e699d3fac068aa663a463e045');
INSERT INTO `version` VALUES ('2020-03-19 08:04:01', 'chess_develop_1.1.354.1621.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.354.1621.tar.gz', '422fa19d5db68c69cfbb512880b1733c');
INSERT INTO `version` VALUES ('2020-03-19 16:41:52', 'chess_develop_1.1.354.1626.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.354.1626.tar.gz', '0598fa9dc89853647a315bc4f723c96c');
INSERT INTO `version` VALUES ('2020-03-19 21:30:15', 'chess_develop_1.1.354.1631.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.354.1631.tar.gz', 'bb6c029bfdf10f02d255862d67fd8f1c');
INSERT INTO `version` VALUES ('2020-03-20 08:04:14', 'chess_develop_1.1.355.1633.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.355.1633.tar.gz', 'c0667f039901c763c9b4173c5057adae');
INSERT INTO `version` VALUES ('2020-03-20 12:33:00', 'chess_develop_1.1.355.1634.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.355.1634.tar.gz', 'd9dbc6ae363aebab08c234918dc91173');
INSERT INTO `version` VALUES ('2020-03-20 14:19:33', 'chess_develop_1.1.355.1635.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.355.1635.tar.gz', 'b36e2c472d48efe27a7566744dae6fb7');
INSERT INTO `version` VALUES ('2020-03-20 18:07:56', 'chess_develop_1.1.355.1639.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.355.1639.tar.gz', '5dce3b63cb44394e8d4f730b9db7dfb6');
INSERT INTO `version` VALUES ('2020-03-20 18:14:01', 'chess_develop_1.1.355.1640.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.355.1640.tar.gz', '2ab1d04a106a07bfb3fca4c8e8cc406a');
INSERT INTO `version` VALUES ('2020-03-20 19:29:34', 'chess_develop_1.1.355.1641.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.355.1641.tar.gz', '05d849f114801903ecbdaf50a1ba91c7');
INSERT INTO `version` VALUES ('2020-03-21 08:04:04', 'chess_develop_1.1.356.1642.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.356.1642.tar.gz', '557ab4e717e9ea20c40a79bfdd71a9b6');
INSERT INTO `version` VALUES ('2020-03-22 08:04:04', 'chess_develop_1.1.357.1644.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.357.1644.tar.gz', '3a0751aa1536faa344bae8e630bc0c44');
INSERT INTO `version` VALUES ('2020-03-23 08:04:18', 'chess_develop_1.1.358.1644.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.358.1644.tar.gz', '5c386ae5505171b68de340a51467ec00');
INSERT INTO `version` VALUES ('2020-03-23 13:44:49', 'chess_develop_1.1.358.1646.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.358.1646.tar.gz', 'de4c1294e2c30db24e31a63b8a558e9f');
INSERT INTO `version` VALUES ('2020-03-23 14:00:10', 'chess_develop_1.1.358.1649.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.358.1649.tar.gz', 'b8cd3787a9d2b6ba8f956af2463dc674');
INSERT INTO `version` VALUES ('2020-03-23 14:12:38', 'chess_develop_1.1.358.1650.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.358.1650.tar.gz', '9d32de8c1d10960fa4efb1799e8367d1');
INSERT INTO `version` VALUES ('2020-03-23 14:48:15', 'chess_develop_1.1.358.1653.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.358.1653.tar.gz', '5824b906ad8a2e8a2799b84a0d94de0b');
INSERT INTO `version` VALUES ('2020-03-23 16:51:32', 'chess_develop_1.1.358.1655.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.358.1655.tar.gz', '79f2bbca12cc9210e4b8bfbcc2fe0e7c');
INSERT INTO `version` VALUES ('2020-03-23 21:10:46', 'chess_develop_1.1.358.1656.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.358.1656.tar.gz', '6482ff37350893fb3e29a796117bc159');
INSERT INTO `version` VALUES ('2020-03-24 08:04:11', 'chess_develop_1.1.359.1657.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.359.1657.tar.gz', '09fa540dccc98b751446e74f16482fcf');
INSERT INTO `version` VALUES ('2020-03-24 12:03:28', 'chess_develop_1.1.359.1658.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.359.1658.tar.gz', 'e872ad6879c8bcf318c028d52cb0859f');
INSERT INTO `version` VALUES ('2020-03-24 15:08:56', 'chess_develop_1.1.359.1659.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.359.1659.tar.gz', 'ea89e4b6f5efc6fd679f1a3fcea1ea07');
INSERT INTO `version` VALUES ('2020-03-24 18:08:56', 'chess_develop_1.1.359.1663.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.359.1663.tar.gz', '8e7587606e9a9a1fabeed938fa831b5a');
INSERT INTO `version` VALUES ('2020-03-25 08:04:12', 'chess_develop_1.1.360.1663.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.360.1663.tar.gz', 'b227524bd543c782a84afbbccf934846');
INSERT INTO `version` VALUES ('2020-03-25 12:11:08', 'chess_develop_1.1.360.1665.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.360.1665.tar.gz', '2162b66daccae8d8ff4afdc5ed035779');
INSERT INTO `version` VALUES ('2020-03-25 13:49:44', 'chess_develop_1.1.360.1666.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.360.1666.tar.gz', '04d74bd805b3991efe02442c84750c80');
INSERT INTO `version` VALUES ('2020-03-25 13:53:19', 'chess_develop_1.1.360.1667.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.360.1667.tar.gz', 'c5b202cffca4722efb04877e143eccbd');
INSERT INTO `version` VALUES ('2020-03-25 16:46:25', 'chess_develop_1.1.360.1669.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.360.1669.tar.gz', '34dc6330e9ac61d1574b6d27ee00c152');
INSERT INTO `version` VALUES ('2020-03-25 17:34:35', 'chess_develop_1.1.360.1674.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.360.1674.tar.gz', '81da9fca1e890fafcf7d766488621948');
INSERT INTO `version` VALUES ('2020-03-25 17:40:07', 'chess_develop_1.1.360.1675.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.360.1675.tar.gz', 'd344604edefc36149d9eb12391c48942');
INSERT INTO `version` VALUES ('2020-03-26 08:04:15', 'chess_develop_1.1.361.1679.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.361.1679.tar.gz', 'a8fd05225b084f50ba78becf4a01f6fe');
INSERT INTO `version` VALUES ('2020-03-26 18:06:27', 'chess_develop_1.1.361.1685.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.361.1685.tar.gz', '8296403788f049ba802c7dbf37668a32');
INSERT INTO `version` VALUES ('2020-03-26 19:20:46', 'chess_develop_1.1.361.1687.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.361.1687.tar.gz', 'b2548764200903d10cd8a22135fb0244');
INSERT INTO `version` VALUES ('2020-03-26 20:48:41', 'chess_develop_1.1.361.1688.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.361.1688.tar.gz', '13574c36e065b66e6412278656adb5e6');
INSERT INTO `version` VALUES ('2020-03-27 08:04:13', 'chess_develop_1.1.362.1690.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.362.1690.tar.gz', 'f8b477a3a1677c990c254af1b5b552e8');
INSERT INTO `version` VALUES ('2020-03-27 18:16:32', 'chess_develop_1.1.362.1694.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.362.1694.tar.gz', '17325b5ad8373e4aaeece065bb1eb53b');
INSERT INTO `version` VALUES ('2020-03-27 19:12:22', 'chess_develop_1.1.362.1695.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.362.1695.tar.gz', '8bf677492da06604b2a9c6d80147eae6');
INSERT INTO `version` VALUES ('2020-03-27 19:23:35', 'chess_develop_1.1.362.1696.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.362.1696.tar.gz', 'e69ad7086041506229cc01e7128843dd');
INSERT INTO `version` VALUES ('2020-03-27 20:19:40', 'chess_develop_1.1.362.1697.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.362.1697.tar.gz', '6369efbfe48dbdf90257b606546b9613');
INSERT INTO `version` VALUES ('2020-03-27 20:36:56', 'chess_develop_1.1.362.1698.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.362.1698.tar.gz', '2f7fd230201453efc862477f3d74abf3');
INSERT INTO `version` VALUES ('2020-03-27 22:00:06', 'chess_develop_1.1.362.1699.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.362.1699.tar.gz', '30b1fab710f9509d993df6d26442aff6');
INSERT INTO `version` VALUES ('2020-03-28 08:04:17', 'chess_develop_1.1.363.1700.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.363.1700.tar.gz', 'b65fe3a964ec8d8b6496050eaddd5ec8');
INSERT INTO `version` VALUES ('2020-03-28 15:36:12', 'chess_develop_1.1.363.1702.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.363.1702.tar.gz', 'a89c9bc954e22cd266a36ce9e381a855');
INSERT INTO `version` VALUES ('2020-03-28 16:44:56', 'chess_develop_1.1.363.1703.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.363.1703.tar.gz', 'e2e4127739168e9fca8e98db3778f119');
INSERT INTO `version` VALUES ('2020-03-28 17:08:51', 'chess_develop_1.1.363.1704.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.363.1704.tar.gz', '30e3bab3d9b1a388d891df307d1ce29a');
INSERT INTO `version` VALUES ('2020-03-28 17:53:44', 'chess_develop_1.1.363.1705.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.363.1705.tar.gz', '16d118b020206aacfa63a22003ac8ebf');
INSERT INTO `version` VALUES ('2020-03-28 18:35:56', 'chess_develop_1.1.363.1707.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.363.1707.tar.gz', '8d48f33cfc36b2c37dd9575362fc5101');
INSERT INTO `version` VALUES ('2020-03-28 19:06:38', 'chess_develop_1.1.363.1708.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.363.1708.tar.gz', '8b39335a6de02b7141c4435a6f4945bf');
INSERT INTO `version` VALUES ('2020-03-28 21:21:38', 'chess_develop_1.1.363.1709.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.363.1709.tar.gz', '6520b82929993990021c4b181f0f820a');
INSERT INTO `version` VALUES ('2020-03-29 08:04:16', 'chess_develop_1.1.364.1709.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.364.1709.tar.gz', 'e3e0dd919cf467792f9fa63440abd329');
INSERT INTO `version` VALUES ('2020-03-30 08:04:19', 'chess_develop_1.1.365.1709.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.365.1709.tar.gz', '69efd0036c14d8f42d29e39c731c4ddd');
INSERT INTO `version` VALUES ('2020-03-30 10:56:21', 'chess_develop_1.1.365.1710.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.365.1710.tar.gz', '219e61e7e34e7ddbc30027af9154c1cf');
INSERT INTO `version` VALUES ('2020-03-30 11:10:20', 'chess_develop_1.1.365.1711.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.365.1711.tar.gz', '58c23679ca34f8277761cb98e5e3805c');
INSERT INTO `version` VALUES ('2020-03-30 11:49:51', 'chess_develop_1.1.365.1712.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.365.1712.tar.gz', 'e948068a627d073f6c0ff15db8c15576');
INSERT INTO `version` VALUES ('2020-03-30 12:37:01', 'chess_develop_1.1.365.1713.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.365.1713.tar.gz', '3934f6c40d74cea532c3e50c27af3db3');
INSERT INTO `version` VALUES ('2020-03-30 16:25:34', 'chess_develop_1.1.365.1714.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.365.1714.tar.gz', 'db370000793f47604e0b5346df0ab1b5');
INSERT INTO `version` VALUES ('2020-03-30 18:08:26', 'chess_develop_1.1.365.1715.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.365.1715.tar.gz', '09688193f89ca0411d7b97e91ee48071');
INSERT INTO `version` VALUES ('2020-03-30 21:55:21', 'chess_develop_1.1.365.1716.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.365.1716.tar.gz', '112ad17713c02ef4217b5e4593fa480c');
INSERT INTO `version` VALUES ('2020-03-30 22:25:26', 'chess_develop_1.1.365.1719.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.365.1719.tar.gz', '9ed4c337878c14852ea7f68dfa4e6b01');
INSERT INTO `version` VALUES ('2020-03-31 08:04:20', 'chess_develop_1.1.366.1719.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.366.1719.tar.gz', '2b0fba35bcb6fac29dfa666ed49ce196');
INSERT INTO `version` VALUES ('2020-04-01 08:04:22', 'chess_develop_1.1.367.1724.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.367.1724.tar.gz', 'ffd1f0ee43793f3453b9a37cd59dcbbb');
INSERT INTO `version` VALUES ('2020-04-01 17:27:37', 'chess_develop_1.1.367.1728.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.367.1728.tar.gz', '34b4de7cf8eb5f6c90bc4e33d3950b9b');
INSERT INTO `version` VALUES ('2020-04-01 18:01:46', 'chess_develop_1.1.367.1729.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.367.1729.tar.gz', '41e33b560f50be4d41328c2f3a6a184b');
INSERT INTO `version` VALUES ('2020-04-01 21:50:26', 'chess_develop_1.1.367.1732.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.367.1732.tar.gz', 'fb0873f22c263eab056a9f3f67d28f72');
INSERT INTO `version` VALUES ('2020-04-02 08:04:20', 'chess_develop_1.1.368.1733.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.368.1733.tar.gz', '54585a0291ce6e9dc08882bd6af772ab');
INSERT INTO `version` VALUES ('2020-04-02 14:45:18', 'chess_develop_1.1.368.1734.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.368.1734.tar.gz', 'f866c1fe3a070988a63a605ea35a2c64');
INSERT INTO `version` VALUES ('2020-04-02 16:30:47', 'chess_develop_1.1.368.1736.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.368.1736.tar.gz', '31c4b482743e9fc751599f3e1575ca7d');
INSERT INTO `version` VALUES ('2020-04-03 08:04:24', 'chess_develop_1.1.369.1736.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.369.1736.tar.gz', 'aa848ac71ffea8f5cde8a5c071fa302e');
INSERT INTO `version` VALUES ('2020-04-03 14:44:00', 'chess_develop_1.1.369.1737.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.369.1737.tar.gz', '4102e92214de6cf782301efe9abbe0e5');
INSERT INTO `version` VALUES ('2020-04-04 08:04:23', 'chess_develop_1.1.370.1737.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.370.1737.tar.gz', '8a3e08d67e0d9c69f0c508ef41837578');
INSERT INTO `version` VALUES ('2020-04-04 13:02:58', 'chess_develop_1.1.370.1739.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.370.1739.tar.gz', '4d81bc77aa2072f984645cc55f94ef47');
INSERT INTO `version` VALUES ('2020-04-05 08:04:24', 'chess_develop_1.1.371.1739.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.371.1739.tar.gz', 'e759ac2bea2969ad7a3acd33749984e7');
INSERT INTO `version` VALUES ('2020-04-06 08:04:32', 'chess_develop_1.1.372.1739.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.372.1739.tar.gz', '9ce123ca56c85c788d7d915b0f346f7a');
INSERT INTO `version` VALUES ('2020-04-07 08:04:21', 'chess_develop_1.1.373.1739.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.373.1739.tar.gz', '3c2b1de0cc8230968412c27148904ede');
INSERT INTO `version` VALUES ('2020-04-07 18:11:36', 'chess_develop_1.1.373.1741.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.373.1741.tar.gz', '0e2a0b7896936e01304b9f84c0394905');
INSERT INTO `version` VALUES ('2020-04-08 08:04:20', 'chess_develop_1.1.374.1745.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.374.1745.tar.gz', '80f9ba75cfbacf0938823e91864e12c3');
INSERT INTO `version` VALUES ('2020-04-08 20:40:03', 'chess_develop_1.1.374.1749.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.374.1749.tar.gz', 'c66234d0d335aaaef9643e44893ca0ad');
INSERT INTO `version` VALUES ('2020-04-08 20:51:41', 'chess_develop_1.1.374.1750.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.374.1750.tar.gz', '8b2b6b7097ae7012f604f893e7eab09d');
INSERT INTO `version` VALUES ('2020-04-09 08:04:36', 'chess_develop_1.1.375.1756.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.375.1756.tar.gz', '7fb65fdeb0236a9c7d25cd0347e39132');
INSERT INTO `version` VALUES ('2020-04-09 17:09:34', 'chess_develop_1.1.375.1758.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.375.1758.tar.gz', '202d34a175337a262b476ed930958034');
INSERT INTO `version` VALUES ('2020-04-09 18:49:07', 'chess_develop_1.1.375.1759.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.375.1759.tar.gz', '9d4bab57c2ce326c91b0a8d15eef5350');
INSERT INTO `version` VALUES ('2020-04-09 20:58:10', 'chess_develop_1.1.375.1761.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.375.1761.tar.gz', '9b900d3805029cada3ce91815533ea4d');
INSERT INTO `version` VALUES ('2020-04-10 08:04:32', 'chess_develop_1.1.376.1761.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.376.1761.tar.gz', '6ca98772e139a43175de6942992c47b3');
INSERT INTO `version` VALUES ('2020-04-10 18:10:10', 'chess_develop_1.1.376.1768.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.376.1768.tar.gz', '5c86cf3345d6d0497e8746928c157571');
INSERT INTO `version` VALUES ('2020-04-11 14:19:33', 'chess_develop_1.1.377.1777.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.377.1777.tar.gz', '55dc4780ae3cbb50449a910cd9593edb');
INSERT INTO `version` VALUES ('2020-04-11 14:38:20', 'chess_develop_1.1.377.1778.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.377.1778.tar.gz', 'f4e81fcce19f4bf33b30791d95ec20ef');
INSERT INTO `version` VALUES ('2020-04-11 16:59:55', 'chess_develop_1.1.377.1780.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.377.1780.tar.gz', '7032536a8f2f88a84c2a02c5effb9369');
INSERT INTO `version` VALUES ('2020-04-12 08:04:23', 'chess_develop_1.1.378.1780.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.378.1780.tar.gz', '96fba8d72251c0062be2ee1eee363107');
INSERT INTO `version` VALUES ('2020-04-13 08:04:31', 'chess_develop_1.1.379.1780.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.379.1780.tar.gz', '2f03b90d5ca446385541561417f384da');
INSERT INTO `version` VALUES ('2020-04-14 08:04:23', 'chess_develop_1.1.380.1782.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.380.1782.tar.gz', '966e4519b8028b2f81c0f1ca7352f0b3');
INSERT INTO `version` VALUES ('2020-04-14 12:11:51', 'chess_develop_1.1.380.1784.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.380.1784.tar.gz', '81e564f9b79fbfdb5bbc98e45fc59c85');
INSERT INTO `version` VALUES ('2020-04-14 13:47:29', 'chess_develop_1.1.380.1785.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.380.1785.tar.gz', 'cb5683548960b8eaf57ad0b6db45676d');
INSERT INTO `version` VALUES ('2020-04-14 14:58:06', 'chess_develop_1.1.380.1786.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.380.1786.tar.gz', '32839800786ee2b840c9f9663c0dab83');
INSERT INTO `version` VALUES ('2020-04-14 18:05:49', 'chess_develop_1.1.380.1788.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.380.1788.tar.gz', '4737a1de760a722d919ec1a901ed82e5');
INSERT INTO `version` VALUES ('2020-04-14 21:06:28', 'chess_develop_1.1.380.1789.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.380.1789.tar.gz', 'a8b6d47d7f33f25d08aef5744d8405dd');
INSERT INTO `version` VALUES ('2020-04-15 08:04:28', 'chess_develop_1.1.381.1789.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.381.1789.tar.gz', '4d0524dc8944bb3ec0914188274f39dc');
INSERT INTO `version` VALUES ('2020-04-15 16:58:30', 'chess_develop_1.1.381.1793.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.381.1793.tar.gz', '2f8362993bfbfd2d897774ff9085b8bb');
INSERT INTO `version` VALUES ('2020-04-15 18:55:00', 'chess_develop_1.1.381.1795.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.381.1795.tar.gz', '872cb1c53649201db9bf50303ae069ca');
INSERT INTO `version` VALUES ('2020-04-16 11:56:31', 'chess_develop_1.1.382.1796.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.382.1796.tar.gz', '3d9e4d348de37ec879b8194cbc2bfb9f');
INSERT INTO `version` VALUES ('2020-04-16 14:23:41', 'chess_develop_1.1.382.1799.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.382.1799.tar.gz', '3c784a10dcc1e5c7ea7a3fe48b908430');
INSERT INTO `version` VALUES ('2020-04-16 15:11:42', 'chess_develop_1.1.382.1800.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.382.1800.tar.gz', 'adf1f03d3e2c1258d4a771d9c719163c');
INSERT INTO `version` VALUES ('2020-04-16 18:08:06', 'chess_develop_1.1.382.1807.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.382.1807.tar.gz', '1561fd22b79f29c365bd62b51776bda3');
INSERT INTO `version` VALUES ('2020-04-17 08:04:33', 'chess_develop_1.1.383.1813.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.383.1813.tar.gz', '234c23aa49e21c64626867ea1c3058f6');
INSERT INTO `version` VALUES ('2020-04-17 18:07:25', 'chess_develop_1.1.383.1819.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.383.1819.tar.gz', 'd78b26c5275cff492b2d52ad5e761c6d');
INSERT INTO `version` VALUES ('2020-04-18 08:04:32', 'chess_develop_1.1.384.1828.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.384.1828.tar.gz', '5ef0beec3b1c9659813fcccd022536eb');
INSERT INTO `version` VALUES ('2020-04-19 08:04:28', 'chess_develop_1.1.385.1828.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.385.1828.tar.gz', '947f51bdb940ee6de94786722f13622c');
INSERT INTO `version` VALUES ('2020-04-20 08:04:29', 'chess_develop_1.1.386.1828.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.386.1828.tar.gz', '8dc14bbd065af22d6aa712ccbd4e2fe4');
INSERT INTO `version` VALUES ('2020-04-20 18:04:15', 'chess_develop_1.1.386.1831.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.386.1831.tar.gz', 'a900884945144b08b8c0d1cc6d41ff5c');
INSERT INTO `version` VALUES ('2020-04-20 21:21:30', 'chess_develop_1.1.386.1833.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.386.1833.tar.gz', 'f8ef46968a32a1a427f35f1df7fa19d4');
INSERT INTO `version` VALUES ('2020-04-21 08:04:39', 'chess_develop_1.1.387.1834.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.387.1834.tar.gz', '38dabbc92daf86e55d03fddcb6146691');
INSERT INTO `version` VALUES ('2020-04-21 18:54:08', 'chess_develop_1.1.387.1841.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.387.1841.tar.gz', '5c3c208093c700ee2d392fa97c936075');
INSERT INTO `version` VALUES ('2020-04-21 18:58:36', 'chess_develop_1.1.387.1842.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.387.1842.tar.gz', '66bade06e0d3cd3c13abd31f3adccec7');
INSERT INTO `version` VALUES ('2020-04-22 08:04:40', 'chess_develop_1.1.388.1843.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.388.1843.tar.gz', 'c5fcd04093c72658dc289b984cd6c670');
INSERT INTO `version` VALUES ('2020-04-22 18:06:18', 'chess_develop_1.1.388.1853.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.388.1853.tar.gz', '105aabab2bd6eb6df15ec98ae45b8efa');
INSERT INTO `version` VALUES ('2020-04-22 20:45:35', 'chess_develop_1.1.388.1857.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.388.1857.tar.gz', '5434d923cae1fbf5b50999572de27af2');
INSERT INTO `version` VALUES ('2020-04-22 22:48:21', 'chess_develop_1.1.388.1863.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.388.1863.tar.gz', '4571e67dfb466ce21b02ba9cf56b1394');
INSERT INTO `version` VALUES ('2020-04-22 22:54:01', 'chess_develop_1.1.388.1864.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.388.1864.tar.gz', 'f28a77b59e97a8c4bbf7d704eaa35821');
INSERT INTO `version` VALUES ('2020-04-23 08:04:36', 'chess_develop_1.1.389.1864.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.389.1864.tar.gz', '244954d2ac0f4a434c0c80b936c9005e');
INSERT INTO `version` VALUES ('2020-04-23 18:26:01', 'chess_develop_1.1.389.1867.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.389.1867.tar.gz', '8e69e3a9a7b4ba24f6db4d464c70ab36');
INSERT INTO `version` VALUES ('2020-04-23 21:27:50', 'chess_develop_1.1.389.1868.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.389.1868.tar.gz', '09c966cf051773fd3de1bcdf3b88dc05');
INSERT INTO `version` VALUES ('2020-04-24 08:04:44', 'chess_develop_1.1.390.1871.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.390.1871.tar.gz', '7b5b2ba9a1f35d1d2533e657e0cecb38');
INSERT INTO `version` VALUES ('2020-04-24 20:13:10', 'chess_develop_1.1.390.1874.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.390.1874.tar.gz', '4ae4235d5c186a18111db29f33d26c29');
INSERT INTO `version` VALUES ('2020-04-25 08:04:44', 'chess_develop_1.1.391.1876.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.391.1876.tar.gz', 'ce20b3a34f5e29bf76e9b3f53ecf651e');
INSERT INTO `version` VALUES ('2020-04-25 17:55:36', 'chess_develop_1.1.391.1877.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.391.1877.tar.gz', '57521075e13191445b45ae4ae56441f6');
INSERT INTO `version` VALUES ('2020-04-25 18:05:22', 'chess_develop_1.1.391.1878.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.391.1878.tar.gz', '7f49609401dee9c84902a196ef1d4882');
INSERT INTO `version` VALUES ('2020-04-26 08:04:40', 'chess_develop_1.1.392.1878.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.392.1878.tar.gz', '2dbe397f285fb2b98937a7bb9815fb61');
INSERT INTO `version` VALUES ('2020-04-26 10:35:55', 'chess_develop_1.1.392.1879.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.392.1879.tar.gz', 'a5a934c543736a43f019bf735eccc721');
INSERT INTO `version` VALUES ('2020-04-26 10:57:18', 'chess_develop_1.1.392.1880.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.392.1880.tar.gz', '273f74f5aa0180c7cdd07a788b0f0a4a');
INSERT INTO `version` VALUES ('2020-04-26 16:07:04', 'chess_develop_1.1.392.1881.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.392.1881.tar.gz', '9ae294d2ea1883a8f51045dfa8e3a628');
INSERT INTO `version` VALUES ('2020-04-26 18:02:12', 'chess_develop_1.1.392.1886.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.392.1886.tar.gz', '8a74882adeac1ad1888e85ee8904989c');
INSERT INTO `version` VALUES ('2020-04-27 08:04:51', 'chess_develop_1.1.393.1886.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.393.1886.tar.gz', '7057ee4e637ba4040daac7f9ed1b04fb');
INSERT INTO `version` VALUES ('2020-04-27 17:16:50', 'chess_develop_1.1.393.1891.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.393.1891.tar.gz', '6031746539ea3409ad6fa1561914a5f7');
INSERT INTO `version` VALUES ('2020-04-27 21:49:01', 'chess_develop_1.1.393.1892.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.393.1892.tar.gz', 'b55b5396d5730b240666b61eeb040886');
INSERT INTO `version` VALUES ('2020-04-28 08:04:47', 'chess_develop_1.1.394.1892.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.394.1892.tar.gz', 'ce2ec04445c4f472c3a45ea78c129723');
INSERT INTO `version` VALUES ('2020-04-28 15:24:04', 'chess_develop_1.1.394.1893.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.394.1893.tar.gz', '4402a34bf6510915b0062251a5091f2d');
INSERT INTO `version` VALUES ('2020-04-28 16:28:27', 'chess_develop_1.1.394.1896.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.394.1896.tar.gz', '10f3cf291c193fb03e182dbb49517172');
INSERT INTO `version` VALUES ('2020-04-28 18:52:37', 'chess_develop_1.1.394.1898.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.394.1898.tar.gz', '5cfdaea3c5d2d8867da766382ce1cf74');
INSERT INTO `version` VALUES ('2020-04-28 21:42:00', 'chess_develop_1.1.394.1902.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.394.1902.tar.gz', 'eb030138d6f170e1170cc28514c17084');
INSERT INTO `version` VALUES ('2020-04-29 08:04:54', 'chess_develop_1.1.395.1902.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.395.1902.tar.gz', '4aa0d73cffa32c38d8b7b7b5ab0c733b');
INSERT INTO `version` VALUES ('2020-04-29 12:13:34', 'chess_develop_1.1.395.1904.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.395.1904.tar.gz', '25ef76f4c28e189c0523977f6a49b5b2');
