<?php

class Db_launch extends MY_Model {
    
    private $_tableName = 'launch';//表名

    function __construct()
    {
        parent::__construct();
    }
    //添加
    function add($infoAry){
        $res = $this->kf->insert($this->_tableName, $infoAry);
        return $res;
    }
    //更新
    function update($name,$type,$upAry){
        $where = array('appname'=>trim($name),'apptype'=>trim($type));
        $res = $this->kf->update($this->_tableName, $upAry,$where);
        return $res;
    }
    //数量
    function count($where=array()){
        $res = $this->kf->from($this->_tableName)->where($where)->count_all_results();
        return $res;
    }
    //列表
    function listAll($where=array()){
        $res = $this->kf->from($this->_tableName)->where($where)->get()->result_array();
        return $res;
    }

    //删除
    function delete($name,$type){
        $res = $this->kf->delete($this->_tableName,array('appname'=>trim($name),'apptype'=>trim($type)));
        return $res;
    }
    //详情
    function detail($name,$type){
        $res = $this->kf->from($this->_tableName)->where(array('appname'=>trim($name),'apptype'=>trim($type)))->get()->row_array();
        if(empty($res)){
            return false;
        }
        return $res;
    }

}

/* end of file */