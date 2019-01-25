<?=$this->load->view('header');?>
<?=$this->load->view('left');?>
    <!-- 模态弹出窗--邮件用户列表 -->
    <div class="modal" id="mymodal-data" tabindex="-1" role="dialog" aria-labelledby="mySmallModalLabel" aria-hidden="true">
        <div class="modal-dialog">
            <div class="modal-content">
                <div class="modal-header">
                    <button type="button" class="close" data-dismiss="modal">
                        <span aria-hidden="true">×</span>
                        <span class="sr-only">Close</span>
                    </button>
                    <h5 class="modal-title">邮件用户列表（每行一个用户）[共<?=count($redisEmail);?>]</h5>
                </div>
                <div class="modal-body">
                    <ul>
                        <?php if(isset($redisEmail)){foreach($redisEmail as $v){?>
                            <li><?php echo $v;?></li>
                        <?php }}?>
                    </ul>
                </div>
            </div>
        </div>
    </div>
    <!-- 模态弹出窗--上传邮件用户 -->
    <div class="modal" id="mymodal-data2" tabindex="-1" role="dialog" aria-labelledby="mySmallModalLabel" aria-hidden="true">
        <div class="modal-dialog">
            <div class="modal-content">
                <div class="modal-header">
                    <button type="button" class="close" data-dismiss="modal">
                        <span aria-hidden="true">×</span>
                        <span class="sr-only">Close</span>
                    </button>
                    <h5 class="modal-title">上传邮件用户（每行一个用户，纯数字）</h5>
                </div>
                <form class="form-horizontal" action="/mail/addPlayerId" id="fileFrom" method="post" >
                <div class="modal-body">
                    <textarea class=" col-md-12" style="height:300px;" placeholder="请输入邮件内容" name="playerList"></textarea>
                </div>
                <div class="modal-footer">
                    <button type="button" class="btn btn-default" data-dismiss="modal">关闭</button>
                    <button type="submit" class="btn btn-primary">保存</button>
                </div>
                </form>
            </div>
        </div>
    </div>
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
                    <h1 class="page-title txt-color-blueDark"><i class="fa fa-pencil-square-o fa-fw"></i>添加邮件</h1>
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
                                    <a data-placement="bottom" href="/notice" title="" rel="tooltip" class="button-icon jarviswidget-delete-btn" data-original-title="">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;返回列表&nbsp;&nbsp;</a>&nbsp;&nbsp;&nbsp;&nbsp;
                                </div>
                                <span class="widget-icon"> <i class="fa fa-table"></i> </span>
                                <h2></h2>
                            </header>

                            <div>
                                <!-- widget content -->
                                <div class="widget-body">
                                <!--    <form class="form-horizontal" action="/mail/upload" id="fileFrom" method="post" enctype="multipart/form-data" >
                                    <fieldset>
                                        <div class="form-group" id="emType" style="display:none">
                                            <label class="col-md-2 control-label">用户列表上传</label>
                                            <div class="col-md-6"><input class="form-control" placeholder="用户上传" type="file" name="userfile" id="userList" size="20"  /></div>
                                            <a class="fl" data-toggle="modal" data-target="#mymodal-data" href="javascript:void(0)">预览</a>
                                        </div>
                                    </fieldset>
                                    </form>-->
                                    <form class="form-horizontal" action="" method="post" id="mailForm">
                                        <fieldset>
                                            <div class="form-group">
                                                <label class="col-md-2 control-label">邮件类型</label>
                                                <div class="btn-group col-md-6" >
                                                    <label><input  type="radio" class="emailType" name="type" value="1" checked />GM系统邮件</label>&nbsp;&nbsp;
                                                    <label><input  type="radio"  class="emailType" name="type" value="2"  />用户补偿邮件</label>&nbsp;
                                                    （<label><input  type="checkbox" name="toolGive" id="toolGive" value="0" disabled />道具发放</label>）&nbsp;&nbsp;
                                                    <a data-toggle="modal" data-target="#mymodal-data2" href="javascript:void(0)">（上传）</a>
                                                    <a data-toggle="modal" data-target="#mymodal-data" href="javascript:void(0)">（预览）</a>
                                                </div>
                                            </div>
                                        </fieldset>
                                        <fieldset>
                                            <div class="form-group">
                                                <label class="col-md-2 control-label">邮件标题</label>
                                                <div class="col-md-6">
                                                    <input class="form-control" placeholder="输入邮件标题" type="text" name="title" value="" maxlength="50" />
                                                </div>
                                            </div>

                                            <div class="form-group">
                                                <label class="col-md-2 control-label">邮件内容</label>
                                                <div class="col-md-6">
                                                   <textarea class="from-control col-md-12" placeholder="请输入邮件内容" name="content"></textarea>
                                                </div>
                                            </div>
                                            <div class="form-group">
                                                <label class="col-md-2 control-label">邮件发送时间</label>
                                                <div class="col-md-6">
                                                    <input class="form-control" placeholder="邮件发送时间（格式要对啊）" type="text" name="expire" id="senddate" onClick="WdatePicker({el:this,dateFmt:'yyyy-MM-dd HH:mm:ss',minDate:'%y-%M-{%d}'})" maxlength="19" readonly  />
                                                </div>
                                            </div>

                                            <div class="form-group">
                                                <label class="col-md-2 control-label">发送者姓名</label>
                                                <div class="col-md-6">
                                                    <input class="form-control" placeholder="发送者姓名" type="text" name="sendername" maxlength="200" />
                                                </div>
                                            </div>


                                            <div class="form-group">
                                                <label class="col-md-2 control-label">邮件的配置ID</label>
                                                <div class="col-md-6">
                                                    <input class="form-control" placeholder="邮件的配置ID" type="text" name="configId" maxlength="10" />
                                                </div>
                                            </div>
                                        </fieldset>
                                        <fieldset id="tool_info" style="display: none">
                                            <div class="form-group">
                                                <label class="col-md-2 control-label">邮件附件（道具码）</label>
                                                <div class="col-md-6">
                                                    <input class="form-control" placeholder="（请操作道具配置选项）" type="text" name="remark" id="toolCode" maxlength="200" readonly />
                                                </div>
                                            </div>
                                            <div class="form-group toolDiv"  >
                                               <label class="col-md-2 control-label">发放道具配置</label>
                                               <div class="col-md-3">
                                                   <select class="form-control" id="tool_id_0"">
                                                       <option value="0" selected="" disabled="">道具选择_0</option>
                                                       <option value="1" disabled>clothes</option>
                                                       <?php if(isset($clothes)){foreach($clothes as $cl){ ?>
                                                           <option value="<?php echo $cl['Id'];?>" >&nbsp;&nbsp;<?php echo $cl['Name'];?></option>
                                                        <?php }} ?>
                                                           <option value="2" disabled>item</option>
                                                       <?php if(isset($item)){foreach($item as $cl){ ?>
                                                           <option value="<?php echo $cl['Id'];?>" >&nbsp;&nbsp;<?php echo $cl['Name'];?></option>
                                                       <?php }} ?>
                                                   </select>
                                               </div>
                                                <div class="col-md-1">
                                                    <input class="form-control" placeholder="道具数量_0" type="text" id="tool_num_0" maxlength="1" />
                                                </div>
                                                <div class="col-md-2">
                                                    <input class="form-control" placeholder="道具使用时长（天）_0" type="text" id="tool_expire_0" maxlength="2" />
                                                </div>
                                                <a id="addToolBtn" href="javascript:void(0)">添加</a> <a id="createToolCode" href="javascript:void(0)">生成道具码</a>
                                            </div>
                                        </fieldset>
                                        <div class="form-actions">
                                            <div class="row">
                                                <div class="col-md-12">
                                                    <button class="btn btn-primary" type="submit"><i class="fa fa-save "></i>&nbsp;生成待发送</button>
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
    <!--<script src="<?/*=STATIC_RESULT_SRC;*/?>js/libs/jquery-1.8.3.min.js"></script>-->
    <script type="text/javascript">
        $(document).ready(function(){
            //表单验证
            var $checkoutForm = $('#mailForm').validate({
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
            //道具发放
            $('.emailType').click(function(){
                if($(this).val() == "1"){
                    $('#toolGive').attr('disabled','disabled');
                    $('#toolGive').removeAttr('checked','checked');
                }else{
                    $('#toolGive').removeAttr('disabled');
                }
            });
            //“道具发放”复选框
            $('#toolGive').click(function(){
                if(this.checked){
                    $(this).val(1);
                    $('#tool_info').show();
                }else{
                    $(this).val(0);
                    $('#tool_info').hide();
                }
            });




            //删除
            $(document).on("click",'.toolExtra',function(){
                $(this).parent().remove();
            });
            //多个道具
            $('#addToolBtn').on("click",function(){
                var lenInt = parseInt($('.toolDiv').length);
                if(lenInt >= 3){
                    alert('道具添加超出限额');
                    return false;
                }
                var optStr = "";
                $.ajax({
                    url:"/mail/ajaxTool",
                    success:function(msg){
                        var obj = jQuery.parseJSON(msg);
                        $.each(obj,function(i,n){
                            optStr += '<option value="'+ n.Id+'" >'+n.Name+'</option>';
                        });
                        $('.tool_sel').append(optStr);//生成道具列表
                    }
                });
                //console.log(optStr);
                var toolStr = '<div class="form-group toolDiv" ><label class="col-md-2"></label>';
                toolStr += '<div class="col-md-3"><select class="form-control tool_sel" id="tool_id_'+lenInt+'"><option value="0" selected="" disabled="">请选择</option>'+optStr+'</select></div>';
                toolStr += '<div class="col-md-1"><input class="form-control" placeholder="道具数量" type="text" id="tool_num_'+lenInt+'" maxlength="1" /></div>';
                toolStr+= "<div class='col-md-2'><input class='form-control' type='text' id='tool_expire_"+lenInt+"' maxlength='2' /></div>";
                toolStr += '<a class="toolExtra">删除</a></div>';
                $('#tool_info').append(toolStr);

            });

            //生成道具码
            $('#createToolCode').click(function(){
                var lenInt = parseInt($('.toolDiv').length);
                var dAry = new Array();
                for(var i=0;i<lenInt;i++){
                    var idStr = $.trim($('#tool_id_'+i).val());
                    var numStr = $.trim($('#tool_num_'+i).val());
                    var timeStr = $.trim($('#tool_expire_'+i).val());
                    dAry[i] = [idStr,numStr,timeStr];
                }
                var par = dAry.join('|');
                //console.log(par);
                $.ajax({
                    url:"/mail/getToolCode?par="+par,
                    success:function(data){
                        var obj = jQuery.parseJSON(data);
                        if(obj.code > 0){
                            alert(obj.msg);
                        }
                        $('#toolCode').val(obj.data);
                        console.log(data);
                    }
                });
            });






        })
    </script>
<?=$this->load->view('footer');?>