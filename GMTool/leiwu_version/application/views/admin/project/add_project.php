<?php $this->load->view('admin/common/header');?>
<?php $this->load->view('admin/common/left');?>
		<div id="main" role="main">
			<div id="ribbon">
				<!--<span class="ribbon-button-alignment">
                    <span id="refresh" class="btn btn-ribbon" data-title="refresh"  rel="tooltip" data-placement="bottom" data-original-title="<i class='text-warning fa fa-warning'></i> Warning! This will reset all your widget settings." data-html="true">
                        <i class="fa fa-refresh"></i>
                    </span>
                </span>-->
			<!--	<ol class="breadcrumb">
					<li>首页</li>
				</ol>-->
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
                                                    <label class="col-md-2 control-label">项目组</label>
                                                    <div class="col-md-3">
                                                        <select class="form-control" name="group_id">
                                                            <?php if(isset($group)){foreach($group as $g){?>
                                                            <option value="<?php echo $g['group_id'];?>"><?php echo $g['name'];?></option>
                                                            <?php }}?>
                                                        </select>
                                                    </div>
                                                </div>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">项目名</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="name"  value="<?php echo $name??null;?>" />
                                                    </div><font color="red">*</font>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">代码仓库</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="repository_url" value="<?php echo $repository_url??null; ?>">
                                                    </div><font color="red">*</font>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">用户名</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="https_username" value="<?php echo $https_username??null; ?>">
                                                    </div><font color="red">*</font>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">密码</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="https_password" value="<?php echo $https_password??null;?>">
                                                    </div><font color="red">*</font>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">代码分支</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="branch" value="<?php echo $branch??null;?>" />
                                                    </div><font color="red">*</font>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">发布路径</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="code_path" value="<?php echo $code_path??null;?>" />
                                                    </div><font color="red">*</font>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">开启灰度发布</label>
                                                    <div class="col-md-3">
                                                       <label><input type="radio" name="is_grayscale_publish" />是</label>
                                                       <label><input type="radio" name="is_grayscale_publish" />否</label>
                                                    </div>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">目录所属用户</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="请输入发布路径所属 linux 用户, agent为 windows 请忽略" type="text" name="code_dir_user" value="<?php echo $code_dir_user??null;?>" />
                                                    </div>
                                                </div>

                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">备注</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="" type="text" name="comment" value="<?php echo $comment??null;?>" />
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
            repository_url : {
                required : true
            },
            https_username : {
                required : true
            },
            https_password : {
                required : true
            },

            branch : {
                required : true
            },
            code_path : {
                required : true
            }
        },

        // Messages for form validation
        messages : {
            name : {
                required : '请输入项目名称'
            },
            repository_url : {
                required : '请输入仓库地址'
            },
            https_username : {
                required : '请输入用户名'
            },
            https_password : {
                required : '请输入密码'
            },

            branch : {
                required : '请输入代码分支'
            },
            code_path : {
                required : '请输入发布路径'
            }
        },

        errorPlacement : function(error, element) {
            error.insertAfter(element.parent());
        }
    });
})
</script>
<?php $this->load->view('admin/common/footer');?>