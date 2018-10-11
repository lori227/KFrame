<?php $this->load->view('admin/common/header');?>
<?php $this->load->view('admin/common/left');?>
    <div role="main" id="main">
        <?php $this->load->view('admin/common/right-nav', array('navTitle' => array('官网管理', '首页banner')));?>
        <!-- MAIN CONTENT -->
        <div id="content">

            <div class="row">
                <div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
                    <h1 class="page-title txt-color-blueDark"><i class="fa fa-lg fa-fw fa-list-alt"></i> 首页banner</h1>
                </div>

            </div>
            <section class="" id="widget-grid">
                <!-- row -->
                <div class="row">
                    <article class="col-xs-12 col-sm-12 col-md-12 col-lg-12 sortable-grid ui-sortable">
                        <div class="jarviswidget jarviswidget-color-white jarviswidget-sortable" style="" role="widget">

                            <header role="heading">
                                <div class="jarviswidget-ctrls" role="menu">
                                    <a data-placement="bottom" href="/admin/admin/add_banner" title="" rel="tooltip" class="button-icon jarviswidget-delete-btn" data-original-title="">&nbsp;&nbsp;
                                        <i class="fa fa-plus"></i>&nbsp;添加banner&nbsp;&nbsp</a>&nbsp;&nbsp;&nbsp;&nbsp;
                                </div>
                                <span class="widget-icon"> <i class="fa fa-table"></i> </span>
                                <h2></h2>
                            </header>

                            <div>

                                <!-- widget content -->
                                <div class="widget-body">
                                    <table class="table table-bordered">
                                        <thead>
                                        <tr>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;ID</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;封面</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;跳转URL</th>

                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;添加人</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;添加时间</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;操作</th>
                                        </tr>
                                        </thead>
                                        <tbody>
                                        <?php if(isset($list) && $list){foreach($list as $v){?>
                                            <tr id="<?=$v['id'];?>">
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['id'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<img src="<?=QINIU_BASIC_URL.$v['img'];?>" style="width:500px;" /></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['url'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?= $v['add_user'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?= $v['add_time'];?></td>
                                                <td>&nbsp;&nbsp;
                                                    <a class="btn btn-labeled btn-danger" href="/admin/admin/del_banner/<?=$v['id'];?>">
                                                        <span class="btn-label"><i class="fa fa-pencil-square-o"></i></span>删除
                                                    </a>&nbsp;
                                                </td>
                                            </tr>
                                        <?php }}?>
                                        </tbody>
                                    </table>

                                </div>
                                <!-- end widget content -->

                            </div>
                            <!-- end widget div -->

                        </div>
                    </article>

                </div>

            </section>
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
var uid = 0;
var actUser = function(id,act){
    if(id){
        $.get('/admin/auth/actUser/'+id+'/'+act, function(flg){
            if(flg){
                window.location.reload();
            }
        });
    }
}
$(function(){
    $(".disableUser").click(function(e){
        uid = $(this).data('uid');
        $('#disableBox').modal('show');
    });
    $(".enableUser").click(function(e){
        uid = $(this).data('uid');
        actUser(uid, 1);
    });
    $("#disableBtn").click(function(){
        actUser(uid, 0);
    });
})

</script>
<?php $this->load->view('admin/common/footer');?>