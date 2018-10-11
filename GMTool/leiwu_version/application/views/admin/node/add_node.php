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
                                                    <label class="col-md-2 control-label">节点IP</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="ip"  value="<?php echo isset($ip)?$ip:null;?>" />
                                                    </div><font color="red">*</font>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">端口</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="port" value="<?php echo isset($port)?$port:null;?>">
                                                    </div><font color="red">*</font>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">token</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="token" value="<?php echo isset($token)?$token:null;?>">
                                                    </div><font color="red">*</font>
                                                </div>


                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">备注</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="comment" value="<?php echo isset($comment)?$comment:null;?>" />
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
            ip : {
                required : true
            },
            port : {
                required : true
            },
            token : {
                required : true
            },
            comment : {
                required : true
            }
        },

        // Messages for form validation
        messages : {
            ip : {
                required : '请输入ip'
            },
            port : {
                required : '请输入端口'
            },
            token : {
                required : '请输入token'
            },
            comment : {
                required : '请输入备注'
            }
        },

        errorPlacement : function(error, element) {
            error.insertAfter(element.parent());
        }
    });
})
</script>
<?php $this->load->view('admin/common/footer');?>