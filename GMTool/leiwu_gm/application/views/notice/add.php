<?php $this->load->view('header');?>
<?php $this->load->view('left');?>

    <script src="<?php echo STATIC_RESULT_SRC;?>js/plugin/summernote/summernote.js"></script>
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
        <div>
            <div class="row">
                <div class="col-xs-12 col-sm-7 col-md-7 col-lg-4">
                    <h1 class="page-title txt-color-blueDark"><i class="fa fa-pencil-square-o fa-fw"></i><?php echo $actNmae;?>公告</h1>
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
                                    <a data-placement="bottom" href="/notice" title="" rel="tooltip" class="button-icon jarviswidget-delete-btn" data-original-title="">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;返回列表&nbsp;&nbsp;</a>&nbsp;&nbsp;&nbsp;&nbsp;
                                </div>
                                <span class="widget-icon"> <i class="fa fa-table"></i> </span>
                                <h2></h2>
                            </header>

                            <div>
                                <!-- widget content -->
                                <div class="widget-body">

                                    <form class="form-horizontal" action="" method="post" onsubmit="return checkData()">
                                        <fieldset>
                                            <div class="form-group">
                                                <label class="col-md-2 control-label">游戏选择</label>
                                                <div class="col-md-6" >
                                                    <select class="form-control" name="game_id">
                                                        <option selected="" disabled="">请选择</option>
                                                        <?php if(isset($games)){foreach($games as $ga){ ?>
                                                            <option value="<?php echo $ga['id'];?>" >&nbsp;&nbsp;<?php echo $ga['name'];?></option>
                                                        <?php }} ?>
                                                    </select>
                                                </div>
                                            </div>
                                        </fieldset>
                                        <fieldset>
                                            <div class="form-group">
                                                <label class="col-md-2 control-label">公告类型</label>
                                                <div class="col-md-6" >
                                                    <select class="form-control" name="type">
                                                        <option selected="" disabled="">请选择</option>
                                                        <option value="1" <?php if(isset($type) && $type=='1'){echo 'selected';}?>>登陆前公告</option>
                                                        <option value="2" <?php if(isset($type) && $type=='2'){echo 'selected';}?>>登陆后公告</option>
                                                        <option value="3" <?php if(isset($type) && $type=='3'){echo 'selected';}?>>跑马灯</option>
                                                    </select>
                                                </div>
                                            </div>
                                        </fieldset>
                                        <fieldset>
                                            <div class="form-group">
                                                <label class="col-md-2 control-label">公告标题</label>
                                                <div class="col-md-6">
                                                    <input class="form-control" placeholder="输入标题" type="text" name="title" value="<?php echo $title??'';?>" maxlength="50" />
                                                </div>
                                            </div>

                                            <div class="form-group">
                                                <label class="col-md-2 control-label">公告内容</label>
                                                <div class="col-md-6">
                                                   <input type="hidden" name="content" id="noticeCon" />
                                                    <div class="summernote"><?php echo $content??'';?></div>
                                                </div>
                                            </div>
                                            <?php if(!isset($is_modify)){?>
                                            <fieldset>
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">推送对象（渠道）</label>
                                                    <div class="col-md-6" >
                                                        <select class="form-control" name="channel_id" >
                                                            <option selected="" disabled="">请选择</option>
                                                            <option value="1" >渠道测试1</option>
                                                        </select>
                                                    </div>
                                                </div>
                                            </fieldset>
                                            <?php }?>
                                            <div class="form-group">
                                                <label class="col-md-2 control-label">推送时间</label>
                                                <div class="col-md-6">
                                                    <input class="form-control" placeholder="时间（格式要对啊）" type="text" name="time1" id="senddate" onClick="WdatePicker({el:this,dateFmt:'yyyy-MM-dd HH:mm:ss',minDate:'%y-%M-{%d}'})" maxlength="19" value="<?php echo $time1??'';?>" readonly  />
                                                </div>
                                            </div>




                                        </fieldset>

                                        <div class="form-actions">
                                            <div class="row">
                                                <div class="col-md-12">
                                                    <button class="btn btn-primary" type="submit"><i class="fa fa-save "></i>&nbsp;确认提交</button>
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
    <script src="<?php echo STATIC_RESULT_SRC;?>js/My97DatePicker/WdatePicker.js"></script>
    <style>.invalid{color:red;}</style>
    <script type="text/javascript">
        $(document).ready(function(){
            $('.summernote').summernote();
            //表单验证
            var $checkoutForm = $('form').validate({
                rules : {
                    game_id : {
                        required : true
                    },
                    type : {
                        required : true
                    },
                    title : {
                        required : true
                    },
                    content : {
                        required : true
                    },
                    time1 : {
                        required : true
                    }
                },
                
                messages : {
                    game_id : {
                        required : '请选择游戏'
                    },
                    type : {
                        required : '请选择类型'
                    },
                    title : {
                        required : '标题不能为空'
                    },
                    content : {
                        required : '内容不能为空'
                    },

                    time1 : {
                        required : '推送时间不能为空'
                    }
                },
                
                errorPlacement : function(error, element) {
                    error.insertAfter(element.parent());
                }
            });
        })

        function checkData(){
            var conStr = $('.summernote').code();//拿去插件的值
            $('#noticeCon').val(conStr);
            console.log(conStr);
            //return false;
        }
    </script>
<?php $this->load->view('footer');?>