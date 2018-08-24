/*
Navicat MySQL Data Transfer

Source Server         : 192.168.1.9
Source Server Version : 50640
Source Host           : 192.168.1.9:3306
Source Database       : kframe

Target Server Type    : MYSQL
Target Server Version : 50640
File Encoding         : 65001

Date: 2018-08-23 16:47:04
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
INSERT INTO `deploy` VALUES ('101.0.11.1', 'data', 'master', '1', '0', '61599', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.111.1', 'log', 'master', '1', '0', '61607', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.112.1', 'log', 'proxy', '1', '0', '61624', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.113.1', 'log', 'shard', '1', '0', '61637', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.12.1', 'data', 'proxy', '1', '0', '61650', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.121.1', 'dir', 'master', '1', '0', '61664', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.122.1', 'dir', 'proxy', '1', '0', '61675', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.123.1', 'dir', 'shard', '1', '0', '61685', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.13.1', 'data', 'shard', '1', '0', '61699', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.21.1', 'public', 'master', '1', '0', '61716', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.22.1', 'public', 'proxy', '1', '0', '61729', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.23.1', 'public', 'shard', '1', '0', '61743', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.31.1', 'route', 'master', '1', '0', '61757', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.32.1', 'route', 'proxy', '1', '0', '61771', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.33.1', 'route', 'shard', '1', '0', '61784', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.4.1', 'auth', 'auth', '1', '0', '61797', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.41.1', 'match', 'master', '1', '0', '61805', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.42.1', 'match', 'proxy', '1', '0', '61827', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.43.1', 'match', 'shard', '1', '0', '61840', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.51.1', 'battle', 'master', '1', '0', '61853', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.52.1', 'battle', 'proxy', '1', '0', '61867', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.53.1', 'battle', 'shard', '1', '0', '61881', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.61.1', 'mail', 'master', '1', '0', '61896', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.62.1', 'mail', 'proxy', '1', '0', '61907', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.63.1', 'mail', 'shard', '1', '0', '61917', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.71.1', 'relation', 'master', '1', '0', '61928', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.72.1', 'relation', 'proxy', '1', '0', '61941', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.73.1', 'relation', 'shard', '1', '0', '61955', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.81.1', 'group', 'master', '1', '0', '61969', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.82.1', 'group', 'proxy', '1', '0', '61983', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.83.1', 'group', 'shard', '1', '0', '62009', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.91.1', 'rank', 'master', '1', '0', '62025', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.92.1', 'rank', 'proxy', '1', '0', '62040', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.0.93.1', 'rank', 'shard', '1', '0', '62063', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.1.241.1', 'zone', 'master', '1', '0', '62080', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.1.242.1', 'zone', 'world', '1', '0', '62094', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.1.243.1', 'zone', 'gate', '1', '0', '62111', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.1.244.1', 'zone', 'login', '1', '0', '62125', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.1.245.1', 'zone', 'game', '1', '0', '62139', '1534996773', '101.0.3.1', '192.168.1.9');
INSERT INTO `deploy` VALUES ('101.3.241.1', 'zone', 'master', '1', '0', '10494', '1534909769', '101.0.3.227', '192.168.10.230');
INSERT INTO `deploy` VALUES ('101.3.242.1', 'zone', 'world', '1', '0', '10504', '1534909769', '101.0.3.227', '192.168.10.230');
INSERT INTO `deploy` VALUES ('101.3.243.1', 'zone', 'gate', '1', '0', '10517', '1534909769', '101.0.3.227', '192.168.10.230');
INSERT INTO `deploy` VALUES ('101.3.244.1', 'zone', 'login', '1', '0', '10527', '1534909769', '101.0.3.227', '192.168.10.230');
INSERT INTO `deploy` VALUES ('101.3.245.1', 'zone', 'game', '1', '0', '10537', '1534909769', '101.0.3.227', '192.168.10.230');
