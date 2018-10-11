<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class MY_Controller extends CI_Controller {

    protected $_data    = '';
    protected $userData = array();

	/**
	 *@info bug函数
	 *@author: hj
	 *@date: 2015-01-24
	 *@param  array/string $msg 需要调试的内容
	 *@return 返回调试结构
	 * */
/*	public function debug($msg){
        var_dump($this->session->userdata('xue'));
	}*/


    public function __construct() {
        parent::__construct();
        $this->load->helper('common');
        $this->load->helper('system');//常用方法 xue
        $this->load->helper('url');
        $this->config->load('adconfig', TRUE);

        //$this->session->all_userdata() ci框架3.X版本session写法换了，注意注意注意！！！
        $uInfo = $this->session->userdata('user_data');
        if(isset($uInfo)){
            $this->userData = $this->session->userdata('user_data');
            $this->admin    = Admin::buildFromSession($this->userData);
            /**
            *   权限控制
            *   admin 用户不参与权限控制
            *   未加入权限控制结点表里的功能点未参与权限控制
            *   权限错误信息显示:
            *        方法：system/core/common/showError('msg');
            *        位置：views/footer.php;
            */
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
            $resVer = $this->_formatTree($tmpTree, $tmpUserAuth);//父级菜单id必须小于子级id，菜单才能显示
            $this->load->vars('tree',$resVer );
        }else{
            $tmpUri = $this->router->class . '/' . $this->router->method;
            if($tmpUri != 'welcome/index' && $tmpUri != 'welcome/login'){
                redirect('/welcome');
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
                $className = $v['class'];
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

    private function _formatToOneArr($data){
        $tmpData = array();
        foreach($data as $v){
            $tmpData[$v['id']] = $v['name'];
        }
        return $tmpData;
    }

    //本项目日志 xue
    function gmLog($logCon,$type='gm'){
        //var_dump(debug_print_backtrace());
        $this->load->helper('common');
        $log_con = '['.date('Y-m-d H:i:s').']，'.get_real_ip().'，path:'.$type.'，content:'.json_encode($logCon).PHP_EOL;
        if(file_put_contents('gm_admin.log', $log_con,FILE_APPEND)){
            //echo "写入成功。<br />";
        }
    }

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
}



class Admin {

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
        return new Admin($userData['uid'], $userData['username'], $userData['realname']);
    }
}