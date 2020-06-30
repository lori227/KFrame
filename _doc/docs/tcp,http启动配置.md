# tcp, http配置启动配置


## HttpServer配置

配置文件位置: `_bin/setting/httpserver.setting`

		<?xml version='1.0' encoding='utf-8' ?>
		<Config>
			<!-- Type 端口计算类型 0=固定端口 1=由id计算 2=共享内存计算 -->
			<!--Port 端口-->
			<!--KeepAlive 是否保持连接-->
			<!--MaxThread 最大线程数量-->
			<!--MaxQueue 最大队列数量-->
			<!--IdleTime 空闲时间-->
		
			<!-- http服务器配置 -->
			<HttpServer KeepAlive="0" MaxThread="4" MaxQueue="64" IdleTime="1000">
				<!-- deploy 服务器 -->
				<Server AppName="deploy" AppType="server" Type="0" Port="9999"/>
		
				<!-- 日志服务器 -->
				<Server AppName="log" AppType="shard" Type="1" Port="6000"/>
		
				<!-- 认证服务器 -->
				<Server AppName="auth" AppType="shard" Type="1" Port="7000" />
		
				<!-- 目录服务器 -->
				<Server AppName="dir" AppType="shard" Type="1" Port="7500" />
		
				<!-- 邮件服务器 -->
				<Server AppName="mail" AppType="shard" Type="1" Port="8000"/>
		
				<!-- world服务器 -->
				<Server AppName="zone" AppType="world" Type="1" Port="9000"/>	
			</HttpServer>
		</Config>

HttpServer节点的都是默认配置 如KeepAlive, MaxThread等, 如需要差异化,在子节点中添加相应字段即可.

	<Server AppName="auth" AppType="shard" Type="1" Port="7000" />

	appname 程序名字
	
	apptype 程序类型
	
	type 端口类型, 0=固定端口 1=由id计算 2=共享内存计算
	
	Port 端口 如果计算端口, 这个端口是计算的起始端口号

## TcpServer配置

配置文件位置: `_bin/setting/tcpserver.setting`	

	<?xml version='1.0' encoding='utf-8' ?>
	<Config>
		<!-- AppName 服务器名字 -->
		<!-- AppType 服务器类型 -->
		<!-- Type 端口计算类型 0=固定端口 1=由id计算 2=共享内存计算 -->
		<!-- Port 端口 -->
		<!-- InteranetIp 外网ip 127.0.0.1 提供局域网ip  0.0.0.0 提供公网ip -->
		<!-- MaxQueue 最大队列数量 默认20000 -->
		<!-- TimeOut 掉线监测时间 默认20 -->
		<!-- MessageType 消息类型 默认为1  1 服务器之间通讯  2 服务器与客户端之间通讯 -->
		<!-- CompressType 压缩类型 1:lz4 2:zstd -->
		<!-- CompressLevel 压缩等级 -->
		<!-- CompressLength 消息超过设定值大小, 则使用压缩, 默认不填是不压缩 -->
		<!-- OpenEncrypt 是否使用加密 0:不开启  1:开启 -->
		<!-- EncryptKey 加密秘钥 -->
		
		<!-- tcp服务器配置 -->
		<TcpServer CompressType="2" CompressLevel="5" EncryptKey="kframe,./">
			<!-- deploy server 服务器-->
			<Server AppName="deploy" AppType="server" Type="0" Port="10000" MaxQueue="20000"/>
	
			<!-- 通用服务器 -->
			<Server AppName="*" AppType="*" Type="2" Port="12000" MaxQueue="20000"/>
	
			<!-- gate服务器 -->
			<Server AppName="zone" AppType="gate" Type="2" Port="12000" MaxQueue="200" MessageType="2" CompressLength="0" OpenEncrypt="0"/>
		</TcpServer>
	</Config>



	appname 程序名字 * 表示所有程序
	
	apptype 程序类型 * 表示所有类型
	type是端口类型, 0=固定端口 1=由id计算 2=共享内存计算
	
	Port 端口 如果计算端口, 这个端口是计算的起始端口号
	
	MaxQueue 消息队列的最大长度 服务器之间默认20000, 客户端最大200
		
	MessageType 消息类型  1 是服务器之间通讯  2 服务器与客户端之间通讯, 服务器之间消息头会带一些数据,比客户端的消息头要大.



