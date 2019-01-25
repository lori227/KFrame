<?php $this->load->view('admin/common/header');?>
<?php $this->load->view('admin/common/left');?>
    <div role="main" id="main">
        <?php $this->load->view('admin/common/right-nav', array('navTitle' => array('开服管理', '列表页')));?>
        <!-- MAIN CONTENT -->
        <div id="content">

            <div class="row">
                <div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
                    <h1 class="page-title txt-color-blueDark"><i class="fa fa-lg fa-fw fa-list-alt"></i>部署（deploy）列表</h1>
                </div>
            </div>

            <section class="" id="widget-grid">
                <!-- row -->
                <div class="row">
                    <article class="col-xs-12 col-sm-12 col-md-12 col-lg-12 sortable-grid ui-sortable">
                        <div class="jarviswidget jarviswidget-color-white jarviswidget-sortable" style="" role="widget">

                            <header role="heading">
                                <div class="jarviswidget-ctrls" role="menu">
                                    <a href="/admin/deploy/add" class="button-icon jarviswidget-delete-btn">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;添加数据&nbsp;&nbsp;</a>
                                </div>
                                <span class="widget-icon"> <i class="fa fa-table"></i> </span>
                                <h2>共<?php echo count($list); ?></h2>
                            </header>
                            <div>
                                <div class="widget-body">
                                    <table class="table table-bordered">
                                        <thead>
                                        <tr>
                                            <th>&nbsp;&nbsp;渠道</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;appid</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;appname</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;apptype</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;是否启动(startup)</th>
                                            <th>&nbsp;&nbsp;是否debug</th>

                                        <!--    <th>&nbsp;&nbsp;shutdown</th>
                                            <th>&nbsp;&nbsp;process</th>
                                            <th>&nbsp;&nbsp;agentid</th>-->

                                            <th>&nbsp;&nbsp;机器IP（localip）</th>
                                            <th>&nbsp;&nbsp;时间戳（time）</th>
                                            <th>&nbsp;&nbsp;操作</th>

                                        </tr>
                                        </thead>
                                        <tbody>
                                        <?php if(isset($list)){foreach($list as $v){?>
                                            <tr >
                                                <td>&nbsp;&nbsp;<?=$v['appchannel'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['appid'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['appname'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['apptype'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['startup'];?></td>
                                                <td>&nbsp;&nbsp;<?=$v['debug'];?></td>

                                          <!--      <td>&nbsp;&nbsp;<?/*=$v['shutdown'];*/?></td>
                                                <td>&nbsp;&nbsp;<?/*=$v['process'];*/?></td>
                                                <td>&nbsp;&nbsp;<?/*=$v['agentid'];*/?></td>-->

                                                <td>&nbsp;&nbsp;<?=$v['localip'];?></td>
                                                <td>&nbsp;&nbsp;<?=date('Y-m-d H:i:s',$v['time']);?></td>
                                                <td>
                                                    <a href="/admin/deploy/modify/<?=$v['appid'];?>" class="btn btn-primary">修改</a>
                                                    <a href="/admin/deploy/del/<?=$v['appid'];?>" class="btn btn-danger">删除</a>
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