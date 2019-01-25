<? $this->load->view('header');?>
<? $this->load->view('left');?>

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
                                    <a data-placement="bottom" href="/mail" title="" rel="tooltip" class="button-icon jarviswidget-delete-btn" data-original-title="">&nbsp;&nbsp;<i class="fa fa-plus"></i>&nbsp;返回列表&nbsp;&nbsp;</a>
                                </div>

                                <span class="widget-icon"> <i class="fa fa-table"></i> </span>
                                <h2><span style="color:red;">（注意：邮件一旦发送出去不可撤回/驳回）</span></h2>
                            </header>

                            <div>
                                <!-- widget content -->
                                <div class="widget-body form-horizontal">
                                   <!-- <form class="" action="" method="post">-->
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
                                                    <textarea class=" col-md-12" style="height:200px;" placeholder="玩家列表（每行一个玩家ID）" name="playerList" disabled><?php if(!empty($redisEmail)){foreach($redisEmail as $ary){echo $ary."\r\n";}}?></textarea>
                                                </div>
                                                <span>共<?php echo isset($redisEmail)?count($redisEmail):0;?></span>
                                            </div>
                                        </fieldset>
                                        <?php } ?>
                                        <fieldset>
                                            <div class="form-group">
                                                <label class="col-md-2 control-label">邮件标题</label>
                                                <div class="col-md-6">
                                                    <input class="form-control" placeholder="输入邮件标题" type="text" value="<?php echo $title;?>" disabled />
                                                </div>
                                            </div>

                                            <div class="form-group">
                                                <label class="col-md-2 control-label">邮件内容</label>
                                                <div class="col-md-6">
                                                   <textarea class="from-control col-md-12" placeholder="请输入邮件内容" name="content" disabled><?php echo $content;?></textarea>
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
                                    <!--        <div class="form-group">
                                                <label class="col-md-2 control-label">邮件的配置ID</label>
                                                <div class="col-md-6">
                                                    <input class="form-control" placeholder="邮件的配置ID" type="text" name="configId" maxlength="10" value="<?php /*echo $configId;*/?>" disabled />
                                                </div>
                                            </div>-->
                                            <div class="form-group">
                                                <label class="col-md-2 control-label">添加人/时间</label>
                                                <div class="col-md-6">
                                                    <input class="form-control" type="text" value="<?php echo $users[$add_user].'/'.date('Y-m-d H:i:s',$add_time);?>" disabled />
                                                </div>
                                            </div>



                                        </fieldset>
                                        <div class="form-actions">
                                            <div class="row">

                                                <div class="col-md-12">
                                                    <?php if($status==0){ ?>
                                                        <form action="/mail/del" method="post" onsubmit="return delAct()">
                                                            <input type="hidden" name="eid" value="<?php echo $id;?>" />
                                                            <button class="btn btn-danger fl" type="submit"><i class="fa fa-save "></i>&nbsp;删除邮件</button>
                                                        </form>
                                                        <form action="/mail/send" method="post" onsubmit="return conAct()">
                                                            <input type="hidden" name="eid" value="<?php echo $id;?>" />
                                                            <button class="btn btn-warning fr" type="submit"><i class="fa fa-save "></i>&nbsp;发送邮件</button>
                                                        </form>
                                                    <?php }?>

                                                </div>
                                            </div>
                                        </div>

                                 <!--   </form>-->

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
        function conAct(){
            var con = confirm('发送出去的邮件不可驳回，确认是否发送？');
            if(con){
                return true;
            }else{
                return false;
            }
        }
        function delAct(){
            var con = confirm('确认删除？');
            if(con){
                return true;
            }else{
                return false;
            }
        }
        $(function(){
           // confirm('发送出去的邮件不可驳回，确认是否发送？');




        })
    </script>
<? $this->load->view('footer');?>