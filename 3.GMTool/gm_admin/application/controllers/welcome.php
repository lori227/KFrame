<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class Welcome extends MY_Controller {


    function __construct() {
        parent::__construct();
    }

    public function index(){
        if ($this->isLogin()) {
            redirect('/main');
        } else {
            if ($this->input->is_post()) {
                $username = trim($this->input->post('username'));
                $password = trim($this->input->post('password'));
                $this->load->model('user');
                $user = $this->user->login($username, $password);
                if($user){
                    redirect('/main');
                }else{
                    redirect('/welcome');
                }
            } else {
                $this->load->view('welcome/login');
            }
        }
    }
    
    public function editPass(){
    
        $this->_data['data'] = $this->db->from('user')->where(array('id' => $this->userData['uid']))->get()->row_array();
        
        if($this->input->is_post()){
            $oldpass = trim($this->input->post('oldpass'));
            $password = trim($this->input->post('password'));
            
            if(!$password){
                die('0 0');
            }
            
            if(md5($oldpass) !== $this->_data['data']['password']){
                $this->_data['act'] = '原密码不正确！';
            }else{
                $data = array(
                        'password' => md5($password)
                );
                $this->db->update('user', $data, array('id' => $this->userData['uid']));

                $this->_data['act'] = 'success';
            }
        }
        
        $this->load->view('welcome/editPass', $this->_data);
    }
    
    public function main(){
        $this->load->view('welcome/main', array('user'=>$this->userData));
    }

    public function logout() {
        $this->session->unset_userdata('user_data');
        redirect('/welcome');
    }
}
/* End of file welcome.php */
/* Location: ./application/controllers/welcome.php */
