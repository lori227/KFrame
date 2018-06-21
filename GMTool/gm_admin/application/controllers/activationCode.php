<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');
//激活码类文件
const ACTCODE = "activationcode";
class ActivationCode extends MY_Controller {


    public function __construct() {
        parent::__construct();//必须有，不然报错
        header("Content-type: text/html; charset=utf-8");
    }
    //生成激活码
    public function add(){
        $this->_data = array();
        if($this->input->isPost()){
            $length = intval(trim($this->input->post('length')));//随机码长度
            $num = intval(trim($this->input->post('num')));
            $resAll = "";
            $codeAll = "";
            $codeAry = array();
            for ($i = 0; $i < $num; $i++) {
                $cdkey = $this->make_rand_code($length);
                $res = $this->redis->sAdd(ACTCODE , $cdkey);
                $this->redis->EXPIRE(ACTCODE, 60*60*24*365); //设置过期时间 （365 天）
                $resAll .= $res;
                $codeAll .= $cdkey.'|';
                $codeAry[] = array("code"=>$cdkey);
            }
            $this->gmLog($codeAll.'--'.$resAll);//写入日志
            //$this->_data['act'] = 'success';
            $this->export_csv($codeAry);
        }else{
            $this->load->view('activationCode/add', $this->_data);
        }
    }
    
    function index() {
        $list = $this->redis->Srandmember(ACTCODE,100); //获取容器key中所有元素
        $this->load->view('activationCode/index', array('list' => $list));
    }

/*    function getrandstr(){
        $str='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890';
        $randStr = str_shuffle($str);//打乱字符串
        $rands= substr($randStr,0,6);//substr(string,start,length);返回字符串的一部分
        return $rands;
    }*/

    //生成随机码
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
    //导出csv文件
    function export_csv($data)
    {
        $string="";
        foreach ($data as $key => $value)
        {
            foreach ($value as $k => $val)
            {
                $value[$k]=iconv('utf-8','gb2312',$value[$k]);
            }

            $string .= implode(",",$value)."\n"; //用英文逗号分开
        }
        $filename = date('YmdHi').'.csv'; //设置文件名
        header("Content-type:text/csv");
        header("Content-Disposition:attachment;filename=".$filename);
        header('Cache-Control:must-revalidate,post-check=0,pre-check=0');
        header('Expires:0');
        header('Pragma:public');
        echo $string;exit;
    }
    //按量导出
    function exportNum(){
        if($this->input->isPost()){
            $num = intval($this->input->post('num'));
            $list = $this->redis->Srandmember(ACTCODE,$num); //多个随机元素
            $exportAry = array();
            foreach($list as $ary){
                $exportAry[] = array("key"=>$ary);
            }
            $this->export_csv($exportAry);exit;
        }
    }

    //导出全部
    function exportAll(){
        $list = $this->redis->sMembers(ACTCODE); //获取容器key中所有元素
        $exportAry = array();
        foreach($list as $ary){
            $exportAry[] = array("key"=>$ary);
        }
        $this->export_csv($exportAry);exit;
    }

}
