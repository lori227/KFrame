<?=$this->load->view('header');?>
<?=$this->load->view('left');?>
		<div id="main" role="main">
			<div id="ribbon">
				<span class="ribbon-button-alignment"> <span id="refresh" class="btn btn-ribbon" data-title="refresh"  rel="tooltip" data-placement="bottom" data-original-title="<i class='text-warning fa fa-warning'></i> Warning! This will reset all your widget settings." data-html="true"><i class="fa fa-refresh"></i></span> </span>

				<ol class="breadcrumb">
					<li>
						权限管理
					</li>
					<li>
						管理员管理
					</li>
				</ol>
			</div>
			<div id="content">
				<div class="row">
					<div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
						<h1 class="page-title txt-color-blueDark"><i class="fa fa-pencil-square-o fa-fw"></i> <?=$actName?></h1>
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
                                        <a data-placement="bottom" href="/auth/index" title="" rel="tooltip" class="button-icon jarviswidget-delete-btn" data-original-title="">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;返回列表&nbsp;&nbsp;</a>&nbsp;&nbsp;&nbsp;&nbsp;
                                    </div>
                                    <span class="widget-icon"> <i class="fa fa-table"></i> </span>
                                    <h2></h2>

                                </header>

                                <div>

                                    <!-- widget content -->
                                    <div class="widget-body">
                                        <form class="form-horizontal" action="" method="post">
                                            <fieldset>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">用户名</label>
                                                    <div class="col-md-6">
                                                        <input class="form-control" placeholder="" type="text" name="username" value="<?=$data['username'];?>">
                                                    </div>
                                                </div>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">真实姓名</label>
                                                    <div class="col-md-6">
                                                        <input class="form-control" placeholder="" type="text" name="realname" value="<?=$data['realname'];?>">
                                                    </div>
                                                </div>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">密码</label>
                                                    <div class="col-md-6">
                                                        <input class="form-control" placeholder="" type="password" name="password" value="<?=$data['password'];?>">
                                                    </div>
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
<script type="text/javascript">
$(function(){
    
    //表单验证
    var $checkoutForm = $('form').validate({
        rules : {
            username : {
                required : true
            },
            realname : {
                required : true
            },
            password : {
                required : true
            }
        },

        // Messages for form validation
        messages : {
            username : {
                required : '请输入用户名'
            },
            realname : {
                required : '请输入真实姓名'
            },
            password : {
                required : '请输入密码'
            }
        },

        errorPlacement : function(error, element) {
            error.insertAfter(element.parent());
        }
    });
})
</script>
<?=$this->load->view('footer');?>