/*
Navicat MySQL Data Transfer

Source Server         : #001-小兵研发192.168.10.230
Source Server Version : 50722
Source Host           : 192.168.10.230:3306
Source Database       : kfversion

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2018-10-29 18:32:31
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `file`
-- ----------------------------
DROP TABLE IF EXISTS `file`;
CREATE TABLE `file` (
  `file_name` varchar(50) NOT NULL,
  `file_path` varchar(50) NOT NULL,
  `file_url` varchar(200) NOT NULL,
  `file_time` datetime NOT NULL,
  `file_md5` varchar(50) NOT NULL,
  PRIMARY KEY (`file_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of file
-- ----------------------------

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
INSERT INTO `version` VALUES ('2018-10-29 10:50:56', 'sgame_develop_305_201810291049.tar.gz', 'http://192.168.10.230/upload/sgame_develop_305_201810291049.tar.gz', 'dabefd9af16b20b478151937b484573f');
INSERT INTO `version` VALUES ('2018-10-29 10:59:54', 'sgame_develop_306_201810291058.tar.gz', 'http://192.168.10.230/upload/sgame_develop_306_201810291058.tar.gz', 'a4affc7341f5c5750f2c809efba21401');
INSERT INTO `version` VALUES ('2018-10-29 11:58:58', 'sgame_develop_308_201810291157.tar.gz', 'http://192.168.10.230/upload/sgame_develop_308_201810291157.tar.gz', 'd9912cb38e73bf53899ef36c4adb4a5b');
INSERT INTO `version` VALUES ('2018-10-29 12:14:50', 'sgame_develop_309_201810291213.tar.gz', 'http://192.168.10.230/upload/sgame_develop_309_201810291213.tar.gz', '0136fec431f91f26411c31c43f598cd2');
INSERT INTO `version` VALUES ('2018-10-29 13:39:24', 'sgame_develop_312_201810291338.tar.gz', 'http://192.168.10.230/upload/sgame_develop_312_201810291338.tar.gz', 'd7847ffac6cf33875d28935e17c07e70');
INSERT INTO `version` VALUES ('2018-10-29 14:33:38', 'sgame_develop_314_201810291432.tar.gz', 'http://192.168.10.230/upload/sgame_develop_314_201810291432.tar.gz', 'f9f0cbe58f783763816894a0c3f43d01');
INSERT INTO `version` VALUES ('2018-10-29 18:16:49', 'sgame_develop_315_201810291815.tar.gz', 'http://192.168.10.230/upload/sgame_develop_315_201810291815.tar.gz', '2b864fa940d75d9874801f9da2770340');
INSERT INTO `version` VALUES ('2018-10-29 12:13:06', 'sgame_internal_308_201810291211.tar.gz', 'http://192.168.10.230/upload/sgame_internal_308_201810291211.tar.gz', 'f48f67b4e32a758b2bdffa9aee34e6b6');
INSERT INTO `version` VALUES ('2018-10-29 12:41:34', 'sgame_internal_309_201810291241.tar.gz', 'http://192.168.10.230/upload/sgame_internal_309_201810291241.tar.gz', '4b57ff4fbf860d5b6b7a5e47a858c578');
