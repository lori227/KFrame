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
						<h1 class="page-title txt-color-blueDark"><i class="fa fa-pencil-square-o fa-fw"></i>生成CDK礼包码</h1>
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
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">礼包选择</label>
                                                    <div class="col-md-3" >
                                                        <select class="form-control" name="cid">
                                                            <option selected="" disabled="">请选择</option>
                                                            <?php if(isset($list)){ foreach($list as $li){ ?>
                                                                <option value="<?php echo $li['id'];?>" ><?php echo '<'.$li['cdkey'].'>-'.$li['title'];?></option>
                                                            <?php }} ?>
                                                        </select>
                                                    </div>
                                                </div>
                                          <!--      <div class="form-group">
                                                    <label class="col-md-2 control-label">激活码长度</label>
                                                    <div class="col-md-1">
                                                        <input class="form-control" placeholder="10-20位之间" type="text" name="length" id="length" value="16" />
                                                    </div>
                                                </div>-->
                                                <div class="form-group">
                                                    <label class="col-md-2 control-label">本次生成条数</label>
                                                    <div class="col-md-1">
                                                        <input class="form-control" placeholder="=3000以内" type="text" name="num" id="num" value="10"/>
                                                    </div>
                                                </div>
                                            </fieldset>

                                            <div class="form-actions">
                                                <div class="row">
                                                    <div class="col-md-12">
                                                        <button class="btn btn-primary" type="submit"><i class="fa fa-save"></i>&nbsp;提交并导出</button>
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
   /* var $checkoutForm = $('form').validate({
        rules : {
            name : {
                required : true
            }
        },

        // Messages for form validation
        messages : {
            name : {
                required : '请输入APP名称'
            }
        },

        errorPlacement : function(error, element) {
            error.insertAfter(element.parent());
        }
    });*/
})
function check() {
    var lenStr = $('#length').val();
    var numStr = $('#num').val();
    if(lenStr > 20 || lenStr < 10){
        alert('长度范围是10-20');
        return false;
    }
    if(numStr > 1000){
        alert('本次生成个数不能超过1000');
        return false;
    }
    $('#codeFrom').submit();
}
</script>
<? $this->load->view('footer');?>