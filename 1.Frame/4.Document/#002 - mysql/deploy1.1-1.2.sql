/*
Navicat MySQL Data Transfer

Source Server         : #001-小兵研发192.168.10.230
Source Server Version : 50722
Source Host           : 192.168.10.230:3306
Source Database       : kfdeploy

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2018-10-29 11:51:30
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
INSERT INTO `deploy` VALUES ('1.0.11.1', '1.1', 'data', 'master', '2.1', '1', '1', '0', '18465', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.11.1', '1.2', 'data', 'master', '2.1', '1', '1', '0', '53700', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.111.1', '1.1', 'log', 'master', '1.1', '1', '1', '0', '18475', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.111.1', '1.2', 'log', 'master', '1.1', '1', '1', '0', '53714', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.112.1', '1.1', 'log', 'proxy', '1.1', '1', '1', '0', '18486', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.112.1', '1.2', 'log', 'proxy', '1.1', '1', '1', '0', '53727', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.113.1', '1.1', 'log', 'shard', '1.1', '1', '1', '0', '18497', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.113.1', '1.2', 'log', 'shard', '1.1', '1', '1', '0', '53739', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.12.1', '1.1', 'data', 'proxy', '2.1', '1', '1', '0', '18508', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.12.1', '1.2', 'data', 'proxy', '2.1', '1', '1', '0', '53750', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.121.1', '1.1', 'dir', 'master', '2.1', '1', '1', '0', '18519', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.121.1', '1.2', 'dir', 'master', '2.1', '1', '1', '0', '53762', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.122.1', '1.1', 'dir', 'proxy', '2.1', '1', '1', '0', '18529', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.122.1', '1.2', 'dir', 'proxy', '2.1', '1', '1', '0', '53772', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.123.1', '1.1', 'dir', 'shard', '2.1', '1', '1', '0', '18539', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.123.1', '1.2', 'dir', 'shard', '2.1', '1', '1', '0', '55401', '1540783977', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.13.1', '1.1', 'data', 'shard', '2.1', '1', '1', '0', '18552', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.13.1', '1.2', 'data', 'shard', '2.1', '1', '1', '0', '53798', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.21.1', '1.1', 'public', 'master', '2.1', '1', '1', '0', '18563', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.21.1', '1.2', 'public', 'master', '2.1', '1', '1', '0', '53809', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.22.1', '1.1', 'public', 'proxy', '2.1', '1', '1', '0', '18574', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.22.1', '1.2', 'public', 'proxy', '2.1', '1', '1', '0', '53821', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.23.1', '1.1', 'public', 'shard', '2.1', '1', '1', '0', '18585', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.23.1', '1.2', 'public', 'shard', '2.1', '1', '1', '0', '53832', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.31.1', '1.1', 'route', 'master', '2.1', '1', '1', '0', '18596', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.31.1', '1.2', 'route', 'master', '2.1', '1', '1', '0', '53843', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.32.1', '1.1', 'route', 'proxy', '2.1', '1', '1', '0', '18607', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.32.1', '1.2', 'route', 'proxy', '2.1', '1', '1', '0', '53855', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.33.1', '1.1', 'route', 'shard', '2.1', '1', '1', '0', '18618', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.33.1', '1.2', 'route', 'shard', '2.1', '1', '1', '0', '53866', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.4.1', '1.1', 'auth', 'auth', '2.1', '1', '1', '0', '18629', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.4.1', '1.2', 'auth', 'auth', '2.1', '1', '1', '0', '53878', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.41.1', '1.1', 'match', 'master', '2.1', '1', '1', '0', '18642', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.41.1', '1.2', 'match', 'master', '2.1', '1', '1', '0', '53892', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.42.1', '1.1', 'match', 'proxy', '2.1', '1', '1', '0', '18653', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.42.1', '1.2', 'match', 'proxy', '2.1', '1', '1', '0', '53904', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.43.1', '1.1', 'match', 'shard', '2.1', '1', '1', '0', '18664', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.43.1', '1.2', 'match', 'shard', '2.1', '1', '1', '0', '53915', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.51.1', '1.1', 'battle', 'master', '2.1', '1', '1', '0', '18675', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.51.1', '1.2', 'battle', 'master', '2.1', '1', '1', '0', '54189', '1540782917', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.52.1', '1.1', 'battle', 'proxy', '2.1', '1', '1', '0', '18686', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.52.1', '1.2', 'battle', 'proxy', '2.1', '1', '1', '0', '53937', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.53.1', '1.1', 'battle', 'shard', '2.1', '1', '1', '0', '18697', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.53.1', '1.2', 'battle', 'shard', '2.1', '1', '1', '0', '53949', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.61.1', '1.1', 'mail', 'master', '2.1', '1', '1', '0', '18708', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.61.1', '1.2', 'mail', 'master', '2.1', '1', '1', '0', '53961', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.62.1', '1.1', 'mail', 'proxy', '2.1', '1', '1', '0', '18722', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.62.1', '1.2', 'mail', 'proxy', '2.1', '1', '1', '0', '53972', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.63.1', '1.1', 'mail', 'shard', '2.1', '1', '1', '0', '18733', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.63.1', '1.2', 'mail', 'shard', '2.1', '1', '1', '0', '53984', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.71.1', '1.1', 'relation', 'master', '2.1', '1', '1', '0', '18744', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.71.1', '1.2', 'relation', 'master', '2.1', '1', '1', '0', '53995', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.72.1', '1.1', 'relation', 'proxy', '2.1', '1', '1', '0', '18755', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.72.1', '1.2', 'relation', 'proxy', '2.1', '1', '1', '0', '54006', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.73.1', '1.1', 'relation', 'shard', '2.1', '1', '1', '0', '18766', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.73.1', '1.2', 'relation', 'shard', '2.1', '1', '1', '0', '54018', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.81.1', '1.1', 'group', 'master', '2.1', '1', '1', '0', '18777', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.81.1', '1.2', 'group', 'master', '2.1', '1', '1', '0', '54029', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.82.1', '1.1', 'group', 'proxy', '2.1', '1', '1', '0', '18788', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.82.1', '1.2', 'group', 'proxy', '2.1', '1', '1', '0', '54041', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.83.1', '1.1', 'group', 'shard', '2.1', '1', '1', '0', '18799', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.83.1', '1.2', 'group', 'shard', '2.1', '1', '1', '0', '54052', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.91.1', '1.1', 'rank', 'master', '2.1', '1', '1', '0', '18810', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.91.1', '1.2', 'rank', 'master', '2.1', '1', '1', '0', '54063', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.92.1', '1.1', 'rank', 'proxy', '2.1', '1', '1', '0', '18821', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.92.1', '1.2', 'rank', 'proxy', '2.1', '1', '1', '0', '54075', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.93.1', '1.1', 'rank', 'shard', '2.1', '1', '1', '0', '18832', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.0.93.1', '1.2', 'rank', 'shard', '2.1', '1', '1', '0', '54086', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.1.241.1', '1.1', 'zone', 'master', '2.1', '1', '1', '0', '18843', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.1.241.1', '1.2', 'zone', 'master', '2.1', '1', '1', '0', '54098', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.1.242.1', '1.1', 'zone', 'world', '2.1', '1', '1', '0', '18854', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.1.242.1', '1.2', 'zone', 'world', '2.1', '1', '1', '0', '54109', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.1.243.1', '1.1', 'zone', 'gate', '2.1', '1', '1', '0', '18868', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.1.243.1', '1.2', 'zone', 'gate', '2.1', '1', '1', '0', '54123', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.1.244.1', '1.1', 'zone', 'login', '2.1', '1', '1', '0', '18879', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.1.244.1', '1.2', 'zone', 'login', '2.1', '1', '1', '0', '54135', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.1.245.1', '1.1', 'zone', 'game', '2.1', '1', '1', '0', '18890', '1540782094', '1.0.3.1', '192.168.10.230', 'sgame_develop_306_201810291058');
INSERT INTO `deploy` VALUES ('1.1.245.1', '1.2', 'zone', 'game', '2.1', '1', '1', '0', '54146', '1540782876', '1.0.3.2', '192.168.1.9', 'sgame_develop_306_201810291058');
