<?php
defined('BASEPATH') OR exit('No direct script access allowed');

class welcome extends CI_Controller {

    public function __construct() {
        parent::__construct();//必须有，不然报错

    }
	public function index()
	{
        $data = [];
		$this->load->view('index/welcome',$data);
	}
    public function test()
    {
        $data = [];
        $this->load->view('index/test',$data);
    }
    //上传测试接口
    public function upload(){
        if ($_FILES["filename"]["error"] > 0)
        {
            $code =1;//失败
            $msg =  "error code is " . $_FILES["filename"]["error"];
        }else{
            $res = move_uploaded_file($_FILES["filename"]["tmp_name"], "static/upload/version/" . $_FILES["filename"]["name"]);
            if($res){
                $code =0;//成功
                $msg = 'ok';
            }else{
                $code =2;//失败
                $msg = 'move error';
            }
        }
        $result = json_encode(array('code'=>$code,'msg'=>$msg));
        echo $result;
    }
}
