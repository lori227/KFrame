<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');
//通知邮件类
class Mail extends MY_Controller {

    const USER_MAIL_LIST = "gm_user_email_list";
    function __construct() {
        parent::__construct();
    }
    function index($page=1){
       $queryParams = array(
            'email_id'   => intval($this->input->get('email_id')),
            'emailType' => intval($this->input->get('emailType')),
            'email_title' => trim($this->input->get('email_title')),
        );

        $where = array();
        $where['is_delete'] = 0;//未删除邮件
        if($queryParams['email_id']){
            $where['email_id'] = $queryParams['email_id'];
        }
        if($queryParams['emailType']){
            $where['type'] = $queryParams['emailType'];
        }
        if($queryParams['email_title']){
            $where['title'] = $queryParams['email_title'];
        }
        $obj = $this->db->from('mail');
        $obj->where($where);
        paging($obj,$page,'/mail/index');//分页函数
        $list = $obj->order_by('id desc')->get()->result_array();
        $data['list'] = $list;
        $data['emailType'] = $this->config->item('emailType', 'adconfig');

        $data['queryParams'] = $queryParams;

        $data['users'] = $this->administrator();
        $this->load->view('mail/index', $data);
    }

    //添加通知邮件
    public function add(){
        //获取用户列表
        $userIdStr = $this->redis->get(self::USER_MAIL_LIST);
        $data['redisEmail'] =array();
        if($userIdStr){
            $data['redisEmail'] = explode(',',$userIdStr);//获取用户列表
        }
        //获取游戏列表
        $this->load->model('Games');
        $data['games'] = $this->Games->listAll();
        if($this->input->post()){
            $gameId = trim($this->input->post('game_id'));
            $type = intval($this->input->post('type'));//邮件类型，1.gm邮件；2.用户补偿邮件
            $title = $this->filterPer($this->input->post('title'));
            $content = $this->filterPer($this->input->post('content'));
            $days = trim($this->input->post('expire'));//邮件过期时间点
            $remark = trim($this->input->post('remark'));//邮件附件
            $sendername = $this->filterPer($this->input->post('sendername'));
            $configId = trim($this->input->post('configId'));

            $toolGive = intval($this->input->post('toolGive'));//道具发放checkbox

            if(empty($title) || empty($content)){
                $this->jsAlert('标题或内容不能为空');
            }
            if(empty($sendername)){
                $this->jsAlert('发送者姓名不能为空');
            }
            //道具附件码
            if($type=="2" && $toolGive >0){
                if(empty($remark)){
                    $this->jsAlert('道具附件码不能为空');
                }
            }

            $now = time();//当前时间戳
            $sendTime = strtotime($days);
            $expireSecond = $sendTime - $now + 3600*24*30;//邮件过期时间秒数(发送后加30天)
            $insert=array(
                'type'=>$type,//GM后台发的邮件type都是1
                'title'=>$title,
                'content'=>$content,
                'validtime'=>$expireSecond,//共多少秒
                'reward'=>$remark,
                'sendername'=>$sendername,
                'flag'=>0,//新的邮件状态肯定为未读
                'sendtime'=>$sendTime,//邮件发送数时间戳
            );
            //插入mysql
            $insert['game_id'] = $gameId;
            $insert['status'] = 0;//未审核
            $insert['add_time'] = $now;
            $insert['add_user'] = $this->userData['uid'];
            if($type == 2){
                if(count($data['redisEmail']) <= 0){
                    $this->jsAlert('用户ID不能为空');
                }
                $insert['playerIdStr'] = implode(',',$data['redisEmail']);//用户邮件的用户ID列表
            }
            $res= $this->db->insert('mail', $insert); //插入mysql
            $this->redis->del(self::USER_MAIL_LIST);
            if($res){
                $this->jsAlert('添加成功','/mail/index');
            }
        }
        //获取道具配置
        $toolInfo = $this->getToolList();
        $data = array_merge($data,$toolInfo);
        $this->load->view('mail/add', $data);
    }
    //过滤百分号
    private function filterPer($var){
        return str_replace( "%", "", trim($var)); // 把 '%'过滤掉
    }
    //修改页
    public function modify($eid){
        $id = intval($eid);
        $data = $this->db->from('mail')->where('id',$id)->get()->row_array();
        if($data['status']==2){
            $this->jsAlert('此邮件不可修改');
        }
        $data['emailType'] = $this->config->item('emailType', 'adconfig');
        $data['redisEmail'] = explode(',',$data['playerIdStr']);//获取用户列表

        if($this->input->post()){
            $title = trim($this->input->post('title'));
            $content = trim($this->input->post('content'));
            //$remark = trim($this->input->post('remark'));//邮件附件
            if(empty($title) || empty($content)){
                $this->jsAlert('标题或内容不能为空');
            }
            $userJson = $this->getPlayerId();
            $upAry =array(
                'title'=>$title,
                'content'=>$content,
                'playerIdStr'=>$userJson,
                'update_time'=>time(),
            );
            $res = $this->db->update('mail', $upAry,array('id'=>$id));
            if($res){
                $this->jsAlert('更新成功_'.$res,'/mail/index');
            }
        }
        $data['actNmae'] = '修改';
        $this->load->view('mail/modify', $data);
    }

    //审核页
    public function check($eid){
        $id = intval($eid);
        $data = $this->db->from('mail')->where('id',$id)->get()->row_array();
        $data['emailType'] = $this->config->item('emailType', 'adconfig');
        $data['redisEmail'] = explode(',',$data['playerIdStr']);//获取用户列表
        $data['actNmae'] = '审核';
        $data['users'] = $this->administrator();
        $this->load->view('mail/check', $data);
    }

