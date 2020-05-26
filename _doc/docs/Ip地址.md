# Ip地址插件


## 主要功能
	
	1. 负责提供进程所在物理机的局域网ip和互联网ip
	2. 负责计算进程(tcp和http服务器)监听端口
	3. 负责提供master服务器的ip和端口
	4. 负责提供认证服务器(auth)的访问地址
	5. 负责提供log服务器集群的http访问地址

## 配置文件
	
	默认配置文件为0_0_ip.setting, 不同的渠道需要配置请根据channel和service配置相应的文件.

	
	<?xml version='1.0' encoding='utf-8' ?>
	<Config>
		<!-- 查询外网ip的dns服务器 -->
		<DNSServer Url="http://members.3322.org/dyndns/getip"/>
	
		<!-- 日志服务器 -->
		<LogServer Url="Http://127.0.0.1:6001/"/>
	
		<!-- 认证服务器地址 -->
		<AuthServer Url="http://127.0.0.1:7001/"/>
	
		<!-- 目录服务器地址 -->
		<DirServer Url="http://127.0.0.1:7501/"/>
	
		<!-- 充值服务器地址 -->
		<PayServer Url="http://127.0.0.1:7001/"/>
		
	  	<!-- vpn 地址-->
	  	<Vpns>
	  	</Vpns>
	</Config>


- LogServer  日志服务器的http访问地址, 通过这个地址得到tcp连接地址
- AuthServer 认证服务器地址

!> 外网发布都是配置域名地址, 通过nginx做反向代理来做负载均衡

!> route.master和zone.master的ip地址不用配置, 进程启动会去数据库中查询动态地址(这些地址都是自动上报的)
