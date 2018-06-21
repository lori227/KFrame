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
    $config['total_rows'] = $obj->count_all_no_reset();
    $obj->limit($config['per_page'], ($page-1)*$config['per_page']);
    $CI->pagination->initialize($config);
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

//获取二级道具列表
function xmlTool($file){
    if(file_exists($file)){
        $con = file_get_contents($file);
        $CI = & get_instance();
        $CI->load->helper('xml');
        $content = xml_convert($con);
        $content=htmlspecialchars_decode($content);
        if(preg_match_all("/[^<Setting].*[\/>$]/", $content, $matches)){
            $infoAry = $matches[0];
            for($i=2;$i<count($infoAry);$i++){
                if(preg_match_all('/(\w+)\s*=\s*(".*?")/', $infoAry[$i], $attr)){
                    //有效属性拼接
                    $kary = $attr[1];//下标
                    $vary = $attr[2];//值
                    foreach($vary as &$val){
                        $val = preg_replace('/\"/',"",$val);
                    }
                    $resAry[] = array($kary[0]=>$vary[0],$kary[1]=>$vary[1],$kary[2]=>$vary[2]);//前三个属性
                }
            }
            return $resAry;
        }
    }
    return false;
}
//替换字符串中
function replace($str){
    return str_replace('""','',$str);
}
//获取道具配置
function xmlToolAry(){
    $CI = & get_instance();//超级ci对象
    $tObj = $CI->config->item('toolPath','adconfig');
    $data['clothes'] = xmlTool($tObj['clothes']);
    $data['item'] = xmlTool($tObj['item']);
    return $data;
}
//根据二级道具id查一级道具名字
function getToolLev1($tid){
    $data = xmlToolAry();
    foreach($data as $k=> $tAry){
        $idAry = array_column($tAry,'Id');//返回Id列的值
        if(in_array($tid,$idAry)){
            return $k;break;
        }
    }
    return false;
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

//礼包生成随机码
function make_rand_code( $length = 12 )
{
    // 密码字符集，[0,0,1,i不要，防止用户混淆]
    $chars = array('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'j', 'k', 'l','m', 'n', 'p', 'q', 'r', 's','t', 'u', 'v', 'w', 'x', 'y','z', '2', '3', '4', '5', '6', '7', '8', '9');
    // 在 $chars 中随机取 $length 个数组元素键名
    $keys = array_rand($chars, $length);
    $password = '';
    foreach($keys as $v)
    {
        $password .= $chars[$v];
    }
    $password = str_shuffle($password);//打乱字符串
    return $password;
}

