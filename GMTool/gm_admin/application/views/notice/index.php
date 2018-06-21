<?=$this->load->view('header');?>
<?=$this->load->view('left');?>
    <div role="main" id="main">
        <?=$this->load->view('right-nav', array('navTitle' => array('平台管理', '公告管理')));?>
        <!-- MAIN CONTENT -->
        <div id="content">

            <div class="row">
                <div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
                    <h1 class="page-title txt-color-blueDark"><i class="fa fa-lg fa-fw fa-list-alt"></i> 公告管理</h1>
                </div>
                 <form role="form" class="form-inline" id="filterBox-new">
                        <div class="form-group sm-input">
                            <span>ID:</span>
                            <input type="text" class="form-control input-sm" name="id" value="<?=$queryParams['id'];?>"  placeholder="ID">
                        </div>
                        <div class="form-group">
                            <span>类型:</span>
                            <select class="form-control input-sm" name="type">
                                <option value="0">请选择</option>
                                <?php foreach($type as $k => $v){ ?>
                                    <option value="<?=$k;?>"<?= $k == $queryParams['type'] ? ' selected="selected"' : '';?>><?=$v;?></option>
                                <?php } ?>
                            </select>
                        </div>
                     <div class="form-group sm-input">
                         <span>标题:</span>
                         <input type="text" class="form-control input-sm" name="title" value="<?=$queryParams['title'];?>" placeholder="标题">
                     </div>
                        <button type="submit" id="getdata" class="btn btn-default">查询</button>
                    </form>
            </div>

            <section class="" id="widget-grid">
                <!-- row -->
                <div class="row">
                    <article class="col-xs-12 col-sm-12 col-md-12 col-lg-12 sortable-grid ui-sortable">
                        <div class="jarviswidget jarviswidget-color-white jarviswidget-sortable" style="" role="widget">

                            <header role="heading">
                                <div class="jarviswidget-ctrls" role="menu">
                                    <a data-placement="bottom" href="/notice/add" title="" rel="tooltip" class="button-icon jarviswidget-delete-btn">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;添加公告&nbsp;&nbsp;</a>
                                    &nbsp;&nbsp;&nbsp;&nbsp;
                                </div>
                                <span class="widget-icon"> <i class="fa fa-table"></i> </span>
                                <h2>共</h2>
                            </header>
                            <div>
                                <div class="widget-body">
                                    <table class="table table-bordered">
                                        <thead>
                                        <tr>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;ID</th>

                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;类型</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;标题</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;内容</th>


                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;推送时间</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;添加人</th>
                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;更新时间</th>

                                            <th>&nbsp;&nbsp;&nbsp;&nbsp;操作</th>
                                        </tr>
                                        </thead>
                                        <tbody>
                                        <?php if(isset($data)){foreach($data as $v){?>
                                            <tr id="<?=$v['id'];?>">
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['id'];?></td>

                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$type[$v['type']];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['title'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['content'];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=date('Y-m-d H:i:s',$v['start_time']);?></td>

                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$users[$v['add_user']];?></td>
                                                <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=date('Y-m-d H:i:s',$v['update_time']);?></td>

                                                <td>
                                                    <a href="/notice/modify/<?=$v['id'];?>" class="btn btn-default">修改</a>

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
        $('.tooltip-show').tooltip({html:true});
    });
</script>

<?=$this->load->view('footer');?>