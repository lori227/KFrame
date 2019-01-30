<?php $this->load->view('admin/common/header');?>
<?php $this->load->view('admin/common/left');?>
		<div id="main" role="main">
			<div id="ribbon">

			</div>
			<div id="content">
				<div class="row">
					<div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
						<h1 class="page-title txt-color-blueDark"><i class="fa fa-pencil-square-o fa-fw"></i> 部署列表-<?php if($act==1){echo '修改';}else{echo '添加';}?>数据</h1>
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
                                            <?php if($act == 0){?>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">appid</label>
                                                    <div class="col-md-2">
                                                        <input class="form-control" placeholder="IP v4格式（唯一）" type="text" name="appid"  value="<?php echo isset($appid)?$appid:null;?>" />
                                                    </div><font color="red">*</font>
                                                </div>
                                            <?php }?>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">appname</label>
                                                    <div class="col-md-2">
                                                        <input class="form-control" placeholder="" type="text" name="appname" value="<?php echo isset($appname)?$appname:null;?>">
                                                    </div><font color="red">*</font>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">apptype</label>
                                                    <div class="col-md-2">
                                                        <input class="form-control" placeholder="" type="text" name="apptype" value="<?php echo isset($apptype)?$apptype:null;?>">
                                                    </div><font color="red">*</font>
                                                </div>


                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">是否启动（startup）</label>
                                                    <div class="col-md-2">
                                                        <select class="form-control" name="startup">
                                                            <option selected value="" disabled>请选择</option>
                                                            <option  value="1" <?php echo (isset($startup)&&$startup==1)?'selected':null;?>>是</option>
                                                            <option  value="0" <?php echo (isset($startup)&&$startup==0)?'selected':null;?>>否</option>
                                                        </select>
                                                    </div>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">是否debug</label>
                                                    <div class="col-md-2">
                                                        <select class="form-control" name="debug">
                                                            <option selected value="" disabled>请选择</option>
                                                            <option  value="1" <?php echo (isset($debug)&&$debug==1)?'selected':null;?>>是</option>
                                                            <option  value="0" <?php echo (isset($debug)&&$debug==0)?'selected':null;?>>否</option>
                                                        </select>
                                                    </div>
                                                </div>



                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">机器列表IP（localip）</label>
                                                    <div class="col-md-2">

                                                        <select class="form-control" name="localip">
                                                            <option selected value="" disabled>请选择</option>
                                                            <?php if(isset($ips)){foreach($ips as $p){?>
                                                                <option value="<?php echo $p['localip'];?>" <?php echo (isset($localip)&&$localip==$p['localip'])?'selected':null;?>><?php echo $p['name'].'-'.$p['localip'];?></option>
                                                            <?php }}?>
                                                        </select>
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
            appid : {
                required : true
            },
            appname : {
                required : true
            },
            apptype : {
                required : true
            },
            startup : {
                required : true
            }
        },

        // Messages for form validation
        messages : {
            appid : {
                required : '请输入appid'
            },
            appname : {
                required : '请输入appname'
            },
            apptype : {
                required : '请输入apptype'
            },
            startup : {
                required : '请选择是否启动'
            }
        },

        errorPlacement : function(error, element) {
            error.insertAfter(element.parent());
        }
    });
})
</script>
<?php $this->load->view('admin/common/footer');?>