<?php $this->load->view('admin/common/head');?>
    <!--登录页专用类文件-->
    <link rel="stylesheet" type="text/css" media="screen" href="/static/css/lockscreen.css">
	<body>

		<div id="main" role="main">

			<!-- MAIN CONTENT -->

			<form class="lockscreen animated flipInY" action="/admin/admin/index" method="post">
				<div class="logo">
					<h1 class="semi-bold"><img src="/static/img/logo-o.png" alt="" /> 雷武游戏-运维后台</h1>
				</div>
				<div>
					<img src="/static/img/avatars/sunny-big.png" alt="" width="120" height="120" />
					<div>
                        
                        <h1>登录</h1>   
                        <div class="input-group">
							<input type="text" placeholder="username" class="form-control" tabindex="1" name="username">
							<div class="input-group-btn">
								<button type="submit" class="btn btn-primary">
									<i class="fa fa-user"></i>
								</button>
							</div>
						</div>
                        <br />
                        <div class="input-group">
							<input type="password" placeholder="Password" class="form-control" tabindex="2" name="password">
							<div class="input-group-btn">
								<button type="submit" class="btn btn-primary">
									<i class="fa fa-key"></i>
								</button>
							</div>
						</div><br />
                        
                        <footer>
                            <button type="submit" class="btn btn-primary">登录</button>
                        </footer>

					</div>

				</div>
				<p class="font-xs margin-top-5">Version manage 1.0 2018-10</p>
			</form>

		</div>
<script type="text/javascript">
    $(function() {
        $("form").validate({
            rules : {
                username : {
                    required : true
                },
                password : {
                    required : true,
                    minlength : 3,
                    maxlength : 20
                }
            },
            messages : {
                username : {
                    required : '请输入用户名'
                },
                password : {
                    required : '请输入密码'
                }
            },
            errorPlacement : function(error, element) {
                error.insertAfter(element.parent());
            }
        });
    });
</script>
<?php $this->load->view('admin/common/footer');?>