# 数据库插件-KFRedis


## 主要功能
	
	virtual KFRedisDriver* Create( const std::string& module, uint32 logicid = 0 ) = 0;
	
	根据模块名(module)和逻辑id(logicid)获得redis操作接口(KFRedisDriver)
	它是线程安全的, 并且每个线程会创建一个连接, 返回的KFRedisDriver也是线程安全的

	
	通过KFRedisDriver接口来操作数据库, 详细请查看接口代码.
	返回的操作结果KFResult< T >是一个智能指针
	内存都是在KFRedis模块中申请, 为了避免跨模块间的内存操作, 所以尽量不要对返回结果有写操作
	
	
## 配置文件
	
	默认配置文件 0_0_redis.setting 不同的渠道需要配置请根据channel和service配置相应的文件.
	
	
 	<Config>
		<!--Name 数据库名字 -->
		<!--Ip 数据库地址 -->
		<!--Port 数据库端口 -->
		<!--User 登录用户名 -->
		<!--Password 登录密码 -->
		<!--MinLogicId 最小逻辑id -->
		<!--MaxLogicId 最大逻辑id -->
	
		<!--登录信息-->
		<Module Name="login">
			<Redis MinLogicId="0" MaxLogicId="0">
				<Write IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
				<Read IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
			</Redis>			 
		</Module>
	
		<!--保存账号信息-->
		<Module Name="account">
			<Redis MinLogicId="0" MaxLogicId="1">
				<Write IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
				<Read IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
			</Redis>			 
		</Module>
	
		<!--公共数据信息-->
		<Module Name="public">
			<Redis MinLogicId="0" MaxLogicId="0">
				<Write IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
				<Read IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
			</Redis>			 
		</Module>
	
		<!--房间数据-->
		<Module Name="room">
			<Redis MinLogicId="0" MaxLogicId="0">
				<Write IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
				<Read IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
			</Redis>			 
		</Module>
	
		<!--关系数据信息-->
		<Module Name="relation">
			<Redis MinLogicId="0" MaxLogicId="0">
				<Write IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
				<Read IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
			</Redis>			 
		</Module>
	
		<!--保存邮件信息-->
		<Module Name="mail">
			<Redis MinLogicId="0" MaxLogicId="0">
				<Write IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
				<Read IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
			</Redis>			 
		</Module>
	
		<!--保存排行榜信息-->
		<Module Name="rank">
			<Redis MinLogicId="0" MaxLogicId="0">
				<Write IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
				<Read IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
			</Redis>			 
		</Module>
	
		<!--帮派信息-->
		<Module Name="guild">
			<Redis MinLogicId="0" MaxLogicId="0">
				<Write IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
				<Read IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
			</Redis>			 
		</Module>
	
		<!--保存全局名字的数据库-->
		<Module Name="name">
			<Redis MinLogicId="0" MaxLogicId="0">
				<Write IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
				<Read IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
			</Redis>			 
		</Module>
	
		<!--保存小区玩家数据信息-->
		<Module Name="data">
			<Redis MinLogicId="0" MaxLogicId="0">
				<Write IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
				<Read IP="127.0.0.1" Port="6379" Password="redis@lori227-="/>
			</Redis>			 
		</Module>
	</Config>


!> 写数据库(Write)和读数据库(Read)两种, 读写分离 都可以配置多个