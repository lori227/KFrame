<?php $this->load->view('admin/common/header');?>
<?php $this->load->view('admin/common/left');?>
    <div role="main" id="main">
        <?php $this->load->view('admin/common/right-nav', array('navTitle' => array('权限管理', '用户列表')));?>
        <!-- MAIN CONTENT -->
        <div id="content">

            <div class="row">
                <div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
                    <h1 class="page-title txt-color-blueDark"><i class="fa fa-lg fa-fw fa-list-alt"></i> 权限编辑</h1>
                </div>
            </div>
            <?php if($act == 'success'){ ?>
            <div class="row">
            <article class="col-sm-12">
                <div class="alert alert-success fade in">
                    <button data-dismiss="alert" class="close">
                        ×
                    </button>
                    <i class="fa-fw fa fa-check"></i>
                    <strong>操作成功</strong>
                </div>
            </article>  
            </div>
            <?php } ?>
            <div class="row">

                <div class="col-xs-12 col-sm-12 col-md-12 col-lg-12">
            
                    <div class="well well-sm">
                        <form class="form-horizontal" action="" method="post">
                            <div class="smart-timeline">
                                <ul class="smart-timeline-list">
                                <?php $tmpUserAuth = json_decode($data['auth']) ? json_decode($data['auth']) : array();?>
                                <?php foreach($node as $v){ ?>
                                    <?php if(isset($v['node'])){ ?>
                                    <li>
                                        <div class="smart-timeline-icon bg-color-blueLight">
                                            <i class="fa fa-lg fa-fw fa-list-alt"></i>
                                        </div>
                                        <div class="smart-timeline-time">
                                            <small><?=$v['name'];?></small>
                                        </div>
                                        <div class="smart-timeline-content">
                                            <?php if(isset($v['node'])){ ?>
                                            <?php foreach($v['node'] as $vv){ ?>
                                            <label class="span1">
                                                
                                                <?php if(in_array($vv['id'], $tmpUserAuth)){ ?>
                                                    <input type="checkbox" value="<?=$vv['id']?>" name="authids[]" checked="checked"><?=$vv['name']?>                                                
                                                <?php }else{ ?>
                                                    <input type="checkbox" value="<?=$vv['id']?>" name="authids[]"><?=$vv['name']?>  
                                                <?php } ?>
                                            </label>
                                            <?php } ?>
                                            <?php } ?>
                                        </div>
                                    </li>
                                    <?php } ?>
                                <?php } ?>
                                  <!--  <li>
                                        <div class="smart-timeline-icon bg-color-blueLight">
                                            <i class="fa fa-lg fa-fw fa-list-alt"></i>
                                        </div>
                                        <div class="smart-timeline-time">
                                            <small>可视应用</small>
                                        </div>
                                        <div class="smart-timeline-content">
                                            <?php
/*                                            $allowApp = explode(",", $data['allow_app']);
                                            foreach($app as $k => $v){
                                            */?>
                                                <label class="span1">
                                                    <input type="checkbox" value="<?/*=$k;*/?>" <?php /*if(in_array($k, $allowApp)) echo 'checked="checked"';*/?> name="app[]"><?/*=$v;*/?>
                                                </label>
                                            <?php /*}*/?>
                                        </div>
                                    </li>-->
                                    <!--<li>
                                        <div class="smart-timeline-icon bg-color-blueLight">
                                            <i class="fa fa-lg fa-fw fa-list-alt"></i>
                                        </div>
                                        <div class="smart-timeline-time">
                                            <small>渠道类型</small>
                                        </div>
                                        <div class="smart-timeline-content">
                                            <?php /*$allowChannelType = explode(",", $data['allow_channel_type']); foreach($channelType as $k => $v){*/?>
                                                <label class="span1">
                                                    <input type="checkbox" value="<?/*=$k;*/?>" <?php /*if(in_array($k, $allowChannelType)) echo 'checked="checked"';*/?> name="channel_type[]"><?/*=$v;*/?>
                                                </label>
                                            <?php /*}*/?>
                                        </div>
                                    </li>-->
                                </ul>
                            </div>
                            <div class="form-actions">
                                <div class="row">
                                    <div class="col-md-12">
                                        <button class="btn btn-primary" type="submit"><i class="fa fa-save"></i>提交</button>
                                    </div>
                                </div>
                            </div>
                        </form>
                    </div>
                </div>

            </div>
        </div>

    </div>


<div class="modal fade" id="disableBox" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
  <div class="modal-dialog">
    <div class="modal-content">
      <div class="modal-header">
        <button type="button" class="close" data-dismiss="modal"><span aria-hidden="true">&times;</span><span class="sr-only">Close</span></button>
        <h4 class="modal-title" id="myModalLabel">禁用用户？</h4>
      </div>
      <div class="modal-body">
        <div>
            <div class="alert alert-success fade in">
                <i class="fa-fw fa fa-info"></i>
                您确认要禁用这个用户？.
            </div>
        </div>
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-default" data-dismiss="modal">关闭窗口</button>
        <button type="button" class="btn btn-primary" id="disableBtn">确认禁用</button>
      </div>
    </div>
  </div>
</div>

<script type="text/javascript">

</script>
<?php $this->load->view('admin/common/footer');?>