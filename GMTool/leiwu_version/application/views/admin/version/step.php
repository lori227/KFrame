<ul class="v_step">
    <li class="on">
        <dl class="clearfix">
            <dt>1</dt><dd></dd>
        </dl>
        <h6>选择IP集合</h6>
    </li>
    <li <?php if(isset($step) && $step>=2){echo 'class="on"';}?>>
        <dl class="clearfix">
            <dt>2</dt><dd></dd>
        </dl>
        <h6>选择命令</h6>
    </li>
    <li <?php if(isset($step) && $step==3){echo 'class="on"';}?>>
        <dl class="clearfix">
            <dt>3</dt>
        </dl>
        <h6>发送请求</h6>
    </li>
</ul>