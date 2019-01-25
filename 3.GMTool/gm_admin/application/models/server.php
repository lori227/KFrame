<?php
/**
 * 与服务器redis通信专用类文件
 * User: xuefeng
 * Date: 18-6-11
 * Time: 下午5:56
 */
class Server extends MY_Model {
    //生成新邮件
    public function createEmail($valAry){
        $valAry['id'] = $valAry['email_id'];
        unset($valAry['email_id']);
        $key = 'mail:'.$valAry['id'];
        $this->redis->hMset($key,$valAry);
        $this->redis->expire($key,$valAry['validtime']);
    }

    //发送邮件
    public function sendEmail($eid){
        $rst = $this->db->from('mail')->where('id',$eid)->get()->row_array();
        if($rst){
            $i=0;
            switch($rst['type']){
                case 1://系统邮件
                    //zadd wholemail 邮件id 邮件id  gm系统邮件
                    $res=$this->redis->zAdd('wholemail',$rst['email_id'],$rst['email_id']);
                    break;
                case 2://用户补偿邮件
                    $idsAry = json_decode($rst['playerIdStr'],true);
                    if(!isset($idsAry) || count($idsAry) <= 0){
                        die('玩家列表没有数据');
                    }//
                    foreach($idsAry as $pid){
                        $pid = intval($pid);//玩家id
                        $res=$this->redis->zAdd('personmailfriend:'.$pid,0,$rst['email_id']);//0表示邮件未读
                        if($res){$i++;}
                    }
                    return $i;
                    break;
            }
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

}