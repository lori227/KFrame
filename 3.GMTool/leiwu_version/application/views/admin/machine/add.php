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
                                    <h2></h2>
                                </header>

                                <div>

                                    <!-- widget content -->
                                    <div class="widget-body">
                                        <form class="form-horizontal" action="" method="post">
                                            <fieldset>
                                                <?php if($act==0){?>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">机器名称</label>
                                                    <div class="col-md-2">
                                                        <input class="form-control" placeholder="添加后不能修改" type="text" name="name" value="<?php echo isset($name)?$name:null;?>" />
                                                    </div><font color="red">*</font>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">localip</label>
                                                    <div class="col-md-2">
                                                        <input class="form-control" placeholder="添加后不能修改" type="text" name="localip" value="<?php echo isset($localip)?$localip:null;?>">
                                                    </div><font color="red">*</font>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">interanetip</label>
                                                    <div class="col-md-2">
                                                        <input class="form-control" placeholder="添加后不能修改" type="text" name="interanetip" value="<?php echo isset($interanetip)?$interanetip:null;?>" />
                                                    </div><font color="red">*</font>
                                                </div>
                                                <?php }?>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">cpu</label>
                                                    <div class="col-md-2">
                                                        <input class="form-control" placeholder="" type="text" name="cpu" value="<?php echo isset($cpu)?$cpu:null;?>" />
                                                    </div>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">thread</label>
                                                    <div class="col-md-2">
                                                        <input class="form-control" placeholder="" type="text" name="thread" value="<?php echo isset($thread)?$thread:null;?>" />
                                                    </div>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">memory</label>
                                                    <div class="col-md-2">
                                                        <input class="form-control" placeholder="" type="text" name="memory" value="<?php echo isset($memory)?$memory:null;?>" />
                                                    </div>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">harddisk</label>
                                                    <div class="col-md-2">
                                                        <input class="form-control" placeholder="" type="text" name="harddisk" value="<?php echo isset($harddisk)?$harddisk:null;?>" />
                                                    </div>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">agentid</label>
                                                    <div class="col-md-2">
                                                        <input class="form-control" placeholder="" type="text" name="agentid" value="<?php echo isset($agentid)?$agentid:null;?>" />
                                                    </div>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">status</label>
                                                    <div class="col-md-2">
                                                        <input class="form-control" placeholder="" type="text" name="status" value="<?php echo isset($status)?$status:null;?>" />
                                                    </div>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">port</label>
                                                    <div class="col-md-2">
                                                        <input class="form-control" placeholder="" type="text" name="port" value="<?php echo isset($port)?$port:null;?>" />
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
            name : {
                required : true
            },
            localip : {
                required : true
            },
            interanetip : {
                required : true
            },
            cpu : {
                required : true
            }
        },

        // Messages for form validation
        messages : {
            name : {
                required : '请输入名称'
            },
            localip : {
                required : '请输入localip'
            },
            interanetip : {
                required : '请输入interanetip'
            },
            cpu : {
                required : '请输入cpu'
            }
        },

        errorPlacement : function(error, element) {
            error.insertAfter(element.parent());
        }
    });
})
</script>
<?php $this->load->view('admin/common/footer');?>