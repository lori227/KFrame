<?php $this->load->view('admin/common/head');?>
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
					<span> <a href="/admin/admin/editPass" title="修改密码"><i class="fa fa-cog"></i></a> </span>
				</div>
				<!-- logout button -->
				<div id="logout" class="btn-header transparent pull-right">
					<span> <a href="/admin/admin/logout" title="退出登录"><i class="fa fa-sign-out"></i></a> </span>
				</div>
				<!-- end logout button -->

			</div>
			<!-- end pulled right: nav area -->

		</header>
		<!-- END HEADER -->
 
<!--<div id='mask' style='display: none'>
<div id='loading-mask'></div>
<div id="loading">
    <div class="loading-indicator">
       <img src="/static/img/select2-spinner.gif" width="32" height="32" style="margin-right:8px;float:left;vertical-align:top;"/>
       <br/><span id="loading-msg">Loading ...</span>
    </div>
</div>
</div>-->