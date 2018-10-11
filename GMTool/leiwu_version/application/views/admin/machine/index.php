<?php $this->load->view('admin/common/header');?>
<?php $this->load->view('admin/common/left');?>
    <div role="main" id="main">
        <?php $this->load->view('admin/common/right-nav', array('navTitle' => array('开服管理', '列表页')));?>
        <!-- MAIN CONTENT -->
        <div id="content">

            <div class="row">
                <div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
                    <h1 class="page-title txt-color-blueDark"><i class="fa fa-lg fa-fw fa-list-alt"></i>机器（machine）列表</h1>
                </div>
            </div>

            <section class="" id="widget-grid">
                <!-- row -->
                <div class="row">
                    <article class="col-xs-12 col-sm-12 col-md-12 col-lg-12 sortable-grid ui-sortable">
                        <div class="jarviswidget jarviswidget-color-white jarviswidget-sortable" style="" role="widget">

                            <header role="heading">
                                <div class="jarviswidget-ctrls" role="menu">
                                    <a href="/admin/machine/add" class="button-icon jarviswidget-delete-btn">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;添加数据&nbsp;&nbsp;</a>
                                </div>
                                <span class="widget-icon"> <i class="fa fa-table"></i> </span>
                                <h2>共<?php echo count($list); ?></h2>
                            </header>
                            <div>
                                <div class="widget-body">
                                    <table class="table table-bordered">
                                        <thead>
                                        <tr>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;名称</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;localip</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;interanetip</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;cpu</th>

                                            <th>&nbsp;&nbsp;thread</th>
                                            <th>&nbsp;&nbsp;memory</th>
                                            <th>&nbsp;&nbsp;harddisk</th>
                                            <th>&nbsp;&nbsp;agentid</th>

                                            <th>&nbsp;&nbsp;status</th>
                                            <th>&nbsp;&nbsp;port</th>
                                            <th>&nbsp;&nbsp;操作</th>
                                        </tr>
                                        </thead>
                                        <tbody>
                                        <?php if(isset($list)){foreach($list as $v){?>
                                            <tr >
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['name'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['localip'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['interanetip'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['cpu'];?></td>

                                                <td>&nbsp;&nbsp;<?=$v['thread'];?></td>
                                                <td>&nbsp;&nbsp;<?=$v['memory'];?></td>
                                                <td>&nbsp;&nbsp;<?=$v['harddisk'];?></td>
                                                <td>&nbsp;&nbsp;<?=$v['agentid'];?></td>

                                                <td>&nbsp;&nbsp;<?=$v['status'];?></td>

                                                <td>&nbsp;&nbsp;<?=$v['port'];?></td>
                                                <td>
                                                    <a href="/admin/machine/modify/<?=$v['localip'];?>" class="btn btn-primary">修改</a>
                                                    <a href="/admin/machine/del/<?=$v['localip'];?>" class="btn btn-danger">删除</a>
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