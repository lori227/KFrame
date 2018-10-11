<?php $this->load->view('admin/common/header');?>
<?php $this->load->view('admin/common/left');?>
		<div id="main" role="main">
			<div id="ribbon"></div>
			<div id="content">
				<div class="row">
					<div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
						<h1 class="page-title txt-color-blueDark"><i class="fa fa-pencil-square-o fa-fw"></i> 服务部署-IP选择</h1>
					</div>
				</div>
                <form action="/admin/version/submit_up1" method="post" id="act_form">
                    <input type="hidden" name="ips" id="ips" />
                    <input type="hidden" name="remark" id="remark" />
                </form>
                <section class="" id="widget-grid">
                    <div class="row">
                        <article class="col-xs-12 col-sm-12 col-md-12 col-lg-12 sortable-grid ui-sortable">
                            <div class="jarviswidget jarviswidget-color-white jarviswidget-sortable" style="" role="widget">

                                <header role="heading">
                                    <h2>部署列表树形图（deploy表）</h2>
                                </header>
                                <div>
                                    <!-- widget content -->
                                    <div class="widget-body clearfix">


                                        <div class="tree well fl">
                                            <dt class="list-group-item active">1：树形图勾选IP</dt>
                                            <ul >
                                                <!--第一层-开始-->
                                                <?php if(isset($channel)){foreach($channel as $gr){ ?>
                                                <li >
                                                    <span class="lev1">
                                                        <i class="fl fa fa-chevron-right"></i>
                                                        <label class="tr1"><input type="checkbox" />&nbsp;<?php echo $gr['appchannel'];?></label>
                                                    </span>
                                                    <a>（appchannel字段）</a>
                                                    <ul class="tr_2">
                                                        <!--第二层-开始-->
                                                        <?php if(isset($gr['name'])){foreach($gr['name'] as $n){ ?>
                                                        <li class="parent_li">
                                                            <span>
                                                                <i class="fl fa fa-chevron-right"></i>
                                                                <label class="tr2">
                                                                    <em class="glyphicon glyphicon-minus-sign"><input type="checkbox" class="tr_i2"/></em>
                                                                    <h8>&nbsp;<?php echo $n['appname'];?></h8>
                                                                </label>
                                                            </span>

                                                            <ul class="tr_last">
                                                                <!--第三层-开始-->
                                                                <?php if(isset($n['list'])){foreach($n['list'] as $li){ ?>
                                                                <li>
                                                                    <span>
                                                                        <label><input type="checkbox" class="tr_i3" value="<?php echo $li['appid'];?>"/><h8>&nbsp;<?php echo $li['apptype'];?></h8></label>
                                                                    </span>
                                                                    <a>IP：<?php echo $li['appid'];?></a>
                                                                </li>
                                                                <?php }}?>
                                                                <!--第三层-结束-->
                                                            </ul>
                                                        </li>
                                                        <?php }}?>
                                                        <!--第二层-结束-->
                                                    </ul>
                                                </li>
                                                <?php }}?>
                                                <!--第一层-结束-->
                                            </ul>
                                        </div><!--tree结束-->


                                        <div class="fl" style="margin-left:50px;">
                                            <div class="well fl" style="min-height:200px;">
                                                <dl class="list-group">
                                                    <dt class="list-group-item active">2：已选中APP ID列表</dt>
                                                    <div id="checked_t3"></div>
                                                </dl>
                                            </div>
                                            <div class="well fl" style="margin-left:50px;">
                                                <dl class="list-group">
                                                    <dt class="list-group-item active">3：操作备注</dt>
                                                    <textarea id="remark_val" class="form-control" placeholder="本次操作备注" style="height:100px;"></textarea>
                                                </dl>
                                            </div>

                                        </div>

                                        <!--分步引导图-->
                                        <?php $this->load->view('admin/version/step');?>
                                    </div>
                                    <!-- end widget content -->
                                    <a class="btn btn-primary fr" style="margin:25px 15px;;" onclick="submit()">确认添加目标IP</a>

                                </div>
                                <!-- end widget div -->

                            </div>
                        </article>

                    </div>
                </section>
			</div>
		</div>

<script type="text/javascript">
    $(function () {
        //一级菜单控制
        $('.lev1>.fa').on('click', function (e) {
            var children = $(this).parents('.lev1').siblings('.tr_2').find('.parent_li');
            if (children.is(":visible")) {
                children.hide('fast');
                $(this).addClass('fa-chevron-right').removeClass('fa-chevron-down');
            } else {
                children.show('fast');
                $(this).addClass('fa-chevron-down').removeClass('fa-chevron-right');
            }
            e.stopPropagation();
        });
        //二级菜单控制
        $('.parent_li').find('.fa').on('click', function (e) {
            var children = $(this).parents('.parent_li').find(' > ul > li');
            if (children.is(":visible")) {
                children.hide('fast');
                $(this).addClass('fa-chevron-right').removeClass('fa-chevron-down');
            } else {
                children.show('fast');
                $(this).addClass('fa-chevron-down').removeClass('fa-chevron-right');
            }
            e.stopPropagation();
        });
        //一级-父类-》勾选子类
        $('.tr1').click(function(){
            if($(this).find('input[type=checkbox]').prop('checked')){
                $(this).parent('span').siblings('.tr_2').find('input[type=checkbox]').prop('checked',true);
            }else{
                $(this).parent('span').siblings('.tr_2').find('input[type=checkbox]').prop('checked',false);
            }
            tri3();//遍历结果
        });
        //二级-父类-》勾选子类
        $('.tr2').click(function(){
            if($(this).find('input[type=checkbox]').prop('checked')){
                $(this).parent('span').siblings('.tr_last').find('.tr_i3').prop('checked',true);
            }else{
                $(this).parent('span').siblings('.tr_last').find('.tr_i3').prop('checked',false);
                $(this).find('.tr_i2').css('z-index','0');
            }
            tri3();//遍历结果
        });
        //子类-》勾选父类
        $('.tr_last').find('span').click(function(){
            var p_len = $(this).parents('.tr_last').children('li').length;//同类长度
            var all_len = 0;//选中的量
            for(var i=0;i<p_len;i++){
                if($(this).parents('.tr_last').find('.tr_i3').eq(i).prop('checked')){
                    all_len++;
                    console.log(p_len+'-------'+i);
                }
            }
            var ch_obj = $(this).parents('.tr_last').siblings('span').find('.tr_i2');//checkbox对象
            if(all_len == p_len){
                ch_obj.prop('checked',true).css('z-index','0');//勾选父类
                console.log('全部选中');
            }
            if(all_len >0 && all_len<p_len){
                ch_obj.prop('checked',true).css('z-index','-1');
                console.log('部分选中');
            }
            if(all_len == 0){
                ch_obj.prop('checked',false).css('z-index','0');//取消父类
                console.log('都没选中');
            }
            tri3();//遍历结果
        });
    });

//遍历所有第三级checkbox
var ips = "";
function tri3(){
    $('#checked_t3>dd').remove();
    ips = "";//重置
    for(var a=0;a<$('.tr_i3').length;a++){

        if($('.tr_i3').eq(a).prop('checked')){
            var id = $('.tr_i3').eq(a).attr('value');
            ips += id+'*';
            $('#checked_t3').append('<dd class="list-group-item">'+id+'</dd>');
        }
        console.log('====='+ips);

    }
}
//表单提交
function submit(){
    if(ips == ""){
        alert('没勾选IP不能提交');return false;
    }
    $('#ips').val(ips);
    $('#remark').val($('#remark_val').val());
    $('#act_form').submit();
}
</script>
<?php $this->load->view('admin/common/footer');?>