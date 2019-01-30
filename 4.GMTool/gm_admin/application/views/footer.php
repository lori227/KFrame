		<!-- PACE LOADER - turn this on if you want ajax loading to show (caution: uses lots of memory on iDevices)-->
		<script data-pace-options='{ "restartOnRequestAfter": true }' src="<?=STATIC_RESULT_SRC;?>js/plugin/pace/pace.min.js"></script>

		<!-- Link to Google CDN's jQuery + jQueryUI; fall back to local -->
		

		<script src="<?=STATIC_RESULT_SRC;?>js/libs/jquery-ui-1.10.3.min.js"></script>

		<!-- BOOTSTRAP JS -->
		<script src="<?=STATIC_RESULT_SRC;?>js/bootstrap/bootstrap.min.js"></script>

		<!-- CUSTOM NOTIFICATION -->
		<script src="<?=STATIC_RESULT_SRC;?>js/notification/SmartNotification.min.js"></script>

		<!-- JARVIS WIDGETS-系统文件，不能删 x -->
		<script src="<?=STATIC_RESULT_SRC;?>js/smartwidgets/jarvis.widget.min.js"></script>

		<!-- EASY PIE CHARTS -->
		<script src="<?=STATIC_RESULT_SRC;?>js/plugin/easy-pie-chart/jquery.easy-pie-chart.min.js"></script>

		<!-- SPARKLINES -->
		<script src="<?=STATIC_RESULT_SRC;?>js/plugin/sparkline/jquery.sparkline.min.js"></script>

		<!-- JQUERY VALIDATE -->
		<script src="<?=STATIC_RESULT_SRC;?>js/plugin/jquery-validate/jquery.validate.min.js"></script>

		<!-- JQUERY MASKED INPUT -->
		<script src="<?=STATIC_RESULT_SRC;?>js/plugin/masked-input/jquery.maskedinput.min.js"></script>

		<!-- JQUERY SELECT2 INPUT -->
		<script src="<?=STATIC_RESULT_SRC;?>js/plugin/select2/select2.min.js"></script>

		<!-- JQUERY UI + Bootstrap Slider -->
		<script src="<?=STATIC_RESULT_SRC;?>js/plugin/bootstrap-slider/bootstrap-slider.min.js"></script>

		<!-- browser msie issue fix -->
		<script src="<?=STATIC_RESULT_SRC;?>js/plugin/msie-fix/jquery.mb.browser.min.js"></script>

		<!-- FastClick: For mobile devices -->
		<script src="<?=STATIC_RESULT_SRC;?>js/plugin/fastclick/fastclick.js"></script>

		<!--[if IE 7]>

		<h1>Your browser is out of date, please update your browser by going to www.microsoft.com/download</h1>

		<![endif]-->



		<!-- 有用-框架js -->
		<script src="<?=STATIC_RESULT_SRC;?>js/app.js"></script>
		
		<!-- PAGE RELATED PLUGIN(S) -->
		
		<!-- Flot Chart Plugin: Flot Engine, Flot Resizer, Flot Tooltip -->
	<!--	<script src="<?/*=STATIC_RESULT_SRC;*/?>js/plugin/flot/jquery.flot.cust.js"></script>
		<script src="<?/*=STATIC_RESULT_SRC;*/?>js/plugin/flot/jquery.flot.resize.js"></script>
		<script src="<?/*=STATIC_RESULT_SRC;*/?>js/plugin/flot/jquery.flot.tooltip.js"></script>-->
		
		<!-- Vector Maps Plugin: Vectormap engine, Vectormap language -->
	<!--	<script src="<?/*=STATIC_RESULT_SRC;*/?>js/plugin/vectormap/jquery-jvectormap-1.2.2.min.js"></script>
		<script src="<?/*=STATIC_RESULT_SRC;*/?>js/plugin/vectormap/jquery-jvectormap-world-mill-en.js"></script>-->
		
		<!-- Full Calendar -->
		<!--<script src="<?/*=STATIC_RESULT_SRC;*/?>js/plugin/fullcalendar/jquery.fullcalendar.min.js"></script>-->
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