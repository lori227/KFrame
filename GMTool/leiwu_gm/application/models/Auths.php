<?php

class Auths extends MY_Model {
    
    private $_tableName = 'auth';
    
    function __construct()
    {
        parent::__construct();

    }

    function getNode($class, $method){
        if(!$class || !$method) {
            return false;
        }
        
        $tmpNode = $this->db->select('id')->from($this->_tableName)->where(array('class' => $class, 'method' => $method))->get()->row_array();

        if(empty($tmpNode)){
            return true;
        }
        
        return $tmpNode['id'];
    }
    //添加游戏
    function add($infoAry){

        $res = $this->db->insert($this->_tableName, $infoAry);

        return $res;
    }


}

/* end of file */