<?php $this->load->view('admin/common/header');?>
<?php $this->load->view('admin/common/left');?>
		<div id="main" role="main">
			<div id="ribbon">

			</div>
			<div id="content">
				<div class="row">
					<div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
						<h1 class="page-title txt-color-blueDark"><i class="fa fa-pencil-square-o fa-fw"></i> 服务部署-时间选择</h1>
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
                                                        <label class="col-md-2 control-label">执行时间点</label>
                                                        <div class="col-md-6">
                                                            <input class="form-control" placeholder="为空表示当时执行" type="text" name="act_time" readonly  id="senddate" onclick="WdatePicker({el:this,dateFmt:'yyyy-MM-dd HH:mm:ss',minDate:'%y-%M-{%d}'})" maxlength="19" />
                                                        </div>
                                                    </div>
                                                   <!-- <div class="form-group clearfix">
                                                        <label class="col-md-2 control-label">更新ip明细</label>
                                                        <div class="col-md-8">
                                                            <div class="list-group" style="height:300px;overflow-y:auto;">
                                                                <?php /*foreach($list as $li){ */?>
                                                                    <a class="list-group-item" style="padding:5px;">
             channel：<?php /*echo $li['appchannel'];*/?>，name：<?php /*echo $li['appname'];*/?>，type：<?php /*echo $li['apptype'];*/?>，id：<?php /*echo $li['appid'];*/?>，zoneid：<?php /*echo $li['zoneid'];*/?>
                                                                    </a>
                                                                <?php /*}*/?>
                                                            </div>
                                                        </div>
                                                    </div>-->

                                                </div>
                                                <!--分步引导图-->
                                                <?php $this->load->view('admin/version/step');?>


                                            </fieldset>

                                            <div class="form-actions">
                                                <div class="row">
                                                    <div class="col-md-12">
                                                        <button class="btn btn-primary" type="submit">确认给服务器发送</button>
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
    $('#comm_sel').change(function(){//alert($(this).val());
        if($(this).val()==5){
            $('#ver_sel').show();
        }else{
            $('#ver_sel').hide();
        }
    });
})
</script>
<?php $this->load->view('admin/common/footer');?>