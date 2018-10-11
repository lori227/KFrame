<?php
$tmpMethod  = $this->router->method == 'add' || $this->router->method == 'edit' ? 'index' : $this->router->method;
$tmpClass   = $this->router->class == 'cost' ? 'channel' : $this->router->class;
$tmpUri = '/' . $tmpClass . '/' . $tmpMethod;
 ?>
<aside id="left-panel">
    <!-- User info -->
    <div class="login-info">
        <span>
            <a href="javascript:void(0);" id="show-shortcut">
                <img src="<?=STATIC_RESULT_SRC;?>img/avatars/sunny.png" alt="me" class="online" /> 
                <span>
                    <?=$this->session->userdata['user_data']['realname'];?>
                </span>
            </a> 
            
        </span>
    </div>
    <nav>
        <ul>
        <?php if($tmpUri == '/welcome/main'){ ?>
            <li class="active">
        <?php }else{ ?>
            <li>
        <?php } ?>
                <a href="/index.php"><i class="fa fa-lg fa-fw fa-list-alt"></i> <span class="menu-item-parent">首页</span></a>
            </li>
        <?php foreach($tree as $nav){ ?>
        <?php if(isset($nav['node'])){ ?>
            <?php if($tmpUri == $nav['uri']){ ?>
                <li class="active">
            <?php }else{ ?>
                <li>
            <?php } ?>    

                    <a href="<?=$nav['uri'];?>"><i class="fa fa-lg fa-fw fa-list-alt"></i> <span class="menu-item-parent"><?=$nav['name'];?></span></a>


                        <ul>
                            <?php foreach($nav['node'] as $k => $v){ ?>
                            <li<?= $tmpUri == $v['uri'] ? ' class="active"' : '';?>>
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