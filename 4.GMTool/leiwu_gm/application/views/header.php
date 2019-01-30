<!DOCTYPE html>
<html lang="en-us">
	<head>
		<meta charset="utf-8">
		<title>雷武游戏-GM平台</title>
		<meta name="description" content="">
		<meta name="author" content="江湖求生">
		
		<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
		<!-- Basic Styles -->
		<link rel="stylesheet" type="text/css" media="screen" href="<?=STATIC_RESULT_SRC;?>css/bootstrap.min.css">
		<link rel="stylesheet" type="text/css" media="screen" href="<?=STATIC_RESULT_SRC;?>css/font-awesome.min.css">

		<!-- 有用-框架样式-->
		<link rel="stylesheet" type="text/css" media="screen" href="<?=STATIC_RESULT_SRC;?>css/smartadmin-production.css">
		<link rel="stylesheet" type="text/css" media="screen" href="<?=STATIC_RESULT_SRC;?>css/smartadmin-skins.css">
       <!-- 非框架样式统一文件-xue-->
        <link rel="stylesheet" type="text/css" media="screen" href="<?=STATIC_RESULT_SRC;?>css/mystyle.css">
		<!-- FAVICONS -->
		<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
		<link rel="icon" href="/favicon.ico" type="image/x-icon">
        <script src="<?=STATIC_RESULT_SRC;?>js/libs/jquery-2.1.4.min.js"></script>
	</head>
	<body class="">
		<!-- HEADER -->
		<header id="header">
			<div id="logo-group">
                <h1>&nbsp;&nbsp;雷武游戏</h1>
			</div>
			<!-- pulled right: nav area -->
			<div class="pull-right">

				<!-- collapse menu button -->
				<div id="hide-menu" class="btn-header pull-right">
					<span> <a href="javascript:void(0);" title="收缩左边栏"><i class="fa fa-reorder"></i></a> </span>
				</div>
				<!-- end collapse menu -->
                <div class="btn-header transparent pull-right">
					<span> <a href="/welcome/editPass" title="修改密码"><i class="fa fa-cog"></i></a> </span>
				</div>
				<!-- logout button -->
				<div id="logout" class="btn-header transparent pull-right">
					<span> <a href="/welcome/logout" title="退出登录"><i class="fa fa-sign-out"></i></a> </span>
				</div>
				<!-- end logout button -->

			</div>
			<!-- end pulled right: nav area -->

		</header>
		<!-- END HEADER -->
 
<div id='mask' style='display: none'>
<div id='loading-mask'></div>
<div id="loading">
    <div class="loading-indicator">
       <img src="/static/img/select2-spinner.gif" width="32" height="32" style="margin-right:8px;float:left;vertical-align:top;"/>
       <br/><span id="loading-msg">Loading ...</span>
    </div>
</div>
</div>