<!DOCTYPE html>
<html lang="en-us">
	<head>
		<meta charset="utf-8">
		<title>GM后台</title>
		<meta name="description" content="">
		<meta name="author" content="">
		<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
		<link rel="stylesheet" type="text/css" media="screen" href="<?=STATIC_RESULT_SRC;?>css/bootstrap.min.css">	
		<link rel="stylesheet" type="text/css" media="screen" href="<?=STATIC_RESULT_SRC;?>css/font-awesome.min.css">

		<link rel="stylesheet" type="text/css" media="screen" href="<?=STATIC_RESULT_SRC;?>css/smartadmin-production.css">
		<link rel="stylesheet" type="text/css" media="screen" href="<?=STATIC_RESULT_SRC;?>css/smartadmin-skins.css">
        <!--登录页专用类文件-->
		<link rel="stylesheet" type="text/css" media="screen" href="<?=STATIC_RESULT_SRC;?>css/lockscreen.css">
		<link rel="shortcut icon" href="img/favicon/favicon.ico" type="image/x-icon">
		<link rel="icon" href="img/favicon/favicon.ico" type="image/x-icon">
		<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
		<link rel="icon" href="/favicon.ico" type="image/x-icon">
        <script src="<?=STATIC_RESULT_SRC;?>js/libs/jquery-2.0.2.min.js"></script>
	</head>
	<body>

		<div id="main" role="main">

			<!-- MAIN CONTENT -->

			<form class="lockscreen animated flipInY" action="/welcome" method="post">
				<div class="logo">
					<h1 class="semi-bold"><img src="<?=STATIC_RESULT_SRC;?>img/logo-o.png" alt="" /> 江湖求生GM后台</h1>
				</div>
				<div>
					<img src="<?=STATIC_RESULT_SRC;?>img/avatars/sunny-big.png" alt="" width="120" height="120" />
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
				<p class="font-xs margin-top-5">GM manage 1.0 2018-6</p>
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
<?php $this->load->view('footer');?>