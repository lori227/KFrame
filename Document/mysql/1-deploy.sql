/*
Navicat MySQL Data Transfer

Source Server         : 外网中控版本
Source Server Version : 50722
Source Host           : 118.25.173.109:3306
Source Database       : kframe

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2018-08-31 18:30:53
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
  `logtype` varchar(50) NOT NULL DEFAULT '2.0' COMMENT '日志类型',
  `startup` int(10) unsigned NOT NULL COMMENT '是否已经关闭',
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
INSERT INTO `deploy` VALUES ('1.0.11.1', 'data', 'master', '2.0', '1', '0', '5747', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.111.1', 'log', 'master', '1.0', '1', '0', '5754', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.112.1', 'log', 'proxy', '1.0', '1', '0', '5765', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.113.1', 'log', 'shard', '1.0', '1', '0', '5777', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.12.1', 'data', 'proxy', '2.0', '1', '0', '5789', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.121.1', 'dir', 'master', '2.0', '1', '0', '7281', '1535011480', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.122.1', 'dir', 'proxy', '2.0', '1', '0', '7739', '1535011950', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.123.1', 'dir', 'shard', '2.0', '1', '0', '7288', '1535011480', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.13.1', 'data', 'shard', '2.0', '1', '0', '5813', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.21.1', 'public', 'master', '2.0', '1', '0', '5820', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.22.1', 'public', 'proxy', '2.0', '1', '0', '5832', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.23.1', 'public', 'shard', '2.0', '1', '0', '5848', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.31.1', 'route', 'master', '2.0', '1', '0', '5871', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.32.1', 'route', 'proxy', '2.0', '1', '0', '5890', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.33.1', 'route', 'shard', '2.0', '1', '0', '5904', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.4.1', 'auth', 'auth', '2.0', '1', '0', '5911', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.41.1', 'match', 'master', '2.0', '1', '0', '5924', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.42.1', 'match', 'proxy', '2.0', '1', '0', '5939', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.43.1', 'match', 'shard', '2.0', '1', '0', '5949', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.51.1', 'battle', 'master', '2.0', '1', '0', '5962', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.52.1', 'battle', 'proxy', '2.0', '1', '0', '5978', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.53.1', 'battle', 'shard', '2.0', '1', '0', '5991', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.61.1', 'mail', 'master', '2.0', '1', '0', '6004', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.62.1', 'mail', 'proxy', '2.0', '1', '0', '6017', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.63.1', 'mail', 'shard', '2.0', '1', '0', '6031', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.71.1', 'relation', 'master', '2.0', '1', '0', '6050', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.72.1', 'relation', 'proxy', '2.0', '1', '0', '6057', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.73.1', 'relation', 'shard', '2.0', '1', '0', '6070', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.81.1', 'group', 'master', '2.0', '1', '0', '6077', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.82.1', 'group', 'proxy', '2.0', '1', '0', '6085', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.83.1', 'group', 'shard', '2.0', '1', '0', '6098', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.91.1', 'rank', 'master', '2.0', '1', '0', '6112', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.92.1', 'rank', 'proxy', '2.0', '1', '0', '6138', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.0.93.1', 'rank', 'shard', '2.0', '1', '0', '6157', '1535010762', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.1.241.1', 'zone', 'master', '2.0', '1', '0', '6494', '1535010936', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.1.242.1', 'zone', 'world', '2.0', '1', '0', '6501', '1535010936', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.1.243.1', 'zone', 'gate', '2.0', '1', '0', '6508', '1535010936', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.1.244.1', 'zone', 'login', '2.0', '1', '0', '6515', '1535010936', '1.0.3.1', '10.105.6.78');
INSERT INTO `deploy` VALUES ('1.1.245.1', 'zone', 'game', '2.0', '1', '0', '6531', '1535010936', '1.0.3.1', '10.105.6.78');
