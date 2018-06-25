/*
Navicat MySQL Data Transfer

Source Server         : 本地
Source Server Version : 50553
Source Host           : localhost:3306
Source Database       : gm_admin

Target Server Type    : MYSQL
Target Server Version : 50553
File Encoding         : 65001

Date: 2018-06-20 17:09:15
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
) ENGINE=InnoDB AUTO_INCREMENT=42 DEFAULT CHARSET=utf8 COMMENT='权限列表';

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
INSERT INTO `gm_auth` VALUES ('41', '1', '1', 'IP限制', 'limit', 'ip', '超级管理员', '1529465889');

-- ----------------------------
-- Table structure for `gm_cdk`
-- ----------------------------
DROP TABLE IF EXISTS `gm_cdk`;
CREATE TABLE `gm_cdk` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `cdkey` varchar(30) NOT NULL DEFAULT '' COMMENT '对外id',
  `title` varchar(50) NOT NULL COMMENT '标题',
  `describe` varchar(200) DEFAULT NULL COMMENT '描述',
  `start_time` int(10) NOT NULL DEFAULT '0' COMMENT '开始时间',
  `end_time` int(10) NOT NULL DEFAULT '0' COMMENT '结束时间',
  `add_user` varchar(56) NOT NULL COMMENT '添加人',
  `add_time` int(11) NOT NULL COMMENT '添加时间',
  `update_time` int(10) DEFAULT '0' COMMENT '更新时间',
  `frequency` tinyint(2) NOT NULL DEFAULT '0' COMMENT '使用频次（1:单码2:通码）',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=56 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of gm_cdk
-- ----------------------------
INSERT INTO `gm_cdk` VALUES ('50', '1806191532', '9999', '999999999999999999999999999999999999999999999999999999999999999', '1529393545', '1529393548', '3', '1529393552', '1529393552', '0');
INSERT INTO `gm_cdk` VALUES ('51', '1806191534', '666666666', '6666666666666666666666666666666666666666666666666666666666666666666', '1529393672', '1529393674', '3', '1529393677', '1529393677', '0');
INSERT INTO `gm_cdk` VALUES ('52', '1806191643', '1000测试55', '1000测试55', '1529397802', '1529657004', '3', '1529397810', '1529397810', '0');
INSERT INTO `gm_cdk` VALUES ('53', '1806191738', '999', '999', '1529401132', '1529401134', '3', '1529401139', '1529401139', '0');
INSERT INTO `gm_cdk` VALUES ('54', '1806191743', '1111', '1111', '1529401375', '1529401377', '3', '1529401383', '1529401383', '0');
INSERT INTO `gm_cdk` VALUES ('55', '1806191746', '555', '5555', '1529401600', '1529401603', '3', '1529401607', '1529401607', '1');

-- ----------------------------
-- Table structure for `gm_cdk_code`
-- ----------------------------
DROP TABLE IF EXISTS `gm_cdk_code`;
CREATE TABLE `gm_cdk_code` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `cdk_id` tinyint(3) DEFAULT '0' COMMENT 'cdk表id',
  `cdkey` varchar(30) NOT NULL DEFAULT '' COMMENT '对外id',
  `code` varchar(50) NOT NULL COMMENT '礼包随机编码',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1274 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of gm_cdk_code
-- ----------------------------
INSERT INTO `gm_cdk_code` VALUES ('52', '51', '1806191534', '');
INSERT INTO `gm_cdk_code` VALUES ('53', '51', '1806191534', '');
INSERT INTO `gm_cdk_code` VALUES ('54', '51', '1806191534', '');
INSERT INTO `gm_cdk_code` VALUES ('55', '51', '1806191534', '');
INSERT INTO `gm_cdk_code` VALUES ('56', '51', '1806191534', '');
INSERT INTO `gm_cdk_code` VALUES ('57', '51', '1806191534', '');
INSERT INTO `gm_cdk_code` VALUES ('58', '51', '1806191534', '');
INSERT INTO `gm_cdk_code` VALUES ('59', '51', '1806191534', '');
INSERT INTO `gm_cdk_code` VALUES ('60', '51', '1806191534', '');
INSERT INTO `gm_cdk_code` VALUES ('61', '51', '1806191534', 'test');
INSERT INTO `gm_cdk_code` VALUES ('62', '51', '1806191534', '25cnqhv86ebd');
INSERT INTO `gm_cdk_code` VALUES ('63', '51', '1806191534', 'rx84f3wmejtc');
INSERT INTO `gm_cdk_code` VALUES ('64', '51', '1806191534', 'qre3kawpbtf5');
INSERT INTO `gm_cdk_code` VALUES ('65', '51', '1806191534', 'bwes3ntdvhkz');
INSERT INTO `gm_cdk_code` VALUES ('66', '51', '1806191534', 'atqgb5fsvld8');
INSERT INTO `gm_cdk_code` VALUES ('67', '51', '1806191534', 'qk5ulrsnjedy');
INSERT INTO `gm_cdk_code` VALUES ('68', '51', '1806191534', 'ameq34ht7j8v');
INSERT INTO `gm_cdk_code` VALUES ('69', '51', '1806191534', 'r2fbwsezj9xq');
INSERT INTO `gm_cdk_code` VALUES ('70', '51', '1806191534', '69yr4jechfms');
INSERT INTO `gm_cdk_code` VALUES ('71', '51', '1806191534', 'qudbz5r9sht3');
INSERT INTO `gm_cdk_code` VALUES ('72', '50', '1806191532', '3urhj45xymgn');
INSERT INTO `gm_cdk_code` VALUES ('73', '50', '1806191532', 'l69atswgxnm4');
INSERT INTO `gm_cdk_code` VALUES ('74', '50', '1806191532', 'kjbtez7av6wg');
INSERT INTO `gm_cdk_code` VALUES ('75', '50', '1806191532', 'bn4up35rgzfs');
INSERT INTO `gm_cdk_code` VALUES ('76', '50', '1806191532', 'a6zpnjk7dtvr');
INSERT INTO `gm_cdk_code` VALUES ('77', '50', '1806191532', 'x9b65veh7kzu');
INSERT INTO `gm_cdk_code` VALUES ('78', '50', '1806191532', 'qh6m27e5butd');
INSERT INTO `gm_cdk_code` VALUES ('79', '50', '1806191532', 'xua9ykhfs2lp');
INSERT INTO `gm_cdk_code` VALUES ('80', '50', '1806191532', 'wr4v2plec7hf');
INSERT INTO `gm_cdk_code` VALUES ('81', '50', '1806191532', 'bmqrez3ay7cg');
INSERT INTO `gm_cdk_code` VALUES ('82', '50', '1806191532', 'gml398fpbkz7');
INSERT INTO `gm_cdk_code` VALUES ('83', '50', '1806191532', 'dkqr7as9lut2');
INSERT INTO `gm_cdk_code` VALUES ('84', '50', '1806191532', 'tn2qdc5vmewf');
INSERT INTO `gm_cdk_code` VALUES ('85', '50', '1806191532', '9ab3xr4h2vmk');
INSERT INTO `gm_cdk_code` VALUES ('86', '50', '1806191532', 'r2khgp3x74cb');
INSERT INTO `gm_cdk_code` VALUES ('87', '50', '1806191532', '5tm8d3gnq497');
INSERT INTO `gm_cdk_code` VALUES ('88', '50', '1806191532', 'mpujdx7k4crb');
INSERT INTO `gm_cdk_code` VALUES ('89', '50', '1806191532', 'e9zc6t58vxha');
INSERT INTO `gm_cdk_code` VALUES ('90', '50', '1806191532', '75pknjb8cgez');
INSERT INTO `gm_cdk_code` VALUES ('91', '50', '1806191532', 'ryjhd26k57zs');
INSERT INTO `gm_cdk_code` VALUES ('92', '50', '1806191532', 'l8gcprjbe5ut');
INSERT INTO `gm_cdk_code` VALUES ('93', '50', '1806191532', 'fa2vtsjngw5l');
INSERT INTO `gm_cdk_code` VALUES ('94', '50', '1806191532', '8z27w3rf4m5t');
INSERT INTO `gm_cdk_code` VALUES ('95', '50', '1806191532', 'tdxm2kysc4au');
INSERT INTO `gm_cdk_code` VALUES ('96', '50', '1806191532', 'gn8rp5u43tjb');
INSERT INTO `gm_cdk_code` VALUES ('97', '50', '1806191532', 'gtpwxqrnzvm2');
INSERT INTO `gm_cdk_code` VALUES ('98', '50', '1806191532', 'p4trfx9v2cde');
INSERT INTO `gm_cdk_code` VALUES ('99', '50', '1806191532', 'g2dsvycutq7h');
INSERT INTO `gm_cdk_code` VALUES ('100', '50', '1806191532', 'f6t7cxmp35ew');
INSERT INTO `gm_cdk_code` VALUES ('101', '50', '1806191532', 'lpg2ukb8mrdw');
INSERT INTO `gm_cdk_code` VALUES ('102', '50', '1806191532', 'gznx7aup3sbk');
INSERT INTO `gm_cdk_code` VALUES ('103', '50', '1806191532', 'njpd6s8lt3me');
INSERT INTO `gm_cdk_code` VALUES ('104', '50', '1806191532', '9qb4apxnyzgd');
INSERT INTO `gm_cdk_code` VALUES ('105', '50', '1806191532', 'a2et3xymu697');
INSERT INTO `gm_cdk_code` VALUES ('106', '50', '1806191532', 'bptmxyv2z8aj');
INSERT INTO `gm_cdk_code` VALUES ('107', '50', '1806191532', 'n7pm3lduaq6c');
INSERT INTO `gm_cdk_code` VALUES ('108', '50', '1806191532', 'pzgylvemnfx5');
INSERT INTO `gm_cdk_code` VALUES ('109', '50', '1806191532', 'h9vup3msb5nk');
INSERT INTO `gm_cdk_code` VALUES ('110', '50', '1806191532', 'ranmw5xchsk8');
INSERT INTO `gm_cdk_code` VALUES ('111', '50', '1806191532', 'vewapqlc649j');
INSERT INTO `gm_cdk_code` VALUES ('112', '50', '1806191532', 'pzuakhm39xdb');
INSERT INTO `gm_cdk_code` VALUES ('113', '50', '1806191532', 'cux9lfar7ez3');
INSERT INTO `gm_cdk_code` VALUES ('114', '50', '1806191532', 'vlw85j3cqs74');
INSERT INTO `gm_cdk_code` VALUES ('115', '50', '1806191532', 'hzkatjnx5rmf');
INSERT INTO `gm_cdk_code` VALUES ('116', '50', '1806191532', 'z4xvuf9ajqrk');
INSERT INTO `gm_cdk_code` VALUES ('117', '50', '1806191532', 'bvxltr9c6ua3');
INSERT INTO `gm_cdk_code` VALUES ('118', '50', '1806191532', 'axkqym7z3uw6');
INSERT INTO `gm_cdk_code` VALUES ('119', '50', '1806191532', 'sjfmyrbutndw');
INSERT INTO `gm_cdk_code` VALUES ('120', '50', '1806191532', 'mh3p654bjkrz');
INSERT INTO `gm_cdk_code` VALUES ('121', '50', '1806191532', 'r65amf4qe873');
INSERT INTO `gm_cdk_code` VALUES ('122', '50', '1806191532', 'uq8sxzrcnmey');
INSERT INTO `gm_cdk_code` VALUES ('123', '50', '1806191532', 'dfn2ylq8me3x');
INSERT INTO `gm_cdk_code` VALUES ('124', '50', '1806191532', 'zs37urmdgk9y');
INSERT INTO `gm_cdk_code` VALUES ('125', '50', '1806191532', '9lq7p3gf4r68');
INSERT INTO `gm_cdk_code` VALUES ('126', '50', '1806191532', 'wcstypxrgjle');
INSERT INTO `gm_cdk_code` VALUES ('127', '50', '1806191532', 'uvm3e6bcy7dp');
INSERT INTO `gm_cdk_code` VALUES ('128', '50', '1806191532', 'v2db5ymtsf3x');
INSERT INTO `gm_cdk_code` VALUES ('129', '50', '1806191532', 'xqj37lby4ags');
INSERT INTO `gm_cdk_code` VALUES ('130', '50', '1806191532', 'b5zwgn9k3sl2');
INSERT INTO `gm_cdk_code` VALUES ('131', '50', '1806191532', 'm6sv24gtfwau');
INSERT INTO `gm_cdk_code` VALUES ('132', '50', '1806191532', '7l35umkwhby6');
INSERT INTO `gm_cdk_code` VALUES ('133', '50', '1806191532', 'uxqp9bljnsf4');
INSERT INTO `gm_cdk_code` VALUES ('134', '50', '1806191532', 'zlegw9h8u6tk');
INSERT INTO `gm_cdk_code` VALUES ('135', '50', '1806191532', 'nq3798jl5rza');
INSERT INTO `gm_cdk_code` VALUES ('136', '50', '1806191532', 'lzw4uj2m3aer');
INSERT INTO `gm_cdk_code` VALUES ('137', '50', '1806191532', 'mdnlcrqf3e5b');
INSERT INTO `gm_cdk_code` VALUES ('138', '50', '1806191532', 'q58dkn6az3cu');
INSERT INTO `gm_cdk_code` VALUES ('139', '50', '1806191532', 'nq6jv3y7ukbr');
INSERT INTO `gm_cdk_code` VALUES ('140', '50', '1806191532', '7hwpavn5cjlq');
INSERT INTO `gm_cdk_code` VALUES ('141', '50', '1806191532', 'pdl8yg35c9eb');
INSERT INTO `gm_cdk_code` VALUES ('142', '50', '1806191532', 'tagr79zljc2s');
INSERT INTO `gm_cdk_code` VALUES ('143', '50', '1806191532', '76rzenfx8ts3');
INSERT INTO `gm_cdk_code` VALUES ('144', '50', '1806191532', 'ndswe63kv9l5');
INSERT INTO `gm_cdk_code` VALUES ('145', '50', '1806191532', 'w3mk8j4uc27v');
INSERT INTO `gm_cdk_code` VALUES ('146', '50', '1806191532', 'pjqk7v9nm6x8');
INSERT INTO `gm_cdk_code` VALUES ('147', '50', '1806191532', '8ztxjyqbhf4e');
INSERT INTO `gm_cdk_code` VALUES ('148', '50', '1806191532', 'pc6jz4wfa27y');
INSERT INTO `gm_cdk_code` VALUES ('149', '50', '1806191532', 'w3b84hzymfv2');
INSERT INTO `gm_cdk_code` VALUES ('150', '50', '1806191532', 'ud2vymakwp6h');
INSERT INTO `gm_cdk_code` VALUES ('151', '50', '1806191532', '85udezkwx49q');
INSERT INTO `gm_cdk_code` VALUES ('152', '50', '1806191532', 'x9h324fn6byw');
INSERT INTO `gm_cdk_code` VALUES ('153', '50', '1806191532', 'pscj7gny5lua');
INSERT INTO `gm_cdk_code` VALUES ('154', '50', '1806191532', 'fqtzpvjl69ds');
INSERT INTO `gm_cdk_code` VALUES ('155', '50', '1806191532', 'da24mu3s59tq');
INSERT INTO `gm_cdk_code` VALUES ('156', '50', '1806191532', 'sty28w7rlkzx');
INSERT INTO `gm_cdk_code` VALUES ('157', '50', '1806191532', 'yf6ge57dzrs9');
INSERT INTO `gm_cdk_code` VALUES ('158', '50', '1806191532', 'r9xt7h2jaqsu');
INSERT INTO `gm_cdk_code` VALUES ('159', '50', '1806191532', 'grxmtpu5za6s');
INSERT INTO `gm_cdk_code` VALUES ('160', '50', '1806191532', '84j3lek9spt2');
INSERT INTO `gm_cdk_code` VALUES ('161', '50', '1806191532', 'z3nwetsja4kg');
INSERT INTO `gm_cdk_code` VALUES ('162', '50', '1806191532', 'fkrx8vbcp753');
INSERT INTO `gm_cdk_code` VALUES ('163', '50', '1806191532', 'v7c4q5ptra9l');
INSERT INTO `gm_cdk_code` VALUES ('164', '50', '1806191532', 'sc4pw2jl5dtr');
INSERT INTO `gm_cdk_code` VALUES ('165', '50', '1806191532', 'xk2v7mnyu3t5');
INSERT INTO `gm_cdk_code` VALUES ('166', '50', '1806191532', 'm5sfuw6tqn9b');
INSERT INTO `gm_cdk_code` VALUES ('167', '50', '1806191532', '5rdz9n837cju');
INSERT INTO `gm_cdk_code` VALUES ('168', '50', '1806191532', 'ek4vunhgbz8d');
INSERT INTO `gm_cdk_code` VALUES ('169', '50', '1806191532', '9krhfntmdqup');
INSERT INTO `gm_cdk_code` VALUES ('170', '50', '1806191532', 'aqvk9znwtxru');
INSERT INTO `gm_cdk_code` VALUES ('171', '50', '1806191532', 'rsgkbxdca5f3');
INSERT INTO `gm_cdk_code` VALUES ('172', '50', '1806191532', '3y4pxnfqksde');
INSERT INTO `gm_cdk_code` VALUES ('173', '50', '1806191532', 'd2snwmbt9fhl');
INSERT INTO `gm_cdk_code` VALUES ('174', '50', '1806191532', 'pcyqhs7263ge');
INSERT INTO `gm_cdk_code` VALUES ('175', '50', '1806191532', 'c3tjyxqsdgh7');
INSERT INTO `gm_cdk_code` VALUES ('176', '50', '1806191532', '5ukcmb67evs9');
INSERT INTO `gm_cdk_code` VALUES ('177', '50', '1806191532', 'd67ytjp42nqe');
INSERT INTO `gm_cdk_code` VALUES ('178', '50', '1806191532', '67xfc3b4qhkv');
INSERT INTO `gm_cdk_code` VALUES ('179', '50', '1806191532', '3sfyu75kxz8r');
INSERT INTO `gm_cdk_code` VALUES ('180', '50', '1806191532', 'c2azhj9wk46u');
INSERT INTO `gm_cdk_code` VALUES ('181', '50', '1806191532', 'rla47skxwdq2');
INSERT INTO `gm_cdk_code` VALUES ('182', '50', '1806191532', 'kypb6tfc3mwa');
INSERT INTO `gm_cdk_code` VALUES ('183', '50', '1806191532', 'fxbkqars6jng');
INSERT INTO `gm_cdk_code` VALUES ('184', '50', '1806191532', '86kq52jbtc7x');
INSERT INTO `gm_cdk_code` VALUES ('185', '50', '1806191532', 'ul3nfthwzr27');
INSERT INTO `gm_cdk_code` VALUES ('186', '50', '1806191532', 'sjk8273g9qpl');
INSERT INTO `gm_cdk_code` VALUES ('187', '50', '1806191532', 'ra9klp3mjy52');
INSERT INTO `gm_cdk_code` VALUES ('188', '50', '1806191532', '7bumvj6qw9r5');
INSERT INTO `gm_cdk_code` VALUES ('189', '50', '1806191532', 'gqafbzskhv32');
INSERT INTO `gm_cdk_code` VALUES ('190', '50', '1806191532', 'as8d4pymnzqk');
INSERT INTO `gm_cdk_code` VALUES ('191', '50', '1806191532', 'mb9uwxnevhc3');
INSERT INTO `gm_cdk_code` VALUES ('192', '50', '1806191532', 'cyae83mpfv24');
INSERT INTO `gm_cdk_code` VALUES ('193', '50', '1806191532', '3zk8twaxud7b');
INSERT INTO `gm_cdk_code` VALUES ('194', '50', '1806191532', 'r93kg4nuhfpc');
INSERT INTO `gm_cdk_code` VALUES ('195', '50', '1806191532', 'uld5g2h4r86b');
INSERT INTO `gm_cdk_code` VALUES ('196', '50', '1806191532', 'hspza69r5c2k');
INSERT INTO `gm_cdk_code` VALUES ('197', '50', '1806191532', 'yv4x367jfg8m');
INSERT INTO `gm_cdk_code` VALUES ('198', '50', '1806191532', 'gucn3j9x8ya5');
INSERT INTO `gm_cdk_code` VALUES ('199', '50', '1806191532', 'ltdbcr5hags2');
INSERT INTO `gm_cdk_code` VALUES ('200', '50', '1806191532', '4a9knyemtucq');
INSERT INTO `gm_cdk_code` VALUES ('201', '50', '1806191532', 'vasrj8nedmh7');
INSERT INTO `gm_cdk_code` VALUES ('202', '50', '1806191532', '8p9a6uqtrblz');
INSERT INTO `gm_cdk_code` VALUES ('203', '50', '1806191532', '6puwmbfqn57x');
INSERT INTO `gm_cdk_code` VALUES ('204', '50', '1806191532', 'jw9blqfshprm');
INSERT INTO `gm_cdk_code` VALUES ('205', '50', '1806191532', 'xkyfdte2g39u');
INSERT INTO `gm_cdk_code` VALUES ('206', '50', '1806191532', '36c7d4ftrahk');
INSERT INTO `gm_cdk_code` VALUES ('207', '50', '1806191532', 'zv7dkl4eg3t2');
INSERT INTO `gm_cdk_code` VALUES ('208', '50', '1806191532', 'phnszfed8ujx');
INSERT INTO `gm_cdk_code` VALUES ('209', '50', '1806191532', 'kwzva2b7hpgd');
INSERT INTO `gm_cdk_code` VALUES ('210', '50', '1806191532', 'rg5p3q67h8ts');
INSERT INTO `gm_cdk_code` VALUES ('211', '50', '1806191532', 'pmyhj74nrave');
INSERT INTO `gm_cdk_code` VALUES ('212', '50', '1806191532', 'w7et3qyhnsf2');
INSERT INTO `gm_cdk_code` VALUES ('213', '50', '1806191532', '5hvy8fejal7p');
INSERT INTO `gm_cdk_code` VALUES ('214', '50', '1806191532', '4kzul3hdn6tf');
INSERT INTO `gm_cdk_code` VALUES ('215', '50', '1806191532', 'tbupnhcqx8wk');
INSERT INTO `gm_cdk_code` VALUES ('216', '50', '1806191532', '7hasf6pz53et');
INSERT INTO `gm_cdk_code` VALUES ('217', '50', '1806191532', '8f6ku49zx3r7');
INSERT INTO `gm_cdk_code` VALUES ('218', '50', '1806191532', '795lfq8zmenh');
INSERT INTO `gm_cdk_code` VALUES ('219', '50', '1806191532', 'ynh4tm2le3bk');
INSERT INTO `gm_cdk_code` VALUES ('220', '50', '1806191532', '3pd9qz4vths6');
INSERT INTO `gm_cdk_code` VALUES ('221', '50', '1806191532', '7eph5std4mg3');
INSERT INTO `gm_cdk_code` VALUES ('222', '50', '1806191532', 'd3nvh6ym9xue');
INSERT INTO `gm_cdk_code` VALUES ('223', '50', '1806191532', 'uz4yt6skcqpb');
INSERT INTO `gm_cdk_code` VALUES ('224', '50', '1806191532', 'hekaz2g4rqxl');
INSERT INTO `gm_cdk_code` VALUES ('225', '50', '1806191532', 'wgcmzu9qyrnd');
INSERT INTO `gm_cdk_code` VALUES ('226', '50', '1806191532', '5w3flscxdpv6');
INSERT INTO `gm_cdk_code` VALUES ('227', '50', '1806191532', 'ct47pkn86eg9');
INSERT INTO `gm_cdk_code` VALUES ('228', '50', '1806191532', 'ub7f3pwzgk8a');
INSERT INTO `gm_cdk_code` VALUES ('229', '50', '1806191532', 'zpnm7ra5dqxh');
INSERT INTO `gm_cdk_code` VALUES ('230', '50', '1806191532', 'lqcxgwdrptmy');
INSERT INTO `gm_cdk_code` VALUES ('231', '50', '1806191532', '3kxrh8sg9wqb');
INSERT INTO `gm_cdk_code` VALUES ('232', '50', '1806191532', 'e9nl67tyjxrw');
INSERT INTO `gm_cdk_code` VALUES ('233', '50', '1806191532', '72jf6ktwgns8');
INSERT INTO `gm_cdk_code` VALUES ('234', '50', '1806191532', '3acb5rznktwf');
INSERT INTO `gm_cdk_code` VALUES ('235', '50', '1806191532', 'lwuj5nbf6teh');
INSERT INTO `gm_cdk_code` VALUES ('236', '50', '1806191532', 'wqa8cxkjt4rv');
INSERT INTO `gm_cdk_code` VALUES ('237', '50', '1806191532', 't24vk9q5djwr');
INSERT INTO `gm_cdk_code` VALUES ('238', '50', '1806191532', 'tn2j7cu5gw4l');
INSERT INTO `gm_cdk_code` VALUES ('239', '50', '1806191532', 'adb3je76yv54');
INSERT INTO `gm_cdk_code` VALUES ('240', '50', '1806191532', 'n4r9swyxkqv6');
INSERT INTO `gm_cdk_code` VALUES ('241', '50', '1806191532', 'esbfptu8wqg7');
INSERT INTO `gm_cdk_code` VALUES ('242', '50', '1806191532', 'a2lhmcdxb879');
INSERT INTO `gm_cdk_code` VALUES ('243', '50', '1806191532', 'aftz9xd23vrm');
INSERT INTO `gm_cdk_code` VALUES ('244', '50', '1806191532', '4u5tm3yhdgql');
INSERT INTO `gm_cdk_code` VALUES ('245', '50', '1806191532', 'h6xdbuymzta8');
INSERT INTO `gm_cdk_code` VALUES ('246', '50', '1806191532', 'zcybjt25aerd');
INSERT INTO `gm_cdk_code` VALUES ('247', '50', '1806191532', 'qgbt4dwahmjv');
INSERT INTO `gm_cdk_code` VALUES ('248', '50', '1806191532', 'bmva2clzdh3k');
INSERT INTO `gm_cdk_code` VALUES ('249', '50', '1806191532', '2vrpa8ycf59l');
INSERT INTO `gm_cdk_code` VALUES ('250', '50', '1806191532', 'fak76tqvnmug');
INSERT INTO `gm_cdk_code` VALUES ('251', '50', '1806191532', 'qd9a6g2rcv78');
INSERT INTO `gm_cdk_code` VALUES ('252', '50', '1806191532', 'hxvdb397ytm5');
INSERT INTO `gm_cdk_code` VALUES ('253', '50', '1806191532', 'bn45xr8au2pj');
INSERT INTO `gm_cdk_code` VALUES ('254', '50', '1806191532', 'jydl6euf3bpr');
INSERT INTO `gm_cdk_code` VALUES ('255', '50', '1806191532', 'skbv6jfcm4z7');
INSERT INTO `gm_cdk_code` VALUES ('256', '50', '1806191532', 'dajq2mu9ysce');
INSERT INTO `gm_cdk_code` VALUES ('257', '50', '1806191532', 'gsv8wyfadh2q');
INSERT INTO `gm_cdk_code` VALUES ('258', '50', '1806191532', 'sqbpvwg53e96');
INSERT INTO `gm_cdk_code` VALUES ('259', '50', '1806191532', 'eknwql2b3u4g');
INSERT INTO `gm_cdk_code` VALUES ('260', '50', '1806191532', 'm7rkb4qapdw6');
INSERT INTO `gm_cdk_code` VALUES ('261', '50', '1806191532', '2dlq9zhfxymg');
INSERT INTO `gm_cdk_code` VALUES ('262', '50', '1806191532', 'qhvzsgdkbnwc');
INSERT INTO `gm_cdk_code` VALUES ('263', '50', '1806191532', '94bgx2kastzm');
INSERT INTO `gm_cdk_code` VALUES ('264', '50', '1806191532', 'xr8s6ltc4fp9');
INSERT INTO `gm_cdk_code` VALUES ('265', '50', '1806191532', 's5z8y3vc6jml');
INSERT INTO `gm_cdk_code` VALUES ('266', '50', '1806191532', 'a3uf96t75my8');
INSERT INTO `gm_cdk_code` VALUES ('267', '50', '1806191532', '593hfk6twlms');
INSERT INTO `gm_cdk_code` VALUES ('268', '50', '1806191532', '2ys8rv5ujqkd');
INSERT INTO `gm_cdk_code` VALUES ('269', '50', '1806191532', 'rc4h9ezqfxgj');
INSERT INTO `gm_cdk_code` VALUES ('270', '50', '1806191532', '8vafph9xtlnm');
INSERT INTO `gm_cdk_code` VALUES ('271', '50', '1806191532', 'l4xkqn6e2bpf');
INSERT INTO `gm_cdk_code` VALUES ('272', '52', '1806191643', 'hztqym89dncv');
INSERT INTO `gm_cdk_code` VALUES ('273', '52', '1806191643', 'fjbg4nzatypl');
INSERT INTO `gm_cdk_code` VALUES ('274', '52', '1806191643', 'mqv3j8d96uax');
INSERT INTO `gm_cdk_code` VALUES ('275', '52', '1806191643', 'jqmgcv5ke97y');
INSERT INTO `gm_cdk_code` VALUES ('276', '52', '1806191643', 'aqt7u6v3wx9c');
INSERT INTO `gm_cdk_code` VALUES ('277', '52', '1806191643', 'rkmglh2ptwd9');
INSERT INTO `gm_cdk_code` VALUES ('278', '52', '1806191643', 'mlwjtfv3x7ek');
INSERT INTO `gm_cdk_code` VALUES ('279', '52', '1806191643', '8udm9bg3yx72');
INSERT INTO `gm_cdk_code` VALUES ('280', '52', '1806191643', 'akg9fvwuhlzb');
INSERT INTO `gm_cdk_code` VALUES ('281', '52', '1806191643', 'pr7f4akeqnxw');
INSERT INTO `gm_cdk_code` VALUES ('282', '52', '1806191643', 'cmgjpw6ua2se');
INSERT INTO `gm_cdk_code` VALUES ('283', '52', '1806191643', 'b7tdavpuml9k');
INSERT INTO `gm_cdk_code` VALUES ('284', '52', '1806191643', 'vkct3xsdp8m2');
INSERT INTO `gm_cdk_code` VALUES ('285', '52', '1806191643', 'mzv78gxejdr6');
INSERT INTO `gm_cdk_code` VALUES ('286', '52', '1806191643', 'yxw2rvneh3su');
INSERT INTO `gm_cdk_code` VALUES ('287', '52', '1806191643', '684xa2qymuk9');
INSERT INTO `gm_cdk_code` VALUES ('288', '52', '1806191643', 'e9yc8sardt27');
INSERT INTO `gm_cdk_code` VALUES ('289', '52', '1806191643', 'rn2zkygshea9');
INSERT INTO `gm_cdk_code` VALUES ('290', '52', '1806191643', 'q7dxl6wgu9rf');
INSERT INTO `gm_cdk_code` VALUES ('291', '52', '1806191643', 'jt4qa3cxvz5f');
INSERT INTO `gm_cdk_code` VALUES ('292', '52', '1806191643', 'dn8jbc2uze9g');
INSERT INTO `gm_cdk_code` VALUES ('293', '52', '1806191643', 'hx7gmv2an3lk');
INSERT INTO `gm_cdk_code` VALUES ('294', '52', '1806191643', 'pxvfjklmys6e');
INSERT INTO `gm_cdk_code` VALUES ('295', '52', '1806191643', 'vyczgrh9aq3m');
INSERT INTO `gm_cdk_code` VALUES ('296', '52', '1806191643', 'b9ravfusy6jh');
INSERT INTO `gm_cdk_code` VALUES ('297', '52', '1806191643', 'n7p8cxwmb2u3');
INSERT INTO `gm_cdk_code` VALUES ('298', '52', '1806191643', '4wzykt8a3ed6');
INSERT INTO `gm_cdk_code` VALUES ('299', '52', '1806191643', 'azgsv4hw9pkm');
INSERT INTO `gm_cdk_code` VALUES ('300', '52', '1806191643', '8dstv3l94agw');
INSERT INTO `gm_cdk_code` VALUES ('301', '52', '1806191643', '26ew7fvtqcsp');
INSERT INTO `gm_cdk_code` VALUES ('302', '52', '1806191643', '9pcjdutwy5fk');
INSERT INTO `gm_cdk_code` VALUES ('303', '52', '1806191643', 'be4p7xd5hsqm');
INSERT INTO `gm_cdk_code` VALUES ('304', '52', '1806191643', 'g5etc8rldu9h');
INSERT INTO `gm_cdk_code` VALUES ('305', '52', '1806191643', 'bnfwzlmxauk7');
INSERT INTO `gm_cdk_code` VALUES ('306', '52', '1806191643', 'uwmbrdqxa39p');
INSERT INTO `gm_cdk_code` VALUES ('307', '52', '1806191643', 'bfch2ea4d9ml');
INSERT INTO `gm_cdk_code` VALUES ('308', '52', '1806191643', '6n2yvxmhsza3');
INSERT INTO `gm_cdk_code` VALUES ('309', '52', '1806191643', 'q8rf3lcj5sdp');
INSERT INTO `gm_cdk_code` VALUES ('310', '52', '1806191643', 'z2b86q5hgulm');
INSERT INTO `gm_cdk_code` VALUES ('311', '52', '1806191643', '8fd7lwhzspgq');
INSERT INTO `gm_cdk_code` VALUES ('312', '52', '1806191643', 'py6e4srwb5tq');
INSERT INTO `gm_cdk_code` VALUES ('313', '52', '1806191643', 'cyea4x6zlwb9');
INSERT INTO `gm_cdk_code` VALUES ('314', '52', '1806191643', 'f7ranw6g8p3m');
INSERT INTO `gm_cdk_code` VALUES ('315', '52', '1806191643', 'ulybv8es5xwq');
INSERT INTO `gm_cdk_code` VALUES ('316', '52', '1806191643', 'yn78rb5sdzac');
INSERT INTO `gm_cdk_code` VALUES ('317', '52', '1806191643', 'lw8uzvb59gy2');
INSERT INTO `gm_cdk_code` VALUES ('318', '52', '1806191643', '7kun24c6pwsl');
INSERT INTO `gm_cdk_code` VALUES ('319', '52', '1806191643', 'qwscugbj98zf');
INSERT INTO `gm_cdk_code` VALUES ('320', '52', '1806191643', 'saukgx68lebd');
INSERT INTO `gm_cdk_code` VALUES ('321', '52', '1806191643', 'tqz823b5fxwm');
INSERT INTO `gm_cdk_code` VALUES ('322', '52', '1806191643', '5c4gvd7k9a3m');
INSERT INTO `gm_cdk_code` VALUES ('323', '52', '1806191643', 'ryuewcn2mlg9');
INSERT INTO `gm_cdk_code` VALUES ('324', '52', '1806191643', '8wuvk9tq5yhd');
INSERT INTO `gm_cdk_code` VALUES ('325', '52', '1806191643', '86tyk5n3h2mr');
INSERT INTO `gm_cdk_code` VALUES ('326', '52', '1806191643', 'upz9vnf2w6ym');
INSERT INTO `gm_cdk_code` VALUES ('327', '52', '1806191643', 'lpz6vawhs2je');
INSERT INTO `gm_cdk_code` VALUES ('328', '52', '1806191643', '692e8jysua4g');
INSERT INTO `gm_cdk_code` VALUES ('329', '52', '1806191643', 'v5tbnq2rh63g');
INSERT INTO `gm_cdk_code` VALUES ('330', '52', '1806191643', '6f4d5h37wzqn');
INSERT INTO `gm_cdk_code` VALUES ('331', '52', '1806191643', '6kaq4x3w8uj2');
INSERT INTO `gm_cdk_code` VALUES ('332', '52', '1806191643', 'a43qltcrgvpj');
INSERT INTO `gm_cdk_code` VALUES ('333', '52', '1806191643', 'r7wm2bqshfyu');
INSERT INTO `gm_cdk_code` VALUES ('334', '52', '1806191643', 'sc4ha9m7uk2y');
INSERT INTO `gm_cdk_code` VALUES ('335', '52', '1806191643', 'zpnk5gj4ev39');
INSERT INTO `gm_cdk_code` VALUES ('336', '52', '1806191643', '7d9jegqnmt8c');
INSERT INTO `gm_cdk_code` VALUES ('337', '52', '1806191643', 'xleb7ychqgtd');
INSERT INTO `gm_cdk_code` VALUES ('338', '52', '1806191643', 'jn4s7bta9gh3');
INSERT INTO `gm_cdk_code` VALUES ('339', '52', '1806191643', 'udaq3fcn7s4e');
INSERT INTO `gm_cdk_code` VALUES ('340', '52', '1806191643', '72u6zh3pxqve');
INSERT INTO `gm_cdk_code` VALUES ('341', '52', '1806191643', 'ktvh2ycmqf6a');
INSERT INTO `gm_cdk_code` VALUES ('342', '52', '1806191643', 'wzfs89r6c7ue');
INSERT INTO `gm_cdk_code` VALUES ('343', '52', '1806191643', 'pwyf6xb842qe');
INSERT INTO `gm_cdk_code` VALUES ('344', '52', '1806191643', 'qrgbxl4nj293');
INSERT INTO `gm_cdk_code` VALUES ('345', '52', '1806191643', 'rxluhw7qjek2');
INSERT INTO `gm_cdk_code` VALUES ('346', '52', '1806191643', '2ymnj768lskf');
INSERT INTO `gm_cdk_code` VALUES ('347', '52', '1806191643', 'nkf7gd69t2rl');
INSERT INTO `gm_cdk_code` VALUES ('348', '52', '1806191643', '9n3jd52bwrfv');
INSERT INTO `gm_cdk_code` VALUES ('349', '52', '1806191643', 'knea7fuygmxt');
INSERT INTO `gm_cdk_code` VALUES ('350', '52', '1806191643', 'n4uzharx9bgc');
INSERT INTO `gm_cdk_code` VALUES ('351', '52', '1806191643', 'nkmfp7zy39s8');
INSERT INTO `gm_cdk_code` VALUES ('352', '52', '1806191643', '5yjv8rsqhx6b');
INSERT INTO `gm_cdk_code` VALUES ('353', '52', '1806191643', 'es79huc2z8mf');
INSERT INTO `gm_cdk_code` VALUES ('354', '52', '1806191643', 'evrfcbuz5h72');
INSERT INTO `gm_cdk_code` VALUES ('355', '52', '1806191643', 'djkxl2yhcgb7');
INSERT INTO `gm_cdk_code` VALUES ('356', '52', '1806191643', '9rsjnadc8mh4');
INSERT INTO `gm_cdk_code` VALUES ('357', '52', '1806191643', '9yl5jtvwxszk');
INSERT INTO `gm_cdk_code` VALUES ('358', '52', '1806191643', 'hk5mqu7j23sx');
INSERT INTO `gm_cdk_code` VALUES ('359', '52', '1806191643', '8p4yqk57fzu6');
INSERT INTO `gm_cdk_code` VALUES ('360', '52', '1806191643', 'lxuhyb3wcamp');
INSERT INTO `gm_cdk_code` VALUES ('361', '52', '1806191643', '935shdrgcp6v');
INSERT INTO `gm_cdk_code` VALUES ('362', '52', '1806191643', '32l86rfdmkb7');
INSERT INTO `gm_cdk_code` VALUES ('363', '52', '1806191643', 'a6nq94lstr7x');
INSERT INTO `gm_cdk_code` VALUES ('364', '52', '1806191643', 'wcb72gsu8tlq');
INSERT INTO `gm_cdk_code` VALUES ('365', '52', '1806191643', 'u7szv46kla9t');
INSERT INTO `gm_cdk_code` VALUES ('366', '52', '1806191643', 'hyxnkrcs287d');
INSERT INTO `gm_cdk_code` VALUES ('367', '52', '1806191643', '7flvqjk2cby6');
INSERT INTO `gm_cdk_code` VALUES ('368', '52', '1806191643', '6hap7te9zcgm');
INSERT INTO `gm_cdk_code` VALUES ('369', '52', '1806191643', 'bk3a9rwngp5d');
INSERT INTO `gm_cdk_code` VALUES ('370', '52', '1806191643', 'egkpfsnb5h3m');
INSERT INTO `gm_cdk_code` VALUES ('371', '52', '1806191643', 'f3m29jsu6qzt');
INSERT INTO `gm_cdk_code` VALUES ('372', '52', '1806191643', 'ub7czrje8qgd');
INSERT INTO `gm_cdk_code` VALUES ('373', '52', '1806191643', '3wm42kuhna6d');
INSERT INTO `gm_cdk_code` VALUES ('374', '52', '1806191643', '3rytpmc9xkqf');
INSERT INTO `gm_cdk_code` VALUES ('375', '52', '1806191643', '6pnhek8lacyf');
INSERT INTO `gm_cdk_code` VALUES ('376', '52', '1806191643', 'ljusmzy5rkdq');
INSERT INTO `gm_cdk_code` VALUES ('377', '52', '1806191643', '4sx83zaepuqh');
INSERT INTO `gm_cdk_code` VALUES ('378', '52', '1806191643', 'n7flmdxwck8b');
INSERT INTO `gm_cdk_code` VALUES ('379', '52', '1806191643', 'rpzq8u2h97lv');
INSERT INTO `gm_cdk_code` VALUES ('380', '52', '1806191643', '7zph4waqcjxv');
INSERT INTO `gm_cdk_code` VALUES ('381', '52', '1806191643', 'v3df8kxwheb9');
INSERT INTO `gm_cdk_code` VALUES ('382', '52', '1806191643', 'x3jmrtluyp2z');
INSERT INTO `gm_cdk_code` VALUES ('383', '52', '1806191643', 'jf6yqhgpnt4z');
INSERT INTO `gm_cdk_code` VALUES ('384', '52', '1806191643', 'y6s94ldvtnwz');
INSERT INTO `gm_cdk_code` VALUES ('385', '52', '1806191643', 'xayv6kmljqpn');
INSERT INTO `gm_cdk_code` VALUES ('386', '52', '1806191643', 'l73d2c5hmks6');
INSERT INTO `gm_cdk_code` VALUES ('387', '52', '1806191643', 'rltju9hgpqny');
INSERT INTO `gm_cdk_code` VALUES ('388', '52', '1806191643', 'p4sjt2mkgea7');
INSERT INTO `gm_cdk_code` VALUES ('389', '52', '1806191643', '56clxjhv3e7m');
INSERT INTO `gm_cdk_code` VALUES ('390', '52', '1806191643', 'us3hvlkn8ct9');
INSERT INTO `gm_cdk_code` VALUES ('391', '52', '1806191643', 'qdahwc5fbnt9');
INSERT INTO `gm_cdk_code` VALUES ('392', '52', '1806191643', 'lcgrasfmbpzh');
INSERT INTO `gm_cdk_code` VALUES ('393', '52', '1806191643', 'qc6yhex43az2');
INSERT INTO `gm_cdk_code` VALUES ('394', '52', '1806191643', '58rmvbusc94w');
INSERT INTO `gm_cdk_code` VALUES ('395', '52', '1806191643', 'h257pz483qwe');
INSERT INTO `gm_cdk_code` VALUES ('396', '52', '1806191643', 'gb58rkejd6hn');
INSERT INTO `gm_cdk_code` VALUES ('397', '52', '1806191643', 'vn3kjwpq7cme');
INSERT INTO `gm_cdk_code` VALUES ('398', '52', '1806191643', 'vcdu9gsyt7xz');
INSERT INTO `gm_cdk_code` VALUES ('399', '52', '1806191643', 'zjrhbwyux7kt');
INSERT INTO `gm_cdk_code` VALUES ('400', '52', '1806191643', 'c74qeupyrxnb');
INSERT INTO `gm_cdk_code` VALUES ('401', '52', '1806191643', 'qdy5uecx4789');
INSERT INTO `gm_cdk_code` VALUES ('402', '52', '1806191643', 'a9dxcbtkmv8r');
INSERT INTO `gm_cdk_code` VALUES ('403', '52', '1806191643', '9863qwcut2hs');
INSERT INTO `gm_cdk_code` VALUES ('404', '52', '1806191643', 'm5bp7hnrja9q');
INSERT INTO `gm_cdk_code` VALUES ('405', '52', '1806191643', 'x7g3hbyuwf2q');
INSERT INTO `gm_cdk_code` VALUES ('406', '52', '1806191643', 'bnxqes75gkw3');
INSERT INTO `gm_cdk_code` VALUES ('407', '52', '1806191643', 'usgfz5w62njl');
INSERT INTO `gm_cdk_code` VALUES ('408', '52', '1806191643', 'tqxzkjr7l358');
INSERT INTO `gm_cdk_code` VALUES ('409', '52', '1806191643', '3x85qza6f4ln');
INSERT INTO `gm_cdk_code` VALUES ('410', '52', '1806191643', 'cajwvf7l4r2e');
INSERT INTO `gm_cdk_code` VALUES ('411', '52', '1806191643', 'cav4tsdug9fr');
INSERT INTO `gm_cdk_code` VALUES ('412', '52', '1806191643', 'ncgrj5sdy78v');
INSERT INTO `gm_cdk_code` VALUES ('413', '52', '1806191643', 'weu64yglmdbt');
INSERT INTO `gm_cdk_code` VALUES ('414', '52', '1806191643', 'zyxtgd3v2ls4');
INSERT INTO `gm_cdk_code` VALUES ('415', '52', '1806191643', 'jq79dnyfpg4t');
INSERT INTO `gm_cdk_code` VALUES ('416', '52', '1806191643', 'nrbz9ck48flv');
INSERT INTO `gm_cdk_code` VALUES ('417', '52', '1806191643', 'jn9czsuar4gd');
INSERT INTO `gm_cdk_code` VALUES ('418', '52', '1806191643', 'rysk3f56ud9h');
INSERT INTO `gm_cdk_code` VALUES ('419', '52', '1806191643', 'efbz4ghmlvya');
INSERT INTO `gm_cdk_code` VALUES ('420', '52', '1806191643', '6wzn57l2fsrv');
INSERT INTO `gm_cdk_code` VALUES ('421', '52', '1806191643', '9w5kygzdns4a');
INSERT INTO `gm_cdk_code` VALUES ('422', '52', '1806191643', 'vsrdbmj6zxlu');
INSERT INTO `gm_cdk_code` VALUES ('423', '52', '1806191643', '2de7f8stnvzr');
INSERT INTO `gm_cdk_code` VALUES ('424', '52', '1806191643', 'lncrq69hwbx2');
INSERT INTO `gm_cdk_code` VALUES ('425', '52', '1806191643', 'gc9ayptnhx5j');
INSERT INTO `gm_cdk_code` VALUES ('426', '52', '1806191643', 'g5hzx9kupve2');
INSERT INTO `gm_cdk_code` VALUES ('427', '52', '1806191643', '8ebmhyskc2x4');
INSERT INTO `gm_cdk_code` VALUES ('428', '52', '1806191643', 'cnaz3q4vm8lf');
INSERT INTO `gm_cdk_code` VALUES ('429', '52', '1806191643', 'xp9w7fs3hybz');
INSERT INTO `gm_cdk_code` VALUES ('430', '52', '1806191643', '8t6jsxa3k2n7');
INSERT INTO `gm_cdk_code` VALUES ('431', '52', '1806191643', 'pm8vz4k93c56');
INSERT INTO `gm_cdk_code` VALUES ('432', '52', '1806191643', 'puxe2v5kf6yc');
INSERT INTO `gm_cdk_code` VALUES ('433', '52', '1806191643', 'kzb9etslrfdj');
INSERT INTO `gm_cdk_code` VALUES ('434', '52', '1806191643', 'qetjuw9ya5dv');
INSERT INTO `gm_cdk_code` VALUES ('435', '52', '1806191643', '3muyjwkn5xst');
INSERT INTO `gm_cdk_code` VALUES ('436', '52', '1806191643', '7by6zafjuwke');
INSERT INTO `gm_cdk_code` VALUES ('437', '52', '1806191643', '4n79gwlcvme5');
INSERT INTO `gm_cdk_code` VALUES ('438', '52', '1806191643', 'r7f8gub4tmj6');
INSERT INTO `gm_cdk_code` VALUES ('439', '52', '1806191643', '37acuwrektfh');
INSERT INTO `gm_cdk_code` VALUES ('440', '52', '1806191643', 'f6yj9sud2anc');
INSERT INTO `gm_cdk_code` VALUES ('441', '52', '1806191643', '5v4knyre2swx');
INSERT INTO `gm_cdk_code` VALUES ('442', '52', '1806191643', 'plxm2huyzgqr');
INSERT INTO `gm_cdk_code` VALUES ('443', '52', '1806191643', 'abx73lfm4qz5');
INSERT INTO `gm_cdk_code` VALUES ('444', '52', '1806191643', 'neqbm2s36uzr');
INSERT INTO `gm_cdk_code` VALUES ('445', '52', '1806191643', '3ua2xnqezwgc');
INSERT INTO `gm_cdk_code` VALUES ('446', '52', '1806191643', 'thxzp2u985fn');
INSERT INTO `gm_cdk_code` VALUES ('447', '52', '1806191643', 'bjg46t8cm5d2');
INSERT INTO `gm_cdk_code` VALUES ('448', '52', '1806191643', 'k2bc8ex9vrfy');
INSERT INTO `gm_cdk_code` VALUES ('449', '52', '1806191643', '9gpqmys5adf3');
INSERT INTO `gm_cdk_code` VALUES ('450', '52', '1806191643', 'ewsxpvyk8r5m');
INSERT INTO `gm_cdk_code` VALUES ('451', '52', '1806191643', '5q43ns9fmbkg');
INSERT INTO `gm_cdk_code` VALUES ('452', '52', '1806191643', '6lchjauf7ezs');
INSERT INTO `gm_cdk_code` VALUES ('453', '52', '1806191643', '8vmfe4sn9jqd');
INSERT INTO `gm_cdk_code` VALUES ('454', '52', '1806191643', 'yak5lgvej4dz');
INSERT INTO `gm_cdk_code` VALUES ('455', '52', '1806191643', '958rgemncp7h');
INSERT INTO `gm_cdk_code` VALUES ('456', '52', '1806191643', 'n3amkbh9zx7v');
INSERT INTO `gm_cdk_code` VALUES ('457', '52', '1806191643', 'xrfm4ahpc3j9');
INSERT INTO `gm_cdk_code` VALUES ('458', '52', '1806191643', 'sklhrgm8v6en');
INSERT INTO `gm_cdk_code` VALUES ('459', '52', '1806191643', '5f7dmhtn6ugc');
INSERT INTO `gm_cdk_code` VALUES ('460', '52', '1806191643', 'v8tbw37kpz26');
INSERT INTO `gm_cdk_code` VALUES ('461', '52', '1806191643', '3bvtmxwp7qzc');
INSERT INTO `gm_cdk_code` VALUES ('462', '52', '1806191643', 'gr4msv2lc7fe');
INSERT INTO `gm_cdk_code` VALUES ('463', '52', '1806191643', '9tdsjnwfgxuk');
INSERT INTO `gm_cdk_code` VALUES ('464', '52', '1806191643', 'cfdkmvwpbsjr');
INSERT INTO `gm_cdk_code` VALUES ('465', '52', '1806191643', 'a826hm4rdx9n');
INSERT INTO `gm_cdk_code` VALUES ('466', '52', '1806191643', 'w5fjkqdyzmxn');
INSERT INTO `gm_cdk_code` VALUES ('467', '52', '1806191643', '8cv6fwpelr2d');
INSERT INTO `gm_cdk_code` VALUES ('468', '52', '1806191643', 'lw3cz5uf7h8k');
INSERT INTO `gm_cdk_code` VALUES ('469', '52', '1806191643', 'ylqj86z5h2kx');
INSERT INTO `gm_cdk_code` VALUES ('470', '52', '1806191643', 'h9zcxjrd8lbw');
INSERT INTO `gm_cdk_code` VALUES ('471', '52', '1806191643', 'el2x3kpndau6');
INSERT INTO `gm_cdk_code` VALUES ('472', '52', '1806191643', 'rx9kbsznmptl');
INSERT INTO `gm_cdk_code` VALUES ('473', '52', '1806191643', 'fhglmpyu6z5x');
INSERT INTO `gm_cdk_code` VALUES ('474', '52', '1806191643', '2tpr6dbqlke8');
INSERT INTO `gm_cdk_code` VALUES ('475', '52', '1806191643', 'wnbz76rm354p');
INSERT INTO `gm_cdk_code` VALUES ('476', '52', '1806191643', 'qjlzf67menh8');
INSERT INTO `gm_cdk_code` VALUES ('477', '52', '1806191643', '5szflpecbnwg');
INSERT INTO `gm_cdk_code` VALUES ('478', '52', '1806191643', 'v7qk598efm43');
INSERT INTO `gm_cdk_code` VALUES ('479', '52', '1806191643', '8xrq2gm3n5e9');
INSERT INTO `gm_cdk_code` VALUES ('480', '52', '1806191643', 'dw7hxaq9jtsm');
INSERT INTO `gm_cdk_code` VALUES ('481', '52', '1806191643', '2zu8fql3aw6g');
INSERT INTO `gm_cdk_code` VALUES ('482', '52', '1806191643', 'sxql5cvj7p9y');
INSERT INTO `gm_cdk_code` VALUES ('483', '52', '1806191643', 'bxur8e7nlwc2');
INSERT INTO `gm_cdk_code` VALUES ('484', '52', '1806191643', 'vanwdt5qhumy');
INSERT INTO `gm_cdk_code` VALUES ('485', '52', '1806191643', 'q7t63erdshxz');
INSERT INTO `gm_cdk_code` VALUES ('486', '52', '1806191643', '48fzatbjvdhe');
INSERT INTO `gm_cdk_code` VALUES ('487', '52', '1806191643', 'fasz974vgcm2');
INSERT INTO `gm_cdk_code` VALUES ('488', '52', '1806191643', '6xsnkc47ated');
INSERT INTO `gm_cdk_code` VALUES ('489', '52', '1806191643', 'y25hcr83nfmx');
INSERT INTO `gm_cdk_code` VALUES ('490', '52', '1806191643', 'tv8l5zwsnq4r');
INSERT INTO `gm_cdk_code` VALUES ('491', '52', '1806191643', 'sn8quvyxa2jp');
INSERT INTO `gm_cdk_code` VALUES ('492', '52', '1806191643', '5fupacmj3hr7');
INSERT INTO `gm_cdk_code` VALUES ('493', '52', '1806191643', 'k4be3jmxlt9c');
INSERT INTO `gm_cdk_code` VALUES ('494', '52', '1806191643', 'lahq23pne7du');
INSERT INTO `gm_cdk_code` VALUES ('495', '52', '1806191643', '8leufs6txrjb');
INSERT INTO `gm_cdk_code` VALUES ('496', '52', '1806191643', 'rjm2q9uapbyc');
INSERT INTO `gm_cdk_code` VALUES ('497', '52', '1806191643', '2mp6k5bql7w9');
INSERT INTO `gm_cdk_code` VALUES ('498', '52', '1806191643', '4z7r2jyua8dt');
INSERT INTO `gm_cdk_code` VALUES ('499', '52', '1806191643', '4bsgdacrhem7');
INSERT INTO `gm_cdk_code` VALUES ('500', '52', '1806191643', 'lmrqpc6f948h');
INSERT INTO `gm_cdk_code` VALUES ('501', '52', '1806191643', 'xgz7uftjsn8l');
INSERT INTO `gm_cdk_code` VALUES ('502', '52', '1806191643', '5mnwa2cgfx9p');
INSERT INTO `gm_cdk_code` VALUES ('503', '52', '1806191643', '5bg29d47fqkl');
INSERT INTO `gm_cdk_code` VALUES ('504', '52', '1806191643', 'zraxlcyk4wf5');
INSERT INTO `gm_cdk_code` VALUES ('505', '52', '1806191643', 'v56ew8plrshb');
INSERT INTO `gm_cdk_code` VALUES ('506', '52', '1806191643', 'crfx7tn8vjqz');
INSERT INTO `gm_cdk_code` VALUES ('507', '52', '1806191643', '6nvj39pwfayl');
INSERT INTO `gm_cdk_code` VALUES ('508', '52', '1806191643', 'jw25gmakd4bu');
INSERT INTO `gm_cdk_code` VALUES ('509', '52', '1806191643', 'sl6frqc7wpvk');
INSERT INTO `gm_cdk_code` VALUES ('510', '52', '1806191643', 'y9vhlnswxdme');
INSERT INTO `gm_cdk_code` VALUES ('511', '52', '1806191643', 'rlebmp42ds56');
INSERT INTO `gm_cdk_code` VALUES ('512', '52', '1806191643', 'n32wqvgjzb6y');
INSERT INTO `gm_cdk_code` VALUES ('513', '52', '1806191643', 'p65zcqnkw2ad');
INSERT INTO `gm_cdk_code` VALUES ('514', '52', '1806191643', 'd2pl8uhkagtq');
INSERT INTO `gm_cdk_code` VALUES ('515', '52', '1806191643', 'pylzxa78bnfg');
INSERT INTO `gm_cdk_code` VALUES ('516', '52', '1806191643', 'rczhbvxdu7k8');
INSERT INTO `gm_cdk_code` VALUES ('517', '52', '1806191643', 'khuz4l39jwpe');
INSERT INTO `gm_cdk_code` VALUES ('518', '52', '1806191643', 'j9k2zenhrdsl');
INSERT INTO `gm_cdk_code` VALUES ('519', '52', '1806191643', '2dh5xfup3ytl');
INSERT INTO `gm_cdk_code` VALUES ('520', '52', '1806191643', 'xb2frhvejtlw');
INSERT INTO `gm_cdk_code` VALUES ('521', '52', '1806191643', 'm9ljg2qenfry');
INSERT INTO `gm_cdk_code` VALUES ('522', '52', '1806191643', 'a29l6ghundkc');
INSERT INTO `gm_cdk_code` VALUES ('523', '52', '1806191643', '3m72nqs9kfbc');
INSERT INTO `gm_cdk_code` VALUES ('524', '52', '1806191643', 'd5jvlpwqc8s2');
INSERT INTO `gm_cdk_code` VALUES ('525', '52', '1806191643', '4598d26mwyts');
INSERT INTO `gm_cdk_code` VALUES ('526', '52', '1806191643', 'r3qk8a6z2tuj');
INSERT INTO `gm_cdk_code` VALUES ('527', '52', '1806191643', '4krmzgjhc9xw');
INSERT INTO `gm_cdk_code` VALUES ('528', '52', '1806191643', '24udpfkncswb');
INSERT INTO `gm_cdk_code` VALUES ('529', '52', '1806191643', 't8lzymvhcsnb');
INSERT INTO `gm_cdk_code` VALUES ('530', '52', '1806191643', 'xrw67z3sgvyq');
INSERT INTO `gm_cdk_code` VALUES ('531', '52', '1806191643', 'upewvtjcy8m3');
INSERT INTO `gm_cdk_code` VALUES ('532', '52', '1806191643', 'axy8ukm4j5gd');
INSERT INTO `gm_cdk_code` VALUES ('533', '52', '1806191643', '97qntwrfb8hp');
INSERT INTO `gm_cdk_code` VALUES ('534', '52', '1806191643', 'm7b3ndpvxzlr');
INSERT INTO `gm_cdk_code` VALUES ('535', '52', '1806191643', 'axkfdr2vpeby');
INSERT INTO `gm_cdk_code` VALUES ('536', '52', '1806191643', '9uc2qxlfbwr8');
INSERT INTO `gm_cdk_code` VALUES ('537', '52', '1806191643', 'gbvd8y9uszwr');
INSERT INTO `gm_cdk_code` VALUES ('538', '52', '1806191643', 'qb3lawumnsvz');
INSERT INTO `gm_cdk_code` VALUES ('539', '52', '1806191643', 'xhgakfj5nvre');
INSERT INTO `gm_cdk_code` VALUES ('540', '52', '1806191643', 'rv7mz8ukat6d');
INSERT INTO `gm_cdk_code` VALUES ('541', '52', '1806191643', 'u7mpvca2tn69');
INSERT INTO `gm_cdk_code` VALUES ('542', '52', '1806191643', 'kxupmgwvc4sf');
INSERT INTO `gm_cdk_code` VALUES ('543', '52', '1806191643', 'thwqkz7dvc35');
INSERT INTO `gm_cdk_code` VALUES ('544', '52', '1806191643', 'vp9xdfrcq2gw');
INSERT INTO `gm_cdk_code` VALUES ('545', '52', '1806191643', 'vtcqmysz2u65');
INSERT INTO `gm_cdk_code` VALUES ('546', '52', '1806191643', 'megwxfz927cp');
INSERT INTO `gm_cdk_code` VALUES ('547', '52', '1806191643', 'aqupnejs25cf');
INSERT INTO `gm_cdk_code` VALUES ('548', '52', '1806191643', 'w7fqkxdhg2jp');
INSERT INTO `gm_cdk_code` VALUES ('549', '52', '1806191643', 'kwp24flhq9js');
INSERT INTO `gm_cdk_code` VALUES ('550', '52', '1806191643', '4a3gnvjcdz7l');
INSERT INTO `gm_cdk_code` VALUES ('551', '52', '1806191643', '9hwx8z4f7m6s');
INSERT INTO `gm_cdk_code` VALUES ('552', '52', '1806191643', 'xjuh2znmsra6');
INSERT INTO `gm_cdk_code` VALUES ('553', '52', '1806191643', 'k9p7234hqvw5');
INSERT INTO `gm_cdk_code` VALUES ('554', '52', '1806191643', 'zmesnwr6vkjb');
INSERT INTO `gm_cdk_code` VALUES ('555', '52', '1806191643', '3s7jxnc8pakw');
INSERT INTO `gm_cdk_code` VALUES ('556', '52', '1806191643', '4ftrk2dwanlx');
INSERT INTO `gm_cdk_code` VALUES ('557', '52', '1806191643', '5j6fml4ahzq7');
INSERT INTO `gm_cdk_code` VALUES ('558', '52', '1806191643', 'emu8j9xkb6yp');
INSERT INTO `gm_cdk_code` VALUES ('559', '52', '1806191643', 'zwdvrm2cyskt');
INSERT INTO `gm_cdk_code` VALUES ('560', '52', '1806191643', 'tuka23956vlx');
INSERT INTO `gm_cdk_code` VALUES ('561', '52', '1806191643', 'tbea3qlgnyu6');
INSERT INTO `gm_cdk_code` VALUES ('562', '52', '1806191643', 'ge9awlujkqz3');
INSERT INTO `gm_cdk_code` VALUES ('563', '52', '1806191643', 'gebw76fahj38');
INSERT INTO `gm_cdk_code` VALUES ('564', '52', '1806191643', 'b6f9ygjhdmaw');
INSERT INTO `gm_cdk_code` VALUES ('565', '52', '1806191643', 'nc8hxjwmlzvg');
INSERT INTO `gm_cdk_code` VALUES ('566', '52', '1806191643', 'bmsv5j7wg4hn');
INSERT INTO `gm_cdk_code` VALUES ('567', '52', '1806191643', '9cwrdx64hzfg');
INSERT INTO `gm_cdk_code` VALUES ('568', '52', '1806191643', 'udnvm5bl8spt');
INSERT INTO `gm_cdk_code` VALUES ('569', '52', '1806191643', '6qafgyvk4b7s');
INSERT INTO `gm_cdk_code` VALUES ('570', '52', '1806191643', '8sqkuztrgxcj');
INSERT INTO `gm_cdk_code` VALUES ('571', '52', '1806191643', 'mfh65bp78gvy');
INSERT INTO `gm_cdk_code` VALUES ('572', '52', '1806191643', '35mvgblqhr64');
INSERT INTO `gm_cdk_code` VALUES ('573', '52', '1806191643', 'lp4m3nh26jtu');
INSERT INTO `gm_cdk_code` VALUES ('574', '52', '1806191643', '5vs7n9pydwxk');
INSERT INTO `gm_cdk_code` VALUES ('575', '52', '1806191643', 'stwygnch85pk');
INSERT INTO `gm_cdk_code` VALUES ('576', '52', '1806191643', 'p87vqjr6hx3k');
INSERT INTO `gm_cdk_code` VALUES ('577', '52', '1806191643', 'rzejqspl367d');
INSERT INTO `gm_cdk_code` VALUES ('578', '52', '1806191643', 'e6cb9lk5a3pq');
INSERT INTO `gm_cdk_code` VALUES ('579', '52', '1806191643', 'lcwk3xarpb75');
INSERT INTO `gm_cdk_code` VALUES ('580', '52', '1806191643', 'g329tyvefpjw');
INSERT INTO `gm_cdk_code` VALUES ('581', '52', '1806191643', '7fcs5habwrdn');
INSERT INTO `gm_cdk_code` VALUES ('582', '52', '1806191643', 'zcgqhb8r6x2n');
INSERT INTO `gm_cdk_code` VALUES ('583', '52', '1806191643', 'urlp923q8zjh');
INSERT INTO `gm_cdk_code` VALUES ('584', '52', '1806191643', '3b74shqmlvky');
INSERT INTO `gm_cdk_code` VALUES ('585', '52', '1806191643', 'uslzm6nbcgth');
INSERT INTO `gm_cdk_code` VALUES ('586', '52', '1806191643', 't4s6wvl8curm');
INSERT INTO `gm_cdk_code` VALUES ('587', '52', '1806191643', 'gpwjs8kncvx7');
INSERT INTO `gm_cdk_code` VALUES ('588', '52', '1806191643', '8nxpkrmud6v7');
INSERT INTO `gm_cdk_code` VALUES ('589', '52', '1806191643', 'ckegusyfxwtj');
INSERT INTO `gm_cdk_code` VALUES ('590', '52', '1806191643', 'lfq8wpxd4bm2');
INSERT INTO `gm_cdk_code` VALUES ('591', '52', '1806191643', 'tys9dljbka3n');
INSERT INTO `gm_cdk_code` VALUES ('592', '52', '1806191643', 'abrmxn3lqv49');
INSERT INTO `gm_cdk_code` VALUES ('593', '52', '1806191643', 'ht28nlrveduq');
INSERT INTO `gm_cdk_code` VALUES ('594', '52', '1806191643', 'jrfpnl8b9hmx');
INSERT INTO `gm_cdk_code` VALUES ('595', '52', '1806191643', '6s2d3gxz7ynm');
INSERT INTO `gm_cdk_code` VALUES ('596', '52', '1806191643', 'l6g3udakqyef');
INSERT INTO `gm_cdk_code` VALUES ('597', '52', '1806191643', 'uwa5ev6x73gn');
INSERT INTO `gm_cdk_code` VALUES ('598', '52', '1806191643', '4lpme2wdgn8k');
INSERT INTO `gm_cdk_code` VALUES ('599', '52', '1806191643', 'vdw42te65n9u');
INSERT INTO `gm_cdk_code` VALUES ('600', '52', '1806191643', 'nhrtxluqdmza');
INSERT INTO `gm_cdk_code` VALUES ('601', '52', '1806191643', 'uwavycn47rqm');
INSERT INTO `gm_cdk_code` VALUES ('602', '52', '1806191643', 'eljzwh64v8bk');
INSERT INTO `gm_cdk_code` VALUES ('603', '52', '1806191643', '3n5zvlpcby4m');
INSERT INTO `gm_cdk_code` VALUES ('604', '52', '1806191643', 'ghxuwfsk9nyb');
INSERT INTO `gm_cdk_code` VALUES ('605', '52', '1806191643', 'sw6j4uvr73lt');
INSERT INTO `gm_cdk_code` VALUES ('606', '52', '1806191643', 'ekmhr8bfsp9l');
INSERT INTO `gm_cdk_code` VALUES ('607', '52', '1806191643', 'h9vbxfe687zq');
INSERT INTO `gm_cdk_code` VALUES ('608', '52', '1806191643', 'ngxsc384mvbt');
INSERT INTO `gm_cdk_code` VALUES ('609', '52', '1806191643', 't3ruldj5wysf');
INSERT INTO `gm_cdk_code` VALUES ('610', '52', '1806191643', '7a3j598epw6u');
INSERT INTO `gm_cdk_code` VALUES ('611', '52', '1806191643', '3aqt6gywsbe7');
INSERT INTO `gm_cdk_code` VALUES ('612', '52', '1806191643', '73x2gsjftdhn');
INSERT INTO `gm_cdk_code` VALUES ('613', '52', '1806191643', 'dsqach7z2ver');
INSERT INTO `gm_cdk_code` VALUES ('614', '52', '1806191643', 'zv5kfg97swcn');
INSERT INTO `gm_cdk_code` VALUES ('615', '52', '1806191643', 'q9eyv5s7axfh');
INSERT INTO `gm_cdk_code` VALUES ('616', '52', '1806191643', 'pwn8exb6juqv');
INSERT INTO `gm_cdk_code` VALUES ('617', '52', '1806191643', 'ypz32nbjamt9');
INSERT INTO `gm_cdk_code` VALUES ('618', '52', '1806191643', 'dq487vjstwck');
INSERT INTO `gm_cdk_code` VALUES ('619', '52', '1806191643', 'adrjqy4hfxe9');
INSERT INTO `gm_cdk_code` VALUES ('620', '52', '1806191643', '8ywgntqak9fz');
INSERT INTO `gm_cdk_code` VALUES ('621', '52', '1806191643', '8sbl7wcem4jt');
INSERT INTO `gm_cdk_code` VALUES ('622', '52', '1806191643', 'lxekudjbrhgy');
INSERT INTO `gm_cdk_code` VALUES ('623', '52', '1806191643', 'ynm8xstzkh7j');
INSERT INTO `gm_cdk_code` VALUES ('624', '52', '1806191643', 'kf2mlcj94b3e');
INSERT INTO `gm_cdk_code` VALUES ('625', '52', '1806191643', 'n9x4cby6uadz');
INSERT INTO `gm_cdk_code` VALUES ('626', '52', '1806191643', 'gwu4jqaclb9r');
INSERT INTO `gm_cdk_code` VALUES ('627', '52', '1806191643', 'ek7ma5g2sjhy');
INSERT INTO `gm_cdk_code` VALUES ('628', '52', '1806191643', '83ycs2pbr5a6');
INSERT INTO `gm_cdk_code` VALUES ('629', '52', '1806191643', 'd48apr6j2g79');
INSERT INTO `gm_cdk_code` VALUES ('630', '52', '1806191643', 'flvs6axbyt2h');
INSERT INTO `gm_cdk_code` VALUES ('631', '52', '1806191643', '643tjwd5vbke');
INSERT INTO `gm_cdk_code` VALUES ('632', '52', '1806191643', 'rdwasmtelhq8');
INSERT INTO `gm_cdk_code` VALUES ('633', '52', '1806191643', 'pnm9g2yvqfsd');
INSERT INTO `gm_cdk_code` VALUES ('634', '52', '1806191643', 'vu752yplwfdt');
INSERT INTO `gm_cdk_code` VALUES ('635', '52', '1806191643', '5xnc7mahvbjq');
INSERT INTO `gm_cdk_code` VALUES ('636', '52', '1806191643', 'aek3cpxdyu4b');
INSERT INTO `gm_cdk_code` VALUES ('637', '52', '1806191643', 'fwp6nsh2aekl');
INSERT INTO `gm_cdk_code` VALUES ('638', '52', '1806191643', 'lab5njp8kf9v');
INSERT INTO `gm_cdk_code` VALUES ('639', '52', '1806191643', 'rywf457ajmtx');
INSERT INTO `gm_cdk_code` VALUES ('640', '52', '1806191643', 'k6xnajv7mscf');
INSERT INTO `gm_cdk_code` VALUES ('641', '52', '1806191643', 'xy45gwmu7fac');
INSERT INTO `gm_cdk_code` VALUES ('642', '52', '1806191643', 'dbklvm8n7fcq');
INSERT INTO `gm_cdk_code` VALUES ('643', '52', '1806191643', 'kjnmudtyq5ze');
INSERT INTO `gm_cdk_code` VALUES ('644', '52', '1806191643', 'pc3x8smtg92k');
INSERT INTO `gm_cdk_code` VALUES ('645', '52', '1806191643', '5ymhazgq2lnu');
INSERT INTO `gm_cdk_code` VALUES ('646', '52', '1806191643', 'h5caw8xsp93e');
INSERT INTO `gm_cdk_code` VALUES ('647', '52', '1806191643', 'qu2klpew4h9r');
INSERT INTO `gm_cdk_code` VALUES ('648', '52', '1806191643', 'arcwbkmqpnu2');
INSERT INTO `gm_cdk_code` VALUES ('649', '52', '1806191643', 'wa4jzxnerf89');
INSERT INTO `gm_cdk_code` VALUES ('650', '52', '1806191643', '36p795xbwmcn');
INSERT INTO `gm_cdk_code` VALUES ('651', '52', '1806191643', 'wsc8x6q3hepj');
INSERT INTO `gm_cdk_code` VALUES ('652', '52', '1806191643', '8zlfqyuw4jht');
INSERT INTO `gm_cdk_code` VALUES ('653', '52', '1806191643', 'ycg8km7pj2r9');
INSERT INTO `gm_cdk_code` VALUES ('654', '52', '1806191643', 'nm34sp5ajwbd');
INSERT INTO `gm_cdk_code` VALUES ('655', '52', '1806191643', '2zvsk496ync7');
INSERT INTO `gm_cdk_code` VALUES ('656', '52', '1806191643', '35feunz7dkr8');
INSERT INTO `gm_cdk_code` VALUES ('657', '52', '1806191643', 'refdpxh4vmab');
INSERT INTO `gm_cdk_code` VALUES ('658', '52', '1806191643', 'qgc9x6nrthfa');
INSERT INTO `gm_cdk_code` VALUES ('659', '52', '1806191643', 'kayrtc8x4j7m');
INSERT INTO `gm_cdk_code` VALUES ('660', '52', '1806191643', 'z64msv5p9dnb');
INSERT INTO `gm_cdk_code` VALUES ('661', '52', '1806191643', 'cq2vrkyhbwln');
INSERT INTO `gm_cdk_code` VALUES ('662', '52', '1806191643', '45vlg9askpbz');
INSERT INTO `gm_cdk_code` VALUES ('663', '52', '1806191643', 'crg74qutp9aw');
INSERT INTO `gm_cdk_code` VALUES ('664', '52', '1806191643', '2yfcmht7w4pg');
INSERT INTO `gm_cdk_code` VALUES ('665', '52', '1806191643', 'xm3sbke6pr82');
INSERT INTO `gm_cdk_code` VALUES ('666', '52', '1806191643', 'japr5qsd8y7n');
INSERT INTO `gm_cdk_code` VALUES ('667', '52', '1806191643', '2n6umdelc7zh');
INSERT INTO `gm_cdk_code` VALUES ('668', '52', '1806191643', 'dp2b5jv8w94l');
INSERT INTO `gm_cdk_code` VALUES ('669', '52', '1806191643', 'wdtcnhra6e89');
INSERT INTO `gm_cdk_code` VALUES ('670', '52', '1806191643', '9bzu8ah2syew');
INSERT INTO `gm_cdk_code` VALUES ('671', '52', '1806191643', 'njw5ek8vgd9s');
INSERT INTO `gm_cdk_code` VALUES ('672', '52', '1806191643', 'pu5t4qjfrmc7');
INSERT INTO `gm_cdk_code` VALUES ('673', '52', '1806191643', 'hpsx7qznkybm');
INSERT INTO `gm_cdk_code` VALUES ('674', '52', '1806191643', 'vzaxn65lt2us');
INSERT INTO `gm_cdk_code` VALUES ('675', '52', '1806191643', '9ldf4bgpt78y');
INSERT INTO `gm_cdk_code` VALUES ('676', '52', '1806191643', 'qa32k4czbxtv');
INSERT INTO `gm_cdk_code` VALUES ('677', '52', '1806191643', 'b9pa5c3s24l8');
INSERT INTO `gm_cdk_code` VALUES ('678', '52', '1806191643', 'mktaewcgd56r');
INSERT INTO `gm_cdk_code` VALUES ('679', '52', '1806191643', '72a69z8j4knt');
INSERT INTO `gm_cdk_code` VALUES ('680', '52', '1806191643', '6ja7hy4sft5p');
INSERT INTO `gm_cdk_code` VALUES ('681', '52', '1806191643', '53e8zw9mf24v');
INSERT INTO `gm_cdk_code` VALUES ('682', '52', '1806191643', '4svpnqr56ygz');
INSERT INTO `gm_cdk_code` VALUES ('683', '52', '1806191643', 'n6rw7v9ecdgp');
INSERT INTO `gm_cdk_code` VALUES ('684', '52', '1806191643', 'sa4j9kylxhzg');
INSERT INTO `gm_cdk_code` VALUES ('685', '52', '1806191643', '7drexgsh9m3w');
INSERT INTO `gm_cdk_code` VALUES ('686', '52', '1806191643', '78gkqrmvnxyd');
INSERT INTO `gm_cdk_code` VALUES ('687', '52', '1806191643', '2hbp7x3vywz9');
INSERT INTO `gm_cdk_code` VALUES ('688', '52', '1806191643', '49ltjxf568en');
INSERT INTO `gm_cdk_code` VALUES ('689', '52', '1806191643', 'qvft6lr2sa58');
INSERT INTO `gm_cdk_code` VALUES ('690', '52', '1806191643', 'v48pgy2u9hxj');
INSERT INTO `gm_cdk_code` VALUES ('691', '52', '1806191643', 'g9q8kurwzfn5');
INSERT INTO `gm_cdk_code` VALUES ('692', '52', '1806191643', '4vyhm36ga2pw');
INSERT INTO `gm_cdk_code` VALUES ('693', '52', '1806191643', '6az98lp3xntk');
INSERT INTO `gm_cdk_code` VALUES ('694', '52', '1806191643', 'jwa5zu64pt93');
INSERT INTO `gm_cdk_code` VALUES ('695', '52', '1806191643', 'es4nyth3u7k2');
INSERT INTO `gm_cdk_code` VALUES ('696', '52', '1806191643', 'edxb5hyrvu7k');
INSERT INTO `gm_cdk_code` VALUES ('697', '52', '1806191643', 'brzn2altf9ec');
INSERT INTO `gm_cdk_code` VALUES ('698', '52', '1806191643', 'pgnfh2vzrjb5');
INSERT INTO `gm_cdk_code` VALUES ('699', '52', '1806191643', 'xpjyfc5tl68z');
INSERT INTO `gm_cdk_code` VALUES ('700', '52', '1806191643', 'zla2gs5h6bfn');
INSERT INTO `gm_cdk_code` VALUES ('701', '52', '1806191643', 'n7kb6ecyjzsv');
INSERT INTO `gm_cdk_code` VALUES ('702', '52', '1806191643', 'xn653gkcbqut');
INSERT INTO `gm_cdk_code` VALUES ('703', '52', '1806191643', 'z3qbymtxs5gu');
INSERT INTO `gm_cdk_code` VALUES ('704', '52', '1806191643', '6jeszwx8ugfn');
INSERT INTO `gm_cdk_code` VALUES ('705', '52', '1806191643', 'npbvrjldx7ug');
INSERT INTO `gm_cdk_code` VALUES ('706', '52', '1806191643', 'kugr92pfd65t');
INSERT INTO `gm_cdk_code` VALUES ('707', '52', '1806191643', 'dalxke96fg8w');
INSERT INTO `gm_cdk_code` VALUES ('708', '52', '1806191643', 'vzmlnyx9gsa8');
INSERT INTO `gm_cdk_code` VALUES ('709', '52', '1806191643', 'np7wszmc8ybq');
INSERT INTO `gm_cdk_code` VALUES ('710', '52', '1806191643', 't3sn4792zwlu');
INSERT INTO `gm_cdk_code` VALUES ('711', '52', '1806191643', 'j5lkdwh38tpx');
INSERT INTO `gm_cdk_code` VALUES ('712', '52', '1806191643', 'kjns26gy89qt');
INSERT INTO `gm_cdk_code` VALUES ('713', '52', '1806191643', '5d8hut9semgl');
INSERT INTO `gm_cdk_code` VALUES ('714', '52', '1806191643', 'crdlv5w2jpm8');
INSERT INTO `gm_cdk_code` VALUES ('715', '52', '1806191643', 'fun75kzev89q');
INSERT INTO `gm_cdk_code` VALUES ('716', '52', '1806191643', 'gnpx64y3thqk');
INSERT INTO `gm_cdk_code` VALUES ('717', '52', '1806191643', 'ucqevmghyxar');
INSERT INTO `gm_cdk_code` VALUES ('718', '52', '1806191643', '83alec9us75z');
INSERT INTO `gm_cdk_code` VALUES ('719', '52', '1806191643', '84x92syf7hnr');
INSERT INTO `gm_cdk_code` VALUES ('720', '52', '1806191643', 'lbxqrehkzfun');
INSERT INTO `gm_cdk_code` VALUES ('721', '52', '1806191643', 'jyzthcf8vxu5');
INSERT INTO `gm_cdk_code` VALUES ('722', '52', '1806191643', '4jqly3n5wrgx');
INSERT INTO `gm_cdk_code` VALUES ('723', '52', '1806191643', '5nl6sahxmfjc');
INSERT INTO `gm_cdk_code` VALUES ('724', '52', '1806191643', 'm4nuswf9pea5');
INSERT INTO `gm_cdk_code` VALUES ('725', '52', '1806191643', 'hpj4lwqys9ue');
INSERT INTO `gm_cdk_code` VALUES ('726', '52', '1806191643', 'c9wqv3ut826d');
INSERT INTO `gm_cdk_code` VALUES ('727', '52', '1806191643', '7jutksgyv6br');
INSERT INTO `gm_cdk_code` VALUES ('728', '52', '1806191643', 'uhxn48jcb37a');
INSERT INTO `gm_cdk_code` VALUES ('729', '52', '1806191643', 'e85jnbsar2v4');
INSERT INTO `gm_cdk_code` VALUES ('730', '52', '1806191643', 'c8jwp2fukzts');
INSERT INTO `gm_cdk_code` VALUES ('731', '52', '1806191643', '6wfza8hcbly2');
INSERT INTO `gm_cdk_code` VALUES ('732', '52', '1806191643', '32g7ma98c5ex');
INSERT INTO `gm_cdk_code` VALUES ('733', '52', '1806191643', 'bw6ztf39lhea');
INSERT INTO `gm_cdk_code` VALUES ('734', '52', '1806191643', '8mjlf7tka6hn');
INSERT INTO `gm_cdk_code` VALUES ('735', '52', '1806191643', '2pthxlbv7qef');
INSERT INTO `gm_cdk_code` VALUES ('736', '52', '1806191643', 'gxvptzadly9u');
INSERT INTO `gm_cdk_code` VALUES ('737', '52', '1806191643', 'wvq27zjtb3sg');
INSERT INTO `gm_cdk_code` VALUES ('738', '52', '1806191643', 'xcmdr46sa582');
INSERT INTO `gm_cdk_code` VALUES ('739', '52', '1806191643', 'ea6db8mcx3l7');
INSERT INTO `gm_cdk_code` VALUES ('740', '52', '1806191643', 'eztbmpn8sd74');
INSERT INTO `gm_cdk_code` VALUES ('741', '52', '1806191643', '7tg5ur9y68sw');
INSERT INTO `gm_cdk_code` VALUES ('742', '52', '1806191643', 'baj4lt96yz7f');
INSERT INTO `gm_cdk_code` VALUES ('743', '52', '1806191643', '6stdarq3x5bp');
INSERT INTO `gm_cdk_code` VALUES ('744', '52', '1806191643', '3crp7tlf4bkg');
INSERT INTO `gm_cdk_code` VALUES ('745', '52', '1806191643', 'jbevnlrkdfht');
INSERT INTO `gm_cdk_code` VALUES ('746', '52', '1806191643', 'acnwx3dvytr5');
INSERT INTO `gm_cdk_code` VALUES ('747', '52', '1806191643', 'aun43wbd9hg7');
INSERT INTO `gm_cdk_code` VALUES ('748', '52', '1806191643', '5rnv67pym2q3');
INSERT INTO `gm_cdk_code` VALUES ('749', '52', '1806191643', 's7cjqu5ad3vh');
INSERT INTO `gm_cdk_code` VALUES ('750', '52', '1806191643', 'zewrym79qf8d');
INSERT INTO `gm_cdk_code` VALUES ('751', '52', '1806191643', 'd6w72jxbsalq');
INSERT INTO `gm_cdk_code` VALUES ('752', '52', '1806191643', 'a4erug7lw5v2');
INSERT INTO `gm_cdk_code` VALUES ('753', '52', '1806191643', 'gdh54exvaq7k');
INSERT INTO `gm_cdk_code` VALUES ('754', '52', '1806191643', 'k8fer2z3xmnc');
INSERT INTO `gm_cdk_code` VALUES ('755', '52', '1806191643', 'jfv8yqse9ckt');
INSERT INTO `gm_cdk_code` VALUES ('756', '52', '1806191643', '356yplw2g8d7');
INSERT INTO `gm_cdk_code` VALUES ('757', '52', '1806191643', 'aqcdfxm8hgkb');
INSERT INTO `gm_cdk_code` VALUES ('758', '52', '1806191643', 'snlmbjwy4763');
INSERT INTO `gm_cdk_code` VALUES ('759', '52', '1806191643', 'rmev39ug4q8s');
INSERT INTO `gm_cdk_code` VALUES ('760', '52', '1806191643', 'pqmvhu6sbc7t');
INSERT INTO `gm_cdk_code` VALUES ('761', '52', '1806191643', '7vsghx2wnf54');
INSERT INTO `gm_cdk_code` VALUES ('762', '52', '1806191643', 'elcg3ztqhbna');
INSERT INTO `gm_cdk_code` VALUES ('763', '52', '1806191643', '8ryh5z7f2tdq');
INSERT INTO `gm_cdk_code` VALUES ('764', '52', '1806191643', 'l3z4u87emgcw');
INSERT INTO `gm_cdk_code` VALUES ('765', '52', '1806191643', '6m38hawnufry');
INSERT INTO `gm_cdk_code` VALUES ('766', '52', '1806191643', '3y7qfl6wmsv4');
INSERT INTO `gm_cdk_code` VALUES ('767', '52', '1806191643', '876v34kjyg2r');
INSERT INTO `gm_cdk_code` VALUES ('768', '52', '1806191643', 'lstrf8b9wm2q');
INSERT INTO `gm_cdk_code` VALUES ('769', '52', '1806191643', 'ykc5fhw4rbe8');
INSERT INTO `gm_cdk_code` VALUES ('770', '52', '1806191643', '65pbadzeg48c');
INSERT INTO `gm_cdk_code` VALUES ('771', '52', '1806191643', '8qncsrx7ykzb');
INSERT INTO `gm_cdk_code` VALUES ('772', '52', '1806191643', '8zbaqrfp2enl');
INSERT INTO `gm_cdk_code` VALUES ('773', '52', '1806191643', 'dtz83ub2vsfy');
INSERT INTO `gm_cdk_code` VALUES ('774', '52', '1806191643', 'nvc27j4qbhl3');
INSERT INTO `gm_cdk_code` VALUES ('775', '52', '1806191643', 'mlwbke85auqn');
INSERT INTO `gm_cdk_code` VALUES ('776', '52', '1806191643', 'ha7683ufgy4r');
INSERT INTO `gm_cdk_code` VALUES ('777', '52', '1806191643', '5acpglyed9mb');
INSERT INTO `gm_cdk_code` VALUES ('778', '52', '1806191643', 'fwxzh2pvky75');
INSERT INTO `gm_cdk_code` VALUES ('779', '52', '1806191643', 'upc4vwlftsmr');
INSERT INTO `gm_cdk_code` VALUES ('780', '52', '1806191643', '4bg9efxh8s7k');
INSERT INTO `gm_cdk_code` VALUES ('781', '52', '1806191643', 'ayn5et6xzp9j');
INSERT INTO `gm_cdk_code` VALUES ('782', '52', '1806191643', 'tzqe6uv7wnps');
INSERT INTO `gm_cdk_code` VALUES ('783', '52', '1806191643', '7xnfqb5s6rw4');
INSERT INTO `gm_cdk_code` VALUES ('784', '52', '1806191643', '69efakrqp5jl');
INSERT INTO `gm_cdk_code` VALUES ('785', '52', '1806191643', '8zmw6k5cgtye');
INSERT INTO `gm_cdk_code` VALUES ('786', '52', '1806191643', 'bv5a29kjsgq3');
INSERT INTO `gm_cdk_code` VALUES ('787', '52', '1806191643', 'hfkj9xv4bqdt');
INSERT INTO `gm_cdk_code` VALUES ('788', '52', '1806191643', 'atmw238exzsc');
INSERT INTO `gm_cdk_code` VALUES ('789', '52', '1806191643', '3yafvn9spxhr');
INSERT INTO `gm_cdk_code` VALUES ('790', '52', '1806191643', 'gunba2plqh74');
INSERT INTO `gm_cdk_code` VALUES ('791', '52', '1806191643', 'ef2zwgnxhm6t');
INSERT INTO `gm_cdk_code` VALUES ('792', '52', '1806191643', 'slm5ckar63hf');
INSERT INTO `gm_cdk_code` VALUES ('793', '52', '1806191643', '76ngkfu5a9wr');
INSERT INTO `gm_cdk_code` VALUES ('794', '52', '1806191643', 'jbhuas5p9rvc');
INSERT INTO `gm_cdk_code` VALUES ('795', '52', '1806191643', 'g3vncbq567ha');
INSERT INTO `gm_cdk_code` VALUES ('796', '52', '1806191643', 'jpmer7fvkc2l');
INSERT INTO `gm_cdk_code` VALUES ('797', '52', '1806191643', 'lhemv7sygpnq');
INSERT INTO `gm_cdk_code` VALUES ('798', '52', '1806191643', 'gltkdsq5b68y');
INSERT INTO `gm_cdk_code` VALUES ('799', '52', '1806191643', '87tvucghn3x6');
INSERT INTO `gm_cdk_code` VALUES ('800', '52', '1806191643', 'h495najxty6c');
INSERT INTO `gm_cdk_code` VALUES ('801', '52', '1806191643', 'nrpcs4ladvj3');
INSERT INTO `gm_cdk_code` VALUES ('802', '52', '1806191643', 'twj8sdbrm9n7');
INSERT INTO `gm_cdk_code` VALUES ('803', '52', '1806191643', 'rkwqn765hzde');
INSERT INTO `gm_cdk_code` VALUES ('804', '52', '1806191643', '4q5h2ztxjsmu');
INSERT INTO `gm_cdk_code` VALUES ('805', '52', '1806191643', 'p8wr4es65bay');
INSERT INTO `gm_cdk_code` VALUES ('806', '52', '1806191643', 'rp2ch7y3zxjt');
INSERT INTO `gm_cdk_code` VALUES ('807', '52', '1806191643', '3g6asbkuh297');
INSERT INTO `gm_cdk_code` VALUES ('808', '52', '1806191643', 'c9d4vkjhue62');
INSERT INTO `gm_cdk_code` VALUES ('809', '52', '1806191643', 'sl9cb4txdpmr');
INSERT INTO `gm_cdk_code` VALUES ('810', '52', '1806191643', '3ag8v9dlbmsn');
INSERT INTO `gm_cdk_code` VALUES ('811', '52', '1806191643', 'he7umnztgbvf');
INSERT INTO `gm_cdk_code` VALUES ('812', '52', '1806191643', 'esjcr486kula');
INSERT INTO `gm_cdk_code` VALUES ('813', '52', '1806191643', '9l5qrtgzkc4v');
INSERT INTO `gm_cdk_code` VALUES ('814', '52', '1806191643', '4sq3e9htyl6z');
INSERT INTO `gm_cdk_code` VALUES ('815', '52', '1806191643', 'y7zlb8nk6wpx');
INSERT INTO `gm_cdk_code` VALUES ('816', '52', '1806191643', 'hguaxp532rsm');
INSERT INTO `gm_cdk_code` VALUES ('817', '52', '1806191643', '3b7f9g6mv5dn');
INSERT INTO `gm_cdk_code` VALUES ('818', '52', '1806191643', 'ynbh3am8q4dl');
INSERT INTO `gm_cdk_code` VALUES ('819', '52', '1806191643', 'mncb7ps8lt5y');
INSERT INTO `gm_cdk_code` VALUES ('820', '52', '1806191643', '3b5wvklqxefg');
INSERT INTO `gm_cdk_code` VALUES ('821', '52', '1806191643', 'cyb36qvrhgj2');
INSERT INTO `gm_cdk_code` VALUES ('822', '52', '1806191643', 'csdra7bq468m');
INSERT INTO `gm_cdk_code` VALUES ('823', '52', '1806191643', 'g8ecrv4ht6ak');
INSERT INTO `gm_cdk_code` VALUES ('824', '52', '1806191643', '5zrd8h9bycj6');
INSERT INTO `gm_cdk_code` VALUES ('825', '52', '1806191643', 'l29b436wa5dk');
INSERT INTO `gm_cdk_code` VALUES ('826', '52', '1806191643', 'jak3qdncvsge');
INSERT INTO `gm_cdk_code` VALUES ('827', '52', '1806191643', '5qsnjlwyrxum');
INSERT INTO `gm_cdk_code` VALUES ('828', '52', '1806191643', 'euljv5gq3kmt');
INSERT INTO `gm_cdk_code` VALUES ('829', '52', '1806191643', 'u2pexg5wymrs');
INSERT INTO `gm_cdk_code` VALUES ('830', '52', '1806191643', '6f5pstw7n2zb');
INSERT INTO `gm_cdk_code` VALUES ('831', '52', '1806191643', 'j3zlu6nm9xqb');
INSERT INTO `gm_cdk_code` VALUES ('832', '52', '1806191643', 'sg3mc6ak4xnh');
INSERT INTO `gm_cdk_code` VALUES ('833', '52', '1806191643', 'wbsn4y3kphux');
INSERT INTO `gm_cdk_code` VALUES ('834', '52', '1806191643', '5n4hwexp7r6t');
INSERT INTO `gm_cdk_code` VALUES ('835', '52', '1806191643', '85k7qczrbaxs');
INSERT INTO `gm_cdk_code` VALUES ('836', '52', '1806191643', 'e46c8yd9hszp');
INSERT INTO `gm_cdk_code` VALUES ('837', '52', '1806191643', '6wj28hslyuak');
INSERT INTO `gm_cdk_code` VALUES ('838', '52', '1806191643', 'xrlknp4jb6sf');
INSERT INTO `gm_cdk_code` VALUES ('839', '52', '1806191643', 'smaldu3kz8x7');
INSERT INTO `gm_cdk_code` VALUES ('840', '52', '1806191643', '9smzw4bqx86a');
INSERT INTO `gm_cdk_code` VALUES ('841', '52', '1806191643', 'rfuhkpx68ctg');
INSERT INTO `gm_cdk_code` VALUES ('842', '52', '1806191643', 'vktjwmfu2zsh');
INSERT INTO `gm_cdk_code` VALUES ('843', '52', '1806191643', 'u2dgc38vjnt6');
INSERT INTO `gm_cdk_code` VALUES ('844', '52', '1806191643', '8d5wsulmvter');
INSERT INTO `gm_cdk_code` VALUES ('845', '52', '1806191643', 'djh972ex6zm8');
INSERT INTO `gm_cdk_code` VALUES ('846', '52', '1806191643', '2kefmv9wus8a');
INSERT INTO `gm_cdk_code` VALUES ('847', '52', '1806191643', '4zum3lscgf76');
INSERT INTO `gm_cdk_code` VALUES ('848', '52', '1806191643', '7pg6aljexrh9');
INSERT INTO `gm_cdk_code` VALUES ('849', '52', '1806191643', 'rtn5x4lq6p97');
INSERT INTO `gm_cdk_code` VALUES ('850', '52', '1806191643', 'fd6zbmxh5l7k');
INSERT INTO `gm_cdk_code` VALUES ('851', '52', '1806191643', 'x27nchlwyvqu');
INSERT INTO `gm_cdk_code` VALUES ('852', '52', '1806191643', 'yh89m7ad4gp2');
INSERT INTO `gm_cdk_code` VALUES ('853', '52', '1806191643', 'r54ucn289ask');
INSERT INTO `gm_cdk_code` VALUES ('854', '52', '1806191643', 'pd84b7w2jant');
INSERT INTO `gm_cdk_code` VALUES ('855', '52', '1806191643', 'pjyfhvbuqdc4');
INSERT INTO `gm_cdk_code` VALUES ('856', '52', '1806191643', '9mbsda4fc5ku');
INSERT INTO `gm_cdk_code` VALUES ('857', '52', '1806191643', 'drs5ge49j38h');
INSERT INTO `gm_cdk_code` VALUES ('858', '52', '1806191643', 'fk5q4a9pzw3x');
INSERT INTO `gm_cdk_code` VALUES ('859', '52', '1806191643', 'jgen6z3hx78r');
INSERT INTO `gm_cdk_code` VALUES ('860', '52', '1806191643', 'ubpq7k5rl2zj');
INSERT INTO `gm_cdk_code` VALUES ('861', '52', '1806191643', 'j9ml6vsdz5hw');
INSERT INTO `gm_cdk_code` VALUES ('862', '52', '1806191643', 'fvsn9p4hdeg3');
INSERT INTO `gm_cdk_code` VALUES ('863', '52', '1806191643', 'ceb7dhzr53xj');
INSERT INTO `gm_cdk_code` VALUES ('864', '52', '1806191643', 'becz5t3dyguj');
INSERT INTO `gm_cdk_code` VALUES ('865', '52', '1806191643', 'xc9q6henzkra');
INSERT INTO `gm_cdk_code` VALUES ('866', '52', '1806191643', 'ylmwvestczqp');
INSERT INTO `gm_cdk_code` VALUES ('867', '52', '1806191643', 'heltks6xu7a4');
INSERT INTO `gm_cdk_code` VALUES ('868', '52', '1806191643', 'auzx8dkfehm7');
INSERT INTO `gm_cdk_code` VALUES ('869', '52', '1806191643', '3qdhwsmr79b4');
INSERT INTO `gm_cdk_code` VALUES ('870', '52', '1806191643', '64yrucdtsbfp');
INSERT INTO `gm_cdk_code` VALUES ('871', '52', '1806191643', '4239c5vd6whr');
INSERT INTO `gm_cdk_code` VALUES ('872', '52', '1806191643', '2z7qa486psy9');
INSERT INTO `gm_cdk_code` VALUES ('873', '52', '1806191643', '6re24l7mjgdb');
INSERT INTO `gm_cdk_code` VALUES ('874', '52', '1806191643', 'bc58gsh3drlz');
INSERT INTO `gm_cdk_code` VALUES ('875', '52', '1806191643', 'mp4c6zxveu3d');
INSERT INTO `gm_cdk_code` VALUES ('876', '52', '1806191643', 'lqm59ypr8ad6');
INSERT INTO `gm_cdk_code` VALUES ('877', '52', '1806191643', 't2auye8vbjzf');
INSERT INTO `gm_cdk_code` VALUES ('878', '52', '1806191643', '3tlmkv5bscdw');
INSERT INTO `gm_cdk_code` VALUES ('879', '52', '1806191643', 'dt9qax7m5eru');
INSERT INTO `gm_cdk_code` VALUES ('880', '52', '1806191643', 'kdurnc2qjexl');
INSERT INTO `gm_cdk_code` VALUES ('881', '52', '1806191643', '5xgwyjtv92hm');
INSERT INTO `gm_cdk_code` VALUES ('882', '52', '1806191643', 'my67dbj95slx');
INSERT INTO `gm_cdk_code` VALUES ('883', '52', '1806191643', 'zfalep8c4rdm');
INSERT INTO `gm_cdk_code` VALUES ('884', '52', '1806191643', 'xknypuq9d8fz');
INSERT INTO `gm_cdk_code` VALUES ('885', '52', '1806191643', 'c9rgqdbe63ha');
INSERT INTO `gm_cdk_code` VALUES ('886', '52', '1806191643', 'ex7zrjcpbvlw');
INSERT INTO `gm_cdk_code` VALUES ('887', '52', '1806191643', 'hb7gj5wy9346');
INSERT INTO `gm_cdk_code` VALUES ('888', '52', '1806191643', 'vhnejxl9c2pm');
INSERT INTO `gm_cdk_code` VALUES ('889', '52', '1806191643', 'nd23p5mxfy6c');
INSERT INTO `gm_cdk_code` VALUES ('890', '52', '1806191643', '6badfv4jn97t');
INSERT INTO `gm_cdk_code` VALUES ('891', '52', '1806191643', 'l54dtrqyscpj');
INSERT INTO `gm_cdk_code` VALUES ('892', '52', '1806191643', 'jvgd6zt7fc9l');
INSERT INTO `gm_cdk_code` VALUES ('893', '52', '1806191643', 'zunyrdbexgct');
INSERT INTO `gm_cdk_code` VALUES ('894', '52', '1806191643', 'dfmzav95bgwr');
INSERT INTO `gm_cdk_code` VALUES ('895', '52', '1806191643', 'k6t4zbuy8nhw');
INSERT INTO `gm_cdk_code` VALUES ('896', '52', '1806191643', 'ctsjdbq3wk6y');
INSERT INTO `gm_cdk_code` VALUES ('897', '52', '1806191643', 'y2p9a3nrfqdu');
INSERT INTO `gm_cdk_code` VALUES ('898', '52', '1806191643', '59mdkbhzjlfp');
INSERT INTO `gm_cdk_code` VALUES ('899', '52', '1806191643', 'mgxqveuj7n62');
INSERT INTO `gm_cdk_code` VALUES ('900', '52', '1806191643', '98ljx57gk4sz');
INSERT INTO `gm_cdk_code` VALUES ('901', '52', '1806191643', '9s7wq5rj2pcl');
INSERT INTO `gm_cdk_code` VALUES ('902', '52', '1806191643', 'shrylu587qb4');
INSERT INTO `gm_cdk_code` VALUES ('903', '52', '1806191643', 'q4hew6vxt7j8');
INSERT INTO `gm_cdk_code` VALUES ('904', '52', '1806191643', '5rwnyfkc7x2q');
INSERT INTO `gm_cdk_code` VALUES ('905', '52', '1806191643', '5t9kxy483wj7');
INSERT INTO `gm_cdk_code` VALUES ('906', '52', '1806191643', 'c5lqxdev3gj2');
INSERT INTO `gm_cdk_code` VALUES ('907', '52', '1806191643', '8p6fw9esgnrb');
INSERT INTO `gm_cdk_code` VALUES ('908', '52', '1806191643', '6xkybasgvzlh');
INSERT INTO `gm_cdk_code` VALUES ('909', '52', '1806191643', 'qlha9vm5kwg8');
INSERT INTO `gm_cdk_code` VALUES ('910', '52', '1806191643', 'wlakytvbmgq5');
INSERT INTO `gm_cdk_code` VALUES ('911', '52', '1806191643', '6ctfsxnl7u93');
INSERT INTO `gm_cdk_code` VALUES ('912', '52', '1806191643', 'mvqzhps654ae');
INSERT INTO `gm_cdk_code` VALUES ('913', '52', '1806191643', 'uqa3wfck82hy');
INSERT INTO `gm_cdk_code` VALUES ('914', '52', '1806191643', '47av59dh3ukn');
INSERT INTO `gm_cdk_code` VALUES ('915', '52', '1806191643', 'p2cj7wefulgs');
INSERT INTO `gm_cdk_code` VALUES ('916', '52', '1806191643', 'rnet3km9flqb');
INSERT INTO `gm_cdk_code` VALUES ('917', '52', '1806191643', 'db7fm5qglepx');
INSERT INTO `gm_cdk_code` VALUES ('918', '52', '1806191643', 'hj5k7qtc9lpf');
INSERT INTO `gm_cdk_code` VALUES ('919', '52', '1806191643', 'cuaqbv4e7mwk');
INSERT INTO `gm_cdk_code` VALUES ('920', '52', '1806191643', 'zets6j3qmbx7');
INSERT INTO `gm_cdk_code` VALUES ('921', '52', '1806191643', 'pfwrq7t8ydxv');
INSERT INTO `gm_cdk_code` VALUES ('922', '52', '1806191643', 'fkzlb4np85wd');
INSERT INTO `gm_cdk_code` VALUES ('923', '52', '1806191643', 'nu8lhxsbe5jr');
INSERT INTO `gm_cdk_code` VALUES ('924', '52', '1806191643', '5wznjlrgsbvu');
INSERT INTO `gm_cdk_code` VALUES ('925', '52', '1806191643', '2k5tmv78yewh');
INSERT INTO `gm_cdk_code` VALUES ('926', '52', '1806191643', 'egz7mb28qa94');
INSERT INTO `gm_cdk_code` VALUES ('927', '52', '1806191643', 'nkgmzypwbqtr');
INSERT INTO `gm_cdk_code` VALUES ('928', '52', '1806191643', 'y64nvmjpflx9');
INSERT INTO `gm_cdk_code` VALUES ('929', '52', '1806191643', 'bn8cgv3m2f5r');
INSERT INTO `gm_cdk_code` VALUES ('930', '52', '1806191643', 'sbc4lw8ejkpu');
INSERT INTO `gm_cdk_code` VALUES ('931', '52', '1806191643', '3q7ut4nypasm');
INSERT INTO `gm_cdk_code` VALUES ('932', '52', '1806191643', '9faklx4csjud');
INSERT INTO `gm_cdk_code` VALUES ('933', '52', '1806191643', 'ld62wq39f7jy');
INSERT INTO `gm_cdk_code` VALUES ('934', '52', '1806191643', 'l9jzamtq85xh');
INSERT INTO `gm_cdk_code` VALUES ('935', '52', '1806191643', 'rne67qsw8h2v');
INSERT INTO `gm_cdk_code` VALUES ('936', '52', '1806191643', 'gpenmwq67jvy');
INSERT INTO `gm_cdk_code` VALUES ('937', '52', '1806191643', '2ly8dzckafm9');
INSERT INTO `gm_cdk_code` VALUES ('938', '52', '1806191643', 'egnyxhqk4pzc');
INSERT INTO `gm_cdk_code` VALUES ('939', '52', '1806191643', 'b36wkfet9qu2');
INSERT INTO `gm_cdk_code` VALUES ('940', '52', '1806191643', 'z56qs9v3fbgp');
INSERT INTO `gm_cdk_code` VALUES ('941', '52', '1806191643', 'jq8nwdfck5sr');
INSERT INTO `gm_cdk_code` VALUES ('942', '52', '1806191643', 's6tm4q95wjaf');
INSERT INTO `gm_cdk_code` VALUES ('943', '52', '1806191643', '9znuqdybc2je');
INSERT INTO `gm_cdk_code` VALUES ('944', '52', '1806191643', 'ynaudt3lwbkz');
INSERT INTO `gm_cdk_code` VALUES ('945', '52', '1806191643', '9ghya3murbtc');
INSERT INTO `gm_cdk_code` VALUES ('946', '52', '1806191643', 'fsuej8tal9z3');
INSERT INTO `gm_cdk_code` VALUES ('947', '52', '1806191643', 'rkhq9j8n7pfy');
INSERT INTO `gm_cdk_code` VALUES ('948', '52', '1806191643', 'pjsh8benlr5y');
INSERT INTO `gm_cdk_code` VALUES ('949', '52', '1806191643', '8qhjubs56mf3');
INSERT INTO `gm_cdk_code` VALUES ('950', '52', '1806191643', '3j4rfzlqbepd');
INSERT INTO `gm_cdk_code` VALUES ('951', '52', '1806191643', 'zp9v68wfushe');
INSERT INTO `gm_cdk_code` VALUES ('952', '52', '1806191643', 's2mtuh5knvq7');
INSERT INTO `gm_cdk_code` VALUES ('953', '52', '1806191643', 'jqxcy2mzwkrp');
INSERT INTO `gm_cdk_code` VALUES ('954', '52', '1806191643', 'rabxqm5yd48l');
INSERT INTO `gm_cdk_code` VALUES ('955', '52', '1806191643', 'd42qj8wrgzsl');
INSERT INTO `gm_cdk_code` VALUES ('956', '52', '1806191643', 'bsa5ed3pflvr');
INSERT INTO `gm_cdk_code` VALUES ('957', '52', '1806191643', '3dfa2nwjgpq9');
INSERT INTO `gm_cdk_code` VALUES ('958', '52', '1806191643', '9nhyk5743erm');
INSERT INTO `gm_cdk_code` VALUES ('959', '52', '1806191643', 'm3fuwg4j8c9t');
INSERT INTO `gm_cdk_code` VALUES ('960', '52', '1806191643', 'r78zexulkdqb');
INSERT INTO `gm_cdk_code` VALUES ('961', '52', '1806191643', 'pjkng8sz4h5y');
INSERT INTO `gm_cdk_code` VALUES ('962', '52', '1806191643', 'le2847tkdnhv');
INSERT INTO `gm_cdk_code` VALUES ('963', '52', '1806191643', 'zd6l93pncxa4');
INSERT INTO `gm_cdk_code` VALUES ('964', '52', '1806191643', '3f5mc2djgvrx');
INSERT INTO `gm_cdk_code` VALUES ('965', '52', '1806191643', 'yaef5r3tn876');
INSERT INTO `gm_cdk_code` VALUES ('966', '52', '1806191643', '8sql6va43ngu');
INSERT INTO `gm_cdk_code` VALUES ('967', '52', '1806191643', 'p67wjfbcadhl');
INSERT INTO `gm_cdk_code` VALUES ('968', '52', '1806191643', 'p8fc5mkbnz7s');
INSERT INTO `gm_cdk_code` VALUES ('969', '52', '1806191643', 'zytwlah5289q');
INSERT INTO `gm_cdk_code` VALUES ('970', '52', '1806191643', '9fbqt8ms5hzy');
INSERT INTO `gm_cdk_code` VALUES ('971', '52', '1806191643', 'ljumezcgqwk9');
INSERT INTO `gm_cdk_code` VALUES ('972', '52', '1806191643', 'k79z3tqsfxce');
INSERT INTO `gm_cdk_code` VALUES ('973', '52', '1806191643', 'bndu9jsa2fml');
INSERT INTO `gm_cdk_code` VALUES ('974', '52', '1806191643', 'dq8tsw7ym2n9');
INSERT INTO `gm_cdk_code` VALUES ('975', '52', '1806191643', 'njm763xtdluy');
INSERT INTO `gm_cdk_code` VALUES ('976', '52', '1806191643', 'j9xnrvsa8mf5');
INSERT INTO `gm_cdk_code` VALUES ('977', '52', '1806191643', 'l6y5a9rxfm7h');
INSERT INTO `gm_cdk_code` VALUES ('978', '52', '1806191643', 'je3cx4a2nvp8');
INSERT INTO `gm_cdk_code` VALUES ('979', '52', '1806191643', '8qunl6khvesa');
INSERT INTO `gm_cdk_code` VALUES ('980', '52', '1806191643', 'qb2yac7usdx3');
INSERT INTO `gm_cdk_code` VALUES ('981', '52', '1806191643', 'gj78ae3uhv2r');
INSERT INTO `gm_cdk_code` VALUES ('982', '52', '1806191643', 'n7kahc2upb54');
INSERT INTO `gm_cdk_code` VALUES ('983', '52', '1806191643', '4jp3rkvfxbmc');
INSERT INTO `gm_cdk_code` VALUES ('984', '52', '1806191643', 'e2rnm56k8qlw');
INSERT INTO `gm_cdk_code` VALUES ('985', '52', '1806191643', 'n2tqxbp4m6ve');
INSERT INTO `gm_cdk_code` VALUES ('986', '52', '1806191643', 'nxf39l4t2awv');
INSERT INTO `gm_cdk_code` VALUES ('987', '52', '1806191643', '8xrmq64ysen2');
INSERT INTO `gm_cdk_code` VALUES ('988', '52', '1806191643', '9ersghqp45ya');
INSERT INTO `gm_cdk_code` VALUES ('989', '52', '1806191643', 'qdx9h73k82pl');
INSERT INTO `gm_cdk_code` VALUES ('990', '52', '1806191643', '7t5xvqg3fcha');
INSERT INTO `gm_cdk_code` VALUES ('991', '52', '1806191643', '2qk3tvurm4hb');
INSERT INTO `gm_cdk_code` VALUES ('992', '52', '1806191643', 'x82qp7eaflc5');
INSERT INTO `gm_cdk_code` VALUES ('993', '52', '1806191643', '8sezpwd3h27y');
INSERT INTO `gm_cdk_code` VALUES ('994', '52', '1806191643', 'h9ngera7xm6p');
INSERT INTO `gm_cdk_code` VALUES ('995', '52', '1806191643', '5bluzhxaf4e3');
INSERT INTO `gm_cdk_code` VALUES ('996', '52', '1806191643', 'mpqgtkuxc6wz');
INSERT INTO `gm_cdk_code` VALUES ('997', '52', '1806191643', 'fc7jem6alb2d');
INSERT INTO `gm_cdk_code` VALUES ('998', '52', '1806191643', 'rd67bxj2ypuk');
INSERT INTO `gm_cdk_code` VALUES ('999', '52', '1806191643', 'z2brpj9aw8cu');
INSERT INTO `gm_cdk_code` VALUES ('1000', '52', '1806191643', 'm7ev2djulxsf');
INSERT INTO `gm_cdk_code` VALUES ('1001', '52', '1806191643', '3r7fu8qctk9m');
INSERT INTO `gm_cdk_code` VALUES ('1002', '52', '1806191643', 'eg46dtxy25s7');
INSERT INTO `gm_cdk_code` VALUES ('1003', '52', '1806191643', 'sryz8pvx43af');
INSERT INTO `gm_cdk_code` VALUES ('1004', '52', '1806191643', 'e5xh8p6l4nmv');
INSERT INTO `gm_cdk_code` VALUES ('1005', '52', '1806191643', '84mj72ubpnkl');
INSERT INTO `gm_cdk_code` VALUES ('1006', '52', '1806191643', 'g3au2bp58wft');
INSERT INTO `gm_cdk_code` VALUES ('1007', '52', '1806191643', 'ab3uxq8kfmys');
INSERT INTO `gm_cdk_code` VALUES ('1008', '52', '1806191643', 'e5thryf264sj');
INSERT INTO `gm_cdk_code` VALUES ('1009', '52', '1806191643', 'xbmn4sdgqwyl');
INSERT INTO `gm_cdk_code` VALUES ('1010', '52', '1806191643', 'pqhakgysef86');
INSERT INTO `gm_cdk_code` VALUES ('1011', '52', '1806191643', '8j2hx5fewtpg');
INSERT INTO `gm_cdk_code` VALUES ('1012', '52', '1806191643', 'hz9g6nvjpr7l');
INSERT INTO `gm_cdk_code` VALUES ('1013', '52', '1806191643', '8j9auwv3es7t');
INSERT INTO `gm_cdk_code` VALUES ('1014', '52', '1806191643', 'sfjludqzcwv3');
INSERT INTO `gm_cdk_code` VALUES ('1015', '52', '1806191643', 'c56eznaxh93t');
INSERT INTO `gm_cdk_code` VALUES ('1016', '52', '1806191643', 'g3ruvj62bdc5');
INSERT INTO `gm_cdk_code` VALUES ('1017', '52', '1806191643', '2r8yfjmxvlk3');
INSERT INTO `gm_cdk_code` VALUES ('1018', '52', '1806191643', 'vy7br9l8d3xm');
INSERT INTO `gm_cdk_code` VALUES ('1019', '52', '1806191643', 'wy67amxquhps');
INSERT INTO `gm_cdk_code` VALUES ('1020', '52', '1806191643', 'gkuqh8z4a6vs');
INSERT INTO `gm_cdk_code` VALUES ('1021', '52', '1806191643', 'qy2urgecpjxf');
INSERT INTO `gm_cdk_code` VALUES ('1022', '52', '1806191643', 'lns5z7ect9ap');
INSERT INTO `gm_cdk_code` VALUES ('1023', '52', '1806191643', 'bxcrhnk7wj4p');
INSERT INTO `gm_cdk_code` VALUES ('1024', '52', '1806191643', 'emqwpdnrxhaz');
INSERT INTO `gm_cdk_code` VALUES ('1025', '52', '1806191643', 'jkcd3x6e5r4u');
INSERT INTO `gm_cdk_code` VALUES ('1026', '52', '1806191643', 'mahju5zwvbt2');
INSERT INTO `gm_cdk_code` VALUES ('1027', '52', '1806191643', '5p68er49hgml');
INSERT INTO `gm_cdk_code` VALUES ('1028', '52', '1806191643', 'qu35ztdalyms');
INSERT INTO `gm_cdk_code` VALUES ('1029', '52', '1806191643', 'xrbn3ups76je');
INSERT INTO `gm_cdk_code` VALUES ('1030', '52', '1806191643', 'sev76wrny4bx');
INSERT INTO `gm_cdk_code` VALUES ('1031', '52', '1806191643', 'yuw73shxqk9c');
INSERT INTO `gm_cdk_code` VALUES ('1032', '52', '1806191643', 'jsaxb2hwzutk');
INSERT INTO `gm_cdk_code` VALUES ('1033', '52', '1806191643', 'srayczqnmlt5');
INSERT INTO `gm_cdk_code` VALUES ('1034', '52', '1806191643', 'layt7mndu6hq');
INSERT INTO `gm_cdk_code` VALUES ('1035', '52', '1806191643', 'kd9qhlvf4pug');
INSERT INTO `gm_cdk_code` VALUES ('1036', '52', '1806191643', 'zswr97qepyct');
INSERT INTO `gm_cdk_code` VALUES ('1037', '52', '1806191643', 'tj7u8fxb5a3m');
INSERT INTO `gm_cdk_code` VALUES ('1038', '52', '1806191643', '382cgu4wkbme');
INSERT INTO `gm_cdk_code` VALUES ('1039', '52', '1806191643', 'r76m9j5desuh');
INSERT INTO `gm_cdk_code` VALUES ('1040', '52', '1806191643', 'ebzynu8x4k3w');
INSERT INTO `gm_cdk_code` VALUES ('1041', '52', '1806191643', 'hztw4vyus2f3');
INSERT INTO `gm_cdk_code` VALUES ('1042', '52', '1806191643', '3a9ygwbjepht');
INSERT INTO `gm_cdk_code` VALUES ('1043', '52', '1806191643', 'qjap2uh8wl64');
INSERT INTO `gm_cdk_code` VALUES ('1044', '52', '1806191643', 'ldmfzksen2r4');
INSERT INTO `gm_cdk_code` VALUES ('1045', '52', '1806191643', 'h4fxr7ysz2ak');
INSERT INTO `gm_cdk_code` VALUES ('1046', '52', '1806191643', 'c97yaqsglb63');
INSERT INTO `gm_cdk_code` VALUES ('1047', '52', '1806191643', 'qaekmthc4fyb');
INSERT INTO `gm_cdk_code` VALUES ('1048', '52', '1806191643', '34ke7jqcv8um');
INSERT INTO `gm_cdk_code` VALUES ('1049', '52', '1806191643', '6f2ty9vhl73z');
INSERT INTO `gm_cdk_code` VALUES ('1050', '52', '1806191643', 'et9gdlbzj74k');
INSERT INTO `gm_cdk_code` VALUES ('1051', '52', '1806191643', 'cm5y8nd6s47r');
INSERT INTO `gm_cdk_code` VALUES ('1052', '52', '1806191643', 'bwskfcptzjh9');
INSERT INTO `gm_cdk_code` VALUES ('1053', '52', '1806191643', 'e6fk2vytgpz9');
INSERT INTO `gm_cdk_code` VALUES ('1054', '52', '1806191643', '9jezmvplw67x');
INSERT INTO `gm_cdk_code` VALUES ('1055', '52', '1806191643', 's6qwzn9uc2rb');
INSERT INTO `gm_cdk_code` VALUES ('1056', '52', '1806191643', 'vy2fgcehx5jq');
INSERT INTO `gm_cdk_code` VALUES ('1057', '52', '1806191643', 'ygr9vlszqmha');
INSERT INTO `gm_cdk_code` VALUES ('1058', '52', '1806191643', 'z7d4w9fvtuy5');
INSERT INTO `gm_cdk_code` VALUES ('1059', '52', '1806191643', 'v2q56n8aglkd');
INSERT INTO `gm_cdk_code` VALUES ('1060', '52', '1806191643', '6b8jq5rx3fak');
INSERT INTO `gm_cdk_code` VALUES ('1061', '52', '1806191643', '9b3s8nwlp5ev');
INSERT INTO `gm_cdk_code` VALUES ('1062', '52', '1806191643', '4zkmltux25dp');
INSERT INTO `gm_cdk_code` VALUES ('1063', '52', '1806191643', 'lmu4zvqxe9n7');
INSERT INTO `gm_cdk_code` VALUES ('1064', '52', '1806191643', 'fqpjrlx7tdc6');
INSERT INTO `gm_cdk_code` VALUES ('1065', '52', '1806191643', 'bquakc2v7d3h');
INSERT INTO `gm_cdk_code` VALUES ('1066', '52', '1806191643', '2ad89mugvhle');
INSERT INTO `gm_cdk_code` VALUES ('1067', '52', '1806191643', 'w2t6pcfkbs78');
INSERT INTO `gm_cdk_code` VALUES ('1068', '52', '1806191643', 'c4vbt6e32wnq');
INSERT INTO `gm_cdk_code` VALUES ('1069', '52', '1806191643', 'xgn8lk6cdfaw');
INSERT INTO `gm_cdk_code` VALUES ('1070', '52', '1806191643', 'zpe28suj5ka3');
INSERT INTO `gm_cdk_code` VALUES ('1071', '52', '1806191643', '7c42grduw5ak');
INSERT INTO `gm_cdk_code` VALUES ('1072', '52', '1806191643', '46dpkhqmbzrn');
INSERT INTO `gm_cdk_code` VALUES ('1073', '52', '1806191643', 'rxagm54uwzs7');
INSERT INTO `gm_cdk_code` VALUES ('1074', '52', '1806191643', '9dl2rpnzvqm3');
INSERT INTO `gm_cdk_code` VALUES ('1075', '52', '1806191643', '9zwp7lk8hf2g');
INSERT INTO `gm_cdk_code` VALUES ('1076', '52', '1806191643', 'xyebgzd3l8fc');
INSERT INTO `gm_cdk_code` VALUES ('1077', '52', '1806191643', '2gp693qc7rb8');
INSERT INTO `gm_cdk_code` VALUES ('1078', '52', '1806191643', 'lk9egwscr2tx');
INSERT INTO `gm_cdk_code` VALUES ('1079', '52', '1806191643', '8h9tfvsaymj5');
INSERT INTO `gm_cdk_code` VALUES ('1080', '52', '1806191643', 'ksf2q7u9mnel');
INSERT INTO `gm_cdk_code` VALUES ('1081', '52', '1806191643', 'bnzc8vwqlkau');
INSERT INTO `gm_cdk_code` VALUES ('1082', '52', '1806191643', 'vnzepq87uslk');
INSERT INTO `gm_cdk_code` VALUES ('1083', '52', '1806191643', 'lxe7sfgkzr53');
INSERT INTO `gm_cdk_code` VALUES ('1084', '52', '1806191643', 'xy3wnd97peqf');
INSERT INTO `gm_cdk_code` VALUES ('1085', '52', '1806191643', '4u6tw5emzvlg');
INSERT INTO `gm_cdk_code` VALUES ('1086', '52', '1806191643', 'mavb26w3h7fl');
INSERT INTO `gm_cdk_code` VALUES ('1087', '52', '1806191643', 'achbjxz4tkns');
INSERT INTO `gm_cdk_code` VALUES ('1088', '52', '1806191643', 'ynq3mlt29zgk');
INSERT INTO `gm_cdk_code` VALUES ('1089', '52', '1806191643', '9r2dzem84un7');
INSERT INTO `gm_cdk_code` VALUES ('1090', '52', '1806191643', 'z4qa3e5y9bnd');
INSERT INTO `gm_cdk_code` VALUES ('1091', '52', '1806191643', 'n25h3bzq4clf');
INSERT INTO `gm_cdk_code` VALUES ('1092', '52', '1806191643', 'swtuh8pbzj6m');
INSERT INTO `gm_cdk_code` VALUES ('1093', '52', '1806191643', 'p9dwnke3lzrf');
INSERT INTO `gm_cdk_code` VALUES ('1094', '52', '1806191643', 'tc5lagqv8pmr');
INSERT INTO `gm_cdk_code` VALUES ('1095', '52', '1806191643', 'wm357spcrjkz');
INSERT INTO `gm_cdk_code` VALUES ('1096', '52', '1806191643', 'jynlr98zm5a2');
INSERT INTO `gm_cdk_code` VALUES ('1097', '52', '1806191643', 'awj97kme6frn');
INSERT INTO `gm_cdk_code` VALUES ('1098', '52', '1806191643', 'nt2kqm9prfed');
INSERT INTO `gm_cdk_code` VALUES ('1099', '52', '1806191643', 'f74l39hnyet8');
INSERT INTO `gm_cdk_code` VALUES ('1100', '52', '1806191643', 'z6snk8gbj2f7');
INSERT INTO `gm_cdk_code` VALUES ('1101', '52', '1806191643', '74gm2jqxnp9h');
INSERT INTO `gm_cdk_code` VALUES ('1102', '52', '1806191643', 'e95pfjqzr6sl');
INSERT INTO `gm_cdk_code` VALUES ('1103', '52', '1806191643', '5ys2abe3lwmf');
INSERT INTO `gm_cdk_code` VALUES ('1104', '52', '1806191643', '63n2urd5t8pk');
INSERT INTO `gm_cdk_code` VALUES ('1105', '52', '1806191643', 'j4ypshgcfxnq');
INSERT INTO `gm_cdk_code` VALUES ('1106', '52', '1806191643', '7a4gmptbdesk');
INSERT INTO `gm_cdk_code` VALUES ('1107', '52', '1806191643', 'nbg92jdk8v3w');
INSERT INTO `gm_cdk_code` VALUES ('1108', '52', '1806191643', 'a2nv8g59u3k7');
INSERT INTO `gm_cdk_code` VALUES ('1109', '52', '1806191643', 'kbecm5pngsua');
INSERT INTO `gm_cdk_code` VALUES ('1110', '52', '1806191643', '2ebw7u4cdnkm');
INSERT INTO `gm_cdk_code` VALUES ('1111', '52', '1806191643', '8mzktpwb3uhq');
INSERT INTO `gm_cdk_code` VALUES ('1112', '52', '1806191643', '5dwb986rgzyt');
INSERT INTO `gm_cdk_code` VALUES ('1113', '52', '1806191643', '4hky9nmcldxp');
INSERT INTO `gm_cdk_code` VALUES ('1114', '52', '1806191643', 'k2x9havyn56j');
INSERT INTO `gm_cdk_code` VALUES ('1115', '52', '1806191643', 'ul9atpdnxch2');
INSERT INTO `gm_cdk_code` VALUES ('1116', '52', '1806191643', 'v8cbgask6mp5');
INSERT INTO `gm_cdk_code` VALUES ('1117', '52', '1806191643', '73v5ga24znrs');
INSERT INTO `gm_cdk_code` VALUES ('1118', '52', '1806191643', 'ravq78xsjlb4');
INSERT INTO `gm_cdk_code` VALUES ('1119', '52', '1806191643', 'jstpghrw8u93');
INSERT INTO `gm_cdk_code` VALUES ('1120', '52', '1806191643', 'ypkm9xzfqg2r');
INSERT INTO `gm_cdk_code` VALUES ('1121', '52', '1806191643', 'cltwvsexq5d3');
INSERT INTO `gm_cdk_code` VALUES ('1122', '52', '1806191643', '927wqfnmhbuc');
INSERT INTO `gm_cdk_code` VALUES ('1123', '52', '1806191643', 'p4576rmuvfdy');
INSERT INTO `gm_cdk_code` VALUES ('1124', '52', '1806191643', 'p7lmntc9w62a');
INSERT INTO `gm_cdk_code` VALUES ('1125', '52', '1806191643', 'f4wbma92tvzy');
INSERT INTO `gm_cdk_code` VALUES ('1126', '52', '1806191643', 'kdpgym839vfc');
INSERT INTO `gm_cdk_code` VALUES ('1127', '52', '1806191643', 'jew2d65sh9a4');
INSERT INTO `gm_cdk_code` VALUES ('1128', '52', '1806191643', 'qjncz7hl2u3p');
INSERT INTO `gm_cdk_code` VALUES ('1129', '52', '1806191643', 'vz75gf3yxt82');
INSERT INTO `gm_cdk_code` VALUES ('1130', '52', '1806191643', 'mq98dnf3rzsw');
INSERT INTO `gm_cdk_code` VALUES ('1131', '52', '1806191643', '469l8etxhwy7');
INSERT INTO `gm_cdk_code` VALUES ('1132', '52', '1806191643', 'z8ug9hrwebt3');
INSERT INTO `gm_cdk_code` VALUES ('1133', '52', '1806191643', 's6kwavytl8me');
INSERT INTO `gm_cdk_code` VALUES ('1134', '52', '1806191643', 'dyj6t8s7ugew');
INSERT INTO `gm_cdk_code` VALUES ('1135', '52', '1806191643', 'psnqfldavuyw');
INSERT INTO `gm_cdk_code` VALUES ('1136', '52', '1806191643', 'v3ymb8rc9nk6');
INSERT INTO `gm_cdk_code` VALUES ('1137', '52', '1806191643', '2pj3emczlgqt');
INSERT INTO `gm_cdk_code` VALUES ('1138', '52', '1806191643', 'sqcr2fxmw7vh');
INSERT INTO `gm_cdk_code` VALUES ('1139', '52', '1806191643', 'f3bwd8yhnrt6');
INSERT INTO `gm_cdk_code` VALUES ('1140', '52', '1806191643', '56kbr4qamctg');
INSERT INTO `gm_cdk_code` VALUES ('1141', '52', '1806191643', 'hubgxs4ycvwk');
INSERT INTO `gm_cdk_code` VALUES ('1142', '52', '1806191643', 'utpnx69hjyew');
INSERT INTO `gm_cdk_code` VALUES ('1143', '52', '1806191643', 'l2wbj9ckhm47');
INSERT INTO `gm_cdk_code` VALUES ('1144', '52', '1806191643', '3zwlqknsxj8v');
INSERT INTO `gm_cdk_code` VALUES ('1145', '52', '1806191643', 'bfd4jgpsawte');
INSERT INTO `gm_cdk_code` VALUES ('1146', '52', '1806191643', '64frgcdsm7va');
INSERT INTO `gm_cdk_code` VALUES ('1147', '52', '1806191643', 'xjk9bguyfd5t');
INSERT INTO `gm_cdk_code` VALUES ('1148', '52', '1806191643', 'dx4a7mun2gb6');
INSERT INTO `gm_cdk_code` VALUES ('1149', '52', '1806191643', '8h2ecrv6yazn');
INSERT INTO `gm_cdk_code` VALUES ('1150', '52', '1806191643', 'j6ku8vrg4m2e');
INSERT INTO `gm_cdk_code` VALUES ('1151', '52', '1806191643', 'lbajc68qufpt');
INSERT INTO `gm_cdk_code` VALUES ('1152', '52', '1806191643', 'nkmd2jzuavwr');
INSERT INTO `gm_cdk_code` VALUES ('1153', '52', '1806191643', 'lu6h5tedg78c');
INSERT INTO `gm_cdk_code` VALUES ('1154', '52', '1806191643', 'sucg34hxqmdp');
INSERT INTO `gm_cdk_code` VALUES ('1155', '52', '1806191643', 'ur3l2qj6t7ha');
INSERT INTO `gm_cdk_code` VALUES ('1156', '52', '1806191643', 'p7cefkt4jvhy');
INSERT INTO `gm_cdk_code` VALUES ('1157', '52', '1806191643', 'ahgkzsdnpc6m');
INSERT INTO `gm_cdk_code` VALUES ('1158', '52', '1806191643', 'ub7qrgjx2z9m');
INSERT INTO `gm_cdk_code` VALUES ('1159', '52', '1806191643', 'cpbgj578tflm');
INSERT INTO `gm_cdk_code` VALUES ('1160', '52', '1806191643', '8fd963jcg4vn');
INSERT INTO `gm_cdk_code` VALUES ('1161', '52', '1806191643', '8rnfymtkc2ph');
INSERT INTO `gm_cdk_code` VALUES ('1162', '52', '1806191643', '6epghqr3db8z');
INSERT INTO `gm_cdk_code` VALUES ('1163', '52', '1806191643', '6y5f8tlsjkre');
INSERT INTO `gm_cdk_code` VALUES ('1164', '52', '1806191643', 'y3s48ex6aw9z');
INSERT INTO `gm_cdk_code` VALUES ('1165', '52', '1806191643', 'f9rmq3k5pa4g');
INSERT INTO `gm_cdk_code` VALUES ('1166', '52', '1806191643', 'r7kdezv68ghp');
INSERT INTO `gm_cdk_code` VALUES ('1167', '52', '1806191643', 'rsw3xdhkaucb');
INSERT INTO `gm_cdk_code` VALUES ('1168', '52', '1806191643', '64maukyvbfd5');
INSERT INTO `gm_cdk_code` VALUES ('1169', '52', '1806191643', 'hvbz9fdyug5l');
INSERT INTO `gm_cdk_code` VALUES ('1170', '52', '1806191643', 'njb3wz5phga8');
INSERT INTO `gm_cdk_code` VALUES ('1171', '52', '1806191643', 'q7h9gtxs4awl');
INSERT INTO `gm_cdk_code` VALUES ('1172', '52', '1806191643', '5xfy4mjk6c9l');
INSERT INTO `gm_cdk_code` VALUES ('1173', '52', '1806191643', 'h3gqcj2f7tn4');
INSERT INTO `gm_cdk_code` VALUES ('1174', '52', '1806191643', 'gqcahuevlzbx');
INSERT INTO `gm_cdk_code` VALUES ('1175', '52', '1806191643', 'zpsjx45b7fuk');
INSERT INTO `gm_cdk_code` VALUES ('1176', '52', '1806191643', 'jl4s8guwp2qn');
INSERT INTO `gm_cdk_code` VALUES ('1177', '52', '1806191643', 'etu2n4mrzxl6');
INSERT INTO `gm_cdk_code` VALUES ('1178', '52', '1806191643', 'bdsx5tkeg2un');
INSERT INTO `gm_cdk_code` VALUES ('1179', '52', '1806191643', 'n59csmr8736p');
INSERT INTO `gm_cdk_code` VALUES ('1180', '52', '1806191643', 'xzl26wajycn8');
INSERT INTO `gm_cdk_code` VALUES ('1181', '52', '1806191643', '4rxa2tvkmznl');
INSERT INTO `gm_cdk_code` VALUES ('1182', '52', '1806191643', '379wnjsecgp5');
INSERT INTO `gm_cdk_code` VALUES ('1183', '52', '1806191643', 'd7t8nxrym3zk');
INSERT INTO `gm_cdk_code` VALUES ('1184', '52', '1806191643', '5mv9g8xnsrh4');
INSERT INTO `gm_cdk_code` VALUES ('1185', '52', '1806191643', 'ue8sjgtf56xd');
INSERT INTO `gm_cdk_code` VALUES ('1186', '52', '1806191643', 'fh8b5tz23dsc');
INSERT INTO `gm_cdk_code` VALUES ('1187', '52', '1806191643', 'dmg5v3r2uqpj');
INSERT INTO `gm_cdk_code` VALUES ('1188', '52', '1806191643', '43ezhpv2adfs');
INSERT INTO `gm_cdk_code` VALUES ('1189', '52', '1806191643', 'fnaszqxrj62y');
INSERT INTO `gm_cdk_code` VALUES ('1190', '52', '1806191643', '7392etjbswk6');
INSERT INTO `gm_cdk_code` VALUES ('1191', '52', '1806191643', 'rqk5e6cgmsbz');
INSERT INTO `gm_cdk_code` VALUES ('1192', '52', '1806191643', 'hblxq2897ydf');
INSERT INTO `gm_cdk_code` VALUES ('1193', '52', '1806191643', 'gnb29qh6actu');
INSERT INTO `gm_cdk_code` VALUES ('1194', '52', '1806191643', 'zbcedrx3g8fl');
INSERT INTO `gm_cdk_code` VALUES ('1195', '52', '1806191643', 'zx2hmjflvdyg');
INSERT INTO `gm_cdk_code` VALUES ('1196', '52', '1806191643', 'ag26dbcshul3');
INSERT INTO `gm_cdk_code` VALUES ('1197', '52', '1806191643', 'vk4mhx9pnalc');
INSERT INTO `gm_cdk_code` VALUES ('1198', '52', '1806191643', 'tgmfvb6kus97');
INSERT INTO `gm_cdk_code` VALUES ('1199', '52', '1806191643', '2l7h9nm4qwvt');
INSERT INTO `gm_cdk_code` VALUES ('1200', '52', '1806191643', 'dnxrt47kw2yl');
INSERT INTO `gm_cdk_code` VALUES ('1201', '52', '1806191643', 'v9axrmjlcg8b');
INSERT INTO `gm_cdk_code` VALUES ('1202', '52', '1806191643', 'mrfy9kdq5jul');
INSERT INTO `gm_cdk_code` VALUES ('1203', '52', '1806191643', 'tj3mns5d6wbk');
INSERT INTO `gm_cdk_code` VALUES ('1204', '52', '1806191643', 'erjfc54d2haw');
INSERT INTO `gm_cdk_code` VALUES ('1205', '52', '1806191643', '3nk5werqvmtp');
INSERT INTO `gm_cdk_code` VALUES ('1206', '52', '1806191643', 'wfjl8dnr26cb');
INSERT INTO `gm_cdk_code` VALUES ('1207', '52', '1806191643', 'chyrm7gp3z98');
INSERT INTO `gm_cdk_code` VALUES ('1208', '52', '1806191643', '57veagprjym9');
INSERT INTO `gm_cdk_code` VALUES ('1209', '52', '1806191643', 'tgm3926akf45');
INSERT INTO `gm_cdk_code` VALUES ('1210', '52', '1806191643', '346ce9gpzwsx');
INSERT INTO `gm_cdk_code` VALUES ('1211', '52', '1806191643', 'kdy6enw4q8v7');
INSERT INTO `gm_cdk_code` VALUES ('1212', '52', '1806191643', '8lxscmn93frj');
INSERT INTO `gm_cdk_code` VALUES ('1213', '52', '1806191643', 'kdywcm78nq4p');
INSERT INTO `gm_cdk_code` VALUES ('1214', '52', '1806191643', 'rxtm46k2sbel');
INSERT INTO `gm_cdk_code` VALUES ('1215', '52', '1806191643', 'sl8pxz6cht5a');
INSERT INTO `gm_cdk_code` VALUES ('1216', '52', '1806191643', 'qu8dcmbrgsej');
INSERT INTO `gm_cdk_code` VALUES ('1217', '52', '1806191643', 'bp4fzvtg3wxk');
INSERT INTO `gm_cdk_code` VALUES ('1218', '52', '1806191643', 'tzndks45a3wg');
INSERT INTO `gm_cdk_code` VALUES ('1219', '52', '1806191643', 'nedf4qhjpcb7');
INSERT INTO `gm_cdk_code` VALUES ('1220', '52', '1806191643', '2ydgrnxahwbk');
INSERT INTO `gm_cdk_code` VALUES ('1221', '52', '1806191643', 'auf9s68rmy2p');
INSERT INTO `gm_cdk_code` VALUES ('1222', '52', '1806191643', '6luq9nxt7rz2');
INSERT INTO `gm_cdk_code` VALUES ('1223', '52', '1806191643', 'mjqhwr9zx5p8');
INSERT INTO `gm_cdk_code` VALUES ('1224', '52', '1806191643', '95vlfzbgdrys');
INSERT INTO `gm_cdk_code` VALUES ('1225', '52', '1806191643', 'k3b42pejtsaq');
INSERT INTO `gm_cdk_code` VALUES ('1226', '52', '1806191643', 'uf2hvcswm6t7');
INSERT INTO `gm_cdk_code` VALUES ('1227', '52', '1806191643', '6yechlt8d2bz');
INSERT INTO `gm_cdk_code` VALUES ('1228', '52', '1806191643', 'yfd4kcqle3bg');
INSERT INTO `gm_cdk_code` VALUES ('1229', '52', '1806191643', 'wt5dnzlcsm2k');
INSERT INTO `gm_cdk_code` VALUES ('1230', '52', '1806191643', 'qlhxp6dgm7fr');
INSERT INTO `gm_cdk_code` VALUES ('1231', '52', '1806191643', 'qhtn2dc3m6lu');
INSERT INTO `gm_cdk_code` VALUES ('1232', '52', '1806191643', 'yktvhj9pnfaq');
INSERT INTO `gm_cdk_code` VALUES ('1233', '52', '1806191643', '6xpq2jfrd3sn');
INSERT INTO `gm_cdk_code` VALUES ('1234', '52', '1806191643', 'x4uvze7kp8yj');
INSERT INTO `gm_cdk_code` VALUES ('1235', '52', '1806191643', '9wl5tjy7eh6a');
INSERT INTO `gm_cdk_code` VALUES ('1236', '52', '1806191643', 't5dfbu2v3lg8');
INSERT INTO `gm_cdk_code` VALUES ('1237', '52', '1806191643', '5ajzvpe6myu9');
INSERT INTO `gm_cdk_code` VALUES ('1238', '52', '1806191643', 'k8safp69lhy5');
INSERT INTO `gm_cdk_code` VALUES ('1239', '52', '1806191643', 'lvnrptq2bysj');
INSERT INTO `gm_cdk_code` VALUES ('1240', '52', '1806191643', '63t2kuec8xfa');
INSERT INTO `gm_cdk_code` VALUES ('1241', '52', '1806191643', 'masyjz3pgu27');
INSERT INTO `gm_cdk_code` VALUES ('1242', '52', '1806191643', 'sbpfrk4v3mtu');
INSERT INTO `gm_cdk_code` VALUES ('1243', '52', '1806191643', 'aqpl8x4hwk2v');
INSERT INTO `gm_cdk_code` VALUES ('1244', '52', '1806191643', '96zh78enyjka');
INSERT INTO `gm_cdk_code` VALUES ('1245', '52', '1806191643', 'pzd46fr9gjac');
INSERT INTO `gm_cdk_code` VALUES ('1246', '52', '1806191643', 'hs4gycnkx9qe');
INSERT INTO `gm_cdk_code` VALUES ('1247', '52', '1806191643', '35zr29xntje4');
INSERT INTO `gm_cdk_code` VALUES ('1248', '52', '1806191643', 'bpd3c9ats78w');
INSERT INTO `gm_cdk_code` VALUES ('1249', '52', '1806191643', 'h35u7lawzbt8');
INSERT INTO `gm_cdk_code` VALUES ('1250', '52', '1806191643', 'fq2r6smzt54x');
INSERT INTO `gm_cdk_code` VALUES ('1251', '52', '1806191643', 'ywsxhvk89q3t');
INSERT INTO `gm_cdk_code` VALUES ('1252', '52', '1806191643', 'nj4b86yh3gxc');
INSERT INTO `gm_cdk_code` VALUES ('1253', '52', '1806191643', 'w8exjkyqhzsn');
INSERT INTO `gm_cdk_code` VALUES ('1254', '52', '1806191643', '2pukmwlsate3');
INSERT INTO `gm_cdk_code` VALUES ('1255', '52', '1806191643', 'q8yghwkfdb7l');
INSERT INTO `gm_cdk_code` VALUES ('1256', '52', '1806191643', 'b2v5r76mtauh');
INSERT INTO `gm_cdk_code` VALUES ('1257', '52', '1806191643', 't9fx5cezuanj');
INSERT INTO `gm_cdk_code` VALUES ('1258', '52', '1806191643', 'knv3pzedyc9s');
INSERT INTO `gm_cdk_code` VALUES ('1259', '52', '1806191643', '5fxjlhaw7zut');
INSERT INTO `gm_cdk_code` VALUES ('1260', '52', '1806191643', 'hgbxuwl38z27');
INSERT INTO `gm_cdk_code` VALUES ('1261', '52', '1806191643', 'bnfx6wy3duk8');
INSERT INTO `gm_cdk_code` VALUES ('1262', '52', '1806191643', '3rwa98es4bpv');
INSERT INTO `gm_cdk_code` VALUES ('1263', '52', '1806191643', 'azp8tlj5u6x2');
INSERT INTO `gm_cdk_code` VALUES ('1264', '52', '1806191643', '7bke5vs4mp8g');
INSERT INTO `gm_cdk_code` VALUES ('1265', '52', '1806191643', 'ak7wf48mlbct');
INSERT INTO `gm_cdk_code` VALUES ('1266', '52', '1806191643', 'crwk7sp42z9a');
INSERT INTO `gm_cdk_code` VALUES ('1267', '52', '1806191643', 'yrtdwb5m92vx');
INSERT INTO `gm_cdk_code` VALUES ('1268', '52', '1806191643', 'ublsx4y3kp7c');
INSERT INTO `gm_cdk_code` VALUES ('1269', '52', '1806191643', 'qfw8v4tr6g3b');
INSERT INTO `gm_cdk_code` VALUES ('1270', '52', '1806191643', '3uclzmr8k4ed');
INSERT INTO `gm_cdk_code` VALUES ('1271', '52', '1806191643', '74dvgsq9lhc3');
INSERT INTO `gm_cdk_code` VALUES ('1272', '50', '1806191532', 'k3pn5u7sdacv');
INSERT INTO `gm_cdk_code` VALUES ('1273', '50', '1806191532', 'xykw8q923tlp');

-- ----------------------------
-- Table structure for `gm_ip_limit`
-- ----------------------------
DROP TABLE IF EXISTS `gm_ip_limit`;
CREATE TABLE `gm_ip_limit` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `status` tinyint(1) NOT NULL DEFAULT '0' COMMENT '状态(1:开启2:关闭)',
  `ip` varchar(30) NOT NULL COMMENT '标题',
  `reason` varchar(500) NOT NULL COMMENT '内容',
  `add_user` varchar(56) NOT NULL COMMENT '添加人',
  `add_time` int(11) NOT NULL COMMENT '添加时间',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=49 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of gm_ip_limit
-- ----------------------------
INSERT INTO `gm_ip_limit` VALUES ('46', '2', '66666666', '<span ><font ><b><i>6666666666666666666666666666666666666</i></b></font></span>', '3', '1529058512');
INSERT INTO `gm_ip_limit` VALUES ('47', '2', '22222', '<b >2222222222222222222222222222</b>', '3', '1529374111');
INSERT INTO `gm_ip_limit` VALUES ('48', '0', '3.3.3.3', '66666', '3', '1529485440');

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
INSERT INTO `gm_user` VALUES ('3', 'admin', 'e10adc3949ba59abbe56e057f20f883e', '超级管理员', '127.0.0.1', '1529460584', '', '1,2,3,4,5,6', '1');
INSERT INTO `gm_user` VALUES ('7', 'test', 'e10adc3949ba59abbe56e057f20f883e', '测试用户', null, null, '', '0', '1');
