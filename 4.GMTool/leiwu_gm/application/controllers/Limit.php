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
        $data['ip_status'] = $this->config->item('ip_status', 'adconfig');
        $this->load->view('limit/ip', $data);
    }
    //添加页
    public function addIp(){
        if($this->input->post()){
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

    //修改页
    public function modify($id){
        $data = $this->db->from('ip_limit')->where('id',$id)->get()->row_array();
        $data['is_modify']=true;

        if($this->input->post()){
            $status = trim($this->input->post('status'));
            $reason = trim($this->input->post('reason'));
            $upAry =array(
                'status'=>$status,
                'reason'=>$reason,
            );
            $res = $this->db->update('ip_limit', $upAry,array('id'=>$id));
            if($res){
                $this->jsAlert('更新成功_'.$res,'/limit/ip');
            }
        }
        $this->load->view('limit/addIp', $data);
    }

    //删除
    public function delete($id){
        $res = $this->db->delete('ip_limit',array('id'=>$id));
        if($res){
            $this->jsAlert('删除成功_'.$res,'/limit/ip');
        }
    }



}
