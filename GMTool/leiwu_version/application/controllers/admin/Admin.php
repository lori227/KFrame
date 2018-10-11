<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class Admin extends MY_Controller {


    function __construct() {
        parent::__construct();
    }
    //后台入口
    public function index(){
        if ($this->isLogin()) {
            redirect('admin/admin/main');
        } else {
            if ($this->input->post()) {
                $username = trim($this->input->post('username'));
                $password = trim($this->input->post('password'));
                $this->load->model('user');
                $user = $this->user->login($username, $password);
                if($user){
                    redirect('admin/admin/main');//验证通过
                }else{
                    redirect('admin');//登录失败
                }
            } else {
                $this->load->view('admin/index/login');
            }
        }
    }
    //修改密码页
    public function editPass(){

        $info['data'] = $this->db->from('user')->where(array('id' => $this->userData['uid']))->get()->row_array();
        
        if($this->input->post()){
            $oldpass = trim($this->input->post('oldpass'));
            $password = trim($this->input->post('password'));
            
            if(!$password){
                die('0 0');
            }
            
            if(md5($oldpass) !== $info['data']['password']){
                $info['act'] = '原密码不正确！';
            }else{
                $data = array(
                        'password' => md5($password)
                );
                $this->db->update('user', $data, array('id' => $this->userData['uid']));

                $info['act'] = 'success';
            }
        }
        
        $this->load->view('admin/index/editPass', $info);
    }
    
    public function main(){
        $this->load->view('admin/index/main', array('user'=>$this->userData));
    }

    public function logout() {
        $this->session->unset_userdata('user_data');
        redirect('admin');
    }
    //首页banner
    public function banner(){
        $res['list'] = $this->db->from('index_banner')->get()->result_array();
        $this->load->view('admin/index/banner', $res);
    }
//首页banner
    public function add_banner(){
        if ($this->input->post()) {
            $url = trim($this->input->post('url'));
            $file = $this->upQiniu('img');//上传图片

            $inAry=array(
                'url'=>$url,
                'img'=>$file,
                'add_user'=>$this->userData['uid'],
                'add_time'=>date('Y-m-d H:i:s',time())
            );
            $res = $this->db->insert('index_banner', $inAry);
            if($res){
                $this->jsAlert('添加成功','/admin/admin/banner');
            }
        }
        $data['act']=0;
        $this->load->view('admin/index/add_banner', $data);
    }
//首页banner
    public function del_banner($id){
        $info = $this->db->from('index_banner')->where(array('id' => $id))->get()->row_array();
        //删除云存储图片
        if(isset($info['img'])){
            $this->load->library('qiniuapi');
            $del_res = $this->qiniuapi->delete($info['img']);
        }
        $res = $this->db->delete('index_banner',array('id'=>$id));
        if($res){
            $this->jsAlert('删除成功_'.$res,'/admin/admin/banner');
        }
    }
}
/* End of file welcome.php */
/* Location: ./application/controllers/welcome.php */
