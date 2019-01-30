<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');
//==类文件
class White extends MY_Controller {


    public function __construct() {
        parent::__construct();//必须有，不然报错
        header("Content-type: text/html; charset=utf-8");
    }

    public function index(){
        $data['users'] = $this->administrator();
        $this->load->model('Whites');
        $data['data'] = $this->Whites->listAll();
        $this->load->view('white/index', $data);
    }
    //添加页
    public function add(){
        if($this->input->post()){
            $game_id = intval($this->input->post('game_id'));
            $account_id = intval($this->input->post('account_id'));//账户ID
            $reason = htmlspecialchars(trim($this->input->post('reason')));

            $accInfo = $this->getAcc($account_id);
            if(empty($accInfo)){
                $this->jsAlert($account_id.'_该账户不存在');
            }
            $inAry = array(
                'game_id'=>$game_id,
                'account_id'=>$account_id,
                'reason'=>$reason,
                'add_user'=>$this->userData['uid'],
                'add_time'=>time(),
            );
            $res = $this->db->insert('white', $inAry);
            if($res){
                $this->jsAlert('添加成功','/white/index');
            }
        }
        $data = array();
        $data['is_modify']=false;
        //获取游戏列表
        $this->load->model('Games');
        $data['games'] = $this->Games->listAll();
        $this->load->view('white/add', $data);
    }

    //修改页
    public function modify($id){
        $this->load->model('Whites');
        $data =  $this->Whites->info(array('id'=>$id));
        $data['is_modify']=true;

        if($this->input->post()){
            $reason = htmlspecialchars(trim($this->input->post('reason')));
            $is_login = intval($this->input->post('is_allow_login'));
            $upAry =array(
                'is_allow_login'=>$is_login,
                'reason'=>$reason,
            );
            $res =  $this->Whites->update($upAry,array('id'=>$id));
            if($res){
                $this->jsAlert('更新成功_'.$res,'/white/index');
            }
        }
        $this->load->view('white/add', $data);
    }

    //删除
    public function delete($id){
        $this->load->model('Whites');
        $res = $this->Whites->delete(array('id'=>$id));
        if($res){
            $this->jsAlert('删除成功_'.$res,'/white/index');
        }
    }

    private function getAcc($account_id){
        $this->load->model('Server');
        $info = $this->Server->getAcc($account_id);
        return $info;
    }
    //查询账户
    public function ajaxAccount(){
        $account_id = intval($this->input->post('account_id'));
        $info = $this->getAcc($account_id);
        echo json_encode($info);
    }



}
