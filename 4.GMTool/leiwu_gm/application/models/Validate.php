<?php
/**
 *@info 验证类
 *@author: hj
 *@date: 2015-01-19
 * */
class Validate extends MY_Model {
	/**
	 *@info 构造函数
	 *@author: hj
	 *@date: 2015-01-19
	 *@param null
	 *@return null
	 * */
	public function __construct(){
		parent::__construct();
	}
	
	/**
	 * 生成UUID
	 * @author	 hj
	 * @date	2015-01-19
	 * @param	null
	 * @return string 不重复的UUID
	 */
	public function getuuid(){
		mt_srand((double)microtime()*10000);
		$charid = strtoupper(md5(uniqid(rand(), true)));
		$uuid = substr($charid, 0, 8).substr($charid, 8, 4).substr($charid,12, 4).substr($charid,16, 4).substr($charid,20,12);
		return $uuid;
	}
	
	/**
	 * 获得客户端IP地址
	 *
	 * @return string
	 */
	public static function getIp(){
		static $ip;
		if (! isset ( $ip )) {
			$c = getenv ( 'HTTP_CLIENT_IP' );
			$x = getenv ( 'HTTP_X_FORWARDED_FOR' );
			$r = getenv ( 'REMOTE_ADDR' );
			$sr = @$_SERVER ['REMOTE_ADDR'];
			if ($c && strcasecmp ( $c, 'unknown' )) {
				$ip = $c;
			} elseif ($x && strcasecmp ( $x, 'unknown' )) {
				$ip = $x;
			} elseif ($r && strcasecmp ( $r, 'unknown' )) {
				$ip = $r;
			} elseif ($sr && strcasecmp ( $sr, 'unknown' )) {
				$ip = $sr;
			}
			$ip = preg_match ( "/^([\d]{1,3}\.){3}[\d]{1,3}$/", $ip ) ? $ip : 'unknown';
		}
		return $ip;
	}
	
	/**
	 *
	 * @todo 返回时间戳计算后的时间
	 * @author HJ
	 *         @date	20140514
	 * @param
	 *        	string StartTime 开始时间
	 * @param
	 *        	string EndTime 结束时间
	 * @param
	 *        	string Tag 格式(Y,M,D)
	 * @return 返回转换好的时间戳
	 *
	 */
	public function Countymd($StartTime, $EndTime, $Tag = 'D') {
		$Stime = '';
		$ETime = '';
	
		! $StartTime ? $Stime = time () : $Stime = $StartTime;
		! $EndTime ? $Etime = time () : $Etime = $EndTime;
	
		if ($EndTime < $StartTime) {
			return false;
		}
	
		Switch ($Tag) {
			case 'Y' :
				return round ( ($EndTime - $StartTime) / 3600 / 24 / 365 );
				break;
			case 'M':
				return round((($EndTime-$StartTime)/3600/24/365)*12);
				break;
			case 'D':
				return round(($EndTime-$StartTime)/3600/24);
				break;
			default:
				return false;
				break;
		}
	
		return false;
	}
	
	/**
	 * @todo	获得毫秒级的时间戳
	 * @author	 HJ
	 * @date	2012-09-28
	 * @param	null
	 * @return 返回转换好的时间戳
	 *
	 */
	public function mk_time()
	{
		$time = explode ( " ", microtime () );
	
		$hm = $time [0] * 1000;
	
		$_hm_arr = explode(".",$hm);
	
		if (strlen($_hm_arr[0]) == 1)
		{
			$_hm_arr[0] = "00".$_hm_arr[0];
		}
	
		if (strlen($_hm_arr[0]) == 2)
		{
	
			$_hm_arr[0] = "0".$_hm_arr[0];
		}
	
		$time2 = $time [1] . $_hm_arr[0];
	
		return $time2;
	}
	
	/**
	 * @todo	将毫秒级的时间戳转换为正常日期
	 * @author	 HJ
	 * @date	2012-09-28
	 * @param	string $times 时间戳
	 * @param	string $format 转换格式
	 * @return 返回转换好的时间
	 *
	 */
	public function de_mk_time($times,$format='Y-m-d h:i:s'){
	
		if ( ! is_numeric($times)){
			return false;
		}
	
		$Date = date($format,substr($times,0,-3));
	
		return $Date;
	}
	
	/**
	 * @todo	判断时间是否合法
	 * @author	 hj
	 * @date	2014-07-06
	 * @param	string $times 时间格式
	 * @return true or false
	 *
	 */
	public function ChkTimes($Times){
		if (strtotime($Times)){
			return true;
		}
		return false;
	}
	
