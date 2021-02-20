# 数据库插件-KFMongo


## 主要功能
	
	virtual KFMongoDriver* Create( const std::string& module, uint32 logicid = 0 ) = 0;
	
	根据模块名(module)和逻辑id(logicid)获得redis操作接口(KFMongoDriver)
	它是线程安全的, 并且每个线程会创建一个连接, 返回的KFMongoDriver也是线程安全的

	
	通过KFMongoDriver接口来操作数据库, 详细请查看接口代码.
	返回的操作结果KFResult<T>是一个智能指针
	内存都是在KFMongo模块中申请, 为了避免跨模块间的内存操作, 所以尽量不要对返回结果有写操作
	
	
## 配置文件
	
	默认配置文件 0_0_mongo.setting 不同的渠道需要配置请根据channel和service配置相应的文件.
	
	
 	<Config>
		<!--Id 配置id -->
		<!--Database 数据库名字 -->
		<!--Ip 数据库地址 -->
		<!--Port 数据库端口 -->
		<!--User 登录用户名 -->
		<!--Password 登录密码 -->
		<!--UseSSL 是否用使用ssl连接 -->
		<!--ConnectTimeout 连接超时时间(毫秒) -->
		<!--ExecuteTimeout 执行超时时间(毫秒) -->
		<!--Auth 认证方式 SCRAM-SHA-1/MONGODB-CR-->

		<!-- 保存玩家数据数据库 -->
		<Module Name="data" ConnectTimeout="10000" ExecuteTimeout="10000" UseSSL="false" Auth="SCRAM-SHA-1">
			<Mongo MinLogicId="1" MaxLogicId="10">
				<Write IP="192.168.2.31" Port="27017" Database="chess" User="root" Password="mongo@lori227-="/>
				<Read IP="192.168.2.31" Port="27017" Database="chess" User="root" Password="mongo@lori227-="/>
			</Mongo>			 
		</Module>
	</Config>


!> 写数据库(Write)和读数据库(Read)两种, 读写分离 都可以配置多个