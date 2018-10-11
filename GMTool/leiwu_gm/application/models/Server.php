<?php
/**
 * 与服务器redis通信专用类文件
 * User: xuefeng
 * Date: 18-6-11
 * Time: 下午5:56
 */

class Server extends MY_Model{
    public static $apiUrl;//全局变量
    public function __construct(){
        parent::__construct();
        $this->load->helper('common');
        if(IS_DEV){
            self::$apiUrl = 'http://192.168.1.57:27710/';
        }else{
            self::$apiUrl = 'http://123.206.102.191:27710/';//正式服
        }
    }
    //服务器端通信
    public function serverCurl($method,$valAry){
        $url = self::$apiUrl.$method;//服务器端接口
        $con = json_encode($valAry);
        $cres = curlPostRequest($url,$con);
        gmLog([$valAry,$cres],__METHOD__);//记录日志
        $rAry = json_decode($cres,true);
        return $rAry['retcode'];
    }

    //发送邮件
    public function sendEmail($eid){
        $rst = $this->db->from('mail')->where('id',$eid)->get()->row_array();
        if($rst){
            $pApi=array(
                'type'=>1,//对服务器而言，页端邮件类型都是系统邮件
                'title'=>$rst['title'],
                'content'=>$rst['content'],
                'validtime'=>$rst['validtime'],
                'reward'=>$rst['reward'],//奖励,json形式==[{"item":{"id":"10"|"count":"1"}}]
                'sendername'=>$rst['sendername'],
            );
            //用户补偿(全局邮件不发playerids)
            if($rst['type']==2){
                $pApi['playerids'] = $rst['playerIdStr'];//玩家id列表，"id,id,id"形式
            }
            //reward处理
            if($rst['type']==1 || ($rst['type']==2 && empty($pApi['reward']))){
                unset($pApi['reward']);
            }
            gmLog([$eid,$pApi],__METHOD__);//记录日志
           $res = $this->serverCurl('addmail',$pApi);//发给服务器固定字段
           return $res;
        }
    }
    //驳回（删除）用户补偿邮件
    public function turnMail($eid){
        $rst = $this->db->from('mail')->where('id',$eid)->get()->row_array();
        if($rst){
            if($rst['status']!=2){
                die('此邮件不可驳回');
            }
            $idsAry = json_decode($rst['playerIdStr'],true);
            if(!isset($idsAry) || count($idsAry) <= 0){
                die('玩家列表没有数据');
            }
            $i=0;
            foreach($idsAry as $pid){
                $pid = intval($pid);//玩家id
                $res=$this->redis->delete('personmailfriend:'.$pid);echo 'personmailfriend:'.$pid;
                if($res){$i++;}
            }
            return $i;
        }
    }
    //用户信息
    public function getAcc($id){
        $accId = 'accountid:'.intval($id);
        $res = $this->redis->hGetAll($accId);
        return $res;
    }

    //选择服务器操作命令
    public function getCommand($val){
       /* case 1:
            $res = array('command'=>'shutdown','value'=>'');
            break;
        case 2:
            $res = array('command'=>'shutdown','value'=>'');
            break;
        case 3:
            $res = array('command'=>'shutdown','value'=>'');
            break;
        case 4:
            $res = array('command'=>'shutdown','value'=>'');
            break;
        case 5:
            $res = array('command'=>'version','value'=>'');//版本更新
            break;
        case 6:
            $res = array('command'=>'shutdown','value'=>'');
            break;*/
    }

}