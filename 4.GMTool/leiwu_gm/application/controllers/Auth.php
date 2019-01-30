<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');
/**
* 权限控制管理
*/
class Auth extends MY_Controller {
    
    function index() {
        $list = $this->db->from('user')->where(array('username !='=>'admin'))->get()->result_array();
        $this->load->view('auth/index', array('list'=>$list));
    }
    
    //添加管理员
    function add(){
        $act="";
        if ($this->input->post()) {
            $username = trim($this->input->post('username'));
            $password = trim($this->input->post('password'));
            $realname = trim($this->input->post('realname'));
            
            $data = array(
                    'username'      => $username,
                    'password'      => md5($password),
                    'realname'      => $realname
                  );
            
            $data = array_filter($data);
            if(count($data) < 3){
                die('缺少参数');
            }
            
            $this->db->insert('user', $data);
            $act = 'success';
        }
        
        $tmpData = array(
                    'username'      => '',
                    'password'      => '',
                    'realname'      => ''
                  );
        $this->load->view('auth/add', array('data'=>$tmpData,'actName'=>'添加管理员','act'=>$act));
    }

    //添加权限节点
    function addAuth(){
        if($this->input->post()){
        
            $fid        = intval($this->input->post('fid'));
            $istree     = intval($this->input->post('istree'));
            $name       = trim($this->input->post('name'));
            $class      = trim($this->input->post('class'));
            $method     = trim($this->input->post('method'));
            
            if(!$name){
                die('请输入结点名称');
            }
            
            $data = array(
                    'fid'           => $fid,
                    'istree'        => $istree,
                    'name'          => $name,
                    'class'         => $class,
                    'method'        => $method,
                    'add_user'      => $this->userData['realname'],
                    'add_time'      => time()
                  );

            $this->db->insert('auth', $data);
            $result['act'] = 'success';
        }
        
        $node = $this->db->select('id,fid,name')->from('auth')->where(array('fid'=>0))->get()->result_array();

        $tmpData = array(
                    'fid'           => 0,
                    'name'          => '',
                    'class'         => '',
                    'method'        => '',
                  );
        $result['data']    = $tmpData;
        $result['node']    = $node;
        $result['actName'] = '添加权限结点';
        
        $this->load->view('auth/addAuth', $result);
    }

    //编辑管理员密码、权限节点（渠道类型）
    function edit($act, $uid){
        if(!$uid){
            die('无效UID');
        }

        $data['uid'] = (int)$uid;

        if($act == 'pass'){
            $data = $this->_editUserPass($uid);
        }else{
            $data['act']     = $this->_editUserAuth($uid);
            $data['node']    = $this->_getNodeList($uid);
        }
        $data['data']= $this->db->from('user')->where(array('id' => $uid))->get()->row_array();
        //$this->_data['app'] = $this->getApps();
        //app权限
    /*    if($this->_data['data']['allow_app'] == 'all'){
            $this->_data['data']['allow_app'] = '';
            foreach ($this->_data['app'] as $k => $v) {
                $this->_data['data']['allow_app'] .= $k.',';
            }
            $this->_data['data']['allow_app'] = rtrim($this->_data['data']['allow_app'],',');
        }*/
        $this->load->view('auth/'.$act, $data);
    }
    
    
    //更改用户状态
    function actUser($id = 0, $act = 0){
        if(!$id){
            die('无效ID');
        }
        $flg = $this->db->update('user', array('status' => intval($act)), array('id' => $id));
        echo $flg;
    }
    
    //管理员修改用户密码
    private function _editUserPass($uid){
        if(!$uid){
            die('无效UID');
        }
        $result=[];
        if($this->input->post()){
            $password = trim($this->input->post('password'));
            if(!$password){
                die('新密码无效');
            }
            $data = array(
                'password'  => md5($password)
            );
            $this->db->update('user', $data, array('id' => $uid));
            $result['act'] = 'success';
        }
        
        return $result;
    }
    
    //获取权限节点
    private function _getNodeList(){
        
        $tmpResult = array();
    
        $tmpNode = $this->db->select('id,fid,name')->from('auth')->get()->result_array();
        foreach($tmpNode as $v){
            if($v['fid'] == 0){
                $tmpResult[$v['id']] = $v;
            }else{
                $tmpResult[$v['fid']]['node'][$v['id']] = $v;
            }
        }
        return $tmpResult;
    }

    //修改用户权限
    private function _editUserAuth($uid){
        if(!$uid){
            die('无效UID');
        }
        if($this->input->isPost()){
            $authids = $this->input->post('authids');
            $data = array(
                'auth'  => json_encode($authids)
            );
            $allowApp = $this->input->post('app');
            if ($allowApp){
            	$data['allow_app'] = implode(",", $allowApp);
            }
            $this->db->update('user', $data, array('id' => $uid));

            return 'success';
        }
        return '修改失败';
    }

    //获取渠道类型
    function getChannelType(){
        $rst = $this->db->select('id,name')->from('channel_type')->order_by("id")->get()->result_array();

        $channelType = array();
        foreach($rst as $v){
            $channelType[$v['id']] = $v['name'];
        }

        return $channelType;
    }

    //获取应用
    function getApps(){
        $rst = $this->db->select('id,name')->from('app')->order_by("id")->get()->result_array();

        $apps = array();
        foreach($rst as $v){
            $apps[$v['id']] = $v['name'];
        }

        return $apps;
    }

}
/* end */