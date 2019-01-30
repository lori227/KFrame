<?php
/**
 * 常用功能自己封装的方法
 * User: xuefeng
 * Date: 18-6-8
 * Time: 上午10:53
 */
//分页
function paging($obj,$page,$jump,$pageCnt=30){
    $CI = & get_instance();//超级ci对象，一般在helper,libraries中不能用this的时候才用
    $CI->load->library('pagination');
    $page < 1 && $page = 1;
    $config = $CI->config->config['pagination'];
    $config['base_url'] = $jump;
    $config['per_page'] = $pageCnt;//每页多少个
    //$config['enable_query_strings'] =true;
    //$config['use_page_numbers'] = TRUE;
    $config['page_query_string'] = TRUE;//传参形式
    $config['total_rows'] = $obj->count_all_results();//总量--必须有，不然报错
    $obj->limit($config['per_page'], ($page-1)*$config['per_page']);

    $CI->pagination->initialize($config);
    return $config['total_rows'];
}

/**
 * 过滤sql与php文件操作的关键字[不含转义的]
 * 防止sql注入1
 */
function filter_keyword( $string ) {
    $keyword = 'select|insert|update|delete|\'|\/\*|\*|\.\.\/|\.\/|union|and|union|order|or|into|load_file|outfile';
    $arr = explode( '|', $keyword );
    $result = str_ireplace( $arr, '', $string );
    return $result;
}

/**
 * 检查输入的数字是否合法，合法返回对应id，否则返回false
 * 防止sql注入2
 */
function check_id( $id ) {
    $result = false;
    if ( $id !== '' && !is_null( $id ) ) {
        $var = filter_keyword( $id ); // 过滤sql与php文件操作的关键字
        if ( $var !== '' && !is_null( $var ) && is_numeric( $var ) ) {
            $result = intval( $var );
        }
    }
    return $result;
}

/**
 * 检查输入的字符是否合法，合法返回对应id，否则返回false[含转义的]
 * 防止sql注入3
 */
function check_str( $string ){
    $string=trim($string);
    $result = false;
    $var = filter_keyword( $string ); // 过滤sql与php文件操作的关键字
    if ( !empty( $var ) ){
        if ( !get_magic_quotes_gpc() ) { // 判断magic_quotes_gpc是否为打开
            $var = addslashes( $var ); // 进行magic_quotes_gpc没有打开的情况对提交数据的过滤
        }
        $var = str_replace( "%", "\%", $var ); // 把 '%'过滤掉
        $var = nl2br( $var ); // 回车转换
        $var = htmlspecialchars( $var ); // html标记转换
        $result = $var;
    }
    return $result;
}
//获取真实ip
function get_real_ip(){
    $ip=false;
    if(!empty($_SERVER['HTTP_CLIENT_IP'])){
        $ip=$_SERVER['HTTP_CLIENT_IP'];
    }
    if(!empty($_SERVER['HTTP_X_FORWARDED_FOR'])){
        $ips=explode (', ', $_SERVER['HTTP_X_FORWARDED_FOR']);
        if($ip){ array_unshift($ips, $ip); $ip=FALSE; }
        for ($i=0; $i < count($ips); $i++){
            if(!eregi ('^(10│172.16│192.168).', $ips[$i])){
                $ip=$ips[$i];
                break;
            }
        }
    }
    return ($ip ? $ip : $_SERVER['REMOTE_ADDR']);
}

//本项目日志 xue
function lwLog($logCon,$type='version'){
    $log_con = '['.date('Y-m-d H:i:s').']，type:'.$type.'，content:'.json_encode($logCon).PHP_EOL;
    if(file_put_contents('application/logs/ver_'.date('ym').'.log', $log_con,FILE_APPEND)){
        //echo "写入成功。<br />";
    }
}

/**
 ** @desc 封装 curl 的调用接口，post的请求方式
 **/
function curlPostRequest($url,$requestString,$timeout = 5){
    if($url == '' || $requestString == '' || $timeout <=0){
        return false;
    }
    $con = curl_init((string)$url);
    curl_setopt($con, CURLOPT_HEADER, false);
    curl_setopt($con, CURLOPT_POSTFIELDS, $requestString);
    curl_setopt($con, CURLOPT_POST,true);
    curl_setopt($con, CURLOPT_RETURNTRANSFER,true);
    curl_setopt($con, CURLOPT_TIMEOUT,(int)$timeout);
    return curl_exec($con);
}





