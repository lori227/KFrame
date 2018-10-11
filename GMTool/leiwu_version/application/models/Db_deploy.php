<?php

class Db_deploy extends MY_Model {
    
    private $_tableName = 'deploy';//表名

    function __construct()
    {
        parent::__construct();
    }
    //添加
    function add($infoAry){
        $res = $this->kf->insert_id($this->_tableName, $infoAry);
        return $res;
    }
    //更新
    function update($id,$upAry){
        //$id = intval($id);
        $res = $this->kf->update($this->_tableName, $upAry,array('appid'=>$id));
        return $res;
    }
    //列表
    function listAll($where,$field="*",$group=""){
         $obj = $this->kf->select($field)->from($this->_tableName)->where($where);
         if(!empty($group)){
            $obj->group_by($group);
         }
         $res = $obj->get()->result_array();
        return $res;
    }
    //列表-where in
    function list_in($field,$value){
        $obj = $this->kf->select('*')->from($this->_tableName)->where_in($field,$value);
        $res = $obj->get()->result_array();
        return $res;
    }
    //详情
    function detail($id){
        if(!$id) {
            return false;
        }
        $res = $this->kf->from($this->_tableName)->where(array('appid' => $id))->get()->row_array();
        if(empty($res)){
            return false;
        }
        return $res;
    }
    //删除
    function delete($id){
        $id = intval($id);
        $res = $this->kf->delete($this->_tableName,array('appid'=>$id));
        return $res;
    }

    //数量
    function count($where=array()){
        $res = $this->kf->from($this->_tableName)->where($where)->count_all_results();
        return $res;
    }


}

/* end of file */