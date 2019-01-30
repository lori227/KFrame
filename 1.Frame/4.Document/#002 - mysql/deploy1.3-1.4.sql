/*
Navicat MySQL Data Transfer

Source Server         : #003-外网版本118.25.173.109
Source Server Version : 50722
Source Host           : 118.25.173.109:3306
Source Database       : kfdeploy

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2018-10-29 13:57:33
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `deploy`
-- ----------------------------
DROP TABLE IF EXISTS `deploy`;
CREATE TABLE `deploy` (
  `appid` varchar(50) NOT NULL COMMENT '服务器id',
  `service` varchar(50) NOT NULL DEFAULT '1.2',
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
  `version` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`appid`,`service`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of deploy
-- ----------------------------
INSERT INTO `deploy` VALUES ('1.0.11.1', '1.3', 'data', 'master', '2.1', '1', '0', '0', '2625', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.11.1', '1.4', 'data', 'master', '2.1', '1', '0', '0', '30271', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.111.1', '1.3', 'log', 'master', '1.1', '1', '0', '0', '2635', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.111.1', '1.4', 'log', 'master', '1.1', '1', '0', '0', '30286', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.112.1', '1.3', 'log', 'proxy', '1.1', '1', '0', '0', '2646', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.112.1', '1.4', 'log', 'proxy', '1.1', '1', '0', '0', '30304', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.113.1', '1.3', 'log', 'shard', '1.1', '1', '0', '0', '2657', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.113.1', '1.4', 'log', 'shard', '1.1', '1', '0', '0', '30320', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.12.1', '1.3', 'data', 'proxy', '2.1', '1', '0', '0', '2668', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.12.1', '1.4', 'data', 'proxy', '2.1', '1', '0', '0', '30336', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.121.1', '1.3', 'dir', 'master', '2.1', '1', '0', '0', '2679', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.121.1', '1.4', 'dir', 'master', '2.1', '1', '0', '0', '30352', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.122.1', '1.3', 'dir', 'proxy', '2.1', '1', '0', '0', '2689', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.122.1', '1.4', 'dir', 'proxy', '2.1', '1', '0', '0', '30367', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.123.1', '1.3', 'dir', 'shard', '2.1', '1', '0', '0', '2700', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.123.1', '1.4', 'dir', 'shard', '2.1', '1', '0', '0', '30382', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.13.1', '1.3', 'data', 'shard', '2.1', '1', '0', '0', '2713', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.13.1', '1.4', 'data', 'shard', '2.1', '1', '0', '0', '30401', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.21.1', '1.3', 'public', 'master', '2.1', '1', '0', '0', '2724', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.21.1', '1.4', 'public', 'master', '2.1', '1', '0', '0', '30417', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.22.1', '1.3', 'public', 'proxy', '2.1', '1', '0', '0', '2735', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.22.1', '1.4', 'public', 'proxy', '2.1', '1', '0', '0', '30433', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.23.1', '1.3', 'public', 'shard', '2.1', '1', '0', '0', '2746', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.23.1', '1.4', 'public', 'shard', '2.1', '1', '0', '0', '30449', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.31.1', '1.3', 'route', 'master', '2.1', '1', '0', '0', '2757', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.31.1', '1.4', 'route', 'master', '2.1', '1', '0', '0', '30465', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.32.1', '1.3', 'route', 'proxy', '2.1', '1', '0', '0', '2768', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.32.1', '1.4', 'route', 'proxy', '2.1', '1', '0', '0', '30481', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.33.1', '1.3', 'route', 'shard', '2.1', '1', '0', '0', '2779', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.33.1', '1.4', 'route', 'shard', '2.1', '1', '0', '0', '30497', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.4.1', '1.3', 'auth', 'auth', '2.1', '1', '0', '0', '2790', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.4.1', '1.4', 'auth', 'auth', '2.1', '1', '0', '0', '30513', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.41.1', '1.3', 'match', 'master', '2.1', '1', '0', '0', '2803', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.41.1', '1.4', 'match', 'master', '2.1', '1', '0', '0', '30532', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.42.1', '1.3', 'match', 'proxy', '2.1', '1', '0', '0', '2815', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.42.1', '1.4', 'match', 'proxy', '2.1', '1', '0', '0', '30548', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.43.1', '1.3', 'match', 'shard', '2.1', '1', '0', '0', '2826', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.43.1', '1.4', 'match', 'shard', '2.1', '1', '0', '0', '30564', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.51.1', '1.3', 'battle', 'master', '2.1', '1', '0', '0', '2837', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.51.1', '1.4', 'battle', 'master', '2.1', '1', '0', '0', '30580', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.52.1', '1.3', 'battle', 'proxy', '2.1', '1', '0', '0', '2848', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.52.1', '1.4', 'battle', 'proxy', '2.1', '1', '0', '0', '30596', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.53.1', '1.3', 'battle', 'shard', '2.1', '1', '0', '0', '2859', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.53.1', '1.4', 'battle', 'shard', '2.1', '1', '0', '0', '30930', '1540788433', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.61.1', '1.3', 'mail', 'master', '2.1', '1', '0', '0', '2870', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.61.1', '1.4', 'mail', 'master', '2.1', '1', '0', '0', '30628', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.62.1', '1.3', 'mail', 'proxy', '2.1', '1', '0', '0', '2884', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.62.1', '1.4', 'mail', 'proxy', '2.1', '1', '0', '0', '30649', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.63.1', '1.3', 'mail', 'shard', '2.1', '1', '0', '0', '2895', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.63.1', '1.4', 'mail', 'shard', '2.1', '1', '0', '0', '30666', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.71.1', '1.3', 'relation', 'master', '2.1', '1', '0', '0', '2906', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.71.1', '1.4', 'relation', 'master', '2.1', '1', '0', '0', '30682', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.72.1', '1.3', 'relation', 'proxy', '2.1', '1', '0', '0', '2917', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.72.1', '1.4', 'relation', 'proxy', '2.1', '1', '0', '0', '30698', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.73.1', '1.3', 'relation', 'shard', '2.1', '1', '0', '0', '2929', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.73.1', '1.4', 'relation', 'shard', '2.1', '1', '0', '0', '30714', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.81.1', '1.3', 'group', 'master', '2.1', '1', '0', '0', '2940', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.81.1', '1.4', 'group', 'master', '2.1', '1', '0', '0', '30730', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.82.1', '1.3', 'group', 'proxy', '2.1', '1', '0', '0', '2951', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.82.1', '1.4', 'group', 'proxy', '2.1', '1', '0', '0', '30746', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.83.1', '1.3', 'group', 'shard', '2.1', '1', '0', '0', '2962', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.83.1', '1.4', 'group', 'shard', '2.1', '1', '0', '0', '30762', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.91.1', '1.3', 'rank', 'master', '2.1', '1', '0', '0', '2973', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.91.1', '1.4', 'rank', 'master', '2.1', '1', '0', '0', '30778', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.92.1', '1.3', 'rank', 'proxy', '2.1', '1', '0', '0', '2984', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.92.1', '1.4', 'rank', 'proxy', '2.1', '1', '0', '0', '30794', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.0.93.1', '1.3', 'rank', 'shard', '2.1', '1', '0', '0', '2995', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.0.93.1', '1.4', 'rank', 'shard', '2.1', '1', '0', '0', '30812', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.1.241.1', '1.3', 'zone', 'master', '2.1', '1', '0', '0', '3006', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.1.241.1', '1.4', 'zone', 'master', '2.1', '1', '0', '0', '30828', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.1.242.1', '1.3', 'zone', 'world', '2.1', '1', '0', '0', '3017', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.1.242.1', '1.4', 'zone', 'world', '2.1', '1', '0', '0', '30844', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.1.243.1', '1.3', 'zone', 'gate', '2.1', '1', '0', '0', '3031', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.1.243.1', '1.4', 'zone', 'gate', '2.1', '1', '0', '0', '30863', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.1.244.1', '1.3', 'zone', 'login', '2.1', '1', '0', '0', '3043', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.1.244.1', '1.4', 'zone', 'login', '2.1', '1', '0', '0', '30879', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
INSERT INTO `deploy` VALUES ('1.1.245.1', '1.3', 'zone', 'game', '2.1', '1', '0', '0', '3054', '1540792456', '1.0.3.1', '10.105.6.78', 'sgame_online_312_201810291345');
INSERT INTO `deploy` VALUES ('1.1.245.1', '1.4', 'zone', 'game', '2.1', '1', '0', '0', '30895', '1540788413', '1.0.3.3', '10.105.51.22', 'sgame_online_309_201810291223');
