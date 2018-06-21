<?php  if ( ! defined('BASEPATH')) exit('No direct script access allowed');

//邮件类型
$config['emailType'] = array(1=> 'GM系统邮件',2 => '补偿邮件');
//邮件状态
$config['emailFlag'] = array(0=> '未读',1 => '已读',2=> '已领取');
//xml道具路径
$config['toolPath'] = array('clothes'=> 'D:\xue\道具配置\clothes.config','item' => 'D:\xue\道具配置\item.config');
//公告类型
$config['noticeType'] = array(1=> '登陆前公告',2 => '登陆后公告',3=> '跑马灯');
//CDK礼包-使用频次
$config['frequency'] = array(1=> '单码',2 => '通码');
