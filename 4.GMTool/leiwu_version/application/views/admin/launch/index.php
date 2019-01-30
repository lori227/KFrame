<?php $this->load->view('admin/common/header');?>
<?php $this->load->view('admin/common/left');?>
    <div role="main" id="main">
        <?php $this->load->view('admin/common/right-nav', array('navTitle' => array('开服管理', '列表页')));?>
        <!-- MAIN CONTENT -->
        <div id="content">

            <div class="row">
                <div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
                    <h1 class="page-title txt-color-blueDark"><i class="fa fa-lg fa-fw fa-list-alt"></i>启动（launch）列表</h1>
                </div>
            </div>

            <section class="" id="widget-grid">
                <!-- row -->
                <div class="row">
                    <article class="col-xs-12 col-sm-12 col-md-12 col-lg-12 sortable-grid ui-sortable">
                        <div class="jarviswidget jarviswidget-color-white jarviswidget-sortable" style="" role="widget">

                            <header role="heading">
                                <div class="jarviswidget-ctrls" role="menu">
                                    <a href="/admin/launch/add" class="button-icon jarviswidget-delete-btn">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;添加数据&nbsp;&nbsp;</a>
                                </div>
                                <span class="widget-icon"> <i class="fa fa-table"></i> </span>
                                <h2>共<?php echo count($list); ?></h2>
                            </header>
                            <div>
                                <div class="widget-body">
                                    <table class="table table-bordered">
                                        <thead>
                                        <tr>
                                           <!-- <th>&nbsp;&nbsp;ID</th>-->
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;appname</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;apptype</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;apppath</th>
                                            <th>&nbsp;&nbsp;appfile</th>

                                            <th>&nbsp;&nbsp;appconfig</th>
                                            <th>&nbsp;&nbsp;service</th>
                                            <th>&nbsp;&nbsp;ftpid</th>
                                            <th>&nbsp;&nbsp;deploypath</th>
                                            <th>&nbsp;&nbsp;操作</th>

                                        </tr>
                                        </thead>
                                        <tbody>
                                        <?php if(isset($list)){foreach($list as $v){?>
                                            <tr >
                                               <!-- <td>&nbsp;&nbsp;<?/*=$v['launch_id'];*/?></td>-->
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['appname'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['apptype'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['apppath'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['appfile'];?></td>

                                                <td>&nbsp;&nbsp;<?=$v['appconfig'];?></td>

                                                <td>&nbsp;&nbsp;<?=$v['service'];?></td>
                                                <td>&nbsp;&nbsp;<?=$v['ftpid'];?></td>

                                                <td>&nbsp;&nbsp;<?=$v['deploypath'];?></td>

                                                <td>
                                                    <a href="/admin/launch/modify/<?=$v['appname'];?>/<?=$v['apptype'];?>" class="btn btn-primary">修改</a>
                                                    <a href="javascript:void(0)" onclick="del('<?=$v['appname'];?>','<?=$v['apptype'];?>')" class="btn btn-danger">删除</a>
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
<script>
    $(function(){
        //$('.tooltip-show').tooltip({html:true});
    });
    function del(name,type){
        if(confirm('确认删除？')){
window.location.href='/admin/launch/del/'+name+'/'+type;
        }
    }
</script>

<? $this->load->view('admin/common/footer');?>