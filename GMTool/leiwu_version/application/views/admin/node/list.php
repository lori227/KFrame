<?php $this->load->view('admin/common/header');?>
<?php $this->load->view('admin/common/left');?>
    <div role="main" id="main">
        <?php $this->load->view('admin/common/right-nav', array('navTitle' => array('管理', '列表页')));?>
        <!-- MAIN CONTENT -->
        <div id="content">

            <div class="row">
                <div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
                    <h1 class="page-title txt-color-blueDark"><i class="fa fa-lg fa-fw fa-list-alt"></i>机器（节点）列表</h1>
                </div>
            </div>

            <section class="" id="widget-grid">
                <!-- row -->
                <div class="row">
                    <article class="col-xs-12 col-sm-12 col-md-12 col-lg-12 sortable-grid ui-sortable">
                        <div class="jarviswidget jarviswidget-color-white jarviswidget-sortable" style="" role="widget">

                            <header role="heading">
                                <div class="jarviswidget-ctrls" role="menu">
                                    <a href="/admin/node/add" class="button-icon jarviswidget-delete-btn">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;添加数据&nbsp;&nbsp;</a>
                                </div>
                                <span class="widget-icon"> <i class="fa fa-table"></i> </span>
                                <h2>共<?php echo count($list); ?></h2>
                            </header>
                            <div>
                                <div class="widget-body">
                                    <table class="table table-bordered">
                                        <thead>
                                        <tr>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;ID</th>
                                            <th>&nbsp;&nbsp;节点IP</th>
                                            <th>&nbsp;&nbsp;端口</th>
                                            <th>&nbsp;&nbsp;Token</th>
                                            <th>&nbsp;&nbsp;备注</th>
                                            <th>&nbsp;&nbsp;服务器返回的状态</th>

                                            <th>&nbsp;&nbsp;添加时间</th>
                                            <th>&nbsp;&nbsp;更新时间</th>
                                            <th>&nbsp;&nbsp;操作</th>
                                        </tr>
                                        </thead>
                                        <tbody>
                                        <?php if(isset($list)){foreach($list as $v){?>
                                            <tr >
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['node_id'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['ip'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['port'];?></td>
                                                <td>&nbsp;&nbsp;<?=$v['token'];?></td>
                                                <td>&nbsp;&nbsp;<?=$v['comment'];?></td>
                                                <td>&nbsp;&nbsp;<?=$v['status'];?></td>
                                                <td>&nbsp;&nbsp;<?=date('Y-m-d H:i:s',$v['create_time']);?></td>
                                                <td>&nbsp;&nbsp;<?=date('Y-m-d H:i:s',$v['create_time']);?></td>
                                                <td>
                                                    <a href="/admin/node/modify/<?=$v['node_id'];?>" class="btn btn-primary">修改</a>
                                                    <a href="/admin/node/del/<?=$v['node_id'];?>" class="btn btn-danger">删除</a>
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
</script>

<? $this->load->view('admin/common/footer');?>