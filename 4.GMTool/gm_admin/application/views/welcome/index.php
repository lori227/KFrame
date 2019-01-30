<?=$this->load->view('header');?>
<?=$this->load->view('left');?>
		<!-- END NAVIGATION -->

		<!-- MAIN PANEL -->
		<div id="main" role="main">
			<div id="ribbon">
				<span class="ribbon-button-alignment"> <span id="refresh" class="btn btn-ribbon" data-title="refresh"  rel="tooltip" data-placement="bottom" data-original-title="<i class='text-warning fa fa-warning'></i> Warning! This will reset all your widget settings." data-html="true"><i class="fa fa-refresh"></i></span> </span>

				<ol class="breadcrumb">
					<li>
						首页
					</li>
				</ol>
			</div>
			<div id="content">
                <table class="table">
                      <tr>
                        <th width="160">当前用户：</th>
                        <th><?php echo $username;?></th>
                      </tr>
                      <tr>
                        <th width="160">登录IP：</th>
                        <th><?php echo $ip;?></th>
                      </tr>
                      <tr>
                        <th width="160">当前时间:</th>
                        <th><?php echo date("Y-m-d H:i:s" , time());?></th>
                      </tr>
                      <tr>
                        <th width="160">上次登录时间:</th>
                        <th><?php echo $last_login_ip;?></th>
                      </tr>
                      <tr>
                        <th width="160">上次登录ip:</th>
                        <th><?php echo $last_login_time;?></th>
                      </tr>
                </table>
			</div>

		</div>
<?=$this->load->view('footer');?>