<?php
//部署操作日志表
class Db_deploy_record extends MY_Model {
    
    private $_tableName = 'deploy_record';//表名

    function __construct()
    {
        parent::__construct();
    }
    //添加
    function add($infoAry){
        $res = $this->db->insert($this->_tableName, $infoAry);
        $res2 = $this->db->insert_id();
        return array($res,$res2);
    }

    //数量
    function count($where=array()){
        $res = $this->db->from($this->_tableName)->where($where)->count_all_results();
        return $res;
    }

    //列表
    function listAll($where=array()){
        $res = $this->db->from($this->_tableName)->where($where)->order_by("id desc")->get()->result_array();
        return $res;
    }

    //详情
    function detail($id){
        $res = $this->db->from($this->_tableName)->where(array('id'=>intval($id)))->get()->row_array();
        if(empty($res)){
            return false;
        }
        return $res;
    }

    //更新
    function update($id,$upAry){
        $res = $this->db->update($this->_tableName, $upAry,array('id'=>$id));
        return $res;
    }


}

/* end of file */