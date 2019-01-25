<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class MY_Model extends CI_Model {

    protected $db = false ;
    protected $ad = false;
    public function __construct(){
        parent::__construct();
        //引入databases里的$active_group xue
        $this->db = $this->load->database('default', TRUE);
        //$this->ad  = $this->load->database('ad', TRUE);//坑死人的ad库
    }
}