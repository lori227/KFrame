/*
Navicat MySQL Data Transfer

Source Server         : 192.168.1.9
Source Server Version : 50640
Source Host           : 192.168.1.9:3306
Source Database       : kframe

Target Server Type    : MYSQL
Target Server Version : 50640
File Encoding         : 65001

Date: 2018-08-09 11:43:28
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `deploy`
-- ----------------------------
DROP TABLE IF EXISTS `deploy`;
CREATE TABLE `deploy` (
  `appid` int(10) unsigned NOT NULL COMMENT '服务器id',
  `appname` varchar(50) NOT NULL COMMENT '服务器名字',
  `apptype` varchar(50) NOT NULL COMMENT '服务器类型',
  `zoneid` int(10) unsigned NOT NULL DEFAULT '0',
  `startup` int(10) unsigned NOT NULL COMMENT '是否已经关闭',
  `shutdown` int(10) NOT NULL DEFAULT '0',
  `process` int(10) unsigned NOT NULL COMMENT '进程id',
  `time` bigint(20) unsigned NOT NULL COMMENT '状态时间( 启动或者关闭 )',
  `agentid` int(10) unsigned NOT NULL COMMENT '部署agentid',
  `localip` varchar(50) NOT NULL COMMENT '局域网ip',
  PRIMARY KEY (`appid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of deploy
-- ----------------------------
INSERT INTO `deploy` VALUES ('1001', 'auth', 'auth', '0', '1', '0', '14270', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('2001', 'platform', 'platform', '0', '1', '0', '14277', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('2002', 'platform', 'platform', '0', '1', '0', '14284', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('10000', 'data', 'master', '0', '1', '0', '14291', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('10001', 'data', 'proxy', '0', '1', '0', '14308', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('12001', 'data', 'shard', '0', '1', '0', '14315', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('12002', 'data', 'shard', '0', '1', '0', '14322', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('12003', 'data', 'shard', '0', '1', '0', '14329', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('12004', 'data', 'shard', '0', '1', '0', '14336', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('12005', 'data', 'shard', '0', '1', '0', '14348', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('12006', 'data', 'shard', '0', '1', '0', '14355', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('12007', 'data', 'shard', '0', '1', '0', '14362', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('12008', 'data', 'shard', '0', '1', '0', '14381', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('12009', 'data', 'shard', '0', '1', '0', '14396', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('12010', 'data', 'shard', '0', '1', '0', '14406', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('12011', 'data', 'shard', '0', '1', '0', '14416', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('12012', 'data', 'shard', '0', '1', '0', '14426', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('20000', 'public', 'master', '0', '1', '0', '14442', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('20001', 'public', 'proxy', '0', '1', '0', '14452', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('22001', 'public', 'shard', '0', '1', '0', '14462', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('30000', 'route', 'master', '0', '1', '0', '14469', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('30001', 'route', 'proxy', '0', '1', '0', '14476', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('32001', 'route', 'shard', '0', '1', '0', '14489', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('40000', 'match', 'master', '0', '1', '0', '14499', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('40001', 'match', 'proxy', '0', '1', '0', '14506', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('42001', 'match', 'shard', '0', '1', '0', '14519', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('42002', 'match', 'shard', '0', '1', '0', '14529', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('42003', 'match', 'shard', '0', '1', '0', '14536', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('50000', 'battle', 'master', '0', '1', '0', '14549', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('50001', 'battle', 'proxy', '0', '1', '0', '14565', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('52001', 'battle', 'shard', '0', '1', '0', '14572', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('60000', 'mail', 'master', '0', '1', '0', '14579', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('60001', 'mail', 'proxy', '0', '1', '0', '14586', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('62001', 'mail', 'shard', '0', '1', '0', '14595', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('70000', 'relation', 'master', '0', '1', '0', '14604', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('70001', 'relation', 'proxy', '0', '1', '0', '14611', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('72001', 'relation', 'shard', '0', '1', '0', '14620', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('80000', 'group', 'master', '0', '1', '0', '14630', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('80001', 'group', 'proxy', '0', '1', '0', '14649', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('82001', 'group', 'shard', '0', '1', '0', '14668', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('90000', 'rank', 'master', '0', '1', '0', '14677', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('90001', 'rank', 'proxy', '0', '1', '0', '14684', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('92001', 'rank', 'shard', '0', '1', '0', '14691', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('110000', 'log', 'master', '0', '1', '0', '14704', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('110001', 'log', 'proxy', '0', '1', '0', '14715', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('112001', 'log', 'shard', '0', '1', '0', '14722', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('1001100', 'zone', 'master', '1', '1', '0', '14740', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('1001200', 'zone', 'world', '1', '1', '0', '14747', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('1001301', 'zone', 'gate', '1', '1', '0', '14757', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('1001401', 'zone', 'login', '1', '1', '0', '14764', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('1001501', 'zone', 'proxy', '1', '1', '0', '14776', '1533785499', '101', '192.168.1.9');
INSERT INTO `deploy` VALUES ('1001601', 'zone', 'game', '1', '1', '0', '14794', '1533785499', '101', '192.168.1.9');

-- ----------------------------
-- Table structure for `launch`
-- ----------------------------
DROP TABLE IF EXISTS `launch`;
CREATE TABLE `launch` (
  `appname` varchar(50) NOT NULL DEFAULT '' COMMENT '进程类名',
  `apptype` varchar(50) NOT NULL DEFAULT '' COMMENT '进程类型',
  `apppath` varchar(50) NOT NULL DEFAULT '' COMMENT '程序目录',
  `appfile` varchar(50) NOT NULL DEFAULT '' COMMENT '进程文件名',
  `minid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '起始id',
  `maxid` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '最大的id',
  `appconfig` varchar(100) NOT NULL DEFAULT '' COMMENT '配置路径',
  `logtype` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '日志类型 0 不打日志 1 本地日志  2远程日志',
  `service` varchar(50) NOT NULL DEFAULT '' COMMENT '服务类型  cluster 集群服务  zone 游戏分区',
  `ftpid` int(11) NOT NULL DEFAULT '1' COMMENT 'ftpid',
  `deploypath` varchar(100) NOT NULL DEFAULT '/data/' COMMENT '部署路径',
  PRIMARY KEY (`appname`,`apptype`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of launch
-- ----------------------------
INSERT INTO `launch` VALUES ('auth', 'auth', 'auth', 'authserver', '1001', '1999', './setting/auth.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('battle', 'master', 'battlecluster', 'battleserver', '50000', '50000', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('battle', 'proxy', 'battlecluster', 'battleserver', '50001', '51999', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('battle', 'shard', 'battlecluster', 'battleserver', '52001', '59999', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('data', 'master', 'datacluster', 'dataserver', '10000', '10000', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('data', 'proxy', 'datacluster', 'dataserver', '10001', '11999', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('data', 'shard', 'datacluster', 'dataserver', '12001', '19999', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('group', 'master', 'groupcluster', 'groupserver', '80000', '80000', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('group', 'proxy', 'groupcluster', 'groupserver', '80001', '81999', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('group', 'shard', 'groupcluster', 'groupserver', '82001', '89999', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('guild', 'master', 'guildcluster', 'guildserver', '100000', '100000', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('guild', 'proxy', 'guildcluster', 'guildserver', '100001', '101999', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('guild', 'shard', 'guildcluster', 'guildserver', '102001', '109999', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('log', 'master', 'logcluster', 'logserver', '110000', '110000', './setting/master.startup', '1', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('log', 'proxy', 'logcluster', 'logserver', '110001', '111999', './setting/proxy.startup', '1', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('log', 'shard', 'logcluster', 'logserver', '112001', '119999', './setting/shard.startup', '1', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('mail', 'master', 'mailcluster', 'mailserver', '60000', '60000', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('mail', 'proxy', 'mailcluster', 'mailserver', '60001', '61999', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('mail', 'shard', 'mailcluster', 'mailserver', '62001', '69999', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('match', 'master', 'matchcluster', 'matchserver', '40000', '40000', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('match', 'proxy', 'matchcluster', 'matchserver', '40001', '41999', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('match', 'shard', 'matchcluster', 'matchserver', '42001', '49999', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('platform', 'platform', 'platform', 'platformserver', '2001', '2999', './setting/platform.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('public', 'master', 'publiccluster', 'publicserver', '20000', '20000', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('public', 'proxy', 'publiccluster', 'publicserver', '20001', '21999', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('public', 'shard', 'publiccluster', 'publicserver', '22001', '29999', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('rank', 'master', 'rankcluster', 'rankserver', '90000', '90000', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('rank', 'proxy', 'rankcluster', 'rankserver', '90001', '91999', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('rank', 'shard', 'rankcluster', 'rankserver', '92001', '99999', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('relation', 'master', 'relationcluster', 'relationserver', '70000', '70000', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('relation', 'proxy', 'relationcluster', 'relationserver', '70001', '71999', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('relation', 'shard', 'relationcluster', 'relationserver', '72001', '79999', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('route', 'master', 'routecluster', 'routeserver', '30000', '30000', './setting/master.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('route', 'proxy', 'routecluster', 'routeserver', '30001', '31999', './setting/proxy.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('route', 'shard', 'routecluster', 'routeserver', '32001', '39999', './setting/shard.startup', '2', 'cluster', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'game', 'zone', 'zoneserver', '601', '699', './setting/game.startup', '2', 'zone', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'gate', 'zone', 'zoneserver', '301', '399', './setting/gate.startup', '2', 'zone', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'login', 'zone', 'zoneserver', '401', '499', './setting/login.startup', '2', 'zone', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'master', 'zone', 'zoneserver', '100', '100', './setting/master.startup', '2', 'zone', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'proxy', 'zone', 'zoneserver', '501', '599', './setting/proxy.startup', '2', 'zone', '1', '../');
INSERT INTO `launch` VALUES ('zone', 'world', 'zone', 'zoneserver', '200', '200', './setting/world.startup', '2', 'zone', '1', '../');

-- ----------------------------
-- Table structure for `machine`
-- ----------------------------
DROP TABLE IF EXISTS `machine`;
CREATE TABLE `machine` (
  `name` varchar(50) NOT NULL DEFAULT '' COMMENT '机器名',
  `localip` varchar(50) NOT NULL DEFAULT '' COMMENT '局域网ip',
  `interanetip` varchar(50) NOT NULL COMMENT '外网ip',
  `cpu` int(10) NOT NULL DEFAULT '8',
  `thread` int(10) NOT NULL DEFAULT '16',
  `memory` int(10) NOT NULL DEFAULT '16',
  `harddisk` int(10) NOT NULL DEFAULT '100',
  `agentid` int(10) NOT NULL DEFAULT '0' COMMENT '开启的agentid',
  `status` int(10) NOT NULL DEFAULT '0' COMMENT '状态 0 agent断开连接 1 agent保持连接',
  `port` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`localip`),
  KEY `ip` (`localip`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of machine
-- ----------------------------
INSERT INTO `machine` VALUES ('内网测试机', '192.168.1.9', '192.168.1.9', '4', '8', '16', '320', '101', '1', '12001');
