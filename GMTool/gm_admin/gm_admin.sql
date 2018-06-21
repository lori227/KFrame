/*
Navicat MySQL Data Transfer

Source Server         : 本地
Source Server Version : 50553
Source Host           : localhost:3306
Source Database       : gm_admin

Target Server Type    : MYSQL
Target Server Version : 50553
File Encoding         : 65001

Date: 2018-06-19 15:39:26
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `gm_auth`
-- ----------------------------
DROP TABLE IF EXISTS `gm_auth`;
CREATE TABLE `gm_auth` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `fid` int(6) NOT NULL COMMENT '父权限，为0时是顶级权限',
  `istree` tinyint(1) NOT NULL DEFAULT '0' COMMENT '左功能树 1:显示 0:不显示',
  `name` varchar(256) NOT NULL COMMENT '描述',
  `class` varchar(128) NOT NULL COMMENT '类名',
  `method` varchar(128) NOT NULL COMMENT '方法名',
  `add_user` varchar(56) NOT NULL COMMENT '添加人',
  `add_time` int(11) NOT NULL COMMENT '添加时间',
  PRIMARY KEY (`id`),
  KEY `fid` (`fid`)
) ENGINE=InnoDB AUTO_INCREMENT=41 DEFAULT CHARSET=utf8 COMMENT='权限列表';

-- ----------------------------
-- Records of gm_auth
-- ----------------------------
INSERT INTO `gm_auth` VALUES ('1', '0', '1', '平台管理', '', '', '超级管理员', '1406535925');
INSERT INTO `gm_auth` VALUES ('4', '0', '1', '权限管理', '', '', '超级管理员', '1406535948');
INSERT INTO `gm_auth` VALUES ('5', '1', '1', '激活码', 'activationCode', 'index', '超级管理员', '1406535998');
INSERT INTO `gm_auth` VALUES ('8', '1', '1', '通知邮件', 'mail', 'index', '超级管理员', '1406536038');
INSERT INTO `gm_auth` VALUES ('15', '4', '1', '管理员列表', 'auth', 'index', '超级管理员', '1406536139');
INSERT INTO `gm_auth` VALUES ('16', '4', '1', '添加管理员', 'auth', 'add', '超级管理员', '1406536150');
INSERT INTO `gm_auth` VALUES ('36', '4', '0', '修改管理员权限及密码', 'auth', 'edit', '测试员', '1406612151');
INSERT INTO `gm_auth` VALUES ('37', '4', '0', '启动禁用管理员', 'auth', 'actUser', '测试员', '1406612175');
INSERT INTO `gm_auth` VALUES ('38', '4', '1', '添加权限结点', 'auth', 'addAuth', '测试员', '1406612200');
INSERT INTO `gm_auth` VALUES ('39', '1', '1', '公告管理', 'notice', 'index', '超级管理员', '1529044987');
INSERT INTO `gm_auth` VALUES ('40', '1', '1', 'CDK礼包', 'cdkCode', 'index', '超级管理员', '1529375595');

-- ----------------------------
-- Table structure for `gm_cdk`
-- ----------------------------
DROP TABLE IF EXISTS `gm_cdk`;
CREATE TABLE `gm_cdk` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `cdk_id` varchar(30) NOT NULL DEFAULT '' COMMENT 'cdk唯一id',
  `title` varchar(50) NOT NULL COMMENT '标题',
  `describe` varchar(200) DEFAULT NULL COMMENT '描述',
  `start_time` int(10) NOT NULL DEFAULT '0' COMMENT '开始时间',
  `end_time` int(10) NOT NULL DEFAULT '0' COMMENT '结束时间',
  `add_user` varchar(56) NOT NULL COMMENT '添加人',
  `add_time` int(11) NOT NULL COMMENT '添加时间',
  `update_time` int(10) DEFAULT '0' COMMENT '更新时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=52 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of gm_cdk
-- ----------------------------
INSERT INTO `gm_cdk` VALUES ('50', '1806191532', '9999', '999999999999999999999999999999999999999999999999999999999999999', '1529393545', '1529393548', '3', '1529393552', '1529393552');
INSERT INTO `gm_cdk` VALUES ('51', '1806191534', '666666666', '6666666666666666666666666666666666666666666666666666666666666666666', '1529393672', '1529393674', '3', '1529393677', '1529393677');

-- ----------------------------
-- Table structure for `gm_mail`
-- ----------------------------
DROP TABLE IF EXISTS `gm_mail`;
CREATE TABLE `gm_mail` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `email_id` varchar(30) DEFAULT '' COMMENT '邮件id',
  `type` tinyint(2) DEFAULT '0' COMMENT '类型(1.系统邮件2.补偿邮件)',
  `title` varchar(100) NOT NULL DEFAULT '' COMMENT '标题',
  `content` varchar(500) NOT NULL DEFAULT '' COMMENT '邮件内容',
  `validtime` int(11) NOT NULL DEFAULT '0' COMMENT '有效时间长（秒）',
  `reward` varchar(200) DEFAULT '' COMMENT '邮件附件',
  `sendername` varchar(50) NOT NULL DEFAULT '' COMMENT '发送者名字',
  `flag` tinyint(2) NOT NULL DEFAULT '0' COMMENT '邮件状态(0.未读1.已读2.已领取)',
  `configId` varchar(30) DEFAULT '' COMMENT '邮件的配置id',
  `sendtime` int(11) NOT NULL DEFAULT '0' COMMENT '发送时间',
  `status` tinyint(2) DEFAULT '0' COMMENT '邮件状态（0:未1:已审核2:已发送3:审核未通过4:已驳回）',
  `playerIdStr` varchar(200) DEFAULT '' COMMENT '玩家id集合',
  `add_user` tinyint(2) DEFAULT '0' COMMENT '添加人id',
  `add_time` int(11) DEFAULT '0' COMMENT '添加时间戳',
  `check_user` tinyint(2) DEFAULT '0' COMMENT '审核人id',
  `check_time` int(10) DEFAULT '0' COMMENT '发送时间',
  `update_time` int(10) DEFAULT '0' COMMENT '更新时间',
  PRIMARY KEY (`id`),
  UNIQUE KEY `eid` (`email_id`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=31 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of gm_mail
-- ----------------------------
INSERT INTO `gm_mail` VALUES ('30', '29', '1', '77', '77', '2612931', '', '77', '0', '777', '1529065080', '2', '', '3', '1529044149', '3', '1529044166', '0');

-- ----------------------------
-- Table structure for `gm_notice`
-- ----------------------------
DROP TABLE IF EXISTS `gm_notice`;
CREATE TABLE `gm_notice` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `type` tinyint(1) NOT NULL DEFAULT '0' COMMENT '类型(1:登陆前2：登陆后3：跑马灯)',
  `title` varchar(256) NOT NULL COMMENT '标题',
  `content` varchar(500) NOT NULL COMMENT '内容',
  `start_time` int(10) DEFAULT '0' COMMENT '开始时间',
  `add_user` varchar(56) NOT NULL COMMENT '添加人',
  `add_time` int(11) NOT NULL COMMENT '添加时间',
  `update_time` int(10) DEFAULT '0' COMMENT '更新时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=48 DEFAULT CHARSET=utf8 COMMENT='权限列表';

-- ----------------------------
-- Records of gm_notice
-- ----------------------------
INSERT INTO `gm_notice` VALUES ('46', '2', '66666666', '<span ><font ><b><i>6666666666666666666666666666666666666</i></b></font></span>', '1529058501', '3', '1529058512', '1529058512');
INSERT INTO `gm_notice` VALUES ('47', '2', '22222', '<b >2222222222222222222222222222</b>', '1529374109', '3', '1529374111', '1529374111');

-- ----------------------------
-- Table structure for `gm_user`
-- ----------------------------
DROP TABLE IF EXISTS `gm_user`;
CREATE TABLE `gm_user` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(100) NOT NULL COMMENT '登录名',
  `password` varchar(100) NOT NULL COMMENT '登录密码',
  `realname` varchar(100) NOT NULL COMMENT '真实姓名',
  `last_login_ip` varchar(100) DEFAULT NULL,
  `last_login_time` int(11) DEFAULT NULL,
  `auth` text NOT NULL COMMENT '权限id',
  `allow_app` varchar(100) DEFAULT '0' COMMENT '允许的APP',
  `status` tinyint(4) NOT NULL DEFAULT '1' COMMENT '状态(0:删除,1:正常)',
  PRIMARY KEY (`id`),
  KEY `status` (`status`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of gm_user
-- ----------------------------
INSERT INTO `gm_user` VALUES ('2', 'xue', 'e10adc3949ba59abbe56e057f20f883e', 'xue', '127.0.0.1', '1527823051', '[\"5\",\"8\",\"16\",\"36\",\"37\"]', '6,7', '0');
INSERT INTO `gm_user` VALUES ('3', 'admin', 'e10adc3949ba59abbe56e057f20f883e', '超级管理员', '127.0.0.1', '1529379709', '', '1,2,3,4,5,6', '1');
INSERT INTO `gm_user` VALUES ('7', 'test', 'e10adc3949ba59abbe56e057f20f883e', '测试用户', null, null, '', '0', '1');
