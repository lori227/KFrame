<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');
//游戏类文件
class Game extends MY_Controller {


    public function __construct() {
        parent::__construct();//必须有，不然报错
        header("Content-type: text/html; charset=utf-8");
    }

    
    function index() {
        $data['users'] = $this->administrator();
        $data['data'] = $this->db->from('game')->order_by('id desc')->get()->result_array();
        $this->load->view('game/index', $data);
    }

    //添加页
    public function add(){
        if($this->input->post()){
            $name = trim($this->input->post('name'));
            $inAry = array(
                'name'=>$name,
                'add_user'=>$this->userData['uid'],
                'add_time'=>date('Y-m-d H:i:s',time()),
            );
            $this->load->model('Games');
            $res = $this->Games->add($inAry);
            if($res){
                $this->jsAlert('添加成功','/game/index');
            }
        }
        $data = array();
        $this->load->view('game/add', $data);
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
}
