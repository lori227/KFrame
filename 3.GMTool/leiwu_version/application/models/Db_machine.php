<?php

class Db_machine extends MY_Model {
    
    private $_tableName = 'machine';//表名

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
        $res = $this->kf->from($this->_tableName)->get()->result_array();
        return $res;
    }
    //删除
    function delete($id){
        $id = $id;
        $res = $this->kf->delete($this->_tableName,array('localip'=>$id));
        return $res;
    }
    //详情
    function detail($id){
        if(!$id) {
            return false;
        }
        $res = $this->kf->from($this->_tableName)->where(array('localip' => $id))->get()->row_array();
        if(empty($res)){
            return false;
        }
        return $res;
    }
    //更新
    function update($id,$upAry){
        $res = $this->kf->update($this->_tableName, $upAry,array('localip'=>$id));
        return $res;
    }
}

/* end of file */