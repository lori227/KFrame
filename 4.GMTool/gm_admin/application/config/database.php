<?php  if ( ! defined('BASEPATH')) exit('No direct script access allowed');

$active_group = 'default';
$active_record = TRUE;
if(IS_DEV){
    $db['default']['hostname'] = '127.0.0.1';
    $db['default']['username'] = 'gmuser';
    $db['default']['password'] = '$my2018';
    $db['default']['database'] = 'gm_admin';
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


    // mg-papa-ad2
/*    $db['ad']['hostname'] = '127.0.0.1';
    $db['ad']['username'] = 'gmuser';
    $db['ad']['password'] = '$my2018';
    $db['ad']['database'] = 'gm_admin';
    $db['ad']['dbdriver'] = 'mysqli';
    $db['ad']['dbprefix'] = 'gm_';
    $db['ad']['pconnect'] = FALSE;
    $db['ad']['db_debug'] = TRUE;
    $db['ad']['cache_on'] = FALSE;
    $db['ad']['cachedir'] = '';
    $db['ad']['char_set'] = 'utf8';
    $db['ad']['dbcollat'] = 'utf8_general_ci';
    $db['ad']['swap_pre'] = '';
    $db['ad']['autoinit'] = TRUE;
    $db['ad']['stricton'] = FALSE;*/
}else{
    $db['default']['hostname'] = '192.168.1.9';
    $db['default']['port'] = 3306;
    $db['default']['username'] = 'gmuser';
    $db['default']['password'] = '$my2018';
    $db['default']['database'] = 'gm_admin';
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
}
/* End of file database.php */
/* Location: ./application/config/database.php */