	/**
	 * @todo	获得http头信息
	 * @author	 hj
	 * @date	2014-07-18
	 * @param	null
	 * @return array
	 *
	 */
	public function GetallHeaders()
	{
		foreach ($_SERVER as $name => $value)
		{
			if (substr($name, 0, 5) == 'HTTP_')
			{
				$headers[str_replace(' ', '-', ucwords(strtolower(str_replace('_', ' ', substr($name, 5)))))] = $value;
			}
		}
		return $headers;
	}
	
	/**
	 *求两个已知经纬度之间的距离,单位为米
	 *@param lng1,lng2 经度
	 *@param lat1,lat2 纬度
	 *@return float 距离，单位米
	 **/
	public function GetDistance($lng1,$lat1,$lng2,$lat2){
		//将角度转为狐度
		$radLat1=deg2rad($lat1);//deg2rad()函数将角度转换为弧度
		$radLat2=deg2rad($lat2);
		$radLng1=deg2rad($lng1);
		$radLng2=deg2rad($lng2);
		$a=$radLat1-$radLat2;
		$b=$radLng1-$radLng2;
		$s=2*asin(sqrt(pow(sin($a/2),2)+cos($radLat1)*cos($radLat2)*pow(sin($b/2),2)))*6378.137*1000;
		return $s;
	}
	
	/**
	 *  说明:二维数组去重
	 *
	 *  @param    array2D    要处理二维数组
	 *  @param    stkeep     是否保留一级数组键值(默认不保留)
	 *  @param    ndformat   是否保留二级数组键值(默认保留)
	 *
	 *  @return   output     返回去重后的数组
	 */
	public function unique_arr($array2D,$stkeep=false,$ndformat=true){
		if($stkeep){    //一级数组键可以为非数字
			$stArr = array_keys($array2D);
		}
		if($ndformat){   //二级数组键必须相同
			$ndArr = array_keys(end($array2D));
		}
		foreach ($array2D as $v){  //降维
			$v = join(",",$v);
			$temp[] = $v;
		}
		$temp = array_unique($temp);
		foreach ($temp as $k => $v){  //数组重新组合
			if($stkeep){
				$k = $stArr[$k];
			}
			if($ndformat){
				$tempArr = explode(",",$v);
				foreach($tempArr as $ndkey => $ndval){
					$output[$k][$ndArr[$ndkey]] = $ndval;
				}
			}else{
				$output[$k] = explode(",",$v);
			}
		}
		return $output;
	}
	
	/**
	 *  说明:获得字符串中间字符信息
	 *
	 *  @param string $string 需要处理的字符按
	 *  @param string $start 开始字符
	 *  @param string $end 结束字符
	 *
	 *  @return string 字符串中间字符信息
	 */
	public function substr_between($string, $start, $end=null) {
		if(($start_pos = strpos($string, $start)) !== false) {
			if($end) {
				if(($end_pos = strpos($string, $end, $start_pos + strlen($start))) !== false) {
					return substr($string, $start_pos + strlen($start), $end_pos - ($start_pos + strlen($start)));
				}
			} else {
				return substr($string, $start_pos);
			}
		}
		return false;
	}
	
	/**
	 *@info 获取客户端ip
	 *@author: HJ
	 *@date: 2014-04-12
	 *@param null
	 *@return 返回获得的ip
	 * */
	public function getRalIp(){
		$ip=false;
		if(!empty($_SERVER["HTTP_CLIENT_IP"])){
			$ip = $_SERVER["HTTP_CLIENT_IP"];
		}
		if (!empty($_SERVER['HTTP_X_FORWARDED_FOR'])){
			$ips = explode (", ", $_SERVER['HTTP_X_FORWARDED_FOR']);
			if ($ip){
				array_unshift($ips, $ip); $ip = FALSE;
			}
			for ($i = 0; $i < count($ips); $i++){
				if (!eregi ("^(10|172\.16|192\.168)\.", $ips[$i])){
					$ip = $ips[$i];
					break;
				}
			}
		}
		return ($ip ? $ip : $_SERVER['REMOTE_ADDR']);
	}
	
	/**
	 *@info 生成token串
	 *@date: 2014-04-10
	 *@author: HJ
	 *@param int $Len  token 长度
	 *@return 返回token
	 * */
	public function createToken($Len = 32){
		mt_srand ( ( double ) microtime () * 1000000 );
		$Chars = array(
				'1','2','3','4','5','6','7','8','9',
				'a','b','c','d','e','f','g','h','i','g','k','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
				'A','C','D','E','F','G','H','I','G','K','L','M','O','P','Q','R','S','T','U','V','W','X','Z','Y'
		);
		$NumChars = count ( $Chars ) - 1;
		$token = '';
		# Create random token at the specified length
		for($i = 0; $i < $Len; $i ++){
			$token .= $Chars [mt_rand ( 0, $NumChars )];
		}
		return $token;
	}
	
