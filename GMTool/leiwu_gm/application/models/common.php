<?php
//表字段注释说明
//mail-status=>邮件状态（0:未1:已审核2:已发送3:审核未通过4:已驳回）




class Common extends MY_Model{
	public static function getCurMonthFirstDay($date) {
		return date('Ym01', strtotime($date));
	}
	public static function getCurMonthLastDay($date) {
		return date('Ymd', strtotime(date('Ym01', strtotime($date)) . ' +1 month -1 day'));
	}
}