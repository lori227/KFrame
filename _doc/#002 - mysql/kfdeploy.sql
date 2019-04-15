/*
Navicat MySQL Data Transfer

Source Server         : #100 - 战旗版本机
Source Server Version : 50722
Source Host           : 192.168.1.250:3306
Source Database       : kfdeploy

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2019-04-10 20:01:03
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
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'auth', 'auth', '1.21.0.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '7740', '1554892094', '1.11.0.1', 'chess_develop_1.0.100.88.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'master', '1.22.0.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '7756', '1554892094', '1.11.0.1', 'chess_develop_1.0.100.88.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'proxy', '1.23.0.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '7785', '1554892094', '1.11.0.1', 'chess_develop_1.0.100.88.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'shard', '1.24.0.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '7861', '1554892094', '1.11.0.1', 'chess_develop_1.0.100.88.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'data', 'shard', '1.31.0.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '7771', '1554892094', '1.11.0.1', 'chess_develop_1.0.100.88.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'match', 'shard', '1.38.0.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '7800', '1554892094', '1.11.0.1', 'chess_develop_1.0.100.88.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'room', 'shard', '1.39.0.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '7814', '1554892094', '1.11.0.1', 'chess_develop_1.0.100.88.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'master', '1.91.1.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '7827', '1554892094', '1.11.0.1', 'chess_develop_1.0.100.88.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'world', '1.92.1.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '7843', '1554892094', '1.11.0.1', 'chess_develop_1.0.100.88.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'gate', '1.93.1.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '7876', '1554892094', '1.11.0.1', 'chess_develop_1.0.100.88.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'login', '1.94.1.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '7718', '1554892094', '1.11.0.1', 'chess_develop_1.0.100.88.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'game', '1.95.1.1', '1.2', '1.0', '', '1.12.0.1', '1', '1', '0', '7710', '1554892094', '1.11.0.1', 'chess_develop_1.0.100.88.tar.gz');

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
INSERT INTO `version` VALUES ('2019-04-10 13:15:22', 'chess_develop_1.0.100.86.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.100.86.tar.gz', '927b696284705a2109f8690b877cd281');
INSERT INTO `version` VALUES ('2019-04-10 17:31:24', 'chess_develop_1.0.100.88.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.100.88.tar.gz', '5a1183815218d9af77474cc7c2beb3e8');
INSERT INTO `version` VALUES ('2019-03-26 18:10:51', 'chess_develop_1.0.85.48.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.85.48.tar.gz', 'a56ff1f79f8ddef47a9c1cf78a3f635a');
INSERT INTO `version` VALUES ('2019-03-26 19:46:50', 'chess_develop_1.0.85.49.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.85.49.tar.gz', 'ec9a59ac0051947ee885788eff7f5a93');
INSERT INTO `version` VALUES ('2019-03-26 20:03:45', 'chess_develop_1.0.85.50.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.85.50.tar.gz', '43a8d60b2eefbf809667835ada41c7e9');
INSERT INTO `version` VALUES ('2019-03-26 20:34:01', 'chess_develop_1.0.85.51.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.85.51.tar.gz', '92760086e5b0d3fed79cf62633b68ec6');
INSERT INTO `version` VALUES ('2019-03-26 20:46:29', 'chess_develop_1.0.85.52.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.85.52.tar.gz', '472433bf2ef5a0d0bb49f6cf244da795');
INSERT INTO `version` VALUES ('2019-03-26 22:28:03', 'chess_develop_1.0.85.53.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.85.53.tar.gz', '3ecb1ac5575cfbe4e07349d38dbe8e83');
INSERT INTO `version` VALUES ('2019-03-27 15:03:33', 'chess_develop_1.0.86.56.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.86.56.tar.gz', '1035cb4de4bd278dcace2922425763d1');
INSERT INTO `version` VALUES ('2019-03-27 18:12:12', 'chess_develop_1.0.86.57.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.86.57.tar.gz', '1855927b294f6868660dbddcfdeb2481');
INSERT INTO `version` VALUES ('2019-03-27 19:38:11', 'chess_develop_1.0.86.58.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.86.58.tar.gz', '1fea8d2cbffb8ffeb2933c2629707af7');
INSERT INTO `version` VALUES ('2019-03-28 17:49:42', 'chess_develop_1.0.87.62.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.87.62.tar.gz', 'd77fa2dc35a6d4867ae0a2f95e3e6274');
INSERT INTO `version` VALUES ('2019-03-28 18:00:17', 'chess_develop_1.0.87.63.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.87.63.tar.gz', '87885edc0afea634249382298a083a85');
INSERT INTO `version` VALUES ('2019-03-28 19:58:57', 'chess_develop_1.0.87.64.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.87.64.tar.gz', 'c7083137ae12426e4c8140d5cecb3e84');
INSERT INTO `version` VALUES ('2019-03-29 15:26:55', 'chess_develop_1.0.88.66.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.88.66.tar.gz', '134ac4b428c68037a47dae6c0cc69b6b');
INSERT INTO `version` VALUES ('2019-03-29 15:35:27', 'chess_develop_1.0.88.67.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.88.67.tar.gz', 'bb89c1ad40e2a4d1491331fbc518a073');
INSERT INTO `version` VALUES ('2019-03-29 16:12:27', 'chess_develop_1.0.88.68.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.88.68.tar.gz', 'ad1c5c4e4fb281f93feac57c6c00f6b4');
INSERT INTO `version` VALUES ('2019-03-29 18:49:26', 'chess_develop_1.0.88.69.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.88.69.tar.gz', '30d0862fc830887510448631577db8e2');
INSERT INTO `version` VALUES ('2019-03-30 15:36:41', 'chess_develop_1.0.89.70.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.89.70.tar.gz', 'ea199776534ed29fd763eb63cfb6103b');
INSERT INTO `version` VALUES ('2019-03-30 15:41:43', 'chess_develop_1.0.89.71.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.89.71.tar.gz', '74f661fd67ea1209d6c7b017f861d855');
INSERT INTO `version` VALUES ('2019-04-01 11:43:08', 'chess_develop_1.0.91.73.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.91.73.tar.gz', 'bb7243414c684e4b46ca0a2cec6e1871');
INSERT INTO `version` VALUES ('2019-04-01 11:50:50', 'chess_develop_1.0.91.74.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.91.74.tar.gz', '6ef972b8985884b144dcc0e97c60019e');
INSERT INTO `version` VALUES ('2019-04-02 11:56:02', 'chess_develop_1.0.92.74.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.92.74.tar.gz', '93b1de46cb21cb89e9252e20b279a36e');
INSERT INTO `version` VALUES ('2019-04-02 15:03:31', 'chess_develop_1.0.92.75.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.92.75.tar.gz', 'f44431abfb45e5e1f4503708721ef16f');
INSERT INTO `version` VALUES ('2019-04-02 15:33:16', 'chess_develop_1.0.92.77.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.92.77.tar.gz', 'a5da7ecba9c5a70e4c4515ccf0582423');
INSERT INTO `version` VALUES ('2019-04-02 15:49:29', 'chess_develop_1.0.92.78.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.92.78.tar.gz', 'bef185d6f43fed6f6ac06a4984555b8d');
INSERT INTO `version` VALUES ('2019-04-02 21:14:08', 'chess_develop_1.0.92.80.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.92.80.tar.gz', 'd4f0d54c5766362664b15f526f28e372');
INSERT INTO `version` VALUES ('2019-04-09 10:49:38', 'chess_develop_1.0.99.85.tar.gz', 'http://192.168.1.250/upload/chess_develop_1.0.99.85.tar.gz', '2cff417921922f64bb64a797a716747c');
