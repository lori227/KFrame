<?php  if ( ! defined('BASEPATH')) exit('No direct script access allowed');

//邮件类型
$config['emailType'] = array(1=> '全局邮件',2 => '用户补偿');
//邮件状态
$config['emailFlag'] = array(0=> '未读',1 => '已读',2=> '已领取');

if(IS_DEV){
    $config['toolPath'] = array('clothes'=> 'D:\xue\道具配置2\clothes.config','item' => 'D:\xue\道具配置2\item.config');
}else{
    $config['toolPath'] = array('clothes'=> '/root/GMTool/tool_config/clothes.config','item' => '/root/GMTool/tool_config/item.config');//xml道具路径
}


//公告类型
$config['noticeType'] = array(1=> '登陆前公告',2 => '登陆后公告',3=> '跑马灯');
//CDK礼包-使用频次
$config['frequency'] = array(1=> '单码',2 => '通码');

//IP开关状态
$config['ip_status'] = array(1=> '开启',2 => '关闭');
