<?=$this->load->view('header');?>
<?=$this->load->view('left');?>
		<div id="main" role="main">
			<div id="ribbon">
				<span class="ribbon-button-alignment"> <span id="refresh" class="btn btn-ribbon" data-title="refresh"  rel="tooltip" data-placement="bottom" data-original-title="<i class='text-warning fa fa-warning'></i> Warning! This will reset all your widget settings." data-html="true"><i class="fa fa-refresh"></i></span> </span>

				<ol class="breadcrumb">
					<li>首页</li>
					<li>平台管理</li>
				</ol>
			</div>
			<div id="content">
				<div class="row">
					<div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
						<h1 class="page-title txt-color-blueDark"><i class="fa fa-pencil-square-o fa-fw"></i>添加IP</h1>
					</div>

				</div>
                <?php if($act == 'success'){ ?>
                <div class="row">
                <article class="col-sm-12">
                    <div class="alert alert-success fade in">
                        <button data-dismiss="alert" class="close">
                            ×
                        </button>
                        <i class="fa-fw fa fa-check"></i>
                        <strong>操作成功</strong>
					</div>
                </article>  
                </div>
                <?php } ?>
                <section class="" id="widget-grid">
                    <div class="row">
                        <article class="col-xs-12 col-sm-12 col-md-12 col-lg-12 sortable-grid ui-sortable">
                            <div class="jarviswidget jarviswidget-color-white jarviswidget-sortable" style="" role="widget">

                                <header role="heading">
                                    <div class="jarviswidget-ctrls" role="menu">
                                        <a data-placement="bottom" href="/" title="" rel="tooltip" class="button-icon jarviswidget-delete-btn" data-original-title="">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;返回列表&nbsp;&nbsp;</a>&nbsp;&nbsp;&nbsp;&nbsp;
                                    </div>
                                    <span class="widget-icon"> <i class="fa fa-table"></i> </span>
                                    <h2></h2>

                                </header>

                                <div>

                                    <!-- widget content -->
                                    <div class="widget-body">
                                        <form class="form-horizontal" action="" method="post" id="codeFrom" onsubmit="return check()">
                                            <fieldset>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">状态</label>
                                                    <div class="col-md-2" >
                                                        <select class="form-control" name="status">
                                                            <option selected="" disabled="">请选择</option>
                                                            <option value="0">关闭</option>
                                                            <option value="1">开启</option>

                                                        </select>
                                                    </div>
                                                </div>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">限制的IP</label>
                                                    <div class="col-md-2">
                                                        <input class="form-control" placeholder="0.0.0.0形式" type="text" name="ip"  maxlength="16"/>
                                                    </div>
                                                </div>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">原因描述</label>
                                                    <div class="col-md-2">
                                                        <textarea class="form-control" placeholder="限制的原因" name="reason"></textarea>
                                                    </div>
                                                </div>
                                            </fieldset>

                                            <div class="form-actions">
                                                <div class="row">
                                                    <div class="col-md-12">
                                                        <button class="btn btn-primary" type="submit"><i class="fa fa-save"></i>&nbsp;确认提交</button>
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
            status : {
                required : true
            },
            ip : {
                required : true
            },
            reason : {
                required : true
            }
        },

        // Messages for form validation
        messages : {
            status : {
                required : '请选择状态'
            },
            ip : {
                required : '请输入ip'
            },
            reason : {
                required : '请输入原因'
            }
        },

        errorPlacement : function(error, element) {
            error.insertAfter(element.parent());
        }
    });
})

</script>
<?=$this->load->view('footer');?>