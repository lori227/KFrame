<?php $this->load->view('admin/common/header');?>
<?php $this->load->view('admin/common/left');?>
    <div role="main" id="main">
        <?php $this->load->view('admin/common/right-nav', array('navTitle' => array('开服管理', '列表页')));?>
        <!-- MAIN CONTENT -->
        <div id="content">

            <div class="row">
                <div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
                    <h1 class="page-title txt-color-blueDark"><i class="fa fa-lg fa-fw fa-list-alt"></i>部署日志列表</h1>
                </div>
            </div>

            <section class="" id="widget-grid">
                <!-- row -->
                <div class="row">
                    <article class="col-xs-12 col-sm-12 col-md-12 col-lg-12 sortable-grid ui-sortable">
                        <div class="jarviswidget jarviswidget-color-white jarviswidget-sortable" style="" role="widget">

                            <header role="heading">
                                <div class="jarviswidget-ctrls" role="menu">
                                    <a href="/admin/version/act_up" class="button-icon ">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;去操作部署&nbsp;&nbsp;</a>
                                </div>
                                <span class="widget-icon"> <i class="fa fa-table"></i> </span>
                                <h2>共<?= count($list);?></h2>
                            </header>
                            <div>
                                <div class="widget-body">
                                    <table class="table table-bordered">
                                        <thead>
                                        <tr>
                                            <th>&nbsp;&nbsp;ID</th>
                                            <th>&nbsp;&nbsp;ip集合</th>
                                            <th>&nbsp;&nbsp;添加人备注</th>

                                            <th>&nbsp;&nbsp;命令</th>
                                            <th>&nbsp;&nbsp;命令值</th>
                                            <th>&nbsp;&nbsp;第几步</th>
                                            <th>&nbsp;&nbsp;服务器执行时间</th>
                                            <th>&nbsp;&nbsp;添加人</th>
                                            <th>&nbsp;&nbsp;添加时间</th>
                                            <th>&nbsp;&nbsp;操作人</th>
                                            <th>&nbsp;&nbsp;操作时间</th>

                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;操作</th>
                                        </tr>
                                        </thead>
                                        <tbody>
                                        <?php if(isset($list)){foreach($list as $v){?>
                                            <tr >
                                                <td >&nbsp;&nbsp;<?=$v['id'];?></td>
                                                <td class="mw350">
                                                    <?php
                                                    $ips = json_decode($v['ips'],true);
                                                    foreach($ips as $ip){
                                                        echo '<span style="width:90px;display:block;float:left;">'.$ip.'</span>';
                                                    }
                                                    ?>
                                                </td>
                                                <td class="mw200">&nbsp;&nbsp;<?=$v['remark'];?></td>

                                                <td >&nbsp;&nbsp;<?=$commandType[$v['command']]??null;?></td>
                                                <td class="mw200"><?=$v['value'];?></td>
                                                <td>&nbsp;&nbsp;<?=$v['step'];?></td>
                                                <td><?=($v['scheduletime']==0)?null:date('Y-m-d H:i:s',$v['scheduletime']);?></td>
                                                <td>&nbsp;&nbsp;<?=$user[$v['add_user']]??null;?></td>
                                                <td>
                                                    <p><?=date('Y-m-d H:i:s',$v['add_time']);?></p>
                                                </td>
                                                <td>&nbsp;&nbsp;<?=$user[$v['op_user']]??null;?></td>
                                                <td><?=($v['op_time']==0)?null:date('Y-m-d H:i:s',$v['op_time']);?></td>
                                                <td>
                                                    <?php if($v['step']<=2){$step=2;?>
                                                        <a href="/admin/version/act_up<?=$step;?>/<?=$v['id'];?>" class="btn btn-info" >查看</a>
                                                    <?php }else{$step=3; ?>
                                                        <a href="#" class="btn btn-warning" disabled >结束</a>
                                                    <?php }?>
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
    function delData(name){
        if(confirm('确认删除？')){
            window.location.href="/admin/version/del/"+name;
        }
    }
</script>

<? $this->load->view('admin/common/footer');?>