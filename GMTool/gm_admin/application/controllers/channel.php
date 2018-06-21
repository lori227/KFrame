<?php if ( ! defined('BASEPATH')) exit('No direct script access allowed');

class Channel extends MY_Controller {


    function __construct() {
        parent::__construct();

    }
	
    function index($page=1){
        $queryParams = array(
                'channelId'   => intval($this->input->get('channel_id')),
                'channel_name'=> $this->input->get('channel_name'),
                'channelType' => intval($this->input->get('channel_type')),
                'siteType'    => intval($this->input->get('siteType')),
                'leader'      => intval($this->input->get('leader')),
                'store'       => intval($this->input->get('store')),
                'page_count'  => intval($this->input->get('page_count')),
        );
        $this->load->library('pagination');
        $page < 1 && $page = 1;
        $config = $this->config->config['pagination'];
        $config['base_url'] = '/channel/index';
        $config['per_page'] = $queryParams['page_count'] ? $queryParams['page_count'] : 25;

        $where = array();
        if($queryParams['channelId']){
            $where['id'] = $queryParams['channelId'];
        }
        
        if($queryParams['channelType']){
            $where['type'] = $queryParams['channelType'];
        }
        
        if($queryParams['siteType']){
            $where['site_type'] = $queryParams['siteType'];
        }
        
        if($queryParams['leader']){
            $where['leader'] = $queryParams['leader'];
        }
        
        if($queryParams['store']){
            $where['store'] = $queryParams['store'];
        }
        
        $obj = $this->db->from('channel');
        $obj->where($where);
        if($queryParams['channel_name']){
            $obj->like("name",$queryParams['channel_name']);
        }
        $config['total_rows'] = $obj->count_all_no_reset();

        $obj->limit($config['per_page'], ($page-1)*$config['per_page']);
        $this->pagination->initialize($config);
        $list = $obj->order_by('id desc')->get()->result_array();
        
        $this->_data['channelList'] = $list;
        
        $leader = array();
        $leaders = $this->db->select('id,realname')->from('user')->where(array('status' => 1))->get()->result_array();
        foreach ($leaders as $val) {
            $leader[$val['id']] = $val['realname'];
        }
        $this->_data['leader'] = $leader;
  /*      $store = array();
        $stores = $this->db->select('id,name')->from('store')->get()->result_array();
        foreach ($stores as $v) {
            $store[$v['id']] = $v['name'];
        }
        $this->_data['store'] = $store;*/
        
        $this->_data['queryParams'] = $queryParams;
        $this->load->view('channel/index', $this->_data);
    }
	
    function delete($id = 0){
        if(!$id){
            die('无效ID');
        }
        $flg = $this->db->delete('channel', array('id' => intval($id)));
        echo $flg;
    }
    
    function type(){
    	if($this->input->isPost()) {
    		$name = trim($this->input->post('name'));
    		$rst = $this->db->select('id,name')->from('channel_type')->where('name',$name)->get()->result_array();
    		if(!$rst){
		    	$data = array(
		    		'id'=>0,
		    		'name'=>$name,
		    		'add_time'=>time(),
		    	);
		    	$this->db->insert('channel_type',$data);
    		}
    		redirect(site_url('/channel/index'));
    	}
    	
    	$this->load->view('channel/type', $this->_data);
    }
    
    /**
     * 获取单个渠道的所有广告ID
     */
    function get_ads() {
        $channelId = intval($this->input->get('channel_id'));
        $ads = $this->db->from('adver')->select("id")->where('channel_id', $channelId)->get()->result_array();
        $formatAds = array();
        foreach($ads as $ad) {
            $formatAds[] = $ad['id'];
        }
        die(implode(",", $formatAds));
    }

}


/**
 * 辅助函数
 */

function array_filter_empty($v) {
    return ($v !== "");
}

function array_filter_zero($v) {
    return (is_numeric($v) && intval($v) !== 0);
}