
<?php

if ($_FILES["filename"]["error"] > 0)
{
        $code =1;//失败
    $msg =  "error code is " . $_FILES["filename"]["error"];
}else{
          $res = move_uploaded_file($_FILES["filename"]["tmp_name"], "upload/" . $_FILES["filename"]["name"]);
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


