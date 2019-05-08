# Ip地址插件


## 主要功能
	
	1. 负责提供进程所在物理机的局域网ip和互联网ip
	2. 负责计算进程(tcp和http服务器)监听端口
	3. 负责提供master服务器的ip和端口
	4. 负责提供认证服务器(auth)的访问地址
	5. 负责提供log服务器集群的http访问地址

## 配置文件
	
	默认配置文件为0_0_ip.setting, 不同的渠道需要配置请根据channel和service配置相应的文件.

	
	<Config>
		<!-- 日志服务器 -->
		<LogServer Url="Http://127.0.0.1:6001/"/>

		<!-- 认证服务器地址 -->
		<AuthServer Url="http://127.0.0.1:7001/"/>

		<!--Port 端口-->
		<!-- Type 端口计算类型 0=固定端口 1=由id计算 2=共享内存计算--> 
		<!-- tcp服务器配置 -->
		<TcpServer>
			<!-- route master-->
			<Server AppName="route" AppType="master">
				<App Id="22.0.1" Ip="127.0.0.1" Type="1" Port="10000"/>
			</Server>

			<!-- zone master -->
			<!-- 通用小区id填0, 如果特殊设置填写设定的小区id, 只能是在后面-->
			<Server AppName="zone" AppType="master">
				<App Id="91.0.1" Ip="127.0.0.1" Type="1" Port="11000"/>
			</Server>
		</TcpServer>
	</Config>


	防止单点问题, TcpServer可以配置多个 
	<App Id="22.0.1" Ip="127.0.0.1" Type="1" Port="10000"/>
	<App Id="22.0.2" Ip="127.0.0.1" Type="1" Port="10000"/>
	这样提供两个地址, 代码会随机连接一个地址进行连接

- LogServer  日志服务器的http访问地址, 通过这个地址得到tcp连接地址
- AuthServer 认证服务器地址
- TcpServer 需要固定连接的服务器地址在这里配置


!> TcpServer 目前只有route.master和zone.master需要配置, 如不需要请自行删除相应配置

!> zone.master是默认通用的配置, 如果需要特殊设置, 请填写正确的id 如:91.1.1, 而且特化的只能在后面添加
