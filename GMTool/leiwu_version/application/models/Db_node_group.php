<?php

class Db_node_group extends MY_Model {
    
    private $_tableName = 'node_group';//表名

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
        $res = $this->db->update($this->_tableName, $upAry,array('group_id'=>$id));
        return $res;
    }
    //列表
    function listAll(){
        $res = $this->db->from($this->_tableName)->where(array('is_delete'=>0))->get()->result_array();
        return $res;
    }
    //详情
    function detail($id){
        $id = intval($id);
        if(!$id) {
            return false;
        }
        $res = $this->db->from($this->_tableName)->where(array('group_id' => $id))->get()->row_array();
        if(empty($res)){
            return false;
        }
        return $res;
    }
    //删除
    function delete($id){
        $id = intval($id);
        $upAry=array('is_delete'=>1);//假删除
        $res = $this->db->update($this->_tableName, $upAry,array('group_id'=>$id));
        return $res;
    }

}

/* end of file */