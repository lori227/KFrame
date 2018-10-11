<?php

class Db_log extends MY_Model {
    
    private $_tableName = 'log';//表名

    function __construct()
    {
        parent::__construct();
    }
    //添加
    function add($infoAry){
        $res = $this->db->insert($this->_tableName, $infoAry);
        return $res;
    }
    //列表
    function listAll($parAry,$page){
        $where = [];
        $obj = $this->db->from($this->_tableName);
        $data['nums'] = paging($obj,$page,'/admin/statist/log?'.http_build_query($parAry));//分页处理必须在列表上面
        $data['list'] = $this->db->from($this->_tableName)->where($where)->order_by("log_id desc")->get()->result_array();
        return $data;
    }


}

/* end of file */