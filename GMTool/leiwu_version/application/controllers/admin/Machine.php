<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class Machine extends MY_Controller {

    public function __construct() {
        parent::__construct();//必须有
        $this->load->model('db_machine');
    }

    function index() {
        $data['list'] = $this->db_machine->listAll();//var_dump($data);
        $this->load->view('admin/machine/index', $data);
    }
    //添加页
    public function add(){
        if($this->input->post()){
            $inAry = array(
                'name'=>intval($this->input->post('name')),
                'localip'=>trim($this->input->post('localip')),
                'interanetip'=>trim($this->input->post('interanetip')),
                'cpu'=>trim($this->input->post('cpu')),

                'thread'=>trim($this->input->post('thread')),
                'memory'=>trim($this->input->post('memory')),
                'harddisk'=>trim($this->input->post('harddisk')),
                'agentid'=>trim($this->input->post('agentid')),

                'status'=>trim($this->input->post('status')),
                'port'=>trim($this->input->post('port')),
            );
            $res = $this->db_machine->add($inAry);
            if($res){
                $dLogAry=['controller'=>__CLASS__,'action'=>__FUNCTION__,'type'=>1];
                $this->dbLog($dLogAry);
                $this->jsAlert('添加成功','/admin/machine/index');
            }
        }
        $data['act']=0;
        $this->load->view('admin/machine/add', $data);
    }

    //修改页
    public function modify($id){
        $id = check_str($id);
        $data = $this->db_machine->detail($id);
        $data['act']=1;

        if($this->input->post()){
            $upAry = array(
                'cpu'=>trim($this->input->post('cpu')),

                'thread'=>trim($this->input->post('thread')),
                'memory'=>trim($this->input->post('memory')),
                'harddisk'=>trim($this->input->post('harddisk')),
                'agentid'=>trim($this->input->post('agentid')),

                'status'=>trim($this->input->post('status')),
                'port'=>trim($this->input->post('port')),
            );
            $res = $this->db_machine->update($id,$upAry);
            if($res){
                $dLogAry=['controller'=>__CLASS__,'action'=>__FUNCTION__,'type'=>3];
                $this->dbLog($dLogAry);
                $logAry = array($id,$res,$this->userData['uid']);
                lwLog($logAry,'machine_modify');
                $this->jsAlert('更新成功_'.$res,'/admin/machine/index');
            }
        }
        $this->load->view('admin/machine/add', $data);
    }

    //删除
    public function del($id){
        $res = $this->db_machine->delete($id);
        if($res){
            $dLogAry=['controller'=>__CLASS__,'action'=>__FUNCTION__,'type'=>2];
            $this->dbLog($dLogAry);
            $logAry = array($id,$res,$this->userData['uid']);
            lwLog($logAry,__METHOD__);
            $this->jsAlert('删除成功_'.$res);
        }
    }
}
