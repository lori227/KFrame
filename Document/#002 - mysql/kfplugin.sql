/*
Navicat MySQL Data Transfer

Source Server         : 小兵研发
Source Server Version : 50722
Source Host           : 192.168.10.230:3306
Source Database       : kfplugin

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2018-10-11 11:28:57
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `account`
-- ----------------------------
DROP TABLE IF EXISTS `account`;
CREATE TABLE `account` (
  `account` varchar(50) NOT NULL,
  `password` varchar(50) NOT NULL,
  `validtime` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of account
-- ----------------------------
INSERT INTO `account` VALUES ('test', '098f6bcd4621d373cade4e832627b4f6', '0');
