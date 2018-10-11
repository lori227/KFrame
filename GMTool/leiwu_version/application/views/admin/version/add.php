<?php $this->load->view('admin/common/header');?>
<?php $this->load->view('admin/common/left');?>
		<div id="main" role="main">
			<div id="ribbon">

			</div>
			<div id="content">
				<div class="row">
					<div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
						<h1 class="page-title txt-color-blueDark"><i class="fa fa-pencil-square-o fa-fw"></i> <?php if($act==1){echo '修改';}else{echo '添加';}?>项目</h1>
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

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">时间</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="version_time" readonly  value="<?php echo isset($version_time)?$version_time:null;?>" id="senddate" onclick="WdatePicker({el:this,dateFmt:'yyyy-MM-dd HH:mm:ss',minDate:'%y-%M-{%d}'})" maxlength="19" />
                                                    </div><font color="red">*</font>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">版本名称</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="版本名称（唯一）" type="text" name="version_name" value="<?php echo isset($version_name)?$version_name:null;?>">
                                                    </div><font color="red">*</font>
                                                </div>

                                               <!-- <div class="form-group">
                                                    <label class="col-md-2 control-label">URL</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="version_url" value="<?php /*echo isset($version_url)?$version_url:null;*/?>">
                                                    </div><font color="red">*</font>
                                                </div>-->


                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">version_md5</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="version_md5" value="<?php echo isset($version_md5)?$version_md5:null;?>" />
                                                    </div>
                                                </div>
                                            </fieldset>

                                            <div class="form-actions">
                                                <div class="row">
                                                    <div class="col-md-12">
                                                        <button class="btn btn-primary" type="submit"><i class="fa fa-save"></i>提交</button>
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
            version_time : {
                required : true
            },
            version_name : {
                required : true
            },
      /*      version_url : {
                required : true
            },*/
            version_md5 : {
                required : true
            }
        },

        // Messages for form validation
        messages : {
            version_time : {
                required : '请输入时间'
            },
            version_name : {
                required : '请输入名称'
            },
       /*     version_url : {
                required : '请输入URL'
            },*/
            version_md5 : {
                required : '请输入version_md5'
            }
        },

        errorPlacement : function(error, element) {
            error.insertAfter(element.parent());
        }
    });
})
</script>
<?php $this->load->view('admin/common/footer');?>