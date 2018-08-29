/*
Navicat MySQL Data Transfer

Source Server         : 192.168.1.9
Source Server Version : 50640
Source Host           : 192.168.1.9:3306
Source Database       : kframe

Target Server Type    : MYSQL
Target Server Version : 50640
File Encoding         : 65001

Date: 2018-08-29 20:25:51
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `deploy`
-- ----------------------------
DROP TABLE IF EXISTS `deploy`;
CREATE TABLE `deploy` (
  `appid` varchar(50) NOT NULL COMMENT '服务器id',
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
  PRIMARY KEY (`appid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of deploy
-- ----------------------------
INSERT INTO `deploy` VALUES ('101.0.11.1', 'data', 'master', '2.0', '1', '0', '0', '31549', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.111.1', 'log', 'master', '1.0', '1', '0', '0', '31560', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.112.1', 'log', 'proxy', '1.0', '1', '0', '0', '31575', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.113.1', 'log', 'shard', '1.0', '1', '0', '0', '31591', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.12.1', 'data', 'proxy', '2.0', '1', '0', '0', '31617', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.121.1', 'dir', 'master', '2.0', '1', '0', '0', '31630', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.122.1', 'dir', 'proxy', '2.0', '1', '0', '0', '31641', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.123.1', 'dir', 'shard', '2.0', '1', '0', '0', '31652', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.13.1', 'data', 'shard', '2.0', '1', '0', '0', '31667', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.21.1', 'public', 'master', '2.0', '1', '0', '0', '31681', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.22.1', 'public', 'proxy', '2.0', '1', '0', '0', '31694', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.23.1', 'public', 'shard', '2.0', '1', '0', '0', '31708', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.31.1', 'route', 'master', '2.0', '1', '0', '0', '31722', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.32.1', 'route', 'proxy', '2.0', '1', '0', '0', '31736', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.33.1', 'route', 'shard', '2.0', '1', '0', '0', '31750', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.4.1', 'auth', 'auth', '2.0', '1', '0', '0', '31763', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.41.1', 'match', 'master', '2.0', '1', '0', '0', '31779', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.42.1', 'match', 'proxy', '2.0', '1', '0', '0', '31792', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.43.1', 'match', 'shard', '2.0', '1', '0', '0', '31806', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.51.1', 'battle', 'master', '2.0', '1', '0', '0', '31820', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.52.1', 'battle', 'proxy', '2.0', '1', '0', '0', '31834', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.53.1', 'battle', 'shard', '2.0', '1', '0', '0', '31848', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.61.1', 'mail', 'master', '2.0', '1', '0', '0', '31863', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.62.1', 'mail', 'proxy', '2.0', '1', '0', '0', '31880', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.63.1', 'mail', 'shard', '2.0', '1', '0', '0', '31894', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.71.1', 'relation', 'master', '2.0', '1', '0', '0', '31910', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.72.1', 'relation', 'proxy', '2.0', '1', '0', '0', '31925', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.73.1', 'relation', 'shard', '2.0', '1', '0', '0', '31938', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.81.1', 'group', 'master', '2.0', '1', '0', '0', '31953', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.82.1', 'group', 'proxy', '2.0', '1', '0', '0', '31967', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.83.1', 'group', 'shard', '2.0', '1', '0', '0', '31980', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.91.1', 'rank', 'master', '2.0', '1', '0', '0', '31994', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.92.1', 'rank', 'proxy', '2.0', '1', '0', '0', '32007', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.93.1', 'rank', 'shard', '2.0', '1', '0', '0', '32021', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.1.241.1', 'zone', 'master', '2.0', '1', '0', '0', '32035', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.1.242.1', 'zone', 'world', '2.0', '1', '0', '0', '32049', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.1.243.1', 'zone', 'gate', '2.0', '1', '0', '0', '32065', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.1.244.1', 'zone', 'login', '2.0', '1', '0', '0', '32078', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.1.245.1', 'zone', 'game', '2.0', '1', '0', '0', '32092', '1535532492', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.3.241.1', 'zone', 'master', '2.0', '1', '0', '0', '25907', '1535532346', '101.0.3.227', '192.168.10.230');
INSERT INTO `deploy` VALUES ('101.3.242.1', 'zone', 'world', '2.0', '1', '0', '0', '25917', '1535532346', '101.0.3.227', '192.168.10.230');
INSERT INTO `deploy` VALUES ('101.3.243.1', 'zone', 'gate', '2.0', '1', '0', '0', '25930', '1535532346', '101.0.3.227', '192.168.10.230');
INSERT INTO `deploy` VALUES ('101.3.244.1', 'zone', 'login', '2.0', '1', '0', '0', '25940', '1535532346', '101.0.3.227', '192.168.10.230');
INSERT INTO `deploy` VALUES ('101.3.245.1', 'zone', 'game', '2.0', '1', '0', '0', '25950', '1535532346', '101.0.3.227', '192.168.10.230');
