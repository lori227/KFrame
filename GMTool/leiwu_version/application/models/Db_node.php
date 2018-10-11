<?php

class Db_node extends MY_Model {
    
    private $_tableName = 'node';//表名

    function __construct()
    {
        parent::__construct();
    }
    //添加
    function add($infoAry){
        $res = $this->db->insert($this->_tableName, $infoAry);
        return $res;
    }
    //更新
    function update($id,$upAry){
        $id = intval($id);
        $res = $this->db->update($this->_tableName, $upAry,array('node_id'=>$id));
        return $res;
    }
    //列表
    function listAll(){
        $res = $this->db->from($this->_tableName)->get()->result_array();
        return $res;
    }

    //详情
    function detail($id){
        $id = intval($id);
        if(!$id) {
            return false;
        }
        $res = $this->db->from($this->_tableName)->where(array('node_id' => $id))->get()->row_array();
        if(empty($res)){
            return false;
        }
        return $res;
    }

    //删除
    function delete($id){
        $id = intval($id);
        $res = $this->db->delete($this->_tableName,array('node_id'=>$id));
        return $res;
    }


}

/* end of file */