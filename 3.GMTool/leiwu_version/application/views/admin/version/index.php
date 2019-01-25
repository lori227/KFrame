<?php $this->load->view('admin/common/header');?>
<?php $this->load->view('admin/common/left');?>
    <div role="main" id="main">
        <?php $this->load->view('admin/common/right-nav', array('navTitle' => array('开服管理', '列表页')));?>
        <!-- MAIN CONTENT -->
        <div id="content">

            <div class="row">
                <div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
                    <h1 class="page-title txt-color-blueDark"><i class="fa fa-lg fa-fw fa-list-alt"></i>版本列表</h1>
                </div>
            </div>

            <section class="" id="widget-grid">
                <!-- row -->
                <div class="row">
                    <article class="col-xs-12 col-sm-12 col-md-12 col-lg-12 sortable-grid ui-sortable">
                        <div class="jarviswidget jarviswidget-color-white jarviswidget-sortable" style="" role="widget">

                            <header role="heading">
                                <div class="jarviswidget-ctrls" role="menu">
                                    <a href="/admin/version/add" class="button-icon jarviswidget-delete-btn">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;添加数据&nbsp;&nbsp;</a>
                                </div>
                                <span class="widget-icon"> <i class="fa fa-table"></i> </span>
                                <h2>共<?php echo count($list); ?></h2>
                            </header>
                            <div>
                                <div class="widget-body">
                                    <table class="table table-bordered">
                                        <thead>
                                        <tr>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;时间</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;名称</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;远程URL</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;MD5</th>

                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;操作</th>
                                        </tr>
                                        </thead>
                                        <tbody>
                                        <?php if(isset($list)){foreach($list as $v){?>
                                            <tr >
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['version_time'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['version_name'];?></td>
                                                <td>&nbsp;&nbsp;<?=$v['version_url'];?><?php if(isset($v['exist'])){echo '（不存在）';}?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['version_md5'];?></td>

                                                <td>
                                                   <!-- <a href="/admin/node/modify/<?/*=$v['node_id'];*/?>" class="btn btn-primary">修改</a>-->
                                                    <a href="javascript:void(0)" class="btn btn-danger" onclick="delData('<?=$v['version_name'];?>')">删除</a>
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