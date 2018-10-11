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
			<div>
				<div class="row">
					<div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
						<h1 class="page-title txt-color-blueDark"><i class="fa fa-pencil-square-o fa-fw"></i>配置礼包码</h1>
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
                                        <a data-placement="bottom" href="/cdkCode" rel="tooltip" class="button-icon jarviswidget-delete-btn" data-original-title="">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;返回列表&nbsp;&nbsp;</a>&nbsp;&nbsp;&nbsp;&nbsp;
                                    </div>
                                    <span class="widget-icon"> <i class="fa fa-table"></i> </span>
                                    <h2></h2>

                                </header>

                                <div>

                                    <!-- widget content -->
                                    <div class="widget-body">
                                        <form class="form-horizontal" action="" method="post" id="cdkForm" >
                                            <?php if(!isset($is_modify)){ ?>
                                            <div class="form-group">
                                                <label class="col-md-2 control-label">游戏选择</label>
                                                <div class="col-md-3" >
                                                    <select class="form-control" name="game_id">
                                                        <option selected="" disabled="">请选择</option>
                                                        <?php if(isset($games)){foreach($games as $ga){ ?>
                                                            <option value="<?php echo $ga['id'];?>" >&nbsp;&nbsp;<?php echo $ga['name'];?></option>
                                                        <?php }} ?>
                                                    </select>
                                                </div>
                                            </div>
                                            <?php }?>
                                            <fieldset>
                                                <?php if(isset($act)&&$act==2){ ?>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">礼包ID</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" type="text" name="idKey"  value="<?php echo $cdkey;?>" readonly />
                                                    </div>
                                                    （自动生成，不可修改）
                                                </div>
                                                <?php }?>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">礼包名称</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="请输入礼包名称" type="text" name="title" maxlength="20" value="<?php echo $title??'';?>" />
                                                    </div>
                                                </div>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">礼包描述</label>
                                                    <div class="col-md-3">
                                                        <textarea class="from-control col-md-12" placeholder="请输入描述" name="describe"><?php echo $describe??'';?></textarea>
                                                    </div>
                                                </div>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">有效时间（开始）</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="时间（格式要对啊）" type="text" name="time1"  onClick="WdatePicker({el:this,dateFmt:'yyyy-MM-dd HH:mm:ss',minDate:'%y-%M-{%d}'})" maxlength="19" value="<?php echo $time1??'';?>" readonly  />
                                                    </div>
                                                </div>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">有效时间（结束）</label>
                                                    <div class="col-md-3">
                                                        <input class="form-control" placeholder="时间（格式要对啊）" type="text" name="time2"  onClick="WdatePicker({el:this,dateFmt:'yyyy-MM-dd HH:mm:ss',minDate:'%y-%M-{%d}'})" maxlength="19" value="<?php echo $time2??'';?>" readonly  />
                                                    </div>
                                                </div>
                                                <?php if(!isset($act)){ ?>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">使用频次</label>
                                                    <div class="col-md-3" >
                                                        <select class="form-control" name="frequency">
                                                            <option selected="" disabled="">请选择</option>
                                                            <option value="1" >单码</option>
                                                            <option value="2" >通码</option>
                                                        </select>
                                                    </div>
                                                </div>
                                                <?php }?>
                                            </fieldset>

                                            <div class="form-actions">
                                                <div class="row">
                                                    <div class="col-md-12">
                                                        <button class="btn btn-primary" type="submit"><i class="fa fa-save"></i>&nbsp;提交</button>
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
    <script src="<?=STATIC_RESULT_SRC;?>js/My97DatePicker/WdatePicker.js"></script>
<script type="text/javascript">
$(function(){console.log('44444444');
    //表单验证
    var $checkoutForm = $('#cdkForm').validate({
        rules : {
            game_id : {
                required : true
            },
            idKey : {
                required : true
            },
            title : {
                required : true
            },
            describe : {
                required : true
            },
            time1 : {
                required : true
            },
            time2 : {
                required : true
            },
            frequency : {
                required : true
            }

        },

        // Messages for form validation
        messages : {
            game_id : {
                required : '请选择游戏'
            },
            idKey : {
                required : '请输入ID'
            },
            title : {
                required : '请输入礼包名称'
            },
            describe : {
                required : '请输入描述'
            },
            time1 : {
                required : '请输入开始时间'
            },
            time2 : {
                required : '请输入结束时间'
            },
            frequency : {
                required : '请选择使用频次'
            }
        },

        errorPlacement : function(error, element) {
            error.insertAfter(element.parent());
        }
    });
})

</script>
<? $this->load->view('footer');?>