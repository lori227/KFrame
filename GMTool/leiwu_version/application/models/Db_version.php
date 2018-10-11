<?php

class Db_version extends MY_Model {
    
    private $_tableName = 'version';//表名

    function __construct()
    {
        parent::__construct();
    }
    //添加
    function add($infoAry){
        $res = $this->kf->insert($this->_tableName, $infoAry);
        return $res;
    }
    //列表
    function listAll(){
        $res = $this->kf->from($this->_tableName)->order_by("version_time")->get()->result_array();
        return $res;
    }
    //删除
    function delete($id){
        $id = trim($id);
        $res = $this->kf->delete($this->_tableName,array('version_name'=>$id));
        return $res;
    }

    //数量
    function count($where=array()){
        $res = $this->kf->from($this->_tableName)->where($where)->count_all_results();
        return $res;
    }

    //某一条
    function detail($where=array()){
        $data = $this->kf->from($this->_tableName)->where($where)->get()->row_array();
        return $data;
    }

}

/* end of file */