<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');
//CDK礼包文件
const ACTCODE = "activationcode";
const CDK_CODE_LENGTH = 12;//CDK礼包码长度
class CdkCode extends MY_Controller {


    public function __construct() {
        parent::__construct();//必须有，不然报错
        header("Content-type: text/html; charset=utf-8");
    }
    //生成cdk码
    public function add(){
        $this->_data = array();
        if($this->input->post()){
            $id = intval(trim($this->input->post('cid')));
            $info = $this->db->from('cdk')->where('id',$id)->get()->row_array();
            if(empty($info)){
                $this->jsAlert('非法操作');
            }
            $num = intval(trim($this->input->post('num')));
            $resAll = "";
            $codeAll = "";
            $codeAry = array();
            for ($i = 0; $i < $num; $i++) {
                $this->load->helper('common');
                $cdkey = make_rand_code(CDK_CODE_LENGTH);//固定长度随机码
                $res = $this->redis->sAdd(ACTCODE , $cdkey);
                $this->redis->EXPIRE(ACTCODE, 60*60*24*365); //设置过期时间 （365 天）
                if($res){
                    $inAry = array('cdk_id'=>$info['id'],'cdkey'=>$info['cdkey'],'code'=>$cdkey);
                    $this->db->insert('cdk_code', $inAry);
                }
                $resAll .= $res;
                $codeAll .= $cdkey.'|';
                $codeAry[] = array("code"=>$cdkey);
            }
            $this->gmLog($codeAll.'--'.$resAll);//写入日志
            $this->export_csv($codeAry);
        }else{
            $data['list'] = $this->db->from('cdk')->get()->result_array();
            $this->load->view('cdkCode/add', $data);
        }
    }

    //礼包码配置
    public function config(){
        if($this->input->post()){
            $gameId = intval(trim($this->input->post('game_id')));
            $title = trim($this->input->post('title'));
            $des = trim($this->input->post('describe'));
            $time1 = trim($this->input->post('time1'));
            $time2 = trim($this->input->post('time2'));
            $frequency = intval(trim($this->input->post('frequency')));
            if(empty($title) || empty($des)){
                $this->jsAlert('名称或描述不能为空');
            }
            if(empty($time1) || empty($time2)){
                $this->jsAlert('时间不能为空');
            }
            if($time1 >= $time2){
                $this->jsAlert('开始时间不能大于结束时间');
            }
            $inAry = array(
                'game_id'=>$gameId,
                'cdkey'=>date('ymdHi'),
                'title'=>$title,
                'describe'=>$des,
                'frequency'=>$frequency,
                'start_time'=>strtotime($time1),
                'end_time'=>strtotime($time2),
                'add_user'=>$this->userData['uid'],
                'add_time'=>time(),
                'update_time'=>time(),
            );
            $res = $this->db->insert('cdk', $inAry);
            if($res){
                $this->jsAlert('添加成功','/cdkCode/index');
            }
        }
        $data=array();
        //获取游戏列表
        $this->load->model('Games');
        $data['games'] = $this->Games->listAll();
        $this->load->view('cdkCode/config', $data);
    }
    
    function index() {
        $obj = $this->db->from('cdk');
        $data['list'] = $obj->order_by("id")->get()->result_array();
        $data['users'] = $this->administrator();
        $data['frequency'] = $this->config->item('frequency', 'adconfig');
        $this->load->view('cdkCode/index', $data);
    }


    //导出csv文件
    function export_csv($data)
    {
        $string="";
        foreach ($data as $key => $value)
        {
            foreach ($value as $k => $val)
            {
                $value[$k]=iconv('utf-8','gb2312',$value[$k]);
            }

            $string .= implode(",",$value)."\n"; //用英文逗号分开
        }
        $filename = date('YmdHi').'.csv'; //设置文件名
        header("Content-type:text/csv");
        header("Content-Disposition:attachment;filename=".$filename);
        header('Cache-Control:must-revalidate,post-check=0,pre-check=0');
        header('Expires:0');
        header('Pragma:public');
        echo $string;exit;
    }
    //按量导出
    function exportNum(){
        if($this->input->post()){
            $num = intval($this->input->post('num'));
            $list = $this->redis->Srandmember(ACTCODE,$num); //多个随机元素
            $exportAry = array();
            foreach($list as $ary){
                $exportAry[] = array("key"=>$ary);
            }
            $this->export_csv($exportAry);exit;
        }
    }

    //导出全部
    function exportAll(){
        $list = $this->redis->sMembers(ACTCODE); //获取容器key中所有元素
        $exportAry = array();
        foreach($list as $ary){
            $exportAry[] = array("key"=>$ary);
        }
        $this->export_csv($exportAry);exit;
    }
    //配置页面修改
    public function modifyConfig($id){
        if($this->input->post()){
            $title = trim($this->input->post('title'));
            $des = trim($this->input->post('describe'));
            $time1 = trim($this->input->post('time1'));
            $time2 = trim($this->input->post('time2'));
            if(empty($title) || empty($des)){
                $this->jsAlert('名称或描述不能为空');
            }
            if(empty($time1) || empty($time2) || ($time1 >= $time2)){
                $this->jsAlert('时间格式不对');
            }
            $upAry = array(
                'title'=>$title,
                'describe'=>$des,
                'start_time'=>strtotime($time1),
                'end_time'=>strtotime($time2),
            );
            $res = $this->db->update('cdk', $upAry,array('id'=>$id));
            if($res){
                $this->jsAlert('修改成功','/cdkCode/index');
            }

        }
        $data = $this->db->from('cdk')->where('id',$id)->get()->row_array();
        $data['time1'] = date('Y-m-d H:i:s',$data['start_time']);
        $data['time2'] = date('Y-m-d H:i:s',$data['end_time']);
        $data['act']=2;
        $data['is_modify']=true;
        $this->load->view('cdkCode/config', $data);
    }
    //礼包码查询
    public function info(){
        $data=array();
        if($this->input->post()){
            $code = trim($this->input->post('code'));
            $this->load->helper('common');
            $code = filter_keyword($code);
            $is_use = $this->redis->hGet('cdk:'.$code,'is_use');
            $use_time = $this->redis->hGet('cdk:'.$code,'useTime');
            $user_id = $this->redis->hGet('cdk:'.$code,'userId');
            $data=array('is_use'=>$is_use,'use_time'=>$use_time,'user_id'=>$user_id);
            $info = $this->db->from('cdk_code')->where('code',$code)->get()->row_array();
            $data['giftId'] = $info['cdkey'];
        }

        $this->load->view('cdkCode/info', $data);
    }

}
