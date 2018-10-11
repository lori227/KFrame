<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class Project extends MY_Controller {

    public function __construct() {
        parent::__construct();//必须有
        $this->load->model('db_project');
    }

    function index() {

        $data['list'] = $this->db_project->listAll();//var_dump($data);
        $this->load->view('admin/project/index', $data);
    }

    //添加页
    public function add(){
        if($this->input->post()){
            $inAry = array(
                'group_id'=>intval($this->input->post('group_id')),
                'name'=>trim($this->input->post('name')),
                'repository_url'=>trim($this->input->post('repository_url')),
                'https_username'=>trim($this->input->post('https_username')),

                'https_password'=>trim($this->input->post('https_password')),
                'branch'=>trim($this->input->post('branch')),
                'code_path'=>trim($this->input->post('code_path')),
                'is_grayscale_publish'=>trim($this->input->post('is_grayscale_publish')),
                'code_dir_user'=>trim($this->input->post('code_dir_user')),
                'comment'=>trim($this->input->post('comment')),

                'add_user'=>$this->userData['uid'],
                'create_time'=>time(),
            );
            $res = $this->db_project->add($inAry);
            if($res){
                $dLogAry=['controller'=>__CLASS__,'action'=>__FUNCTION__,'type'=>1];
                $this->dbLog($dLogAry);
                $this->jsAlert('添加成功','/admin/project');
            }
        }
        $data['act']=0;
        $this->load->model('db_project_group');
        $data['group'] = $this->db_project_group->listAll();
        $this->load->view('admin/project/add_project', $data);
    }

    //修改页
    public function modify($id){
        $data = $this->db_project->detail($id);
        $data['act']=1;

        if($this->input->post()){
            $upAry = array(
                'group_id'=>intval($this->input->post('group_id')),
                'name'=>trim($this->input->post('name')),
                'repository_url'=>trim($this->input->post('repository_url')),
                'https_username'=>trim($this->input->post('https_username')),
                'https_password'=>trim($this->input->post('https_password')),

                'branch'=>trim($this->input->post('branch')),
                'code_path'=>trim($this->input->post('code_path')),
                'is_grayscale_publish'=>trim($this->input->post('is_grayscale_publish')),
                'code_dir_user'=>trim($this->input->post('code_dir_user')),
                'comment'=>trim($this->input->post('comment')),
                'update_time'=>time(),
            );
            $res = $this->db_project->update($id,$upAry);
            if($res){
                $dLogAry=['controller'=>__CLASS__,'action'=>__FUNCTION__,'type'=>3];
                $this->dbLog($dLogAry);
                $logAry = array($id,$res,$this->userData['uid']);
                lwLog($logAry,'project_modify');
                $this->jsAlert('更新成功_'.$res,'/admin/project/index');
            }
        }
        $this->load->model('db_project_group');
        $data['group'] = $this->db_project_group->listAll();
        $this->load->view('admin/project/add_project', $data);
    }

    //删除
    public function del($id){
        $res = $this->db_project->delete($id);
        if($res){
            $dLogAry=['controller'=>__CLASS__,'action'=>__FUNCTION__,'type'=>2];
            $this->dbLog($dLogAry);
            $logAry = array($id,$res,$this->userData['uid']);
            lwLog($logAry,'project_del');
            $this->jsAlert('删除成功_'.$res,'/admin/project/index');
        }
    }
}
