<?php
//namespace cont;
if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class MY_Controller extends CI_Controller {

    protected $_data    = '';
    protected $userData = array();


    public function __construct() {
        parent::__construct();
		
        $this->load->helper('url');//框架要求必须加载
        $this->load->helper('common');//自己封装常用方法
        $this->kf = $this->load->database('kf', TRUE);//kframe库
        //$this->session->all_userdata() ci框架3.X版本session写法换了，注意注意注意！！！
        $uInfo = $this->session->userdata('user_data');
        if(isset($uInfo)){
            $this->userData = $this->session->userdata('user_data');
            $this->admin    = AdminInfo::buildFromSession($this->userData);
            $tmpUserAuth=[];
            if(!$this->admin->isSuper()){
                $this->load->model('user');
                $this->load->model('auths');
                $tmpUserAuth = $this->user->getAuth($this->userData['uid']);
                $tmpNowNode = $this->auths->getNode($this->router->class, $this->router->method);
                
                if($tmpNowNode !== true && !in_array($tmpNowNode, $tmpUserAuth)){
                    if($this->input->is_ajax_request()){
                        $this->output->set_status_header('405');
                    }else{
                        if(strpos($_SERVER["HTTP_REFERER"], '?') !== false){
                            redirect($_SERVER["HTTP_REFERER"].'&authError');
                        }else{
                            redirect($_SERVER["HTTP_REFERER"].'?authError');
                        }
                    }
                    exit;
                }
            }
            //左功能树
            $tmpTree = $this->db->from('auth')->where(array('istree' => 1))->get()->result_array();
            $resVer = $this->_formatTree($tmpTree, $tmpUserAuth);
            $this->load->vars('tree',$resVer );
            //查出父类模块id
            $t_info = $this->db->from('auth')->where(array('class'=>$this->router->class,'method'=>$this->router->method))->get()->row_array();
            $this->load->vars('ci',array('fid'=>$t_info['fid'],'mid'=>$t_info['id'],'realname'=>$uInfo['realname']));
        }else{
            $tmpUri = $this->router->class . '/' . $this->router->method;
            if($tmpUri != 'admin/index' && $tmpUri != 'admin/login'){
                redirect('/admin');
            }
        }
        
    }

    function isLogin() {
        return (isset($this->userData['uid']) && $this->userData['uid'] > 0);
    }
    //左侧菜单
    private function _formatTree($data, $tmpUserAuth){
        $tmpTree = array();
        if( ! $data){
            return $data;
        }

        foreach($data as $v){
            $className="";
            if($v['fid']>0){
                $className = 'admin/'.$v['class'];
            }
            if($v['fid'] == 0){
                $tmpTree[$v['id']]          = $v;
                $tmpTree[$v['id']]['uri']   = '/' . $className . '/' . $v['method'] != '//' ? '/' . $className . '/' . $v['method'] : '#';//父类标签是#号才能产生下拉特效
            }else{
                if($this->userData['username'] != 'admin'){
                	if ( ! $tmpUserAuth){
                		return $data;
                	}
                    if(in_array($v['id'], $tmpUserAuth)){
                        $tmpTree[$v['fid']]['node'][$v['id']]           = $v;
                        $tmpTree[$v['fid']]['node'][$v['id']]['uri']    = '/' . $className . '/' . $v['method'];
                    }
                }else{
                    $tmpTree[$v['fid']]['node'][$v['id']]           = $v;
                    $tmpTree[$v['fid']]['node'][$v['id']]['uri']    = '/' . $className . '/' . $v['method'];
                }
            }
        }
        return $tmpTree;
    }

/*    private function _formatToOneArr($data){
        $tmpData = array();
        foreach($data as $v){
            $tmpData[$v['id']] = $v['name'];
        }
        return $tmpData;
    }*/

    //js提示 x
    function jsAlert($con,$url=""){
        if(!empty($url)){
            echo "<script>alert('".$con."');window.location.href = '".$url."'</script>";exit;
        }else{
            echo "<script>alert('".$con."');history.go(-1);</script>";exit;
        }
        exit;
    }
    //json状态返回
    function jsResponse($code,$con,$data=""){
        echo json_encode(array('code'=>$code,'msg'=>$con,'data'=>$data));
        exit;
    }
    //管理员列表
    function administrator(){
        $leaders = $this->db->select('id,realname')->from('user')->where(array('status' => 1))->get()->result_array();
        foreach ($leaders as $val) {
            $leader[$val['id']] = $val['realname'];
        }
        return $leader;
    }

    //上传七牛云
    function upQiniu($name){
        //上传七牛云
        $this->load->library('qiniuapi');
        $up_res = $this->qiniuapi->upload($name);
        if(!$up_res->hash){
            $this->jsAlert('图片上传失败');
        }
        return $up_res->key;
    }

    //插入数据库日志log表
    protected function dbLog($info){
        $this->load->model('db_log');
        $inAry=array(
            'level'=>isset($info['level'])?$info['level']:6,//7:DEBUG,6:INFO,4:WARNING,3:ERROR
            'controller'=>$info['controller'],
            'action'=>$info['action'],
            'get'=>$_SERVER['REQUEST_URI'],
            'post'=>json_encode($_POST),
            'message'=>isset($info['msg'])?$info['msg']:'',//描述
            'ip'=>get_real_ip(),
            'user_agent'=>$_SERVER['HTTP_USER_AGENT'],
            'referer'=>$_SERVER['HTTP_REFERER'],
            'user_id'=>$this->userData['uid'],
            'username'=>$this->userData['username'],
            'create_time'=>time(),
            'type'=>$info['type'],//增删改查
        );
        $res = $this->db_log->add($inAry);
        return $res;
    }
}



class AdminInfo {

    private $id;

    private $username;

    private $realName;

    private $sourceData;


    function __construct($id, $username, $realName) {
        $this->id = $id;
        $this->username = $username;
        $this->realName = $realName;
    }

    function getId() {
        return $this->id;
    }

    function getUsername() {
        return $this->username;
    }

    function getRealName() {
        return $this->realName;
    }

    function getAllowChannelType() {
        $this->loadSourceData();
        return $this->sourceData['allow_channel_type'];
    }

    function getAllowApps(){
        $this->loadSourceData();
        return $this->sourceData['allow_app'];
    }

    private function loadSourceData() {
        if ($this->sourceData == null) {
            $rs = get_instance()->db->get_where('user', array('username' => $this->username))->row_array();
            $rs['allow_channel_type'] = explode(",", $rs['allow_channel_type']);

            if($rs['allow_app'] != 'all'){
                $rs['allow_app'] = explode(",", $rs['allow_app']);
            }

            $this->sourceData = $rs;
        }
    }


    /**
     * 检测是否是超级管理员
     *
     * @return bool
     */
    function isSuper() {
        return ($this->username == 'admin');
    }

    public static function buildFromSession($userData) {
        return new AdminInfo($userData['uid'], $userData['username'], $userData['realname']);
    }
}