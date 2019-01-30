<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class version extends MY_Controller {
    const serUrl = 'http://192.168.10.230:9999/deploy';//版本发布端口
    const UPLOAD_VERSION_DIR = 'static/upload/version/';//版本文件上传路径
    public static $commandType;
    public function __construct() {
        parent::__construct();//必须有，不然报错
        $this->load->model('db_version');
        $this->load->model('db_deploy');
        $this->load->model('db_deploy_record');
        self::$commandType = array(1=> 'shutdown',2 => 'kill',3=>'startup',4=>'launch',5=>'version',6=>'log');//服务器操作命令,重要！！！
    }

    
    function index() {
        $list = $this->db_version->listAll();
        foreach($list as &$li){
            if(!file_exists(self::UPLOAD_VERSION_DIR.$li['version_name'])){
                $li['exist'] = false;
            }
        }
        $data['list'] = $list;
        $this->load->view('admin/version/index', $data);
    }

    //添加页
    public function add(){
        if($this->input->post()){
            $inAry = array(
                'version_time'=>trim($this->input->post('version_time')),
                'version_name'=>check_str($this->input->post('version_name')),
                //'version_url'=>trim($this->input->post('version_url')),
                'version_md5'=>trim($this->input->post('version_md5')),
            );
            $cnt = $this->db_version->count(array('version_name'=>$inAry['version_name']));
            if($cnt > 0){
                $this->jsAlert('版本名不能重复');
            }
            //插入操作
            $inAry['version_url'] = 'http://'.$_SERVER['HTTP_HOST'].'/'.self::UPLOAD_VERSION_DIR.$inAry['version_name'];
            $res = $this->db_version->add($inAry);
            $dLogAry=['controller'=>__CLASS__,'action'=>__FUNCTION__,'type'=>1,'message'=>'添加人'.$this->userData['uid'].'返回值'.$res];
            $this->dbLog($dLogAry);
            if($res){
                $this->jsAlert('添加成功','/admin/version/index');
            }
        }
        $data['act']=0;
        $this->load->view('admin/version/add', $data);
    }

    //删除
    public function del($name){

        $res = $this->db_version->delete($name);
        if($res){
            @unlink('static/upload/version/'.$name);//删除对应文件（相对路径）
            $dLogAry=['controller'=>__CLASS__,'action'=>__FUNCTION__,'type'=>2];
            $this->dbLog($dLogAry);
            $logAry = array($name,$res,$this->userData['uid']);
            lwLog($logAry,__METHOD__);
            $this->jsAlert('删除成功_'.$res);
        }
    }
//===========================以下是部署系统========================================
    //服务器部署页-列表
    public function act_ver(){
        $data['user'] = $this->administrator();
        $list = $this->db_deploy_record->listAll();
        $data['list'] = $list;
        $data['commandType'] = self::$commandType;
        $this->load->view('admin/version/act_ver', $data);
    }

    //部署-1
    public function act_up(){
        $where = [];
        $data['channel'] = $this->db_deploy->listAll($where,'appchannel','appchannel');
        foreach($data['channel'] as &$g){
            $where2 = array('appchannel'=>$g['appchannel']);
            $g['name'] = $this->db_deploy->listAll($where2,'appname','appname');//var_dump($g['name']);
            foreach($g['name'] as &$i){
                $where3 = array('appname'=>$i['appname']);
                $i['list'] = $this->db_deploy->listAll($where3);

            }
        }
        //$data['tips'] = $this->kf->hostname;
        $this->load->view('admin/version/act_up', $data);
    }
    //第一页提交
    public function submit_up1(){
        if($this->input->post()){
            $ips = check_str($this->input->post('ips'));//ip集合
            $remark = trim($this->input->post('remark'));
            if(empty($ips)){
                $this->jsAlert('IP不能为空');
            }
            $ipAry = explode('*',$ips);//字符串转数组
            $inAry = array(
                'ips'=>json_encode($ipAry),
                'step'=>1,
                'remark'=>$remark,
                'add_user'=>$this->userData['uid'],
                'add_time'=>time(),
            );
            $res = $this->db_deploy_record->add($inAry);
            lwLog([$inAry,$res],__METHOD__);
            if($res[0]){
                $this->jsAlert('添加成功_'.$res[0],'/admin/version/act_up2/'.$res[1]);
            }
        }
    }
    //部署-2
    public function act_up2($id){
        $info = $this->check_id($id);
        if($this->input->post()){
            $command = intval($this->input->post('command'));
            switch($command){
                case 1:
                    $val = 60000;//单位毫秒
                    break;
                case 2:
                case 3:
                case 4:
                    $val = "";
                    break;
                case 5:
                    $val = check_str($this->input->post('version'));
                    break;
                case 6:
                    $val = check_id($this->input->post('log_lev'));
                    break;
            }
            $upAry = array(
                'command'=>$command,
                'value'=>$val,
                'step'=>2,
                'op_user'=>$this->userData['uid'],
                'op_time'=>time(),
            );
            $res = $this->db_deploy_record->update($id,$upAry);
            if($res){
                $this->jsAlert('提交成功_'.$res,'/admin/version/act_up3/'.$id);
            }
        }else{
            $ips_ary = json_decode($info['ips'],true);
            $data['list'] = $this->db_deploy->list_in('appid',$ips_ary);
            $data['ver'] = $this->db_version->listAll();//版本列表
            $data['step']=2;
            $data['info']=$info;
            $this->load->view('admin/version/act_up2', $data);
        }
    }

    //部署-3
    public function act_up3($id){
        $info = $this->check_id($id);
        if($this->input->post()){
            $act_time = check_str($this->input->post('act_time'));
            $schedule = strtotime($act_time);//日期转时间戳
            $ipAry = json_decode($info['ips'],true);
            if(!empty($ipAry)){
                $callback = "http://".$_SERVER['HTTP_HOST']."/admin/version/ver_callback";//回调
                foreach($ipAry as $ip){
                    $deploy = $this->db_deploy->detail($ip);
                    if(!empty($deploy)){
                        $dAry = array(
                            'command'=>self::$commandType[$info['command']],
                            'value'=>$info['value'],
                            'appchannel'=>$deploy['appchannel'],
                            'appname'=>$deploy['appname'],
                            'apptype'=>$deploy['apptype'],
                            'appid'=>$deploy['appid'],
                            'zoneid'=>$deploy['zoneid'],
                            'callback'=>$callback,
                            'scheduletime'=>$schedule,
                        );
                        $res = $this->serverCurl($dAry);
                        lwLog([$dAry,$res],__METHOD__);
                        $result[] = $res;
                    }
                }
                $upAry = array(
                    'step'=>3,
                    'scheduletime'=>$schedule,
                    'op_user'=>$this->userData['uid'],
                    'op_time'=>time(),
                );
                $this->db_deploy_record->update($id,$upAry);
                $this->jsAlert('执行结束_'.json_encode($result),'/admin/version/act_ver');
            }
        }else{

            $data['step']=3;
            $this->load->view('admin/version/act_up3', $data);
        }
    }

    private function check_id($id){
        $id = intval($id);
        $info = $this->db_deploy_record->detail($id);
        if(empty($info)){
            $this->jsAlert('访问的页面不存在','/admin/version/act_up');exit;
        }else{
            return $info;
        }
    }

    //服务器端通信
    public function serverCurl($valAry){
        $url = self::serUrl;//服务器端接口
        $con = json_encode($valAry);
        $cres = curlPostRequest($url,$con);
        $rAry = json_decode($cres,true);
        return $rAry['retcode'];
    }

    //服务器回调接口
    public function ver_callback(){
        //$msg = $_POST;
        lwLog($_POST,__METHOD__);
    }
}
