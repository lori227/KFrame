/*
Navicat MySQL Data Transfer

Source Server         : 小兵研发
Source Server Version : 50722
Source Host           : 192.168.10.230:3306
Source Database       : kframe

Target Server Type    : MYSQL
Target Server Version : 50722
File Encoding         : 65001

Date: 2018-10-18 14:09:00
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
INSERT INTO `file` VALUES ('enter.lua', 'script', 'http://192.168.10.230/upload/enter.lua', '2018-10-18 14:01:29', '8e6d2f8f825e447398283bb08c1bf290');
INSERT INTO `file` VALUES ('option.config', 'config', 'http://192.168.10.230/upload/option.config', '2018-10-18 13:23:03', 'daf7dd1210711728e1eadaf9b68767fd');
