<?=$this->load->view('header');?>
<?=$this->load->view('left');?>
    <div role="main" id="main">
        <?=$this->load->view('right-nav', array('navTitle' => array('平台管理', '激活码列表')));?>
    <div id="content">
        <div class="row">
            <div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
                <h1 class="page-title txt-color-blueDark"><i class="fa fa-lg fa-fw fa-list-alt"></i>激活码列表</h1>
            </div>
            <form role="form" class="form-inline" method="post" action="/activationCode/exportNum" id="export" style="margin-top:10px;">
                <div class="form-group sm-input">
                    <span>导出多少条:</span>
                    <input type="text" class="form-control input-sm" name="num"  placeholder="多少个" />
                </div>
                <button type="submit" class="btn btn-default">按量导出</button>
                <a href="/activationCode/exportAll" class="btn btn-success">导出全部</a>
            </form>
        </div>
    <section class="" id="widget-grid">
    <div class="row">
    <article class="col-xs-12 col-sm-12 col-md-12 col-lg-12 sortable-grid ui-sortable">
    <div class="jarviswidget jarviswidget-color-white jarviswidget-sortable" style="" role="widget">

        <header role="heading">
            <div class="jarviswidget-ctrls" role="menu">
                <a data-placement="bottom" href="/activationCode/add" title="" rel="tooltip" class="button-icon jarviswidget-delete-btn">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;生成激活码</a>&nbsp;&nbsp;&nbsp;&nbsp;
            </div>
            <span class="widget-icon"> <i class="fa fa-table"></i> </span>
            <h2>共<?php echo count($list);?>个</h2>
        </header>
        <div>
            <div class="widget-body">
                <div class="list-group">
                    <button type="button" class="list-group-item active">当前总量：<font color="red"><?php echo count($list);?></font></button>
                    <button type="button" class="list-group-item">
                        <span>随机输出100个：</span>
                    <?php if(isset($list)){foreach($list as $key=> $v){?>
                        <?=$v;?>&nbsp;|&nbsp;
                    <?php }}?>
                    </button>
                </div>
            </div>

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