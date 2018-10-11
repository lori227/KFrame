<?php

class Games extends MY_Model {
    
    private $_tableName = 'game';

    function __construct()
    {
        parent::__construct();
    }
    //添加游戏
    function add($infoAry){

        $res = $this->db->insert($this->_tableName, $infoAry);

        return $res;
    }
    //游戏列表
    function listAll(){
        $res = $this->db->from($this->_tableName)->order_by("id")->get()->result_array();
        return $res;
    }


}

/* end of file */