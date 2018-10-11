<?php $this->load->view('header');?>
<?php $this->load->view('left');?>

    <div id="main" role="main">
        <div id="ribbon">
            <span class="ribbon-button-alignment"> <span id="refresh" class="btn btn-ribbon" data-title="refresh"  rel="tooltip" data-placement="bottom" data-original-title="<i class='text-warning fa fa-warning'></i> Warning! This will reset all your widget settings." data-html="true"><i class="fa fa-refresh"></i></span> </span>

            <ol class="breadcrumb">
                <li>
                    首页
                </li>
                <li>
                    平台管理
                </li>
            </ol>
        </div>
        <div id="content">
            <div class="row">
                <div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
                    <h1 class="page-title txt-color-blueDark"><i class="fa fa-pencil-square-o fa-fw"></i>邮件<?php echo $actNmae;?></h1>
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
                                    <a data-placement="bottom" href="/notice" title="" rel="tooltip" class="button-icon jarviswidget-delete-btn" data-original-title="">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;返回列表&nbsp;&nbsp;</a>
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
                                                <label class="col-md-2 control-label">邮件类型</label>
                                                <div class="btn-group col-md-6" >
                                                    <input class="form-control" type="text" value="<?php echo $emailType[$type];?>" disabled />
                                                </div>
                                            </div>
                                        </fieldset>
                                        <?php if($type=='2'){?>
                                        <fieldset>
                                            <div class="form-group">
                                                <label class="col-md-2 control-label">玩家列表</label>
                                                <div class="btn-group col-md-6" >
                                                    <textarea class=" col-md-12" style="height:200px;" placeholder="玩家列表（每行一个玩家ID）" name="playerList"><?php if(isset($redisEmail)){foreach($redisEmail as $ary){echo $ary."\r\n";}}?></textarea>
                                                </div>
                                                <span>共<?php echo isset($redisEmail)?count($redisEmail):0;?></span>
                                            </div>
                                        </fieldset>
                                        <?php } ?>
                                        <fieldset>
                                            <div class="form-group">
                                                <label class="col-md-2 control-label">邮件标题</label>
                                                <div class="col-md-6">
                                                    <input class="form-control" placeholder="输入邮件标题" type="text" name="title" value="<?php echo $title;?>" maxlength="50" />
                                                </div>
                                            </div>

                                            <div class="form-group">
                                                <label class="col-md-2 control-label">邮件内容</label>
                                                <div class="col-md-6">
                                                   <textarea class="from-control col-md-12" placeholder="请输入邮件内容" name="content"><?php echo $content;?></textarea>
                                                </div>
                                            </div>
                                            <div class="form-group">
                                                <label class="col-md-2 control-label">邮件发送时间</label>
                                                <div class="col-md-6">
                                                    <input class="form-control" placeholder="邮件发送时间（格式要对啊）" type="text" value="<?php echo date('Y-m-d H:i:s',$sendtime);?>"  maxlength="19" disabled  />
                                                </div>
                                            </div>
                                            <div class="form-group">
                                                <label class="col-md-2 control-label">邮件附件</label>
                                                <div class="col-md-6">
                                                    <input class="form-control" placeholder="邮件附件内容" type="text"  value="<?php echo htmlspecialchars($reward);?>" readonly />
                                                </div>
                                            </div>
                                            <div class="form-group">
                                                <label class="col-md-2 control-label">发送者姓名</label>
                                                <div class="col-md-6">
                                                    <input class="form-control" placeholder="发送者姓名" type="text"  maxlength="200" value="<?php echo $sendername;?>" disabled />
                                                </div>
                                            </div>
                                     <!--       <div class="form-group">
                                                <label class="col-md-2 control-label">邮件的配置ID</label>
                                                <div class="col-md-6">
                                                    <input class="form-control" placeholder="邮件的配置ID" type="text" name="configId" maxlength="10" value="<?php /*echo $configId;*/?>" disabled />
                                                </div>
                                            </div>-->



                                        </fieldset>
                                        <div class="form-actions">
                                            <div class="row">
                                                <div class="col-md-12">
                                                    <?php if($status==0){ ?><button class="btn btn-info" type="submit"><i class="fa fa-save "></i>&nbsp;修改邮件</button>&nbsp;&nbsp;<?php }?>
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
    <script src="<?php STATIC_RESULT_SRC;?>js/My97DatePicker/WdatePicker.js"></script>
    <script type="text/javascript">
        $(function(){
            //表单验证
            var $checkoutForm = $('form').validate({
                rules : {
                    title : {
                        required : true
                    },
                    content : {
                        required : true
                    },

                    expire : {
                        required : true
                    },
                    sendername : {
                        required : true
                    }
                },
                
                messages : {
                    title : {
                        required : '标题不能为空'
                    },
                    content : {
                        required : '内容不能为空'
                    },
                    expire : {
                        required : '邮件发送时间不能为空'
                    },
                    sendername : {
                        required : '发送者姓名不能为空'
                    }
                },
                
                errorPlacement : function(error, element) {
                    error.insertAfter(element.parent());
                }
            });




        })
    </script>
<?php $this->load->view('footer');?>