<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');
//公告系统类文件
class Notice extends MY_Controller {


    public function __construct() {
        parent::__construct();//必须有，不然报错
        header("Content-type: text/html; charset=utf-8");
    }

    
    function index() {
        $queryParams = array(
            'id'   => intval($this->input->get('id')),
            'type' => intval($this->input->get('type')),
            'title' => trim($this->input->get('title')),
        );

        $where = array();
        if($queryParams['id']){
            $where['id'] = $queryParams['id'];
        }
        if($queryParams['type']){
            $where['type'] = $queryParams['type'];
        }
        $obj = $this->db->from('notice');
        $obj->where($where);
        $this->load->helper('common');
        $searchTitle = check_str($queryParams['title']);//过滤防sql注入
        if($searchTitle){
            $obj->like("title",$searchTitle);
        }

        $data = array();
        $data['data'] = $obj->order_by("id")->get()->result_array();
        $data['type'] = $this->config->item('noticeType', 'adconfig');
        $data['users'] = $this->administrator();
        $data['queryParams'] = $queryParams;
        $this->load->view('notice/index', $data);
    }

    function add() {
        if($this->input->post()){
            $inAty['channel_id'] = intval($this->input->post('channel_id'));//渠道id
            $inAty['game_id'] = intval($this->input->post('game_id'));//游戏
            $inAty['type'] = intval($this->input->post('type'));//类型
            $inAty['title'] = trim($this->input->post('title'));
            $inAty['content'] = trim($this->input->post('content'));
            $time1 = trim($this->input->post('time1'));
            $inAty['start_time'] = strtotime($time1);
           if(count(array_filter($inAty)) <4 ){
               $this->jsAlert('提交的数据不对');
           }
            $inAty['add_user'] = $this->userData['uid'];
            $inAty['add_time'] = time();
            $inAty['update_time'] = time();
            $res = $this->db->insert('notice', $inAty);
            if($res){
                $this->gmLog($inAty,__METHOD__);
                $this->jsAlert('插入成功_'.$res,'/notice/index');
            }
        }
        $data = array();
        //获取游戏列表
        $this->load->model('Games');
        $data['games'] = $this->Games->listAll();
        $data['actNmae'] = '添加';
        $this->load->view('notice/add', $data);
    }

    function modify($id){
        if($this->input->post()){
            $inAty['type'] = intval($this->input->post('type'));
            $inAty['title'] = trim($this->input->post('title'));
            $inAty['content'] = trim($this->input->post('content'));
            if(count(array_filter($inAty)) <3 ){
                $this->jsAlert('提交的数据不对');
            }
            $inAty['update_time'] = time();
            $res = $this->db->update('notice', $inAty,array('id'=>$id));
            if($res){
                $this->gmLog($inAty,__METHOD__);
                $this->jsAlert('修改成功_'.$res,'/notice/index');
            }
        }
        $data = $this->db->from('notice')->where('id',$id)->get()->row_array();
        $data['actNmae'] = '修改';
        $data['time1'] = date('Y-m-d H:i:s',$data['start_time']);
        $data['is_modify']=true;
        $this->load->view('notice/add', $data);
    }



}
