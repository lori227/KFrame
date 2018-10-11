<?php $this->load->view('admin/common/header');?>
<?php $this->load->view('admin/common/left');?>
		<div id="main" role="main">
			<div id="ribbon">

			</div>
			<div id="content">
				<div class="row">
					<div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
						<h1 class="page-title txt-color-blueDark"><i class="fa fa-pencil-square-o fa-fw"></i> <?php if($act==1){echo '修改';}else{echo '添加';}?>数据</h1>
					</div>
				</div>

                <section class="" id="widget-grid">
                    <div class="row">
                        <article class="col-xs-12 col-sm-12 col-md-12 col-lg-12 sortable-grid ui-sortable">
                            <div class="jarviswidget jarviswidget-color-white jarviswidget-sortable" style="" role="widget">

                                <header role="heading">
                                    <h2>（注：appname+apptype唯一性）</h2>
                                </header>

                                <div>

                                    <!-- widget content -->
                                    <div class="widget-body">
                                        <form class="form-horizontal" action="" method="post">
                                            <fieldset>
<?php if($act==0){?>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">appname</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="appname" value="<?php echo isset($appname)?$appname:null;?>">
                                                    </div><font color="red">*</font>
                                                </div>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">apptype</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="apptype" value="<?php echo isset($apptype)?$apptype:null;?>">
                                                    </div><font color="red">*</font>
                                                </div>
<?php }?>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">apppath</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="apppath" value="<?php echo isset($apppath)?$apppath:null;?>" />
                                                    </div>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">appfile</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="appfile" value="<?php echo isset($appfile)?$appfile:null;?>" />
                                                    </div>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">appconfig</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="appconfig" value="<?php echo isset($appconfig)?$appconfig:null;?>" />
                                                    </div>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">logtype</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="logtype" value="<?php echo isset($logtype)?$logtype:null;?>" />
                                                    </div>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">service</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="service" value="<?php echo isset($service)?$service:null;?>" />
                                                    </div>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">ftpid</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="ftpid" value="<?php echo isset($ftpid)?$ftpid:null;?>" />
                                                    </div>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">deploypath</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="deploypath" value="<?php echo isset($deploypath)?$deploypath:null;?>" />
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
<script type="text/javascript">
$(function(){
    //表单验证
    var $checkoutForm = $('form').validate({
        rules : {
            appname : {
                required : true
            },
            apptype : {
                required : true
            },
            apppath : {
                required : true
            },
            appfile : {
                required : true
            },
            appconfig : {
                required : true
            }
        },

        // Messages for form validation
        messages : {
            appname : {
                required : '请输入appname'
            },
            apptype : {
                required : '请输入apptype'
            },
            apppath : {
                required : '请输入apppath'
            },
            appfile : {
                required : '请输入appfile'
            },
            appconfig : {
                required : '请输入appconfig'
            }
        },

        errorPlacement : function(error, element) {
            error.insertAfter(element.parent());
        }
    });
})
</script>
<?php $this->load->view('admin/common/footer');?>