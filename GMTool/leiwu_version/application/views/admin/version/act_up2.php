<?php $this->load->view('admin/common/header');?>
<?php $this->load->view('admin/common/left');?>
<style>
    .invalid{color:red;}
</style>
		<div id="main" role="main">
            <div id="ribbon"></div>
			<div id="content">
				<div class="row">
					<div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
						<h1 class="page-title txt-color-blueDark"><i class="fa fa-pencil-square-o fa-fw"></i> 服务部署-命令选择</h1>
					</div>
				</div>

                <section class="" id="widget-grid">
                    <div class="row">
                        <article class="col-xs-12 col-sm-12 col-md-12 col-lg-12 sortable-grid ui-sortable">
                            <div class="jarviswidget jarviswidget-color-white jarviswidget-sortable" style="" role="widget">

                                <header role="heading">
                                    <h2></h2>
                                </header>

                                <div>

                                    <!-- widget content -->
                                    <div class="widget-body">
                                        <form class="form-horizontal" action="" method="post">

                                            <fieldset>
                                                <div class="fl"  style="width:800px;margin-left:10%;">
                                                    <div class="form-group clearfix">
                                                        <label class="col-md-2 control-label">服务器命令</label>
                                                        <div class="col-md-6">
                                                            <select class="form-control" name="command" id="comm_sel">
                                                                <option disabled value="" selected>请选择</option>
                                                                <option value="1" <?php echo ($info['command']==1)?'selected':null;?>>正常关闭（shutdown）</option>
                                                                <option value="2" <?php echo ($info['command']==2)?'selected':null;?>>强制关闭（kill）</option>
                                                                <option value="3" <?php echo ($info['command']==3)?'selected':null;?>>启动服务器（startup）</option>
                                                                <option value="4" <?php echo ($info['command']==4)?'selected':null;?>>重新加载launch（launch）</option>
                                                                <option value="5" <?php echo ($info['command']==5)?'selected':null;?>>更新版本（version）</option>
                                                                <option value="6" <?php echo ($info['command']==6)?'selected':null;?>>修改日志级别（log）</option>
                                                            </select>
                                                        </div>
                                                    </div>
                                                    <div class="form-group" id="ver_sel" <?php if($info['command']!=5){echo 'style="display:none;"';}?>>
                                                        <label class="col-md-2 control-label">要更新的版本</label>
                                                        <div class="col-md-6">
                                                            <select class="form-control"  name="version">
                                                                <option disabled value="" selected>请选择</option>
                                                                <?php if(isset($ver)){foreach($ver as $v){ ?>
                                                                    <option value="<?php echo $v['version_name'];?>" <?php echo ($info['value']==$v['version_name'])?'selected':null;?>><?php echo $v['version_name'];?></option>
                                                                <?php }}?>
                                                            </select>
                                                        </div>
                                                    </div>
                                                    <div class="form-group" id="log_sel" <?php if($info['command']!=6){echo 'style="display:none;"';}?>>
                                                        <label class="col-md-2 control-label">日志级别</label>
                                                        <div class="col-md-6">
                                                            <select class="form-control"  name="log_lev">
                                                                <option disabled value="" selected>请选择</option>
                                                                <option value="1" <?php echo ($info['command']==1)?'selected':null;?>>（1）debug</option>
                                                                <option value="2" <?php echo ($info['command']==2)?'selected':null;?>>（2）info</option>
                                                                <option value="3" <?php echo ($info['command']==3)?'selected':null;?>>（3）warn</option>
                                                                <option value="4" <?php echo ($info['command']==4)?'selected':null;?>>（4）error</option>
                                                                <option value="5" <?php echo ($info['command']==5)?'selected':null;?>>（5）critical</option>
                                                            </select>
                                                        </div>
                                                    </div>

                                                    <div class="form-group clearfix">
                                                        <label class="col-md-2 control-label">更新ip明细</label>
                                                        <div class="col-md-8">
                                                            <div class="list-group" style="height:300px;overflow-y:auto;">
                                                                <?php foreach($list as $li){ ?>
                                                                    <a class="list-group-item" style="padding:5px;">
             channel：<?php echo $li['appchannel'];?>，name：<?php echo $li['appname'];?>，type：<?php echo $li['apptype'];?>，id：<?php echo $li['appid'];?>，zoneid：<?php echo $li['zoneid'];?>
                                                                    </a>
                                                                <?php }?>
                                                            </div>
                                                        </div>
                                                    </div>

                                                </div>
                                                <!--分步引导图-->
                                                <?php $this->load->view('admin/version/step');?>


                                            </fieldset>

                                            <div class="form-actions">
                                                <div class="row">
                                                    <div class="col-md-12">
                                                        <button class="btn btn-primary" type="submit">生成待发送</button>
                                                    </div>
                                                </div>
                                            </div>

                                        </form>

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
    <script src="/static/js/My97DatePicker/WdatePicker.js"></script>
<script type="text/javascript">
$(function(){
    //表单验证
    var $checkoutForm = $('form').validate({
        rules : {
            command : {
                required : true
            },
            version : {
                required : true
            }
        },

        // Messages for form validation
        messages : {
            command : {
                required : '请选择命令'
            },
            version : {
                required : '请选择版本'
            }
        },

        errorPlacement : function(error, element) {
            error.insertAfter(element.parent());
        }
    });
    $('#comm_sel').change(function(){//alert($(this).val());
        if($(this).val()==5){
            $('#ver_sel').show();
        }else if($(this).val()==6){
            $('#log_sel').show();
        }else{
            $('#ver_sel').hide();
        }
    });
})
</script>
<?php $this->load->view('admin/common/footer');?>