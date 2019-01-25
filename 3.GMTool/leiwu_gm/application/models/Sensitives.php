<?php

class Sensitives extends MY_Model {
    
    private $_tableName = 'sensitive';

    function __construct()
    {
        parent::__construct();
    }

    function update($upAry,$where){
        $res = $this->db->update($this->_tableName, $upAry,$where);
        return $res;
    }

    function info($level){
        $data = $this->db->from($this->_tableName)->where('level',$level)->get()->row_array();
        return $data;
    }

}

/* end of file */