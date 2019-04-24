/*
Navicat MySQL Data Transfer

Source Server         : #100 - 战旗版本机
Source Server Version : 50722
Source Host           : 192.168.1.250:3306
Source Database       : kfdeploy

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2019-04-22 23:02:12
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
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'auth', 'auth', '21.0.1', '1.2', '1', '1.0', '', '12.0.1', '1', '1', '0', '28301', '1555774762', '11.0.1', 'chess_develop_1.0.20.134.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'master', '22.0.1', '1.2', '1', '1.0', '', '12.0.1', '1', '1', '0', '28324', '1555774762', '11.0.1', 'chess_develop_1.0.20.134.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'proxy', '23.0.1', '1.2', '1', '1.0', '', '12.0.1', '1', '1', '0', '28345', '1555774762', '11.0.1', 'chess_develop_1.0.20.134.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'shard', '24.0.1', '1.2', '1', '1.0', '', '12.0.1', '1', '1', '0', '28356', '1555774762', '11.0.1', 'chess_develop_1.0.20.134.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'data', 'shard', '31.0.1', '1.2', '1', '1.0', '', '12.0.1', '1', '1', '0', '28335', '1555774762', '11.0.1', 'chess_develop_1.0.20.134.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'match', 'shard', '38.0.1', '1.2', '1', '1.0', '', '12.0.1', '1', '1', '0', '28387', '1555774762', '11.0.1', 'chess_develop_1.0.20.134.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'room', 'shard', '39.0.1', '1.2', '1', '1.0', '', '12.0.1', '1', '1', '0', '28367', '1555774762', '11.0.1', 'chess_develop_1.0.20.134.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'master', '91.1.1', '1.2', '1', '1.0', '', '12.0.1', '1', '1', '0', '28313', '1555774762', '11.0.1', 'chess_develop_1.0.20.134.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'world', '92.1.1', '1.2', '1', '1.0', '', '12.0.1', '1', '1', '0', '28375', '1555774762', '11.0.1', 'chess_develop_1.0.20.134.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'gate', '93.1.1', '1.2', '1', '1.0', '', '12.0.1', '1', '1', '0', '28401', '1555774762', '11.0.1', 'chess_develop_1.0.20.134.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'login', '94.1.1', '1.2', '1', '1.0', '', '12.0.1', '1', '1', '0', '28290', '1555774762', '11.0.1', 'chess_develop_1.0.20.134.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'game', '95.1.1', '1.2', '1', '1.0', '', '12.0.1', '1', '1', '0', '28279', '1555774762', '11.0.1', 'chess_develop_1.0.20.134.tar.gz');

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
INSERT INTO `agent` VALUES ('192.168.1.155', '192.168.1.155', '12.0.1', '1.2', '11.0.1', '192.168.1.250', '10000', '1', '12008', '');

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
INSERT INTO `resource` VALUES ('2019-04-22 20:32:10', 'chess_develop_1.0.22.139.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.22.139.tar.gz', '95d7a102a1e65113b20b33303ea6e37b');

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
INSERT INTO `version` VALUES ('2019-04-16 19:52:09', 'chess_develop_1.0.16.124.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.16.124.tar.gz', 'd2912ffdf4edd094927bd463b49aaa18');
INSERT INTO `version` VALUES ('2019-04-16 19:57:14', 'chess_develop_1.0.16.125.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.16.125.tar.gz', '1a81b40b67c5d5e25d6d2ae6230b4357');
INSERT INTO `version` VALUES ('2019-04-18 18:12:21', 'chess_develop_1.0.18.127.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.18.127.tar.gz', '4f1e93ceab493c8a8dee3051a45071c2');
INSERT INTO `version` VALUES ('2019-04-19 14:31:43', 'chess_develop_1.0.19.129.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.19.129.tar.gz', '361c649167636baf2dd8ee28ee776f15');
INSERT INTO `version` VALUES ('2019-04-19 15:31:54', 'chess_develop_1.0.19.130.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.19.130.tar.gz', '1be99556bc320fb7a2bea903ce68a203');
INSERT INTO `version` VALUES ('2019-04-19 22:15:40', 'chess_develop_1.0.19.131.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.19.131.tar.gz', '8a8dc6c6de3ffce99fe6b140896058b2');
INSERT INTO `version` VALUES ('2019-04-20 15:46:27', 'chess_develop_1.0.20.132.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.20.132.tar.gz', '325dfb5a6359ac50af6b32b5135a6e86');
INSERT INTO `version` VALUES ('2019-04-20 23:37:13', 'chess_develop_1.0.20.134.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.20.134.tar.gz', '60d9282f599d5b859c1f22ddc5823043');
INSERT INTO `version` VALUES ('2019-04-22 15:54:21', 'chess_develop_1.0.22.134.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.22.134.tar.gz', 'd914d8fda57e5f6412bf0dffa9bc02d4');
INSERT INTO `version` VALUES ('2019-04-22 23:01:53', 'chess_develop_1.0.22.140.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.22.140.tar.gz', '524352786c6cc1c11614f4fe99d68257');
