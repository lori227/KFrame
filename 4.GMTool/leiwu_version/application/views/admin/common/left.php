<?php
$tmpMethod  = $this->router->method == 'add' || $this->router->method == 'edit' ? 'index' : $this->router->method;
$tmpClass   = $this->router->class == 'cost' ? 'channel' : $this->router->class;
$tmpUri = '/' . $tmpClass . '/' . $tmpMethod;
 ?>
<style>
.menu_list li:hover{background:#367fa9;}
</style>
<aside id="left-panel" >
    <!-- User info -->
    <div class="login-info" style="background: #367fa9;">
        <span>
            <a href="javascript:void(0);" id="show-shortcut">
                <img src="/static/img/avatars/sunny.png" class="" />
                <span style="color:#fff"><?php echo $ci['realname'];?></span>
            </a>
            
        </span>
    </div>
    <nav class="menu_nav">
        <ul>
        <?php if($tmpUri == '/admin/admin/main'){ ?>
            <li class="active">
        <?php }else{ ?>
            <li>
        <?php } ?>
                <a href="/admin/admin/index"><i class="fa fa-lg fa-fw fa-list-alt"></i> <span class="menu-item-parent">首页</span></a>
            </li>
        <?php foreach($tree as $nav){ ?>
        <?php if(isset($nav['node'])){ ?>
                <li <?php if($tmpUri == $nav['uri']){ ?> class="active"<?php } ?>>
                    <a href="<?=$nav['uri'];?>"><i class="fa fa-lg fa-fw fa-list-alt"></i> <span class="menu-item-parent"><?=$nav['name'];?></span></a>
                        <ul class="menu_list" <?php if($ci['fid']==$nav['id']){echo 'style="display:block"';}?>>
                            <?php foreach($nav['node'] as $k => $v){ ?>
                            <li <?php if($ci['mid']==$v['id']){ echo 'class="active"';}?> >
                                <a href="<?=$v['uri'];?>"><i class="fa fa-fw fa-file-text"></i><?=$v['name'];?></a>
                            </li>
                            <?php } ?>
                        </ul>
                </li>
        <?php } ?>
        <?php } ?>    
        </ul>
    </nav>
    <span class="minifyme"> <i class="fa fa-arrow-circle-left hit"></i> </span>
</aside>