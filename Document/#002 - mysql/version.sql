/*
Navicat MySQL Data Transfer

Source Server         : 192.168.1.9
Source Server Version : 50640
Source Host           : 192.168.1.9:3306
Source Database       : kframe

Target Server Type    : MYSQL
Target Server Version : 50640
File Encoding         : 65001

Date: 2018-08-30 11:20:28
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `version`
-- ----------------------------
DROP TABLE IF EXISTS `version`;
CREATE TABLE `version` (
  `version_time` datetime NOT NULL,
  `version_name` varchar(128) NOT NULL,
  `version_url` varchar(512) NOT NULL,
  `version_md5` varchar(32) NOT NULL,
  PRIMARY KEY (`version_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of version
-- ----------------------------
INSERT INTO `version` VALUES ('2018-08-29 12:13:16', 'sgame_svn_9216_201808291212.tar.gz', 'http://version.leiwu.com/upload/sgame_svn_9216_201808291212.tar.gz', 'b0c792e23567a983df2d2b7337a5eb70');
INSERT INTO `version` VALUES ('2018-08-29 16:20:27', 'sgame_svn_9267_201808291619.tar.gz', 'http://version.leiwu.com/upload/sgame_svn_9267_201808291619.tar.gz', 'f8b291c558236a29f790df7aaebbfc3d');
INSERT INTO `version` VALUES ('2018-08-29 16:47:09', 'sgame_svn_9273_201808291646.tar.gz', 'http://version.leiwu.com/upload/sgame_svn_9273_201808291646.tar.gz', '00fdbe8502e4f61504fcebe1a02d634d');
INSERT INTO `version` VALUES ('2018-08-29 19:13:05', 'sgame_svn_9296_201808291912.tar.gz', 'http://version.leiwu.com/upload/sgame_svn_9296_201808291912.tar.gz', '9786cb6718a7baaed1b96eeb25537cfe');
INSERT INTO `version` VALUES ('2018-08-29 20:43:04', 'sgame_svn_9332_201808292042.tar.gz', 'http://version.leiwu.com/upload/sgame_svn_9332_201808292042.tar.gz', '7107e3537bf95585ef9233a1587e520e');
INSERT INTO `version` VALUES ('2018-08-29 21:18:31', 'sgame_svn_9340_201808292117.tar.gz', 'http://version.leiwu.com/upload/sgame_svn_9340_201808292117.tar.gz', '56ca6ecbda43778686dd00bd27f7fb01');
