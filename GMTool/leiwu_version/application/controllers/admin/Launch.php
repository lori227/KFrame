<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class launch extends MY_Controller {

    public function __construct() {
        parent::__construct();//必须有
        $this->load->model('db_launch');
    }

    function index() {
        $data['list'] = $this->db_launch->listAll();//var_dump($data);
        $this->load->view('admin/launch/index', $data);
    }

    //添加页
    public function add(){
        if($this->input->post()){
            $app['appname'] = check_str($this->input->post('appname'));//appname+apptype唯一性
            $app['apptype'] = check_str($this->input->post('apptype'));
            if(empty($app['appname']) || empty($app['apptype'])){
                $this->jsAlert('appname或apptype不能为空');
            }
            $cnt = $this->db_launch->count($app);
            if($cnt > 0){
                $this->jsAlert('appname+apptype已有记录');
            }
            $inAry = array(
                'appname'=>$app['appname'],
                'apptype'=>$app['apptype'],
                'apppath'=>trim($this->input->post('apppath')),
                'appfile'=>trim($this->input->post('appfile')),
                'appconfig'=>trim($this->input->post('appconfig')),
                'logtype'=>trim($this->input->post('logtype')),
                'service'=>trim($this->input->post('service')),
                'ftpid'=>trim($this->input->post('ftpid')),
                'deploypath'=>trim($this->input->post('deploypath')),
            );
            $res = $this->db_launch->add($inAry);
            $dLogAry=['controller'=>__CLASS__,'action'=>__FUNCTION__,'type'=>1,'msg'=>$res];
            $this->dbLog($dLogAry);
            if($res){
                $this->jsAlert('添加成功','/admin/launch/index');
            }else{
                $this->jsAlert('添加失败');
            }
        }
        $data['act']=0;
        $this->load->view('admin/launch/add', $data);
    }

    //修改页
    public function modify($name,$type){
        $name = check_str($name);
        $type = check_str($type);
        $data = $this->db_launch->detail($name,$type);
        $data['act']=1;

        if($this->input->post()){
            $upAry = array(
                'apppath'=>trim($this->input->post('apppath')),
                'appfile'=>trim($this->input->post('appfile')),
                'appconfig'=>trim($this->input->post('appconfig')),
                'logtype'=>trim($this->input->post('logtype')),
                'service'=>trim($this->input->post('service')),
                'ftpid'=>trim($this->input->post('ftpid')),
                'deploypath'=>trim($this->input->post('deploypath')),
            );
            $res = $this->db_launch->update($name,$type,$upAry);
            if($res){
                $dLogAry=['controller'=>__CLASS__,'action'=>__FUNCTION__,'type'=>3];
                $this->dbLog($dLogAry);
                $logAry = array($id,$res,$this->userData['uid']);
                lwLog($logAry,'launch_modify');
                $this->jsAlert('更新成功_'.$res,'/admin/launch/index');
            }
        }
        $this->load->view('admin/launch/add', $data);
    }

    //删除
    public function del($name,$type){
        $name = check_str($name);
        $type = check_str($type);
        $res = $this->db_launch->delete($name,$type);
        if($res){
            $dLogAry=['controller'=>__CLASS__,'action'=>__FUNCTION__,'type'=>2];
            $this->dbLog($dLogAry);
            $logAry = array($res,$this->userData['uid']);
            lwLog($logAry,__METHOD__);
            $this->jsAlert('删除成功_'.$res,'/admin/launch/index');
        }
    }
}
