/*
Navicat MySQL Data Transfer

Source Server         : 本地
Source Server Version : 50553
Source Host           : localhost:3306
Source Database       : web_games_version

Target Server Type    : MYSQL
Target Server Version : 50553
File Encoding         : 65001

Date: 2018-09-13 12:17:12
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `lw_auth`
-- ----------------------------
DROP TABLE IF EXISTS `lw_auth`;
CREATE TABLE `lw_auth` (
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
) ENGINE=InnoDB AUTO_INCREMENT=50 DEFAULT CHARSET=utf8 COMMENT='权限列表';

-- ----------------------------
-- Records of lw_auth
-- ----------------------------
INSERT INTO `lw_auth` VALUES ('1', '0', '1', '开服管理', '', '', '超级管理员', '1406535925');
INSERT INTO `lw_auth` VALUES ('4', '0', '1', '权限管理', '', '', '超级管理员', '1406535948');
INSERT INTO `lw_auth` VALUES ('5', '1', '1', '版本列表', 'version', 'index', '超级管理员', '1406535998');
INSERT INTO `lw_auth` VALUES ('6', '0', '1', '部署管理', '', '', '超级管理员', '1536746223');
INSERT INTO `lw_auth` VALUES ('15', '4', '1', '管理员列表', 'auth', 'index', '超级管理员', '1406536139');
INSERT INTO `lw_auth` VALUES ('16', '4', '1', '添加管理员', 'auth', 'add', '超级管理员', '1406536150');
INSERT INTO `lw_auth` VALUES ('36', '4', '0', '修改管理员权限及密码', 'auth', 'edit', '测试员', '1406612151');
INSERT INTO `lw_auth` VALUES ('37', '4', '0', '启动禁用管理员', 'auth', 'actUser', '测试员', '1406612175');
INSERT INTO `lw_auth` VALUES ('38', '4', '1', '添加权限结点', 'auth', 'addAuth', '测试员', '1406612200');
INSERT INTO `lw_auth` VALUES ('39', '1', '1', '机器列表', 'Machine', 'index', '超级管理员', '1535629286');
INSERT INTO `lw_auth` VALUES ('40', '1', '1', '启动列表', 'launch', 'index', '超级管理员', '1535629944');
INSERT INTO `lw_auth` VALUES ('41', '1', '1', '部署列表', 'deploy', 'index', '超级管理员', '1535681285');
INSERT INTO `lw_auth` VALUES ('42', '0', '1', '项目机器', '', '', '超级管理员', '1535682707');
INSERT INTO `lw_auth` VALUES ('43', '42', '1', '项目组列表', 'project_group', 'index', '超级管理员', '1535683116');
INSERT INTO `lw_auth` VALUES ('44', '42', '1', '项目列表', 'project', 'index', '超级管理员', '1535689432');
INSERT INTO `lw_auth` VALUES ('45', '42', '1', '节点/机器组', 'node_group', 'index', '超级管理员', '1535694498');
INSERT INTO `lw_auth` VALUES ('46', '42', '1', '节点/机器', 'node', 'index', '超级管理员', '1535695091');
INSERT INTO `lw_auth` VALUES ('47', '0', '1', '日志统计', '', '', '超级管理员', '1535947245');
INSERT INTO `lw_auth` VALUES ('48', '47', '1', '日志列表', 'statist', 'log', '超级管理员', '1535947334');
INSERT INTO `lw_auth` VALUES ('49', '6', '1', '部署日志', 'version', 'act_ver', '超级管理员', '1536643793');

-- ----------------------------
-- Table structure for `lw_deploy_record`
-- ----------------------------
DROP TABLE IF EXISTS `lw_deploy_record`;
CREATE TABLE `lw_deploy_record` (
  `id` int(10) NOT NULL AUTO_INCREMENT COMMENT '主键id',
  `ips` text NOT NULL COMMENT 'ips',
  `step` tinyint(2) NOT NULL DEFAULT '0' COMMENT '第几步',
  `command` tinyint(2) NOT NULL DEFAULT '0' COMMENT '命令',
  `act_time` int(11) DEFAULT '0' COMMENT '执行时间',
  `value` varchar(200) DEFAULT '' COMMENT '更新版本',
  `add_user` tinyint(2) NOT NULL DEFAULT '0',
  `add_time` int(11) NOT NULL DEFAULT '0' COMMENT '创建时间',
  `op_user` tinyint(2) DEFAULT NULL,
  `op_time` int(10) DEFAULT '0' COMMENT '管理员执行时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=15 DEFAULT CHARSET=utf8 COMMENT='项目组表';

-- ----------------------------
-- Records of lw_deploy_record
-- ----------------------------
INSERT INTO `lw_deploy_record` VALUES ('11', '101.0.11.1*101.0.12.1*101.0.13.1*', '2', '5', '0', '0', '3', '1536810583', null, '0');
INSERT INTO `lw_deploy_record` VALUES ('12', '101.0.11.1*101.0.12.1*101.0.13.1*101.0.121.1*101.0.122.1*101.0.123.1*101.0.81.1*101.0.82.1*101.0.83.1*101.0.111.1*101.0.112.1*101.0.113.1*', '2', '5', '0', 'sgame_internal_svn_39_201809031912.tar.gz', '3', '1536810841', null, '0');
INSERT INTO `lw_deploy_record` VALUES ('13', '101.0.51.1*101.0.52.1*101.0.53.1*', '2', '4', '0', '0', '3', '1536811683', null, '0');
INSERT INTO `lw_deploy_record` VALUES ('14', '101.0.121.1*101.0.122.1*101.0.123.1*', '2', '4', '0', '0', '3', '1536812097', null, '0');

-- ----------------------------
-- Table structure for `lw_log`
-- ----------------------------
DROP TABLE IF EXISTS `lw_log`;
CREATE TABLE `lw_log` (
  `log_id` int(10) NOT NULL AUTO_INCREMENT COMMENT '日志id',
  `level` tinyint(3) NOT NULL DEFAULT '6' COMMENT '日志级别',
  `controller` char(100) NOT NULL DEFAULT '' COMMENT '控制器',
  `action` char(100) NOT NULL DEFAULT '' COMMENT '动作',
  `get` text NOT NULL COMMENT 'get参数',
  `post` text NOT NULL COMMENT 'post参数',
  `message` varchar(255) NOT NULL DEFAULT '' COMMENT '信息',
  `ip` char(100) NOT NULL DEFAULT '' COMMENT 'ip地址',
  `user_agent` char(200) NOT NULL DEFAULT '' COMMENT '用户代理',
  `referer` char(100) NOT NULL DEFAULT '' COMMENT 'referer',
  `user_id` int(10) NOT NULL DEFAULT '0' COMMENT '帐号id',
  `username` char(100) NOT NULL DEFAULT '' COMMENT '帐号名',
  `create_time` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '创建时间',
  `type` tinyint(2) DEFAULT '0' COMMENT '类型（增1删2改3查4）',
  PRIMARY KEY (`log_id`)
) ENGINE=InnoDB AUTO_INCREMENT=140 DEFAULT CHARSET=utf8 COMMENT='行为日志表';

-- ----------------------------
-- Records of lw_log
-- ----------------------------
INSERT INTO `lw_log` VALUES ('68', '6', 'Project', 'add', '/admin/project/add', '{\"group_id\":\"1\",\"name\":\"555\",\"repository_url\":\"555\",\"https_username\":\"55\",\"https_password\":\"55\",\"branch\":\"55\",\"code_path\":\"5\",\"code_dir_user\":\"5\",\"comment\":\"\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/project/add', '3', 'admin', '1536026907', '1');
INSERT INTO `lw_log` VALUES ('69', '6', 'Deploy', 'del', '/admin/deploy/del/0', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/deploy/index', '3', 'admin', '1536027480', '2');
INSERT INTO `lw_log` VALUES ('70', '6', 'Deploy', 'del', '/admin/deploy/del/4', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/deploy/index', '3', 'admin', '1536027849', '2');
INSERT INTO `lw_log` VALUES ('71', '6', 'Deploy', 'modify', '/admin/deploy/modify/101.1.241.1', '{\"appid\":\"101.1.241.1\",\"appname\":\"zone--2\",\"apptype\":\"master\",\"startup\":\"1\",\"debug\":\"0\",\"shutdown\":\"0\",\"process\":\"32035\",\"agentid\":\"101.0.3.1\",\"localip\":\"192.168.1.9\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/deploy/modify/101.1.241.1', '3', 'admin', '1536029049', '3');
INSERT INTO `lw_log` VALUES ('72', '6', 'Deploy', 'modify', '/admin/deploy/modify/101', '{\"appid\":\"aaaaa\",\"appname\":\"zone--2\",\"apptype\":\"master\",\"startup\":\"1\",\"debug\":\"0\",\"shutdown\":\"0\",\"process\":\"32035\",\"agentid\":\"101.0.3.1\",\"localip\":\"192.168.1.9\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/deploy/modify/101', '3', 'admin', '1536029077', '3');
INSERT INTO `lw_log` VALUES ('73', '6', 'launch', 'del', '/admin/launch/del/1', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/launch/index', '3', 'admin', '1536031024', '2');
INSERT INTO `lw_log` VALUES ('74', '6', 'launch', 'del', '/admin/launch/del/2', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/launch/index', '3', 'admin', '1536031029', '2');
INSERT INTO `lw_log` VALUES ('75', '6', 'launch', 'del', '/admin/launch/del/43', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/launch/index', '3', 'admin', '1536031248', '2');
INSERT INTO `lw_log` VALUES ('76', '6', 'launch', 'add', '/admin/launch/add', '{\"appname\":\"3\",\"apptype\":\"4\",\"apppath\":\"4\",\"appfile\":\"4\",\"appconfig\":\"4\",\"logtype\":\"4\",\"service\":\"4\",\"ftpid\":\"\",\"deploypath\":\"\"}', '1', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/launch/add', '3', 'admin', '1536032676', '1');
INSERT INTO `lw_log` VALUES ('77', '6', 'launch', 'del', '/admin/launch/del/46', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/launch/index', '3', 'admin', '1536032686', '2');
INSERT INTO `lw_log` VALUES ('78', '6', 'launch', 'modify', '/admin/launch/modify/3', '{\"appname\":\"battle\",\"apptype\":\"proxy\",\"apppath\":\"battle\",\"appfile\":\"battleserver\",\"appconfig\":\".\\/startup\\/battle.proxy.startup\",\"logtype\":\"2\",\"service\":\"cluster\",\"ftpid\":\"1\",\"deploypath\":\"..\\/\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/launch/modify/3', '3', 'admin', '1536032966', '3');
INSERT INTO `lw_log` VALUES ('79', '6', 'launch', 'modify', '/admin/launch/modify/3', '{\"appname\":\"battle\",\"apptype\":\"proxy\",\"apppath\":\"battle\",\"appfile\":\"battleserver\",\"appconfig\":\".\\/startup\\/battle.proxy.startup\",\"logtype\":\"2\",\"service\":\"cluster\",\"ftpid\":\"1\",\"deploypath\":\"..\\/\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/launch/modify/3', '3', 'admin', '1536032984', '3');
INSERT INTO `lw_log` VALUES ('80', '6', 'launch', 'modify', '/admin/launch/modify/3', '{\"apppath\":\"battle\",\"appfile\":\"battleserver\",\"appconfig\":\".\\/startup\\/battle.proxy.startup\",\"logtype\":\"2\",\"service\":\"cluster\",\"ftpid\":\"1\",\"deploypath\":\"..\\/\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/launch/modify/3', '3', 'admin', '1536034050', '3');
INSERT INTO `lw_log` VALUES ('81', '6', 'Machine', 'add', '/admin/machine/add', '{\"name\":\"6\",\"localip\":\"6\",\"interanetip\":\"6\",\"cpu\":\"6\",\"thread\":\"6\",\"memory\":\"6\",\"harddisk\":\"6\",\"agentid\":\"6\",\"status\":\"6\",\"port\":\"6\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/machine/add', '3', 'admin', '1536034559', '1');
INSERT INTO `lw_log` VALUES ('82', '6', 'Machine', 'modify', '/admin/machine/modify/6', '{\"cpu\":\"6\",\"thread\":\"6\",\"memory\":\"6\",\"harddisk\":\"6\",\"agentid\":\"9\",\"status\":\"6\",\"port\":\"6\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/machine/modify/6', '3', 'admin', '1536034637', '3');
INSERT INTO `lw_log` VALUES ('83', '6', 'Machine', 'modify', '/admin/machine/modify/6', '{\"cpu\":\"6\",\"thread\":\"a\",\"memory\":\"a\",\"harddisk\":\"6\",\"agentid\":\"9\",\"status\":\"a\",\"port\":\"a\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/machine/modify/6', '3', 'admin', '1536034698', '3');
INSERT INTO `lw_log` VALUES ('84', '6', 'Machine', 'del', '/admin/machine/del/6', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/machine/index', '3', 'admin', '1536034723', '2');
INSERT INTO `lw_log` VALUES ('85', '6', 'Project_group', 'add', '/admin/project_group/add', '{\"name\":\"\\u6d4b\\u8bd51\",\"comment\":\"\\u6d4b\\u8bd51\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/project_group/add', '3', 'admin', '1536112525', '1');
INSERT INTO `lw_log` VALUES ('86', '6', 'Project_group', 'del', '/admin/project_group/del/4', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/project_group/index', '3', 'admin', '1536112542', '2');
INSERT INTO `lw_log` VALUES ('87', '6', 'Project_group', 'modify', '/admin/project_group/modify/1', '{\"name\":\"\\u6c5f\\u6e56\\u6c42\\u751f2344\",\"comment\":\"\\u6c5f\\u6e56\\u6c42\\u751f23444\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/project_group/modify/1', '3', 'admin', '1536112614', '3');
INSERT INTO `lw_log` VALUES ('88', '6', 'Project', 'add', '/admin/project/add', '{\"group_id\":\"1\",\"name\":\"4\",\"repository_url\":\"4\",\"https_username\":\"4\",\"https_password\":\"4\",\"branch\":\"4\",\"code_path\":\"4\",\"code_dir_user\":\"4\",\"comment\":\"4\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/project/add', '3', 'admin', '1536112638', '1');
INSERT INTO `lw_log` VALUES ('89', '6', 'Project', 'del', '/admin/project/del/2', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/project', '3', 'admin', '1536112644', '2');
INSERT INTO `lw_log` VALUES ('90', '6', 'Project', 'modify', '/admin/project/modify/1', '{\"group_id\":\"1\",\"name\":\"555\",\"repository_url\":\"555\",\"https_username\":\"55\",\"https_password\":\"55\",\"branch\":\"55\",\"code_path\":\"5\",\"code_dir_user\":\"5\",\"comment\":\"88888888\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/project/modify/1', '3', 'admin', '1536112809', '3');
INSERT INTO `lw_log` VALUES ('91', '6', 'Node_group', 'add', '/admin/node_group/add', '{\"name\":\"77777\",\"comment\":\"777777\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/node_group/add', '3', 'admin', '1536112829', '1');
INSERT INTO `lw_log` VALUES ('92', '6', 'Node_group', 'del', '/admin/node_group/del/5', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/node_group/index', '3', 'admin', '1536112835', '2');
INSERT INTO `lw_log` VALUES ('93', '6', 'Node_group', 'modify', '/admin/node_group/modify/4', '{\"name\":\"aa8888\",\"comment\":\"aa88888\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/node_group/modify/4', '3', 'admin', '1536112846', '3');
INSERT INTO `lw_log` VALUES ('94', '6', 'Node', 'add', '/admin/node/add', '{\"ip\":\"222\",\"port\":\"22\",\"token\":\"22\",\"comment\":\"22\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/node/add', '3', 'admin', '1536112908', '1');
INSERT INTO `lw_log` VALUES ('95', '6', 'Node', 'del', '/admin/node/del/5', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/node/index', '3', 'admin', '1536112919', '2');
INSERT INTO `lw_log` VALUES ('96', '6', 'Node', 'modify', '/admin/node/modify/4', '{\"ip\":\"7766666666\",\"port\":\"766666666666\",\"token\":\"76666666666666\",\"comment\":\"7888888866666666666\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/node/modify/4', '3', 'admin', '1536112933', '3');
INSERT INTO `lw_log` VALUES ('97', '6', 'version', 'add', '/admin/version/add', '{\"version_time\":\"2018-09-05 10:03:40\",\"version_name\":\"2\",\"version_url\":\"2\",\"version_md5\":\"2\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/add', '3', 'admin', '1536113026', '1');
INSERT INTO `lw_log` VALUES ('98', '6', 'version', 'del', '/admin/version/del/2', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/index', '3', 'admin', '1536113142', '2');
INSERT INTO `lw_log` VALUES ('99', '6', 'Machine', 'add', '/admin/machine/add', '{\"name\":\"2\",\"localip\":\"2\",\"interanetip\":\"2\",\"cpu\":\"2\",\"thread\":\"2\",\"memory\":\"2\",\"harddisk\":\"2\",\"agentid\":\"2\",\"status\":\"2\",\"port\":\"2\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/machine/add', '3', 'admin', '1536113169', '1');
INSERT INTO `lw_log` VALUES ('100', '6', 'Machine', 'del', '/admin/machine/del/2', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/machine/index', '3', 'admin', '1536113176', '2');
INSERT INTO `lw_log` VALUES ('101', '6', 'Machine', 'modify', '/admin/machine/modify/192.168.10.230', '{\"cpu\":\"8\",\"thread\":\"16\",\"memory\":\"16\",\"harddisk\":\"100\",\"agentid\":\"101.0.3.227\",\"status\":\"12\",\"port\":\"120012\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/machine/modify/192.168.10.230', '3', 'admin', '1536113384', '3');
INSERT INTO `lw_log` VALUES ('102', '6', 'launch', 'add', '/admin/launch/add', '{\"appname\":\"1\",\"apptype\":\"1\",\"apppath\":\"1\",\"appfile\":\"1\",\"appconfig\":\"1\",\"logtype\":\"1\",\"service\":\"1\",\"ftpid\":\"1\",\"deploypath\":\"1\"}', '1', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/launch/add', '3', 'admin', '1536113409', '1');
INSERT INTO `lw_log` VALUES ('103', '6', 'launch', 'del', '/admin/launch/del/43', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/launch/index', '3', 'admin', '1536113416', '2');
INSERT INTO `lw_log` VALUES ('104', '6', 'launch', 'modify', '/admin/launch/modify/42', '{\"apppath\":\"zone\",\"appfile\":\"zoneserver\",\"appconfig\":\".\\/startup\\/zone.world.startup\",\"logtype\":\"2\",\"service\":\"zone\",\"ftpid\":\"12\",\"deploypath\":\"..\\/\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/launch/modify/42', '3', 'admin', '1536113429', '3');
INSERT INTO `lw_log` VALUES ('105', '6', 'Deploy', 'add', '/admin/deploy/add', '{\"appid\":\"2\",\"appname\":\"2\",\"apptype\":\"2\",\"startup\":\"2\",\"debug\":\"2\",\"shutdown\":\"2\",\"process\":\"\",\"agentid\":\"\",\"localip\":\"\"}', '0', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/deploy/add', '3', 'admin', '1536113573', '1');
INSERT INTO `lw_log` VALUES ('106', '6', 'Deploy', 'add', '/admin/deploy/add', '{\"appid\":\"2\",\"appname\":\"5\",\"apptype\":\"2\",\"startup\":\"2\",\"debug\":\"2\",\"shutdown\":\"22\",\"process\":\"2\",\"agentid\":\"2\",\"localip\":\"2\"}', '0', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/deploy/add', '3', 'admin', '1536113588', '1');
INSERT INTO `lw_log` VALUES ('107', '6', 'Deploy', 'del', '/admin/deploy/del/2', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/deploy/index', '3', 'admin', '1536113603', '2');
INSERT INTO `lw_log` VALUES ('108', '6', 'Deploy', 'add', '/admin/deploy/add', '{\"appid\":\"aa\",\"appname\":\"nn\",\"apptype\":\"7\",\"startup\":\"7\",\"debug\":\"7\",\"shutdown\":\"5\",\"process\":\"6\",\"agentid\":\"6\",\"localip\":\"6\"}', '1', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/deploy/add', '3', 'admin', '1536113626', '1');
INSERT INTO `lw_log` VALUES ('109', '6', 'Deploy', 'del', '/admin/deploy/del/0', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/deploy/index', '3', 'admin', '1536113661', '2');
INSERT INTO `lw_log` VALUES ('110', '6', 'Deploy', 'modify', '/admin/deploy/modify/101.3.245.1', '{\"appid\":\"101.3.245.1\",\"appname\":\"zone\",\"apptype\":\"game\",\"startup\":\"1\",\"debug\":\"0\",\"shutdown\":\"0\",\"process\":\"259509999\",\"agentid\":\"101.0.3.227\",\"localip\":\"192.168.10.230\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/deploy/modify/101.3.245.1', '3', 'admin', '1536113673', '3');
INSERT INTO `lw_log` VALUES ('111', '6', 'version', 'del', '/admin/version/del/sgame_svn_9296_201808291912.tar.gz', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/index', '3', 'admin', '1536632901', '2');
INSERT INTO `lw_log` VALUES ('112', '6', 'version', 'add', '/admin/version/add', '{\"version_time\":\"2018-09-11 10:31:08\",\"version_name\":\"7\",\"version_url\":\"88888\",\"version_md5\":\"8888888\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/add', '3', 'admin', '1536633076', '1');
INSERT INTO `lw_log` VALUES ('113', '6', 'version', 'del', '/admin/version/del/7', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/index', '3', 'admin', '1536634297', '2');
INSERT INTO `lw_log` VALUES ('114', '6', 'version', 'add', '/admin/version/add', '{\"version_time\":\"2018-09-11 10:51:46\",\"version_name\":\"7\",\"version_url\":\"88\",\"version_md5\":\"88\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/add', '3', 'admin', '1536634313', '1');
INSERT INTO `lw_log` VALUES ('115', '6', 'version', 'add', '/admin/version/add', '{\"version_time\":\"2018-09-11 11:01:42\",\"version_name\":\"8\",\"version_url\":\"99\",\"version_md5\":\"99\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/add', '3', 'admin', '1536634908', '1');
INSERT INTO `lw_log` VALUES ('116', '6', 'version', 'del', '/admin/version/del/8', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/index', '3', 'admin', '1536634914', '2');
INSERT INTO `lw_log` VALUES ('117', '6', 'version', 'add', '/admin/version/add', '{\"version_time\":\"2018-09-11 11:32:02\",\"version_name\":\"7\",\"version_url\":\"99\",\"version_md5\":\"665\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/add', '3', 'admin', '1536636729', '1');
INSERT INTO `lw_log` VALUES ('118', '6', 'version', 'add', '/admin/version/add', '{\"version_time\":\"2018-09-11 11:33:05\",\"version_name\":\"7\",\"version_url\":\"D:\\\\GMTool\\\\leiwu_version\\\\static\\\\upload\\\\version\",\"version_md5\":\"D:\\\\GMTool\\\\leiwu_version\\\\static\\\\upload\\\\version\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/add', '3', 'admin', '1536636798', '1');
INSERT INTO `lw_log` VALUES ('119', '6', 'version', 'del', '/admin/version/del/7', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/index', '3', 'admin', '1536636805', '2');
INSERT INTO `lw_log` VALUES ('120', '6', 'version', 'add', '/admin/version/add', '{\"version_time\":\"2018-09-11 11:34:00\",\"version_name\":\"7\",\"version_url\":\"http:\\/\\/test.version.com\\/static\\/upload\\/version\\/test.sql\",\"version_md5\":\"44\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/add', '3', 'admin', '1536636936', '1');
INSERT INTO `lw_log` VALUES ('121', '6', 'version', 'del', '/admin/version/del/7', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/index', '3', 'admin', '1536636965', '2');
INSERT INTO `lw_log` VALUES ('122', '6', 'version', 'add', '/admin/version/add', '{\"version_time\":\"2018-09-11 11:41:36\",\"version_name\":\"8\",\"version_url\":\"\\/static\\/upload\\/version\\/test.sql\",\"version_md5\":\"555\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/add', '3', 'admin', '1536637350', '1');
INSERT INTO `lw_log` VALUES ('123', '6', 'version', 'add', '/admin/version/add', '{\"version_time\":\"2018-09-11 11:42:33\",\"version_name\":\"99\",\"version_url\":\"\\/static\\/upload\\/version\\/test.sql\",\"version_md5\":\"443\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/add', '3', 'admin', '1536637361', '1');
INSERT INTO `lw_log` VALUES ('124', '6', 'version', 'del', '/admin/version/del/99', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/index', '3', 'admin', '1536637367', '2');
INSERT INTO `lw_log` VALUES ('125', '6', 'version', 'del', '/admin/version/del/8', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/index', '3', 'admin', '1536637432', '2');
INSERT INTO `lw_log` VALUES ('126', '6', 'version', 'add', '/admin/version/add', '{\"version_time\":\"2018-09-11 11:49:33\",\"version_name\":\"88\",\"version_url\":\"66\",\"version_md5\":\"434\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/add', '3', 'admin', '1536637779', '1');
INSERT INTO `lw_log` VALUES ('127', '6', 'version', 'del', '/admin/version/del/88', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/index', '3', 'admin', '1536637782', '2');
INSERT INTO `lw_log` VALUES ('128', '6', 'version', 'add', '/admin/version/add', '{\"version_time\":\"2018-09-11 11:55:13\",\"version_name\":\"8\",\"version_url\":\"333\",\"version_md5\":\"8888888\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/add', '3', 'admin', '1536638124', '1');
INSERT INTO `lw_log` VALUES ('129', '6', 'version', 'add', '/admin/version/add', '{\"version_time\":\"2018-09-11 12:04:15\",\"version_name\":\"555\",\"version_md5\":\"8888888\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/add', '3', 'admin', '1536638661', '1');
INSERT INTO `lw_log` VALUES ('130', '6', 'version', 'del', '/admin/version/del/8', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/index', '3', 'admin', '1536638699', '2');
INSERT INTO `lw_log` VALUES ('131', '6', 'version', 'add', '/admin/version/add', '{\"version_time\":\"2018-09-11 12:24:32\",\"version_name\":\"866\",\"version_md5\":\"554\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/add', '3', 'admin', '1536639878', '1');
INSERT INTO `lw_log` VALUES ('132', '6', 'version', 'del', '/admin/version/del/866', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/index', '3', 'admin', '1536639920', '2');
INSERT INTO `lw_log` VALUES ('133', '6', 'version', 'del', '/admin/version/del/test.txt', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/index', '3', 'admin', '1536640024', '2');
INSERT INTO `lw_log` VALUES ('134', '6', 'version', 'add', '/admin/version/add', '{\"version_time\":\"2018-09-11 12:27:08\",\"version_name\":\"55\",\"version_md5\":\"222\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/version/add', '3', 'admin', '1536640034', '1');
INSERT INTO `lw_log` VALUES ('135', '6', 'launch', 'del', '/admin/launch/del/battle/proxy', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/launch/index', '3', 'admin', '1536654803', '2');
INSERT INTO `lw_log` VALUES ('136', '6', 'launch', 'del', '/admin/launch/del/battle/shard', '[]', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/launch/index', '3', 'admin', '1536654809', '2');
INSERT INTO `lw_log` VALUES ('137', '6', 'launch', 'modify', '/admin/launch/modify/data/master', '{\"apppath\":\"data2\",\"appfile\":\"dataserver2\",\"appconfig\":\".\\/startup\\/data.master.startup2\",\"logtype\":\"22\",\"service\":\"cluster2\",\"ftpid\":\"12\",\"deploypath\":\"..\\/2\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/launch/modify/data/master', '3', 'admin', '1536655161', '3');
INSERT INTO `lw_log` VALUES ('138', '6', 'Deploy', 'add', '/admin/deploy/add', '{\"appid\":\"101.0.11.1\",\"appname\":\"101.0.11.1\",\"apptype\":\"101.0.11.1\",\"startup\":\"101.0.11.1\",\"debug\":\"\",\"shutdown\":\"\",\"process\":\"\",\"agentid\":\"\",\"localip\":\"\"}', '0', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/deploy/add', '3', 'admin', '1536730148', '1');
INSERT INTO `lw_log` VALUES ('139', '6', 'Deploy', 'modify', '/admin/deploy/modify/101.0.11.1', '{\"appname\":\"data\",\"apptype\":\"master\",\"startup\":\"1\",\"debug\":\"1\",\"shutdown\":\"0\",\"process\":\"36335\",\"agentid\":\"101.0.3.1\",\"localip\":\"192.168.1.9\"}', '', '127.0.0.1', 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36', 'http://test.version.com/admin/deploy/modify/101.0.11.1', '3', 'admin', '1536730597', '3');

-- ----------------------------
-- Table structure for `lw_node`
-- ----------------------------
DROP TABLE IF EXISTS `lw_node`;
CREATE TABLE `lw_node` (
  `node_id` int(10) NOT NULL AUTO_INCREMENT COMMENT '节点信息表主键id',
  `ip` varchar(15) NOT NULL DEFAULT '' COMMENT '节点主机IP',
  `port` int(10) NOT NULL DEFAULT '0' COMMENT '节点主机端口',
  `token` varchar(32) NOT NULL DEFAULT '' COMMENT 'token',
  `comment` varchar(30) NOT NULL DEFAULT '' COMMENT '备注',
  `last_active_time` int(11) NOT NULL DEFAULT '0' COMMENT '最后存活时间',
  `is_delete` tinyint(3) NOT NULL DEFAULT '0' COMMENT '是否删除 0 否 1 是',
  `create_time` int(11) NOT NULL DEFAULT '0' COMMENT '创建时间',
  `update_time` int(11) NOT NULL DEFAULT '0' COMMENT '更新时间',
  `add_user` tinyint(2) DEFAULT '0',
  `status` tinyint(2) DEFAULT '0' COMMENT '服务器返回状态',
  PRIMARY KEY (`node_id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8 COMMENT='节点信息表';

-- ----------------------------
-- Records of lw_node
-- ----------------------------
INSERT INTO `lw_node` VALUES ('1', '192', '9091', '45ac11889b349a279f7c878d0f654ece', '999995555', '0', '0', '1535534683', '1535534971', '0', '0');
INSERT INTO `lw_node` VALUES ('4', '2147483647', '2147483647', '76666666666666', '7888888866666666666', '0', '0', '1535954194', '0', '3', '0');

-- ----------------------------
-- Table structure for `lw_node_group`
-- ----------------------------
DROP TABLE IF EXISTS `lw_node_group`;
CREATE TABLE `lw_node_group` (
  `group_id` int(10) NOT NULL AUTO_INCREMENT COMMENT '节点组表主键id',
  `name` varchar(50) NOT NULL DEFAULT '' COMMENT '节点组名称',
  `comment` varchar(100) NOT NULL DEFAULT '' COMMENT '备注',
  `is_delete` tinyint(3) NOT NULL DEFAULT '0' COMMENT '是否删除 0 否 1 是',
  `create_time` int(11) NOT NULL DEFAULT '0' COMMENT '创建时间',
  `update_time` int(11) NOT NULL DEFAULT '0' COMMENT '更新时间',
  `add_user` tinyint(2) DEFAULT '0',
  PRIMARY KEY (`group_id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8 COMMENT='节点组表';

-- ----------------------------
-- Records of lw_node_group
-- ----------------------------
INSERT INTO `lw_node_group` VALUES ('1', '江湖求生', '江湖求生2', '1', '1535534657', '1535534657', '0');
INSERT INTO `lw_node_group` VALUES ('4', 'aa8888', 'aa88888', '0', '1535969520', '1536112846', '3');
INSERT INTO `lw_node_group` VALUES ('5', '77777', '777777', '1', '1536112829', '1536112829', '3');

-- ----------------------------
-- Table structure for `lw_project`
-- ----------------------------
DROP TABLE IF EXISTS `lw_project`;
CREATE TABLE `lw_project` (
  `project_id` int(11) NOT NULL AUTO_INCREMENT COMMENT '项目id',
  `name` varchar(50) NOT NULL DEFAULT '' COMMENT '项目名称',
  `user_id` int(11) NOT NULL COMMENT 'user id',
  `group_id` int(11) NOT NULL COMMENT '项目组 id',
  `repository_url` varchar(300) NOT NULL COMMENT 'git 仓库地址 https or ssh',
  `branch` varchar(50) NOT NULL COMMENT '分支',
  `ssh_key` text NOT NULL COMMENT 'ssh key ',
  `ssh_key_salt` text NOT NULL COMMENT 'ssh  key salt',
  `https_username` varchar(50) NOT NULL COMMENT 'https 用户名',
  `https_password` varchar(50) NOT NULL COMMENT 'https 密码',
  `code_path` varchar(200) NOT NULL COMMENT '代码发布目录',
  `code_dir_user` varchar(50) NOT NULL COMMENT '目录所属用户',
  `pre_command` text NOT NULL COMMENT '前置命令',
  `pre_command_exec_type` tinyint(1) NOT NULL DEFAULT '1' COMMENT '前置命令执行方式, 1 同步执行，遇到错误停止;2 同步执行，遇到错误继续;3 异步执行',
  `pre_command_exec_timeout` int(11) NOT NULL DEFAULT '30' COMMENT '前置命令超时时间,单位秒',
  `post_command` text NOT NULL COMMENT '后置命令',
  `post_command_exec_type` int(11) NOT NULL DEFAULT '1' COMMENT '后置命令执行方式, 1 同步执行，遇到错误停止;2 同步执行，遇到错误继续;3 异步执行',
  `post_command_exec_timeout` int(11) NOT NULL DEFAULT '30' COMMENT '后置命令超时时间,单位秒',
  `comment` varchar(200) NOT NULL DEFAULT '' COMMENT '备注',
  `is_delete` tinyint(1) NOT NULL DEFAULT '0' COMMENT '删除 0 否 1 是',
  `is_grayscale_publish` tinyint(1) NOT NULL DEFAULT '0' COMMENT '是否开启灰度发布 0 否 1 是',
  `last_publish_time` int(11) NOT NULL DEFAULT '0' COMMENT '最后一次发布时间',
  `create_time` int(11) NOT NULL DEFAULT '0' COMMENT '创建时间',
  `update_time` int(11) NOT NULL DEFAULT '0' COMMENT '修改时间',
  `add_user` tinyint(2) DEFAULT '0' COMMENT '管理员id',
  PRIMARY KEY (`project_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COMMENT='项目表';

-- ----------------------------
-- Records of lw_project
-- ----------------------------
INSERT INTO `lw_project` VALUES ('1', '555', '0', '1', '555', '55', '', '', '55', '55', '5', '5', '', '1', '30', '', '1', '30', '88888888', '0', '0', '0', '1536026907', '1536112809', '3');

-- ----------------------------
-- Table structure for `lw_project_group`
-- ----------------------------
DROP TABLE IF EXISTS `lw_project_group`;
CREATE TABLE `lw_project_group` (
  `group_id` int(10) NOT NULL AUTO_INCREMENT COMMENT '项目组表主键id',
  `name` varchar(50) NOT NULL DEFAULT '' COMMENT '项目组名称',
  `comment` varchar(100) NOT NULL DEFAULT '' COMMENT '备注',
  `is_delete` tinyint(3) NOT NULL DEFAULT '0' COMMENT '是否删除 0 否 1 是',
  `create_time` int(11) NOT NULL DEFAULT '0' COMMENT '创建时间',
  `update_time` int(11) NOT NULL DEFAULT '0' COMMENT '更新时间',
  `add_user` tinyint(2) DEFAULT '0',
  PRIMARY KEY (`group_id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8 COMMENT='项目组表';

-- ----------------------------
-- Records of lw_project_group
-- ----------------------------
INSERT INTO `lw_project_group` VALUES ('1', '江湖求生2344', '江湖求生23444', '0', '1535534540', '1536112614', '0');
INSERT INTO `lw_project_group` VALUES ('2', '88', '88', '1', '1535969920', '1535969920', '3');
INSERT INTO `lw_project_group` VALUES ('3', '99', '999', '1', '1535969972', '1535969972', '3');
INSERT INTO `lw_project_group` VALUES ('4', '测试1', '测试1', '1', '1536112525', '1536112525', '3');

-- ----------------------------
-- Table structure for `lw_user`
-- ----------------------------
DROP TABLE IF EXISTS `lw_user`;
CREATE TABLE `lw_user` (
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
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of lw_user
-- ----------------------------
INSERT INTO `lw_user` VALUES ('2', 'xue', '7ba6f30f6aad55ade5ca0e378817e7d2', 'xue', '127.0.0.1', '1527823051', '[\"5\",\"8\",\"15\",\"16\",\"36\",\"37\"]', '6,7', '0');
INSERT INTO `lw_user` VALUES ('3', 'admin', 'e10adc3949ba59abbe56e057f20f883e', '超级管理员', '127.0.0.1', '1536803286', '', '1,2,3,4,5,6', '1');
INSERT INTO `lw_user` VALUES ('7', 'test', 'e10adc3949ba59abbe56e057f20f883e', '测试用户', null, null, '[\"5\",\"8\",\"15\"]', '0', '1');
INSERT INTO `lw_user` VALUES ('8', 'test', '96e79218965eb72c92a549dd5a330112', 'test', null, null, '[\"5\",\"8\",\"40\"]', '0', '0');
