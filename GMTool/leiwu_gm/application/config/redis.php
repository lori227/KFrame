<?php
/**
 * Created by PhpStorm.
 * User: xuefeng
 * Date: 2018/6/1
 * redis服务器配置专用文件
 */
if(IS_DEV){
    // Default connection group
    $config['redis_default']['host'] = '127.0.0.1';   // IP address or host
    $config['redis_default']['port'] = '6379';     // Default Redis port is 6379
    $config['redis_default']['password'] = '';     // 密码
}else{
    $config['redis_default']['host'] = '192.168.1.9';   // IP address or host
    $config['redis_default']['port'] = '6379';     // Default Redis port is 6379
    $config['redis_default']['password'] = 'redis@lori227-=';     // 密码
}

//var_dump($config);die('===================');

/*$config['redis_slave']['host'] = '';
$config['redis_slave']['port'] = '6379';
$config['redis_slave']['password'] = '';*/