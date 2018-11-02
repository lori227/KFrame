/*
Navicat MySQL Data Transfer

Source Server         : 内网测试
Source Server Version : 50640
Source Host           : 192.168.1.9:3306
Source Database       : kframe

Target Server Type    : MYSQL
Target Server Version : 50640
File Encoding         : 65001

Date: 2018-10-29 09:53:09
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
  `appchannel` int(10) NOT NULL DEFAULT '0',
  `zoneid` int(10) NOT NULL DEFAULT '0' COMMENT '如果是小区, 这个是zoneid',
  `logtype` varchar(50) NOT NULL DEFAULT '1.0' COMMENT 'x.y x 0 不打日志 1 本地日志  2远程日志 y 日志级别',
  `service` varchar(50) NOT NULL DEFAULT '1.2',
  `startup` int(10) unsigned NOT NULL COMMENT '是否已经关闭',
  `debug` int(10) NOT NULL DEFAULT '0' COMMENT '1=debug 0 release',
  `shutdown` int(10) NOT NULL DEFAULT '0',
  `process` int(10) unsigned NOT NULL COMMENT '进程id',
  `time` bigint(20) unsigned NOT NULL COMMENT '状态时间( 启动或者关闭 )',
  `agentid` varchar(50) NOT NULL COMMENT '部署agentid',
  `localip` varchar(50) NOT NULL COMMENT '局域网ip',
  `version` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`appid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of deploy
-- ----------------------------
INSERT INTO `deploy` VALUES ('101.0.11.1', 'data', 'master', '101', '0', '2.1', '1.2', '1', '1', '0', '16949', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.111.1', 'log', 'master', '101', '0', '1.1', '1.2', '1', '1', '0', '16960', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.112.1', 'log', 'proxy', '101', '0', '1.1', '1.2', '1', '1', '0', '16973', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.113.1', 'log', 'shard', '101', '0', '1.1', '1.2', '1', '1', '0', '16985', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.12.1', 'data', 'proxy', '101', '0', '2.1', '1.2', '1', '1', '0', '16997', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.121.1', 'dir', 'master', '101', '0', '2.1', '1.2', '1', '1', '0', '17009', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.122.1', 'dir', 'proxy', '101', '0', '2.1', '1.2', '1', '1', '0', '17019', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.123.1', 'dir', 'shard', '101', '0', '2.1', '1.2', '1', '1', '0', '17030', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.13.1', 'data', 'shard', '101', '0', '2.1', '1.2', '1', '1', '0', '17043', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.21.1', 'public', 'master', '101', '0', '2.1', '1.2', '1', '1', '0', '17055', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.22.1', 'public', 'proxy', '101', '0', '2.1', '1.2', '1', '1', '0', '17066', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.23.1', 'public', 'shard', '101', '0', '2.1', '1.2', '1', '1', '0', '17078', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.31.1', 'route', 'master', '101', '0', '2.1', '1.2', '1', '1', '0', '17089', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.32.1', 'route', 'proxy', '101', '0', '2.1', '1.2', '1', '1', '0', '17100', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.33.1', 'route', 'shard', '101', '0', '2.1', '1.2', '1', '1', '0', '17112', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.4.1', 'auth', 'auth', '101', '0', '2.1', '1.2', '1', '1', '0', '17123', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.41.1', 'match', 'master', '101', '0', '2.1', '1.2', '1', '1', '0', '17137', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.42.1', 'match', 'proxy', '101', '0', '2.1', '1.2', '1', '1', '0', '17148', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.43.1', 'match', 'shard', '101', '0', '2.1', '1.2', '1', '1', '0', '17159', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.51.1', 'battle', 'master', '101', '0', '2.1', '1.2', '1', '1', '0', '17171', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.52.1', 'battle', 'proxy', '101', '0', '2.1', '1.2', '1', '1', '0', '17182', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.53.1', 'battle', 'shard', '101', '0', '2.1', '1.2', '1', '1', '0', '17193', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.61.1', 'mail', 'master', '101', '0', '2.1', '1.2', '1', '1', '0', '17205', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.62.1', 'mail', 'proxy', '101', '0', '2.1', '1.2', '1', '1', '0', '17216', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.63.1', 'mail', 'shard', '101', '0', '2.1', '1.2', '1', '1', '0', '17228', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.71.1', 'relation', 'master', '101', '0', '2.1', '1.2', '1', '1', '0', '17239', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.72.1', 'relation', 'proxy', '101', '0', '2.1', '1.2', '1', '1', '0', '17251', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.73.1', 'relation', 'shard', '101', '0', '2.1', '1.2', '1', '1', '0', '17262', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.81.1', 'group', 'master', '101', '0', '2.1', '1.2', '1', '1', '0', '17274', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.82.1', 'group', 'proxy', '101', '0', '2.1', '1.2', '1', '1', '0', '17285', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.83.1', 'group', 'shard', '101', '0', '2.1', '1.2', '1', '1', '0', '17298', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.91.1', 'rank', 'master', '101', '0', '2.1', '1.2', '1', '1', '0', '17312', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.92.1', 'rank', 'proxy', '101', '0', '2.1', '1.2', '1', '1', '0', '17323', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.0.93.1', 'rank', 'shard', '101', '0', '2.1', '1.2', '1', '1', '0', '17334', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.1.241.1', 'zone', 'master', '101', '1', '2.1', '1.2', '1', '1', '0', '17346', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.1.242.1', 'zone', 'world', '101', '1', '2.1', '1.2', '1', '1', '0', '17358', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.1.243.1', 'zone', 'gate', '101', '1', '2.1', '1.2', '1', '1', '0', '17372', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.1.244.1', 'zone', 'login', '101', '1', '2.1', '1.2', '1', '1', '0', '17383', '1540542948', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
INSERT INTO `deploy` VALUES ('101.1.245.1', 'zone', 'game', '101', '1', '2.1', '1.2', '1', '1', '0', '39218', '1540555234', '101.0.3.1', '192.168.1.9', 'sgame_internal_292_201810261618');
