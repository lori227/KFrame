<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class MY_Model extends CI_Model {

    protected $db = false ;
    protected $ad = false;
    public function __construct(){
        parent::__construct();
        $this->db = $this->load->database('default', TRUE);
        $this->load->helper('system');//常用方法 xue
    }
}