<?php $this->load->view('admin/common/header');?>
<?php $this->load->view('admin/common/left');?>
    <div role="main" id="main">
        <?php $this->load->view('admin/common/right-nav', array('navTitle' => array('管理', '列表页')));?>
        <!-- MAIN CONTENT -->
        <div id="content">

            <div class="row">
                <div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
                    <h1 class="page-title txt-color-blueDark"><i class="fa fa-lg fa-fw fa-list-alt"></i>日志列表</h1>
                </div>
            </div>

            <section class="" id="widget-grid">
                <!-- row -->
                <div class="row">
                    <article class="col-xs-12 col-sm-12 col-md-12 col-lg-12 sortable-grid ui-sortable">
                        <div class="jarviswidget jarviswidget-color-white jarviswidget-sortable" style="" role="widget">

                            <header role="heading">
                             <!--   <div class="jarviswidget-ctrls" role="menu">
                                    <a href="/admin/node/add" class="button-icon jarviswidget-delete-btn">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;添加数据&nbsp;&nbsp;</a>
                                </div>-->
                                <span class="widget-icon"> <i class="fa fa-table"></i> </span>
                                <h2><?php echo "总共{$nums}条--本页".count($list); ?></h2>
                            </header>
                            <div>
                                <div class="widget-body">
                                    <table class="table table-bordered">
                                        <thead>
                                        <tr>
                                            <th>&nbsp;ID</th>
                                            <th>&nbsp;级别</th>
                                            <th>&nbsp;用户</th>
                                            <th>&nbsp;描述</th>
                                            <th>&nbsp;动作</th>
                                            <th>&nbsp;URL/POST</th>
                                            <th>&nbsp;类型</th>
                                            <th>&nbsp;IP</th>
                                            <th>&nbsp;时间</th>

                                        </tr>
                                        </thead>
                                        <tbody>
                                        <?php if(isset($list)){foreach($list as $v){?>
                                            <tr >
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['log_id'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['level'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['username'];?></td>
                                                <td>&nbsp;&nbsp;<?=$v['message'];?></td>
                                                <td>&nbsp;&nbsp;<?=$v['controller'].'/'.$v['action'];?></td>
                                                <td style="max-width:400px;word-break: break-all;">
                                                    <p style="margin: 0px;"><?=$v['get'];?></p>
                                                    <p><?=$v['post'];?></p>
                                                </td>
                                                <td>&nbsp;&nbsp;<?=$v['type'];?></td>
                                                <td>&nbsp;&nbsp;<?=$v['ip'];?></td>
                                                <td>&nbsp;&nbsp;<?=date('Y-m-d H:i:s',$v['create_time']);?></td>

                                            </tr>
                                        <?php }}?>
                                        </tbody>
                                    </table>

                                </div>
                                <!-- end widget content -->

                            </div>
                            <!-- end widget div -->

                        </div>
                        <?php echo $this->pagination->create_links();?>
                    </article>
                     
                </div>
            
            </section>
        </div>

    </div>
<script>
    $(function(){
        //$('.tooltip-show').tooltip({html:true});
    });
</script>

<? $this->load->view('admin/common/footer');?>