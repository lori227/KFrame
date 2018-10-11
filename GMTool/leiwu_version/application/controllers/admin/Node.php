<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class Node extends MY_Controller {

    public function __construct() {
        parent::__construct();//必须有
        $this->load->model('db_node');
    }

    function index() {
        $data['list'] = $this->db_node->listAll();
        $this->load->view('admin/node/list', $data);
    }

    //添加页
    public function add(){
        if($this->input->post()){
            $inAry = array(
                'ip'=>intval($this->input->post('ip')),
                'port'=>trim($this->input->post('port')),
                'token'=>trim($this->input->post('token')),
                'comment'=>trim($this->input->post('comment')),
                'add_user'=>$this->userData['uid'],
                'create_time'=>time(),
            );
            $res = $this->db_node->add($inAry);
            if($res){
                $dLogAry=['controller'=>__CLASS__,'action'=>__FUNCTION__,'type'=>1];
                $this->dbLog($dLogAry);
                $this->jsAlert('添加成功','/admin/node/index');
            }
        }
        $data['act']=0;
        $this->load->view('admin/node/add_node', $data);
    }

    //修改页
    public function modify($id){
        $data = $this->db_node->detail($id);
        $data['act']=1;

        if($this->input->post()){
            $upAry = array(
                'ip'=>intval($this->input->post('ip')),
                'port'=>trim($this->input->post('port')),
                'token'=>trim($this->input->post('token')),
                'comment'=>trim($this->input->post('comment')),
            );
            $res = $this->db_node->update($id,$upAry);
            if($res){
                $dLogAry=['controller'=>__CLASS__,'action'=>__FUNCTION__,'type'=>3];
                $this->dbLog($dLogAry);
                $logAry = array($id,$res,$this->userData['uid']);
                lwLog($logAry,__METHOD__);
                $this->jsAlert('更新成功_'.$res,'/admin/node/index');
            }
        }
        $this->load->view('admin/node/add_node', $data);
    }

    //删除
    public function del($id){
        $res = $this->db_node->delete($id);
        if($res){
            $dLogAry=['controller'=>__CLASS__,'action'=>__FUNCTION__,'type'=>2];
            $this->dbLog($dLogAry);
            $logAry = array($id,$res,$this->userData['uid']);
            lwLog($logAry,__METHOD__);
            $this->jsAlert('删除成功_'.$res,'/admin/node/index');
        }
    }
}
