<?php $this->load->view('admin/common/header');?>
<?php $this->load->view('admin/common/left');?>
		<div id="main" role="main">
			<div id="ribbon">
				<span class="ribbon-button-alignment"> <span id="refresh" class="btn btn-ribbon" data-title="refresh"  rel="tooltip" data-placement="bottom" data-original-title="<i class='text-warning fa fa-warning'></i> Warning! This will reset all your widget settings." data-html="true"><i class="fa fa-refresh"></i></span> </span>

				<ol class="breadcrumb">
					<li>官网管理</li>
					<li>首页banner</li>
				</ol>
			</div>
			<div id="content">
				<div class="row">
					<div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
						<h1 class="page-title txt-color-blueDark"><i class="fa fa-pencil-square-o fa-fw"></i> <?php echo (isset($act)&&($act==1))?'修改':'添加'; ?>banner</h1>
					</div>
				</div>

                <section class="" id="widget-grid">
                    <div class="row">
                        <article class="col-xs-12 col-sm-12 col-md-12 col-lg-12 sortable-grid ui-sortable">
                            <div class="jarviswidget jarviswidget-color-white jarviswidget-sortable" style="" role="widget">

                                <header role="heading">
                                    <div class="jarviswidget-ctrls" role="menu">
                                        <a data-placement="bottom" href="/auth/index" title="" rel="tooltip" class="button-icon jarviswidget-delete-btn" data-original-title="">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;返回列表&nbsp;&nbsp;</a>&nbsp;&nbsp;&nbsp;&nbsp;
                                    </div>
                                    <span class="widget-icon"> <i class="fa fa-table"></i> </span>
                                    <h2></h2>

                                </header>

                                <div>

                                    <!-- widget content -->
                                    <div class="widget-body">
                                        <form class="form-horizontal" action="" method="post"  enctype="multipart/form-data">
                                            <fieldset>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">跳转URL</label>
                                                    <div class="col-md-6">
                                                        <input class="form-control" placeholder="请输入跳转URL" type="text" name="url"  />
                                                    </div>
                                                </div>



                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">上传封面</label>
                                                    <div class="col-md-6">
                                                        <input class="form-control" type="file" name="img" />
                                                    </div>
                                                    （1550*860像素）
                                                </div>
                                            </fieldset>

                                            <div class="form-actions">
                                                <div class="row">
                                                    <div class="col-md-12">
                                                        <button class="btn btn-primary" type="submit">
                                                            <i class="fa fa-save"></i>
                                                            提交
                                                        </button>
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
    <!--日历框控件-->
    <script src="/static/js/My97DatePicker/WdatePicker.js"></script>
<script type="text/javascript">
$(function(){

    //表单验证
    var $checkoutForm = $('form').validate({
        rules : {
            url : {
                required : true
            },
            img : {
                required : true
            },
        },
        messages : {
            url : {
                required : '请输入跳转url'
            },
            img : {
                required : '请选择banner上传'
            },
        },
        errorPlacement : function(error, element) {
            error.insertAfter(element.parent());
        }
    });
})

</script>
<?php $this->load->view('admin/common/footer');?>