	# 请求变量进行处理函数系列
	public function addslashDeep ($val ,$force = false){
		if ( ! get_magic_quotes_gpc() || $force){
			if (is_array($val)){
				foreach ($val as $key => $value){
					if (is_array($value)){
						$val[$key] = self::addslashDeep($value, $force);
					}else{
						$val[$key] = addslashes($value);
					}
				}
			}else{
				$val = addslashes($val);
			}
		}
		return $val;
	}
	#二维数组排序
	public function multi_array_sort($multi_array,$sort_key,$sort=SORT_ASC){
		if(is_array($multi_array)){
			foreach ($multi_array as $row_array){
				if(is_array($row_array)){
					$key_array[] = $row_array[$sort_key];
				}else{
					return false;
				}
			}
		}else{
			return false;
		}
		array_multisort($key_array,$sort,$multi_array);
		return $multi_array;
	}
	
	public function tableCutPrefix($id){
		return substr(md5($id),0,1);
	}
	#是否是数字
	public function isAlnum($str){
		return ( ! preg_match("/^([a-zA-Z0-9])+$/i", $str)) ? false : true;
	}
	#最小长度
	public function minLen($str, $len){
		if (function_exists('mb_strlen')){
			return (mb_strlen($str) < $len) ? false : true;
		}
		return (strlen($str) < $len) ? false : true;
	}
	#最大长度
	public function maxLen($str, $len){
		if (function_exists('mb_strlen')){
			return (mb_strlen($str) > $len) ? false : true;
		}
		return (strlen($str) > $len) ? false : true;
	}
	
	/**
	* 验证传入字符串的长度是否在指定范围内
	*
	* @param  string    $str    待验证数据
	 * @param  integer   $min    起始长度
	 * @param  integer   $max    截止长度
	 * @param  boolen
	 */
	 public function isRangeLen($str, $min,$max){
		if( !self::minLen($str,$min)){
		return false ;
		}
		return self::maxLen($str,$max);
	 }
	
	public function isMobile($phone){
		$pattern = "/^1(3|4|5|7|8)[0-9]{1}\d{8}$/";
			return (bool)(preg_match($pattern,$phone)) ;
		}

	public function isNatural($str){
		return (bool) preg_match('/^[0-9]+$/', $str);
	}

	public function isTime($str){
		if( self::isNatural($str) === false || $str < 1 || strlen($str) != 10 ){
			return false ;
		}
		if (strtotime(date('YmdHis', $str)) == $str){
			return true ;
		}
		return false ;
	}
	#判断邮箱
	public function isEmail($str){
		return ( ! preg_match("/^([a-z0-9\+_\-]+)(\.[a-z0-9\+_\-]+)*@([a-z0-9\-]+\.)+[a-z]{2,6}$/ix", $str)) ? false : true;
	}
	#转换为crc32值
	public function convertToCRC32($str){
		return sprintf('%u',crc32($str));
	}
	#扫描目录下的文件
	public function deepScanDir($dir) {
		$fileArr = array ();
		$dirArr = array ();
		$dir = rtrim($dir, '//');
		if (is_dir($dir)) {
			$dirHandle = opendir($dir);
			while (false !== ($fileName = readdir($dirHandle))) {
				$subFile = $dir . DIRECTORY_SEPARATOR . $fileName;
				if (is_file($subFile)) {
					$fileArr[] = $subFile;
				}elseif (is_dir($subFile) && str_replace('.', '', $fileName) != ''){
					$dirArr[] = $subFile;
					$arr = deepScanDir($subFile);
					$dirArr = array_merge($dirArr, $arr['dir']);
					$fileArr = array_merge($fileArr, $arr['file']);
				}
			}
			closedir($dirHandle);
		}
		return array (
			'dir' => $dirArr,
			'file' => $fileArr
		);
	}
	#判断是否无效值
	public function chkVal($val){
		if (empty($val) || is_null($val) || $val === '' || $val === false){
			return false;
		}
		return true;
	}
	#判断日期（根据格式）
	public function chkDate($date){
		return strtotime($date);
	}
	#判断是否日期
	public function validateDate($date){
		return preg_match("/^[0-9]{4}(\-|\/)[0-9]{1,2}(\\1)[0-9]{1,2}(|\s+[0-9]{1,2}(|:[0-9]{1,2}(|:[0-9]{1,2})))$/",$date);
	}
}