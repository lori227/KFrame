<?php $this->load->view('header');?>
<?php $this->load->view('left');?>
    <div role="main" id="main">
        <?php $this->load->view('right-nav', array('navTitle' => array('权限管理', '用户列表')));?>
        <!-- MAIN CONTENT -->
        <div id="content">

            <div class="row">
                <div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
                    <h1 class="page-title txt-color-blueDark"><i class="fa fa-lg fa-fw fa-list-alt"></i> 管理员列表</h1>
                </div>

            </div>
            <section class="" id="widget-grid">
                <!-- row -->
                <div class="row">
                    <article class="col-xs-12 col-sm-12 col-md-12 col-lg-12 sortable-grid ui-sortable">
                        <div class="jarviswidget jarviswidget-color-white jarviswidget-sortable" style="" role="widget">

                            <header role="heading">

                                <h2></h2>
                            </header>

                            <div>

                                <!-- widget content -->
                                <div class="widget-body">
                                    <table class="table table-bordered">
                                        <thead>
                                        <tr>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;用户ID</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;用户名</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;真实姓名</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;最后登录IP</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;最后登录时间</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;状态</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;操作</th>
                                        </tr>
                                        </thead>
                                        <tbody>
                                        <?php if(isset($list) && $list){foreach($list as $v){?>
                                            <tr id="<?=$v['id'];?>">
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['id'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['username'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['realname'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['last_login_ip'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=date('Y-m-d H:i', $v['last_login_time']);?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?= $v['status'] == 1 ? '<font color="green">正常</font>' : '<font color="reg">禁用</font>';?></td>
                                                <td>&nbsp;&nbsp;<a class="btn btn-labeled btn-default" href="/auth/edit/pass/<?=$v['id'];?>"> <span class="btn-label"><i class="fa fa-pencil-square-o"></i></span>修改密码</a>&nbsp;&nbsp;<a class="btn btn-labeled btn-primary" href="/auth/edit/auth/<?=$v['id'];?>"> <span class="btn-label"><i class="fa fa-gear"></i></span>权限设置</a>&nbsp;&nbsp;
                                                <?php if($v['status'] == 1){ ?>
                                                <a class="btn btn-labeled btn-danger disableUser" data-uid="<?=$v['id'];?>" href="javascript:void(0)" data-toggle="modal"> <span class="btn-label"><i class="glyphicon glyphicon-trash"></i></span>禁用</a></td>
                                                <?php }else{ ?>
                                                <a class="btn btn-labeled btn-success enableUser" data-uid="<?=$v['id'];?>" href="javascript:void(0)" data-toggle="modal"> <span class="btn-label"><i class="glyphicon glyphicon-trash"></i></span>启用</a></td>
                                                <?php } ?>
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
        $.get('/auth/actUser/'+id+'/'+act, function(flg){
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
<?php $this->load->view('footer');?>