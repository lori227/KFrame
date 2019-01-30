<?php  if ( ! defined('BASEPATH')) exit('No direct script access allowed');

$active_group = 'default';
$active_record = TRUE;
if(IS_DEV){
    $db['default']['hostname'] = '127.0.0.1';
    $db['default']['username'] = 'root';
    $db['default']['password'] = 'pass@1234';
}else{
    $db['default']['hostname'] = '192.168.1.9';
    $db['default']['username'] = 'gmadmin';
    $db['default']['password'] = '@ci2018';

}
$db['default']['port'] = 3306;
$db['default']['database'] = 'web_games_gm';
$db['default']['dbdriver'] = 'mysqli';
$db['default']['dbprefix'] = 'gm_';
$db['default']['pconnect'] = FALSE;
$db['default']['db_debug'] = TRUE;
$db['default']['cache_on'] = FALSE;
$db['default']['cachedir'] = '';
$db['default']['char_set'] = 'utf8';
$db['default']['dbcollat'] = 'utf8_general_ci';
$db['default']['swap_pre'] = '';
$db['default']['autoinit'] = TRUE;
$db['default']['stricton'] = FALSE;

/* End of file database.php */
/* Location: ./application/config/database.php */
