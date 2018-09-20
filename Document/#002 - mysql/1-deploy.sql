/*
Navicat MySQL Data Transfer

Source Server         : 点晴中控180.169.238.146
Source Server Version : 50722
Source Host           : 180.169.238.146:13306
Source Database       : kframe

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2018-09-20 10:18:37
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `deploy`
-- ----------------------------
DROP TABLE IF EXISTS `deploy`;
CREATE TABLE `deploy` (
  `appid` varchar(50) NOT NULL COMMENT '服务器id',
  `appchannel` int(10) NOT NULL DEFAULT '0',
  `appname` varchar(50) NOT NULL COMMENT '服务器名字',
  `apptype` varchar(50) NOT NULL COMMENT '服务器类型',
  `logtype` varchar(50) NOT NULL DEFAULT '2.0' COMMENT '日志类型',
  `debug` int(10) NOT NULL DEFAULT '0' COMMENT '是否debug',
  `startup` int(10) unsigned NOT NULL COMMENT '是否已经关闭',
  `shutdown` int(10) NOT NULL DEFAULT '0',
  `process` int(10) unsigned NOT NULL COMMENT '进程id',
  `time` bigint(20) unsigned NOT NULL COMMENT '状态时间( 启动或者关闭 )',
  `agentid` varchar(50) NOT NULL COMMENT '部署agentid',
  `localip` varchar(50) NOT NULL COMMENT '局域网ip',
  `version` varchar(50) NOT NULL DEFAULT '' COMMENT '版本号',
  PRIMARY KEY (`appid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of deploy
-- ----------------------------
INSERT INTO `deploy` VALUES ('1.0.11.1', '1', 'data', 'master', '2.0', '0', '1', '0', '13979', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.111.1', '1', 'log', 'master', '1.0', '0', '1', '0', '13986', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.112.1', '1', 'log', 'proxy', '1.0', '0', '1', '0', '13993', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.113.1', '1', 'log', 'shard', '1.0', '0', '1', '0', '14001', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.12.1', '1', 'data', 'proxy', '2.0', '0', '1', '0', '14008', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.121.1', '1', 'dir', 'master', '2.0', '0', '1', '0', '14015', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.122.1', '1', 'dir', 'proxy', '2.0', '0', '1', '0', '14025', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.123.1', '1', 'dir', 'shard', '2.0', '0', '1', '0', '14032', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.13.1', '1', 'data', 'shard', '2.0', '0', '1', '0', '14039', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.21.1', '1', 'public', 'master', '2.0', '0', '1', '0', '14047', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.22.1', '1', 'public', 'proxy', '2.0', '0', '1', '0', '14060', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.23.1', '1', 'public', 'shard', '2.0', '0', '1', '0', '14108', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.31.1', '1', 'route', 'master', '2.0', '0', '1', '0', '14116', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.32.1', '1', 'route', 'proxy', '2.0', '0', '1', '0', '14123', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.33.1', '1', 'route', 'shard', '2.0', '0', '1', '0', '14134', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.4.1', '1', 'auth', 'auth', '2.0', '0', '1', '0', '14149', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.41.1', '1', 'match', 'master', '2.0', '0', '1', '0', '14156', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.42.1', '1', 'match', 'proxy', '2.0', '0', '1', '0', '14176', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.43.1', '1', 'match', 'shard', '2.0', '0', '1', '0', '14195', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.51.1', '1', 'battle', 'master', '2.0', '0', '1', '0', '14213', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.52.1', '1', 'battle', 'proxy', '2.0', '0', '1', '0', '14229', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.53.1', '1', 'battle', 'shard', '2.0', '0', '1', '0', '14243', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.61.1', '1', 'mail', 'master', '2.0', '0', '1', '0', '14250', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.62.1', '1', 'mail', 'proxy', '2.0', '0', '1', '0', '14257', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.63.1', '1', 'mail', 'shard', '2.0', '0', '1', '0', '14267', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.71.1', '1', 'relation', 'master', '2.0', '0', '1', '0', '14281', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.72.1', '1', 'relation', 'proxy', '2.0', '0', '1', '0', '14298', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.73.1', '1', 'relation', 'shard', '2.0', '0', '1', '0', '14327', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.81.1', '1', 'group', 'master', '2.0', '0', '1', '0', '14337', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.82.1', '1', 'group', 'proxy', '2.0', '0', '1', '0', '14353', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.83.1', '1', 'group', 'shard', '2.0', '0', '1', '0', '14361', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.91.1', '1', 'rank', 'master', '2.0', '0', '1', '0', '14368', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.92.1', '1', 'rank', 'proxy', '2.0', '0', '1', '0', '14375', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.0.93.1', '1', 'rank', 'shard', '2.0', '0', '1', '0', '14388', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.1.241.1', '1', 'zone', 'master', '2.0', '0', '1', '0', '14410', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.1.242.1', '1', 'zone', 'world', '2.0', '0', '1', '0', '14433', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.1.243.1', '1', 'zone', 'gate', '2.0', '0', '1', '0', '14448', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.1.244.1', '1', 'zone', 'login', '2.0', '0', '1', '0', '14459', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
INSERT INTO `deploy` VALUES ('1.1.245.1', '1', 'zone', 'game', '2.0', '0', '1', '0', '14475', '1536310413', '1.0.3.1', '10.105.6.78', 'sgame_online_svn_95_201809071648');
