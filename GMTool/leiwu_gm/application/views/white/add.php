<? $this->load->view('header');?>
<? $this->load->view('left');?>
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
						<h1 class="page-title txt-color-blueDark"><i class="fa fa-pencil-square-o fa-fw"></i><?php echo isset($is_modify)?'修改':'添加';?>白名单</h1>
					</div>

				</div>
                <?php if(isset($act) && $act == 'success'){ ?>
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
                                                <?php if(!$is_modify){ ?>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">游戏选择</label>
                                                    <div class="col-md-2" >
                                                        <select class="form-control" name="game_id" >
                                                            <option selected="" disabled="">请选择</option>
                                                            <?php if(isset($games)){foreach($games as $ga){ ?>
                                                                <option value="<?php echo $ga['id'];?>" >&nbsp;&nbsp;<?php echo $ga['name'];?></option>
                                                            <?php }} ?>
                                                        </select>
                                                    </div>
                                                </div>
                                                <?php } ?>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">账户ID</label>
                                                    <div class="col-md-2">
                                                        <input class="form-control" placeholder="通行证用户id" type="text" name="account_id" id="account_id"  maxlength="16" value="<?php echo $account_id??null;?>" <?php echo ($is_modify)?'readonly':null ?> />
                                                    </div>
                                                    <span id="acc_msg"></span>
                                                </div>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">原因描述</label>
                                                    <div class="col-md-2">
                                                        <textarea class="form-control" placeholder="添加的原因" name="reason" style="height:80px;"><?php echo $reason??null;?></textarea>
                                                    </div>
                                                </div>

                                                <?php if($is_modify){ ?>
                                                    <div class="form-group">
                                                        <label class="col-md-2 control-label">登录白名单</label>
                                                        <div class="col-md-2" >
                                                            <select class="form-control" name="is_allow_login" >
                                                                <option value="0" >&nbsp;&nbsp;关闭</option>
                                                                <option value="1" <?php if($is_allow_login==1){echo 'selected';}?>>&nbsp;&nbsp;开启</option>
                                                            </select>
                                                        </div>
                                                    </div>
                                                <?php } ?>
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
            game_id : {
                required : true
            },
            account_id : {
                required : true
            },
            reason : {
                required : true
            }
        },

        // Messages for form validation
        messages : {
            game_id : {
                required : '请选择游戏'
            },
            account_id : {
                required : '请输入账户ID'
            },
            reason : {
                required : '请输入原因'
            }
        },

        errorPlacement : function(error, element) {
            error.insertAfter(element.parent());
        }
    });
    
    $('#account_id').blur(function () {

        $.ajax({
            type:"post",
            url:"/white/ajaxAccount",
            data:"account_id="+$(this).val(),
            success:function(msg){
                //alert(msg);
                //var obj = jQuery.parseJSON(msg);
                $('#acc_msg').html(msg);
            }
        });
    });
})

</script>
<? $this->load->view('footer');?>