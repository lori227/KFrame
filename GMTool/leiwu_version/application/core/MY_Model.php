<?php
//namespace model;
if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class MY_Model extends CI_Model {

    protected $db = false ;
    protected $ad = false;
    public function __construct(){
        parent::__construct();
        $this->db = $this->load->database('default', TRUE);
        $this->kf = $this->load->database('kf', TRUE);//kframe库
        $this->load->helper('common');//自己封装常用方法
        //var_dump($this->kf->last_query());
    }
}