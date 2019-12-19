/*
Navicat MySQL Data Transfer

Source Server         : #100 - 战旗版本机
Source Server Version : 50722
Source Host           : 192.168.2.31:3306
Source Database       : kfdeploy

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2019-12-19 12:56:36
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
-- Records of 1_1_deploy
-- ----------------------------
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'auth', 'auth', '21.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '13750', '1576685549', '11.0.1', 'chess_develop_1.1.263.1232.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'master', '22.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '13788', '1576685549', '11.0.1', 'chess_develop_1.1.263.1232.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'proxy', '23.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '13811', '1576685549', '11.0.1', 'chess_develop_1.1.263.1232.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'shard', '24.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '13822', '1576685549', '11.0.1', 'chess_develop_1.1.263.1232.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'data', 'shard', '31.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '13799', '1576685549', '11.0.1', 'chess_develop_1.1.263.1232.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'logic', 'shard', '32.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '13763', '1576685549', '11.0.1', 'chess_develop_1.1.263.1232.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'match', 'shard', '38.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '13857', '1576685549', '11.0.1', 'chess_develop_1.1.263.1232.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'room', 'shard', '39.0.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '13833', '1576685549', '11.0.1', 'chess_develop_1.1.263.1232.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'master', '91.1.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '13773', '1576685549', '11.0.1', 'chess_develop_1.1.263.1232.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'world', '92.1.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '13843', '1576685549', '11.0.1', 'chess_develop_1.1.263.1232.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'gate', '93.1.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '13728', '1576685549', '11.0.1', 'chess_develop_1.1.263.1232.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'login', '94.1.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '13739', '1576685549', '11.0.1', 'chess_develop_1.1.263.1232.tar.gz');
INSERT INTO `1_1_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'game', '95.1.1', '1.1', '1', '', '12.0.1', '0', '1', '1', '0', '13720', '1576685549', '11.0.1', 'chess_develop_1.1.263.1232.tar.gz');

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
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'auth', 'auth', '21.0.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '19102', '1576685310', '11.0.2', 'chess_config_1.1.263.1232.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'master', '22.0.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '19140', '1576685310', '11.0.2', 'chess_config_1.1.263.1232.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'proxy', '23.0.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '19162', '1576685310', '11.0.2', 'chess_config_1.1.263.1232.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'route', 'shard', '24.0.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '19177', '1576685310', '11.0.2', 'chess_config_1.1.263.1232.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'data', 'shard', '31.0.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '19151', '1576685310', '11.0.2', 'chess_config_1.1.263.1232.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'logic', 'shard', '32.0.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '19113', '1576685310', '11.0.2', 'chess_config_1.1.263.1232.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'match', 'shard', '38.0.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '19206', '1576685310', '11.0.2', 'chess_config_1.1.263.1232.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'room', 'shard', '39.0.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '19188', '1576685310', '11.0.2', 'chess_config_1.1.263.1232.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'master', '91.1.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '19126', '1576685310', '11.0.2', 'chess_config_1.1.263.1232.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'world', '92.1.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '19196', '1576685310', '11.0.2', 'chess_config_1.1.263.1232.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'gate', '93.1.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '19083', '1576685310', '11.0.2', 'chess_config_1.1.263.1232.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'login', '94.1.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '19091', '1576685310', '11.0.2', 'chess_config_1.1.263.1232.tar.gz');
INSERT INTO `1_2_deploy` VALUES ('/data/chess', '', 'bin/KFStartup', 'zone', 'game', '95.1.1', '1.2', '1', '', '12.0.2', '0', '1', '1', '0', '19075', '1576685310', '11.0.2', 'chess_config_1.1.263.1232.tar.gz');

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
INSERT INTO `agent` VALUES ('192.168.2.30', '192.168.2.30', '12.0.2', '1.2', '11.0.2', '192.168.2.30', '10000', '1', '12001', '');
INSERT INTO `agent` VALUES ('192.168.2.31', '192.168.2.31', '12.0.1', '1.1', '11.0.1', '192.168.2.31', '10000', '1', '12001', '');

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
INSERT INTO `version` VALUES ('2019-12-10 13:40:12', 'chess_config_1.1.254.1178.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.254.1178.tar.gz', '43d5cd73d88a9dfc01309ff47f7264ad');
INSERT INTO `version` VALUES ('2019-12-10 21:49:03', 'chess_config_1.1.254.1181.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.254.1181.tar.gz', '9d36b9c8feea92299cf6889a7bfb74e6');
INSERT INTO `version` VALUES ('2019-12-11 11:53:10', 'chess_config_1.1.255.1182.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.255.1182.tar.gz', '43f4f3204894127e05c8f6cca108349f');
INSERT INTO `version` VALUES ('2019-12-11 19:23:44', 'chess_config_1.1.255.1185.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.255.1185.tar.gz', '7323b4e1f6ed61f366d1a651f7a600e2');
INSERT INTO `version` VALUES ('2019-12-12 20:11:54', 'chess_config_1.1.256.1187.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.256.1187.tar.gz', 'ec557014207cdca3196816a034f04c55');
INSERT INTO `version` VALUES ('2019-12-12 23:01:51', 'chess_config_1.1.256.1188.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.256.1188.tar.gz', '641f0370c0d6b15d1c2bc15ee218ffd9');
INSERT INTO `version` VALUES ('2019-12-13 17:21:44', 'chess_config_1.1.257.1190.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.257.1190.tar.gz', 'cc5e918c01ed2f4b893d6c0f0452ff68');
INSERT INTO `version` VALUES ('2019-12-13 17:59:57', 'chess_config_1.1.257.1191.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.257.1191.tar.gz', '28b8e59e379fab7ecafedcfeb24b861f');
INSERT INTO `version` VALUES ('2019-12-16 10:29:44', 'chess_config_1.1.260.1192.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.260.1192.tar.gz', '56be2e0d0678bd6930fc9c201cd59d3a');
INSERT INTO `version` VALUES ('2019-12-16 11:21:28', 'chess_config_1.1.260.1193.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.260.1193.tar.gz', 'aa080ddd7ab57a95c17a72e07a653244');
INSERT INTO `version` VALUES ('2019-12-17 15:14:45', 'chess_config_1.1.261.1206.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.261.1206.tar.gz', 'c4a2e10c0ba6a047038d54ba257773d9');
INSERT INTO `version` VALUES ('2019-12-17 15:50:25', 'chess_config_1.1.261.1208.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.261.1208.tar.gz', 'a734b581a6c853bde660d83c5e504f99');
INSERT INTO `version` VALUES ('2019-12-17 17:34:50', 'chess_config_1.1.261.1214.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.261.1214.tar.gz', 'a58c98f3fbd3b3de3d38e1ac109798d6');
INSERT INTO `version` VALUES ('2019-12-17 21:44:01', 'chess_config_1.1.261.1218.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.261.1218.tar.gz', '3bb76e531f4de83f673fef2e053dc055');
INSERT INTO `version` VALUES ('2019-12-17 21:46:47', 'chess_config_1.1.261.1219.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.261.1219.tar.gz', '8e13e429bf95c494060ac96498a31a26');
INSERT INTO `version` VALUES ('2019-12-17 21:50:26', 'chess_config_1.1.261.1220.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.261.1220.tar.gz', 'd2f727d9ac5cc5fefb292514c39be18a');
INSERT INTO `version` VALUES ('2019-12-18 14:38:19', 'chess_config_1.1.262.1224.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.262.1224.tar.gz', 'f888030a9e6707a71fe3cf627fae68de');
INSERT INTO `version` VALUES ('2019-12-18 15:55:16', 'chess_config_1.1.262.1226.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.262.1226.tar.gz', '912d7ae75ce23f4e24e539aba4b7f178');
INSERT INTO `version` VALUES ('2019-12-18 22:29:23', 'chess_config_1.1.262.1232.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.262.1232.tar.gz', '9fd5b13053627b14a12566d259757faa');
INSERT INTO `version` VALUES ('2019-12-19 00:07:49', 'chess_config_1.1.263.1232.tar.gz', 'http://192.168.2.31/upload/chess_config_1.1.263.1232.tar.gz', '183fe8eb9771f876dd4606b6a5462657');
INSERT INTO `version` VALUES ('2019-12-10 13:32:44', 'chess_config_1_2_1.1.254.1177.tar.gz', 'http://192.168.2.31/upload/chess_config_1_2_1.1.254.1177.tar.gz', '673acc0c2abad5e040ed40380efcaee8');
INSERT INTO `version` VALUES ('2019-12-10 13:38:51', 'chess_develop_1.1.254.1178.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.254.1178.tar.gz', '8a1ff407b96667a7534303ae8f2a595c');
INSERT INTO `version` VALUES ('2019-12-10 21:47:33', 'chess_develop_1.1.254.1181.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.254.1181.tar.gz', 'cec72c15eaad45467a0cff4fa830782e');
INSERT INTO `version` VALUES ('2019-12-11 11:51:46', 'chess_develop_1.1.255.1182.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.255.1182.tar.gz', '796ae41dd596b621173195ea007fc856');
INSERT INTO `version` VALUES ('2019-12-11 19:22:24', 'chess_develop_1.1.255.1185.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.255.1185.tar.gz', 'eca3a4d0d1fa74484e1fc7a85845d4c6');
INSERT INTO `version` VALUES ('2019-12-12 20:10:33', 'chess_develop_1.1.256.1187.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.256.1187.tar.gz', '4f57a3e488039db4e027298d0347d477');
INSERT INTO `version` VALUES ('2019-12-12 23:00:20', 'chess_develop_1.1.256.1188.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.256.1188.tar.gz', '82b2b760593bd546923ab8b16055bcb4');
INSERT INTO `version` VALUES ('2019-12-13 17:20:17', 'chess_develop_1.1.257.1190.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.257.1190.tar.gz', '83e0613e52ebe626fad26006d0a4063e');
INSERT INTO `version` VALUES ('2019-12-13 17:58:32', 'chess_develop_1.1.257.1191.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.257.1191.tar.gz', 'cfed40b004fd834f8614f07b9f48c0e0');
INSERT INTO `version` VALUES ('2019-12-16 10:28:54', 'chess_develop_1.1.260.1192.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.260.1192.tar.gz', 'dc9cd2e14a12cc30cd21fe09f7c28556');
INSERT INTO `version` VALUES ('2019-12-16 11:20:05', 'chess_develop_1.1.260.1193.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.260.1193.tar.gz', '739a8a276cc470c315a0108896465ab9');
INSERT INTO `version` VALUES ('2019-12-17 15:54:46', 'chess_develop_1.1.261.1208.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.261.1208.tar.gz', 'cc65d876b24fafd9ff4d7fd9430e87c0');
INSERT INTO `version` VALUES ('2019-12-17 17:33:23', 'chess_develop_1.1.261.1214.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.261.1214.tar.gz', 'ca3d50c57ab7ce701452188a6daadeaf');
INSERT INTO `version` VALUES ('2019-12-17 21:43:06', 'chess_develop_1.1.261.1218.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.261.1218.tar.gz', '778d7a6742a3e53864a43da7a0b9ecff');
INSERT INTO `version` VALUES ('2019-12-17 21:45:22', 'chess_develop_1.1.261.1219.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.261.1219.tar.gz', '7565635c74fc06d7ca5aecec113cd007');
INSERT INTO `version` VALUES ('2019-12-17 21:48:58', 'chess_develop_1.1.261.1220.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.261.1220.tar.gz', '7871c631d4ca65e522e173fde9e57ae0');
INSERT INTO `version` VALUES ('2019-12-18 14:36:44', 'chess_develop_1.1.262.1224.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.262.1224.tar.gz', '773311fe802273cef9a9d85b5ec3df38');
INSERT INTO `version` VALUES ('2019-12-18 15:53:42', 'chess_develop_1.1.262.1226.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.262.1226.tar.gz', '3062aa4a28a9f7a13b212fcd4abdc8a4');
INSERT INTO `version` VALUES ('2019-12-18 22:11:36', 'chess_develop_1.1.262.1232.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.262.1232.tar.gz', '72f20d45a3959502bd57c2ba768c9769');
INSERT INTO `version` VALUES ('2019-12-19 00:11:34', 'chess_develop_1.1.263.1232.tar.gz', 'http://192.168.2.31/upload/chess_develop_1.1.263.1232.tar.gz', '73d127987cfadd836a13636b76f70827');
INSERT INTO `version` VALUES ('2019-12-03 16:46:15', 'chess_develop_1_1_1.1.247.1078.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.247.1078.tar.gz', 'ecd11c336c9fa1db984b04601394289f');
INSERT INTO `version` VALUES ('2019-12-04 14:21:19', 'chess_develop_1_1_1.1.248.1081.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.248.1081.tar.gz', 'dbe78abababb3fb87bad24e7c01fa977');
INSERT INTO `version` VALUES ('2019-12-04 17:12:14', 'chess_develop_1_1_1.1.248.1082.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.248.1082.tar.gz', 'b051a4fd39a05de38861614cee73f3d5');
INSERT INTO `version` VALUES ('2019-12-04 17:30:33', 'chess_develop_1_1_1.1.248.1084.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.248.1084.tar.gz', 'cf3865f56766c6e195bbb2148b6ed01f');
INSERT INTO `version` VALUES ('2019-12-04 17:37:03', 'chess_develop_1_1_1.1.248.1085.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.248.1085.tar.gz', 'c71321e54a9a6459219e2758e47b45b2');
INSERT INTO `version` VALUES ('2019-12-04 18:23:01', 'chess_develop_1_1_1.1.248.1087.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.248.1087.tar.gz', '83823754f0ef111ff78eb778e5e28ed8');
INSERT INTO `version` VALUES ('2019-12-04 20:39:36', 'chess_develop_1_1_1.1.248.1091.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.248.1091.tar.gz', '8da26fd9dc00d5f8adf1c1f5282b91f8');
INSERT INTO `version` VALUES ('2019-12-04 20:43:39', 'chess_develop_1_1_1.1.248.1092.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.248.1092.tar.gz', '6ab7e3da6f3b2a863c7cc5b70c4bf4d8');
INSERT INTO `version` VALUES ('2019-12-04 20:46:49', 'chess_develop_1_1_1.1.248.1093.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.248.1093.tar.gz', '80a21dc77c765ecbed0c8fd66a1a7901');
INSERT INTO `version` VALUES ('2019-12-05 12:50:57', 'chess_develop_1_1_1.1.249.1107.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.249.1107.tar.gz', '08738ee0fc2e82abc46520b898e8f899');
INSERT INTO `version` VALUES ('2019-12-05 13:13:52', 'chess_develop_1_1_1.1.249.1108.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.249.1108.tar.gz', '9e4841770f0e6a51118bcc24e0c0649c');
INSERT INTO `version` VALUES ('2019-12-05 14:57:35', 'chess_develop_1_1_1.1.249.1110.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.249.1110.tar.gz', '4f555a01e4eb4fa426352b18601aad39');
INSERT INTO `version` VALUES ('2019-12-05 19:57:09', 'chess_develop_1_1_1.1.249.1112.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.249.1112.tar.gz', '31988632d14faa1f5a83bd01914adc09');
INSERT INTO `version` VALUES ('2019-12-05 20:40:17', 'chess_develop_1_1_1.1.249.1113.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.249.1113.tar.gz', '3a57da05cf0842c823ea81fb31ab71d4');
INSERT INTO `version` VALUES ('2019-12-05 21:16:52', 'chess_develop_1_1_1.1.249.1116.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.249.1116.tar.gz', '87e6a4edd37d09a4850247eb7f0aea1e');
INSERT INTO `version` VALUES ('2019-12-05 22:25:50', 'chess_develop_1_1_1.1.249.1117.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.249.1117.tar.gz', 'de921e7892d70e8c633d632c1cb75514');
INSERT INTO `version` VALUES ('2019-12-06 11:54:54', 'chess_develop_1_1_1.1.250.1118.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.250.1118.tar.gz', 'b6f793c99fac7535eb91b325fc55eb0b');
INSERT INTO `version` VALUES ('2019-12-06 13:45:32', 'chess_develop_1_1_1.1.250.1119.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.250.1119.tar.gz', '6cdeaee24b2ec60480db670ec95a05a9');
INSERT INTO `version` VALUES ('2019-12-06 14:14:53', 'chess_develop_1_1_1.1.250.1121.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.250.1121.tar.gz', '704559e20c7bbcc14cf15fa1bcad3c9e');
INSERT INTO `version` VALUES ('2019-12-06 15:33:16', 'chess_develop_1_1_1.1.250.1126.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.250.1126.tar.gz', 'd3e9437c40a6e3b2bd9b8a496034fbb7');
INSERT INTO `version` VALUES ('2019-12-06 17:56:26', 'chess_develop_1_1_1.1.250.1127.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.250.1127.tar.gz', 'cabc32044f883cb7571bc84a79310861');
INSERT INTO `version` VALUES ('2019-12-06 21:51:48', 'chess_develop_1_1_1.1.250.1131.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.250.1131.tar.gz', '08404da1ed7d50ef1b56ccd527f1d86d');
INSERT INTO `version` VALUES ('2019-12-06 22:12:31', 'chess_develop_1_1_1.1.250.1133.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.250.1133.tar.gz', 'e29b0c3e8b092ccf642183711b561e64');
INSERT INTO `version` VALUES ('2019-12-06 23:12:11', 'chess_develop_1_1_1.1.250.1137.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.250.1137.tar.gz', '302f9f33312935c1e5810ba5a2bc0bf8');
INSERT INTO `version` VALUES ('2019-12-07 17:30:07', 'chess_develop_1_1_1.1.251.1140.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.251.1140.tar.gz', 'f22c1cde6598c3aaf298bef561b8f202');
INSERT INTO `version` VALUES ('2019-12-07 17:50:17', 'chess_develop_1_1_1.1.251.1142.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.251.1142.tar.gz', '0e3d6b216c4a6a7a11aeb93fdbbd3806');
INSERT INTO `version` VALUES ('2019-12-07 18:11:07', 'chess_develop_1_1_1.1.251.1143.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.251.1143.tar.gz', '67c1762e1913c966fe6e5898ad62f2da');
INSERT INTO `version` VALUES ('2019-12-09 19:59:47', 'chess_develop_1_1_1.1.253.1154.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.253.1154.tar.gz', '8d1b4e28cbd6e43b841580e71097fedd');
INSERT INTO `version` VALUES ('2019-12-09 21:15:10', 'chess_develop_1_1_1.1.253.1163.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.253.1163.tar.gz', '37c172fc851d45f87859c017da1be80f');
INSERT INTO `version` VALUES ('2019-12-09 21:23:37', 'chess_develop_1_1_1.1.253.1164.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.253.1164.tar.gz', 'd2302f9b152bed6e14217e469758482e');
INSERT INTO `version` VALUES ('2019-12-09 22:27:15', 'chess_develop_1_1_1.1.253.1171.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.253.1171.tar.gz', '5066fc81979e84d93fb7966806eeaed5');
INSERT INTO `version` VALUES ('2019-12-09 23:49:16', 'chess_develop_1_1_1.1.253.1176.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.253.1176.tar.gz', '2a96e46e93ba744bd5cc7ecab2c00b40');
INSERT INTO `version` VALUES ('2019-12-10 13:26:40', 'chess_develop_1_1_1.1.254.1177.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_1_1.1.254.1177.tar.gz', 'cfcf5923fbf99e89a11bafb588fe688d');
INSERT INTO `version` VALUES ('2019-12-03 14:49:20', 'chess_develop_1_2_1.1.247.1076.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.247.1076.tar.gz', '64bb1523e6b506b427145ad5e1058846');
INSERT INTO `version` VALUES ('2019-12-03 15:31:59', 'chess_develop_1_2_1.1.247.1077.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.247.1077.tar.gz', '591ee019d35b3072353c825e2dc4bf9c');
INSERT INTO `version` VALUES ('2019-12-03 16:46:40', 'chess_develop_1_2_1.1.247.1078.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.247.1078.tar.gz', 'd2ab359210a16f164005581ee01ca07a');
INSERT INTO `version` VALUES ('2019-12-04 14:22:49', 'chess_develop_1_2_1.1.248.1081.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.248.1081.tar.gz', 'dd799cdba841736264b9d22c5badfbf6');
INSERT INTO `version` VALUES ('2019-12-04 17:13:42', 'chess_develop_1_2_1.1.248.1082.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.248.1082.tar.gz', '7820fad1c0abc554d31b7ca3795b0b52');
INSERT INTO `version` VALUES ('2019-12-04 17:31:35', 'chess_develop_1_2_1.1.248.1084.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.248.1084.tar.gz', '6b5a99f087c811df0c0422c99b1b9c4b');
INSERT INTO `version` VALUES ('2019-12-04 17:38:30', 'chess_develop_1_2_1.1.248.1085.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.248.1085.tar.gz', '7e4ce8a6980fd31d2def51af69bb324e');
INSERT INTO `version` VALUES ('2019-12-04 18:24:27', 'chess_develop_1_2_1.1.248.1087.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.248.1087.tar.gz', '7b053e2d59aadf90ef8c0e800122eb79');
INSERT INTO `version` VALUES ('2019-12-04 20:40:23', 'chess_develop_1_2_1.1.248.1091.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.248.1091.tar.gz', '3b6bdcffea2bb3751659960f8fa75ba6');
INSERT INTO `version` VALUES ('2019-12-04 20:45:01', 'chess_develop_1_2_1.1.248.1092.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.248.1092.tar.gz', 'a27de55793f783d7455cac32e753328d');
INSERT INTO `version` VALUES ('2019-12-04 20:51:13', 'chess_develop_1_2_1.1.248.1094.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.248.1094.tar.gz', '4e2dde57e40bac37ffdddd98ff1a110a');
INSERT INTO `version` VALUES ('2019-12-04 21:42:16', 'chess_develop_1_2_1.1.248.1096.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.248.1096.tar.gz', '94e83d616ee6f2d19f8b76e08c967174');
INSERT INTO `version` VALUES ('2019-12-04 22:16:12', 'chess_develop_1_2_1.1.248.1099.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.248.1099.tar.gz', 'ce244a302c06b4340124626bda47a109');
INSERT INTO `version` VALUES ('2019-12-05 12:52:00', 'chess_develop_1_2_1.1.249.1107.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.249.1107.tar.gz', 'cd29335d71e2d049aa5262af6bbeb613');
INSERT INTO `version` VALUES ('2019-12-05 13:15:23', 'chess_develop_1_2_1.1.249.1108.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.249.1108.tar.gz', 'd83a05a0e51c92e8ac2c23a92b935622');
INSERT INTO `version` VALUES ('2019-12-05 14:58:59', 'chess_develop_1_2_1.1.249.1110.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.249.1110.tar.gz', 'b52cd5c5d92e63b94cfd2c33ef76aca1');
INSERT INTO `version` VALUES ('2019-12-05 19:58:26', 'chess_develop_1_2_1.1.249.1112.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.249.1112.tar.gz', '251c7b360d6b7954e3e897b09ea2b3dc');
INSERT INTO `version` VALUES ('2019-12-05 20:40:56', 'chess_develop_1_2_1.1.249.1113.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.249.1113.tar.gz', '4fb51d67b1ad1e6392b528efc89a6e65');
INSERT INTO `version` VALUES ('2019-12-05 21:06:34', 'chess_develop_1_2_1.1.249.1115.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.249.1115.tar.gz', '29a6e32e92fd42ca29fac90aeed7f377');
INSERT INTO `version` VALUES ('2019-12-05 21:18:17', 'chess_develop_1_2_1.1.249.1116.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.249.1116.tar.gz', 'f5c21d4c6a8a3be826062f5173a07a32');
INSERT INTO `version` VALUES ('2019-12-05 22:27:17', 'chess_develop_1_2_1.1.249.1117.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.249.1117.tar.gz', '049bad2e4983e436db3785b55f0ad7ab');
INSERT INTO `version` VALUES ('2019-12-06 11:56:26', 'chess_develop_1_2_1.1.250.1118.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.250.1118.tar.gz', '56002f1055b5c16220eac62b7c1ededb');
INSERT INTO `version` VALUES ('2019-12-06 13:46:51', 'chess_develop_1_2_1.1.250.1119.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.250.1119.tar.gz', 'f0208ecc07998f231a731e75122d9f17');
INSERT INTO `version` VALUES ('2019-12-06 14:16:17', 'chess_develop_1_2_1.1.250.1121.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.250.1121.tar.gz', '7659aef00580f39b34ffe1d6f445ad54');
INSERT INTO `version` VALUES ('2019-12-06 15:34:48', 'chess_develop_1_2_1.1.250.1126.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.250.1126.tar.gz', '0e2b494bbf10bf3b98cdeebad5f95c98');
INSERT INTO `version` VALUES ('2019-12-06 17:57:55', 'chess_develop_1_2_1.1.250.1127.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.250.1127.tar.gz', '712734492590a0cf709898d07382f441');
INSERT INTO `version` VALUES ('2019-12-06 21:53:22', 'chess_develop_1_2_1.1.250.1131.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.250.1131.tar.gz', '554071196d7863182f59d0051c14c2b5');
INSERT INTO `version` VALUES ('2019-12-06 22:13:54', 'chess_develop_1_2_1.1.250.1133.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.250.1133.tar.gz', 'b4173b775359d925b27cf9ea70dec09c');
INSERT INTO `version` VALUES ('2019-12-06 23:13:21', 'chess_develop_1_2_1.1.250.1137.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.250.1137.tar.gz', 'f4f47cc5491db972102d70d2b23d8a0e');
INSERT INTO `version` VALUES ('2019-12-07 17:31:26', 'chess_develop_1_2_1.1.251.1140.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.251.1140.tar.gz', '7da8901387a9cc99af65bd82d6f62f52');
INSERT INTO `version` VALUES ('2019-12-07 17:51:41', 'chess_develop_1_2_1.1.251.1142.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.251.1142.tar.gz', '17a3c8d9f21e06075b517800c01da354');
INSERT INTO `version` VALUES ('2019-12-07 18:12:33', 'chess_develop_1_2_1.1.251.1143.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.251.1143.tar.gz', '81c35a3878324de763675037d9da17c6');
INSERT INTO `version` VALUES ('2019-12-09 20:04:38', 'chess_develop_1_2_1.1.253.1154.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.253.1154.tar.gz', 'c974523fbdcdf6969d6c9609e2992bbc');
INSERT INTO `version` VALUES ('2019-12-09 21:16:12', 'chess_develop_1_2_1.1.253.1163.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.253.1163.tar.gz', '0cc75b3cc767bdbe9987027d9f09a9e1');
INSERT INTO `version` VALUES ('2019-12-09 21:25:05', 'chess_develop_1_2_1.1.253.1164.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.253.1164.tar.gz', '3438fc3486085cc7b49f24745c74c6ff');
INSERT INTO `version` VALUES ('2019-12-09 22:28:47', 'chess_develop_1_2_1.1.253.1171.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.253.1171.tar.gz', 'bbf9d743fab4d957fd9d8e85e72b502c');
INSERT INTO `version` VALUES ('2019-12-09 23:40:39', 'chess_develop_1_2_1.1.253.1175.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.253.1175.tar.gz', '5431890d6a8bbfa147f65f0ec28b4150');
INSERT INTO `version` VALUES ('2019-12-09 23:44:28', 'chess_develop_1_2_1.1.253.1176.tar.gz', 'http://192.168.2.31/upload/chess_develop_1_2_1.1.253.1176.tar.gz', '42bad8c4bc5c4a22e746a3c1e055c2eb');
