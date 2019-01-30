<?php  if ( ! defined('BASEPATH')) exit('No direct script access allowed');

$active_group = 'default';
$active_record = TRUE;
if(IS_DEV){
    $db['default']['hostname'] = '127.0.0.1';
    $db['default']['username'] = 'root';
    $db['default']['password'] = 'pass@1234';

    $db['kf']['hostname'] = '127.0.0.1';
    $db['kf']['username'] = 'root';
    $db['kf']['password'] = 'pass@1234';
}else{
    $db['default']['hostname'] = '192.168.1.9';
    $db['default']['username'] = 'gmadmin';
    $db['default']['password'] = '@ci2018';

    $db['kf']['hostname'] = '192.168.10.230';
    $db['kf']['username'] = 'root';
    $db['kf']['password'] = 'mysql@lori227-=';
}
$db['default']['port'] = 3306;
$db['default']['database'] = 'web_games_version';
$db['default']['dbdriver'] = 'mysqli';
$db['default']['dbprefix'] = 'lw_';
$db['default']['pconnect'] = FALSE;
$db['default']['db_debug'] = TRUE;
$db['default']['cache_on'] = FALSE;
$db['default']['cachedir'] = '';
$db['default']['char_set'] = 'utf8';
$db['default']['dbcollat'] = 'utf8_general_ci';
$db['default']['swap_pre'] = '';
$db['default']['autoinit'] = TRUE;
$db['default']['stricton'] = FALSE;

//开服部署数据库
$db['kf']['port'] = 3306;
$db['kf']['database'] = 'kframe';
$db['kf']['dbdriver'] = 'mysqli';
$db['kf']['dbprefix'] = '';
/* End of file database.php */
/* Location: ./application/config/database.php */
