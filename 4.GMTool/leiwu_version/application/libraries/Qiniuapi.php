<?php
use Qiniu\Qiniu;
class Qiniuapi{
    private $qiniuObj = null;
    function __construct()
    {
        $accessKey = 'UxfwgmFHQn_pfye98S1aMMneGJKjfqrXuP1X8tb1';
        $secretKey = 'bZr7puMv_G3opyNA7PtxyMsPSbkWpxvPHPIY_XbB';
        require dirname(__DIR__) . '/third_party/Qiniu/Qiniu.php';

        $this->qiniuObj = new Qiniu($accessKey, $secretKey);
    }
    //上传至七牛云($fileName1：来自input里name属性)
    public function upload($fileName1=""){
        $fileName2 = date('ymdHis').'-'.mt_rand(100,999).'.png';//$fileName2:七牛云上名字
        $file = $this->qiniuObj->file(QINIU_DEFAULT_BUCKET,$fileName2);
        $response = $file->put($_FILES[$fileName1]['tmp_name']);
        return $response;
    }

    //删除
    public function delete($fileName){
        $file = $this->qiniuObj->file(QINIU_DEFAULT_BUCKET,$fileName);
        $response = $file->delete();
        return $response;
    }
}