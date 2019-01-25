<?php

class Whites extends MY_Model {
    
    private $_tableName = 'white';

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
    function listAll(){
        $res = $this->db->from($this->_tableName)->order_by("id desc")->get()->result_array();
        return $res;
    }

    //删除
    function delete($where){
        $res = $this->db->delete($this->_tableName, $where);
        return $res;
    }
    //详情页
    function info($where){
        $res = $this->db->from($this->_tableName)->where($where)->get()->row_array();
        return $res;
    }
    //修改
    function update($upAry,$where){
        $res = $this->db->update($this->_tableName, $upAry,$where);
        return $res;
    }


}

/* end of file */