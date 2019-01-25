<?php

class User extends MY_Model {
    
    private $_tableName = 'user';
    
    function __construct()
    {
        parent::__construct();

    }



    function login($username, $password){

        if(!$username || !$password) {
            return false;
        }

        $user = $this->db->from($this->_tableName)->where(array('username' => $username, 'status' => 1))->get()->row_array();

        if(empty($user)){
            return false;
        }

        if($user['password'] === md5($password)){
            $userData = array(
                    'uid'               => $user['id'],
                    'username'          => $user['username'],
                    'realname'          => $user['realname'],
                    'last_login_ip'     => $user['last_login_ip'],
                    'last_login_time'   => $user['last_login_time'],
                    
            );
            
            $this->session->set_userdata('user_data', $userData);
            $this->db->update($this->_tableName, array('last_login_ip'=>$this->input->ip_address(), 'last_login_time'=>time()), array('id'=>$user['id']));

            return true;
        }else{
            return false;
        }
    }

    function getAuth($uid){

        if(!$uid) {
            return false;
        }

        $tmpAuth = $this->db->select('auth')->from($this->_tableName)->where(array('id' => $uid))->get()->row_array();

        if(empty($tmpAuth)){
            return false;
        }
        
        return json_decode($tmpAuth['auth']);
    }
    
    /**
     * 获得所有拥抱
     * by worm
     */
    function getAllUser(){
        return  $this->db->from($this->_tableName)->where(array('status' => 1))->get()->result_array();
    }
}

/* end of file */