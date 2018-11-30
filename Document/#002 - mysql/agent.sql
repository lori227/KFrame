/*
Navicat MySQL Data Transfer

Source Server         : #001-小兵研发
Source Server Version : 50722
Source Host           : 192.168.10.230:3306
Source Database       : kfdeploy

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2018-11-30 12:56:31
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `agent`
-- ----------------------------
DROP TABLE IF EXISTS `agent`;
CREATE TABLE `agent` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `localip` varchar(50) NOT NULL DEFAULT '',
  `strappid` varchar(50) NOT NULL DEFAULT '',
  `appid` bigint(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `ip` (`localip`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of agent
-- ----------------------------
