		<!-- PACE LOADER - turn this on if you want ajax loading to show (caution: uses lots of memory on iDevices)-->
		<script data-pace-options='{ "restartOnRequestAfter": true }' src="/static/js/plugin/pace/pace.min.js"></script>

		<!-- Link to Google CDN's jQuery + jQueryUI; fall back to local -->
		

		<script src="/static/js/jquery/jquery-ui-1.10.3.min.js"></script>

		<!-- BOOTSTRAP JS -->
		<script src="/static/js/bootstrap/bootstrap.min.js"></script>

		<!-- 暂时注释-xuefeng 20180803 -->
		<!--<script src="/static/js/notification/SmartNotification.min.js"></script>-->

		<!-- JARVIS WIDGETS-系统文件，不能删 x -->
		<!--<script src="/static/js/smartwidgets/jarvis.widget.min.js"></script>-->

		<!-- EASY PIE CHARTS -->
		<script src="/static/js/plugin/easy-pie-chart/jquery.easy-pie-chart.min.js"></script>

		<!-- SPARKLINES -->
		<script src="/static/js/plugin/sparkline/jquery.sparkline.min.js"></script>

		<!-- JQUERY VALIDATE -->
		<script src="/static/js/plugin/jquery-validate/jquery.validate.min.js"></script>

		<!-- JQUERY MASKED INPUT -->
		<script src="/static/js/plugin/masked-input/jquery.maskedinput.min.js"></script>

		<!-- JQUERY SELECT2 INPUT -->
		<script src="/static/js/plugin/select2/select2.min.js"></script>

		<!-- JQUERY UI + Bootstrap Slider -->
		<script src="/static/js/plugin/bootstrap-slider/bootstrap-slider.min.js"></script>

		<!-- browser msie issue fix -->
		<script src="/static/js/plugin/msie-fix/jquery.mb.browser.min.js"></script>

		<!-- FastClick: For mobile devices -->
		<script src="/static/js/plugin/fastclick/fastclick.js"></script>

		<!--[if IE 7]>

		<h1>Your browser is out of date, please update your browser by going to www.microsoft.com/download</h1>

		<![endif]-->



		<!-- 有用-框架js -->
		<script src="/static/js/app.js"></script>
        
        <?php
        if(strpos($_SERVER["QUERY_STRING"], 'authError') !== false){
            showError('没有权限');
        }
        ?>
        <script type="text/javascript">
            $(function(){
                $.ajaxSetup({
                    error:function(XHR, msg, error){
                        if(XHR.status == 405){
                            var tmpUrl = "<?=current_url();?>";
                            if(tmpUrl.indexOf('?') !== -1){
                                location.href = "<?=current_url();?>&authError";
                            }else{
                                location.href = "<?=current_url();?>?authError";
                            }
                        }
                    }
                });
            })
        </script>
	</body>
</html>