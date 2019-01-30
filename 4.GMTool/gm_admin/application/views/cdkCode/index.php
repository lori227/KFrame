<?=$this->load->view('header');?>
<?=$this->load->view('left');?>
    <div role="main" id="main">
        <?=$this->load->view('right-nav', array('navTitle' => array('平台管理', 'CDK礼包列表')));?>
    <div id="content">
        <div class="row">
            <div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
                <h1 class="page-title txt-color-blueDark"><i class="fa fa-lg fa-fw fa-list-alt"></i>CDK礼包</h1>
            </div>
          <!--  <form role="form" class="form-inline" method="post" action="/activationCode/exportNum" id="export" style="margin-top:10px;">
                <div class="form-group sm-input">
                    <span>导出多少条:</span>
                    <input type="text" class="form-control input-sm" name="num"  placeholder="多少个" />
                </div>
                <button type="submit" class="btn btn-default">按量导出</button>
                <a href="/activationCode/exportAll" class="btn btn-success">导出全部</a>
            </form>-->
        </div>
    <section class="" id="widget-grid">
    <div class="row">
    <article class="col-xs-12 col-sm-12 col-md-12 col-lg-12 sortable-grid ui-sortable">
    <div class="jarviswidget jarviswidget-color-white jarviswidget-sortable" style="" role="widget">

        <header role="heading">
            <div class="jarviswidget-ctrls" role="menu">

                <a data-placement="bottom" href="/cdkCode/config"  class="button-icon" style="margin-right:30px;">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;CDK礼包配置</a>
                <a data-placement="bottom" href="/cdkCode/add" class="button-icon" style="margin-right:30px;">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;生成CDK礼包</a>
                <a data-placement="bottom" href="/cdkCode/info"  class="button-icon" style="margin-right:30px;">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;礼包码查询</a>
            </div>
            <span class="widget-icon"> <i class="fa fa-table"></i> </span>
            <h2>共<?php echo count($list);?>个</h2>
        </header>
        <div>
            <div class="widget-body">
                <table class="table table-bordered">
                    <thead>
                    <tr>
                        <th>&nbsp;&nbsp;&nbsp;&nbsp;ID</th>
                        <th>&nbsp;&nbsp;&nbsp;&nbsp;礼包ID</th>
                        <th>&nbsp;&nbsp;&nbsp;&nbsp;标题</th>
                        <th>&nbsp;&nbsp;&nbsp;&nbsp;描述</th>
                        <th>&nbsp;&nbsp;&nbsp;&nbsp;使用频次</th>
                        <th>&nbsp;&nbsp;&nbsp;&nbsp;开始时间</th>
                        <th>&nbsp;&nbsp;&nbsp;&nbsp;结束时间</th>
                        <th>&nbsp;&nbsp;&nbsp;&nbsp;添加人</th>
                        <th>&nbsp;&nbsp;&nbsp;&nbsp;更新时间</th>

                        <th>&nbsp;&nbsp;&nbsp;&nbsp;操作</th>
                    </tr>
                    </thead>
                    <tbody>
                    <?php if(isset($list)){foreach($list as $v){?>
                        <tr id="<?=$v['id'];?>">
                            <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['id'];?></td>
                            <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['cdkey'];?></td>
                            <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['title'];?></td>
                            <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$v['describe'];?></td>
                            <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$frequency[$v['frequency']];?></td>
                            <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=date('Y-m-d H:i:s',$v['start_time']);?></td>
                            <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=date('Y-m-d H:i:s',$v['end_time']);?></td>
                            <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=$users[$v['add_user']];?></td>
                            <td>&nbsp;&nbsp;&nbsp;&nbsp;<?=date('Y-m-d H:i:s',$v['update_time']);?></td>
                            <td>
                                <a href="/cdkCode/modifyConfig/<?=$v['id'];?>" class="btn btn-default">修改</a>

                            </td>
                        </tr>
                    <?php }}?>
                    </tbody>
                </table>

            </div>
            <!-- end widget content -->

        </div>
    </div>
    </article>
    </div>
    </section>
    </div>
    </div>
<script type="text/javascript">
$(function(){
    $('.exportBtn').click(function(){

    });
}
</script>
<?=$this->load->view('footer');?>