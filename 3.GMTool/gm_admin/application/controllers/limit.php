<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');
//==类文件
class Limit extends MY_Controller {


    public function __construct() {
        parent::__construct();//必须有，不然报错
        header("Content-type: text/html; charset=utf-8");
    }

    
    function index() {
        $data = array();
        $this->load->view('index/index', $data);
    }

    public function ip(){
        $data['users'] = $this->administrator();
        $data['data'] = $this->db->from('ip_limit')->get()->result_array();

        $this->load->view('limit/ip', $data);
    }

    public function addIp(){
        if($this->input->isPost()){
            $status = trim($this->input->post('status'));
            $ip = trim($this->input->post('ip'));
            $reason = trim($this->input->post('reason'));
            if(!filter_var($ip,FILTER_VALIDATE_IP)){
                $this->jsAlert('IP形式错误');
            }
            $inAry = array(
                'status'=>$status,
                'ip'=>$ip,
                'reason'=>$reason,
                'add_user'=>$this->userData['uid'],
                'add_time'=>time(),
            );
            $res = $this->db->insert('ip_limit', $inAry);
            if($res){
                $this->jsAlert('添加成功','/limit/ip');
            }
        }
        $data = array();
        $this->load->view('limit/addIp', $data);
    }



}
