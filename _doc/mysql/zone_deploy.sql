/*
Navicat MySQL Data Transfer

Source Server         : #100 - 战旗版本机
Source Server Version : 50722
Source Host           : 192.168.2.31:3306
Source Database       : kfdeploy

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2019-12-26 20:40:48
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
  `service` varchar(50) NOT NULL DEFAULT '1.1',
  `net` varchar(50) NOT NULL DEFAULT '1',
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
) ENGINE=InnoDB AUTO_INCREMENT=24 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of 1_1_deploy
-- ----------------------------
INSERT INTO `1_1_deploy` VALUES ('1', '/data/chess/auth', '', 'bin/KFStartup', 'auth', 'auth', '21.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '25452', '1577318653', '11.0.1', 'chess_develop_1.1.270.1293.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('2', '/data/chess/route', '', 'bin/KFStartup', 'route', 'master', '22.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '25489', '1577318653', '11.0.1', 'chess_develop_1.1.270.1293.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('3', '/data/chess/route', '', 'bin/KFStartup', 'route', 'proxy', '23.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '25512', '1577318653', '11.0.1', 'chess_develop_1.1.270.1293.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('4', '/data/chess/route', '', 'bin/KFStartup', 'route', 'shard', '24.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '25526', '1577318653', '11.0.1', 'chess_develop_1.1.270.1293.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('5', '/data/chess/data', '', 'bin/KFStartup', 'data', 'shard', '31.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '25504', '1577318653', '11.0.1', 'chess_develop_1.1.270.1293.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('6', '/data/chess/logic', '', 'bin/KFStartup', 'logic', 'shard', '32.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '25465', '1577318653', '11.0.1', 'chess_develop_1.1.270.1293.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('7', '/data/chess/match', '', 'bin/KFStartup', 'match', 'shard', '38.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '25563', '1577318653', '11.0.1', 'chess_develop_1.1.270.1293.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('8', '/data/chess/match', '', 'bin/KFStartup', 'room', 'shard', '39.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '25537', '1577318653', '11.0.1', 'chess_develop_1.1.270.1293.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('9', '/data/chess/zone1', '', 'bin/KFStartup', 'zone', 'master', '91.1.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '25475', '1577318653', '11.0.1', 'chess_develop_1.1.270.1293.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('10', '/data/chess/zone1', '', 'bin/KFStartup', 'zone', 'world', '92.1.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '25548', '1577318653', '11.0.1', 'chess_develop_1.1.270.1293.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('11', '/data/chess/zone1', '', 'bin/KFStartup', 'zone', 'gate', '93.1.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '25429', '1577318653', '11.0.1', 'chess_develop_1.1.270.1293.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('12', '/data/chess/zone1', '', 'bin/KFStartup', 'zone', 'login', '94.1.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '25440', '1577318653', '11.0.1', 'chess_develop_1.1.270.1293.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('13', '/data/chess/zone1', '', 'bin/KFStartup', 'zone', 'game', '95.1.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '25421', '1577318653', '11.0.1', 'chess_develop_1.1.270.1293.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('14', '/data/chess/zone2', '', 'bin/KFStartup', 'zone', 'master', '91.2.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '25475', '1577318653', '11.0.1', 'chess_develop_1.1.270.1293.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('15', '/data/chess/zone2', '', 'bin/KFStartup', 'zone', 'world', '92.2.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '0', '0', '11.0.1', '');
INSERT INTO `1_1_deploy` VALUES ('16', '/data/chess/zone2', '', 'bin/KFStartup', 'zone', 'gate', '93.2.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '0', '0', '11.0.1', '');
INSERT INTO `1_1_deploy` VALUES ('17', '/data/chess/zone2', '', 'bin/KFStartup', 'zone', 'login', '94.2.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '0', '0', '11.0.1', '');
INSERT INTO `1_1_deploy` VALUES ('18', '/data/chess/zone2', '', 'bin/KFStartup', 'zone', 'game', '95.2.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '0', '0', '11.0.1', '');
INSERT INTO `1_1_deploy` VALUES ('19', '/data/chess/zone3', '', 'bin/KFStartup', 'zone', 'master', '91.3.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '25475', '1577318653', '11.0.1', 'chess_develop_1.1.270.1293.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('20', '/data/chess/zone3', '', 'bin/KFStartup', 'zone', 'world', '92.3.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '0', '0', '11.0.1', '');
INSERT INTO `1_1_deploy` VALUES ('21', '/data/chess/zone3', '', 'bin/KFStartup', 'zone', 'gate', '93.3.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '0', '0', '11.0.1', '');
INSERT INTO `1_1_deploy` VALUES ('22', '/data/chess/zone3', '', 'bin/KFStartup', 'zone', 'login', '94.3.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '0', '0', '11.0.1', '');
INSERT INTO `1_1_deploy` VALUES ('23', '/data/chess/zone3', '', 'bin/KFStartup', 'zone', 'game', '95.3.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '0', '0', '11.0.1', '');
