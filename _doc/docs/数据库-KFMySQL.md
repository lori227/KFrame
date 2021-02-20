# 数据库插件-KFMySQL


## 主要功能
	
    virtual KFMySQLDriver* Create( const std::string& module, uint32 logicid = 0 ) = 0;
	
	根据模块名(module)和逻辑id(logicid)获得mysql操作接口(KFMySQLDriver)
	它是线程安全的, 并且每个线程会创建一个连接, 返回的KFMySQLDriver也是线程安全的

	
	通过KFMySQLDriver接口来操作数据库, 详细请查看接口代码.
	返回的操作结果KFResult<T>是一个智能指针
	内存都是在KFMySQL模块中申请, 为了避免跨模块间的内存操作, 所以尽量不要对返回结果有写操作
	
	
## 配置文件
	
	默认配置文件 0_0_mysql.setting 不同的渠道需要配置请根据channel和service配置相应的文件.
	
	<Config>
		<!--Redis数据库配置 -->
		<!--Id 配置id -->
		<!--Database 数据库名字 -->
		<!--Ip 数据库地址 -->
		<!--Port 数据库端口 -->
		<!--User 登录用户名 -->
		<!--Password 登录密码 -->

		<!-- 部署数据库 -->
		<Module Name="deploy">
			<MySQL MinLogicId="0" MaxLogicId="0">
				<Write IP="192.168.1.250" Port="3306" Database="kfdeploy" User="root" Password="mysql@lori227-="/>
				<Read IP="192.168.1.250" Port="3306" Database="kfdeploy" User="root" Password="mysql@lori227-="/>
			</MySQL>			 
		</Module>
	</Config>


!> 写数据库(Write)和读数据库(Read)两种, 读写分离 都可以配置多个