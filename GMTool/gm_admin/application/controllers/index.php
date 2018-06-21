<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');
//==类文件
class Index extends MY_Controller {


    public function __construct() {
        parent::__construct();//必须有，不然报错
        header("Content-type: text/html; charset=utf-8");
    }

    
    function index() {
        $data = array();
        $this->load->view('index/index', $data);
    }



}
