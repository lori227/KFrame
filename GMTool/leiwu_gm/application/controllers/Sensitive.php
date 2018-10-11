<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');
//敏感词类文件
class sensitive extends MY_Controller {


    public function __construct() {
        parent::__construct();//必须有，不然报错
        header("Content-type: text/html; charset=utf-8");
    }

    //修改页
    public function info(){
        $data = array();
        if($this->input->post()){
            $word_yy= htmlspecialchars(trim($this->input->post('word_yy')));
            $upAry =array(
                'content'=>$word_yy,
                'update_user' => $this->userData['uid'],
                'update_time'=>date('Y-m-d H:i:s',time()),
            );
            $this->load->model('Sensitives');
            $where['level']=3;//敏感词级别
            $res = $this->Sensitives->update($upAry,$where);
            if($res){
                $this->jsAlert('更新成功_'.$res,'/sensitive/info');
            }
        }
        //获取敏感词信息
        $this->load->model('Sensitives');
        $data['info'] = $this->Sensitives->info(3);
        $this->load->view('sensitive/info', $data);
    }
}
