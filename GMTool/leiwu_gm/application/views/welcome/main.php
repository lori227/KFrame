<?php $this->load->view('header');?>
<?php $this->load->view('left');?>
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
                        <th><?=$user['username'];?></th>
                      </tr>
                      <tr>
                        <th width="160">登录IP：</th>
                        <th><?=$this->input->ip_address();?></th>
                      </tr>
                      <tr>
                        <th width="160">上次登录时间:</th>
                        <th><?=date('Y-m-d H:i', $user['last_login_time']);?></th>
                      </tr>
                      <tr>
                        <th width="160">上次登录ip:</th>
                        <th><?=$user['last_login_ip'];?></th>
                      </tr>
                </table>
			</div>

		</div>
<?php $this->load->view('footer');?>