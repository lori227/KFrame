<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class Statist extends MY_Controller {

    public function __construct() {
        parent::__construct();//必须有
        $this->load->model('db_log');
    }

    //日志列表
    function log() {
        $page = intval($this->input->get('per_page'));//第几页 系统参数
        $queryParams = array(
            'searchType' => trim($this->input->get('searchType')),
            'date1' => trim($this->input->get('date1')),
        );
        $data = $this->db_log->listAll($queryParams,$page);


        $this->load->view('admin/statist/log', $data);
    }
}
