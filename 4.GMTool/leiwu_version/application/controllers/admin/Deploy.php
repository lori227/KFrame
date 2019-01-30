<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class Deploy extends MY_Controller {

    public function __construct() {
        parent::__construct();//必须有
        $this->load->model('db_deploy');
        $this->load->model('db_machine');
    }

    function index() {
        $data['list'] = $this->db_deploy->listAll([]);//var_dump($data);
        $this->load->view('admin/deploy/index', $data);
    }

    //添加页
    public function add(){
        if($this->input->post()){
            $inAry = array(
                'appid'=>check_str($this->input->post('appid')),
                'appname'=>trim($this->input->post('appname')),
                'apptype'=>trim($this->input->post('apptype')),
                'startup'=>trim($this->input->post('startup')),
                'debug'=>trim($this->input->post('debug')),

               // 'shutdown'=>trim($this->input->post('shutdown')),
               // 'process'=>trim($this->input->post('process')),
                //'agentid'=>trim($this->input->post('agentid')),

                'localip'=>trim($this->input->post('localip')),
                'time'=>time(),
            );
            $cnt = $this->db_deploy->count(array('appid'=>$inAry['appid']));
            if($cnt > 0){
                $this->jsAlert('appid已存在');
            }
            $res = $this->db_deploy->add($inAry);

            $dLogAry=['controller'=>__CLASS__,'action'=>__FUNCTION__,'type'=>1,'msg'=>$res];  $this->dbLog($dLogAry);
            if($res){
                $this->jsAlert('添加成功','/admin/deploy/index');
            }
        }
        $data['act']=0;
        $data['ips'] = $this->db_machine->listAll();
        $this->load->view('admin/deploy/add', $data);
    }

    //修改页
    public function modify($id){
        $data = $this->db_deploy->detail($id);
        $data['act']=1;

        if($this->input->post()){
            $upAry = array(
                'appname'=>trim($this->input->post('appname')),
                'apptype'=>trim($this->input->post('apptype')),
                'startup'=>trim($this->input->post('startup')),
                'debug'=>trim($this->input->post('debug')),

               // 'shutdown'=>trim($this->input->post('shutdown')),
               // 'process'=>trim($this->input->post('process')),
               // 'agentid'=>trim($this->input->post('agentid')),

                'localip'=>trim($this->input->post('localip')),
            );
            $res = $this->db_deploy->update($id,$upAry);
            if($res){
                $dLogAry=['controller'=>__CLASS__,'action'=>__FUNCTION__,'type'=>3];
                $this->dbLog($dLogAry);
                $logAry = array($id,$res,$this->userData['uid']);
                lwLog($logAry,'deploy_modify');
                $this->jsAlert('更新成功_'.$res,'/admin/deploy/index');
            }
        }
        $data['ips'] = $this->db_machine->listAll();
        $this->load->view('admin/deploy/add', $data);
    }

    //删除
    public function del($id){
        $res = $this->db_deploy->delete($id);
        if($res){
            $dLogAry=['controller'=>__CLASS__,'action'=>__FUNCTION__,'type'=>2];
            $this->dbLog($dLogAry);
            $logAry = array($id,$res,$this->userData['uid']);
            lwLog($logAry,__METHOD__);
            $this->jsAlert('删除成功_'.$res,'/admin/deploy/index');
        }
    }
}