    //邮件id
    private  function getId(){
        $rid = $this->redis->incr('mailidcreater');
        return $rid;
    }

    //发邮件
    public function send(){
        $eid = intval(trim($this->input->post('eid')));
        if($eid == 0 || !is_numeric($eid)){
            $this->jsAlert('操作有误');
        }
        $this->load->model('server');
        $res = $this->server->sendEmail($eid);
        if($res){
            $upAry['status'] = 2;//邮件状态（0:未1:已审核2:已发送3:审核未通过4:已驳回）
            $upAry['check_user'] = $this->userData['uid'];
            $upAry['check_time'] = time();
            $res = $this->db->update('mail', $upAry,array('id'=>$eid));
            gmLog([$upAry,$res],__METHOD__);
            $this->jsAlert('发送成功_'.$res,'/mail/index');
        }else{
            $this->jsAlert('发送失败_'.$res);
        }
    }

    //邮件驳回
    public function eReturn(){
        $eid = intval(trim($this->input->post('eid')));
        if($eid == 0 || !is_numeric($eid)){
            $this->jsAlert('id操作有误');
        }
        $this->load->model('server');
        $res = $this->server->turnMail($eid);
        if($res){
            $upAry['status'] = 4;//邮件状态（0:未1:已审核2:已发送3:审核未通过4:已驳回）
            $upAry['check_user'] = $this->userData['uid'];
            $upAry['check_time'] = time();
            $this->db->update('mail', $upAry,array('id'=>$eid));
            $this->gmLog($upAry,__METHOD__);
            $this->jsAlert('驳回成功_'.$res,'/mail/index');
        }else{
            $this->jsAlert('驳回失败_'.$res);
        }
    }

    //上传文件==运营改成text-area上传，此方法废弃
/*    public function upload(){
        $config['upload_path'] = './static/uploads/emailUser';
        $config['allowed_types'] = 'txt';
        $config['max_size'] = 200;//最大大小
        $this->load->library('upload', $config);

        if (!$this->upload->do_upload('userfile'))
        {
            $error = array('error' => $this->upload->display_errors());
            die($error['error']);
        }else{
            $info = $this->upload->data();
            if(count($info)>0){
                $fileContent = file_get_contents($info['full_path']);
                $fileContent = str_replace("\r\n","<br/>",$fileContent);
                $userAry = explode('<br/>',$fileContent);//字符串转数组
                $userAry = array_unique($userAry);//数组去重
                $userAry = array_filter($userAry);//数组去空
                $userJson = json_encode($userAry);
                $redCont = $this->redis->set(self::USER_MAIL_LIST,$userJson);
                unlink($info['full_path']);//删除
                $this->jsAlert('上传成功_'.$redCont);
            }
        }
    }*/

    //上传玩家id
    public function addPlayerId(){
        $userJson = $this->getPlayerId();
        $redCont = $this->redis->set(self::USER_MAIL_LIST,$userJson);
        $this->jsAlert('上传成功_'.$redCont,'/mail/add');
    }

    //获取玩家列表
    private function getPlayerId(){
        $content = trim($this->input->post('playerList'));
        $content = filter_keyword($content);//过滤-防止sql注入
        $userAry = explode("\r\n",$content);//字符串转数组
        $userAry = array_unique($userAry);//数组去重
        $userAll = array_filter($userAry);//数组去空
        if(count($userAll)>0){
            foreach($userAll as &$ary){
                if(!is_numeric($ary)){
                    $this->jsAlert('玩家列表：'.$ary.'为非法字符');
                }
                $ary=intval($ary);
            }
            $userJson = implode(',',$userAll);//数组转字符串
            return $userJson;
        }
        return false;
    }

    //ajax获取道具列表
    public function ajaxTool(){
        $data = $this->getToolList();
        $result = array_merge($data['clothes'],$data['item']);
        echo json_encode($result);
    }

    //ajax获取道具生成码
    public function getToolCode(){
        $content = trim($this->input->get('par'));
        $dAry = explode('|',$content);
        $fjAry=array();

        foreach($dAry as $key=> $one){
            $eAry = array_filter(explode(',',$one));
            if(count($eAry)<3){
                $this->jsResponse(1,'第'.$key.'组部分参数项不能为空');
            }

            $tid = $eAry[0];//二级道具id
            $tnum = $eAry[1];//二级道具数量
            $tDays = intval($eAry[2]);//道具时长(天)

            if($tnum <= 0 || $tDays<=0){$this->jsResponse(1,'第'.$key.'组数量或时长形式错误');}
            $lev1 = getToolLev1($tid);//一级道具名
            $tSecond = (string)(3600*24*$tDays);//天-》秒
            $fjAry[$key][$lev1]=array('id'=>$tid,'count'=>$tnum,'time'=>$tSecond);
            $ids[] = $tid;//Id集合
        }
        if(count(array_unique($ids)) < count($dAry)){
            $this->jsResponse(1,'道具不能重复选择');
        }
        $resJson = json_encode($fjAry);
        $resJson = preg_replace('/\,/',"|",$resJson);//“，”转“|”
        $this->jsResponse(0,'生成成功',$resJson);
    }



    public  function getToolList(){
        //获取道具配置
        return xmlToolAry();
    }

    //删除
    public function del(){
        $id = intval($this->input->post('eid'));
        $data = $this->db->from('mail')->where('id',$id)->get()->row_array();
        if($data['status']==2){
            $this->jsAlert('此邮件不可删除');
        }
        $upAry=array('is_delete'=>1);
        $res = $this->db->update('mail', $upAry,array('id'=>$id));
        if($res){
            $this->jsAlert('删除成功_'.$res,'/mail/index');
        }
    }


}
/* End of file */
