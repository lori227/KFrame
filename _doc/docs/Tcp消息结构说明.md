# Tcp消息结构

消息头结构定义

 	class KFNetHead
    {
    public:
        uint32 _length;		// 消息长度
        uint16 _msgid;		// 消息类型
        uint16 _child;		// 子消息个数( 包括自己 )
    };

    // 客户端与服务器之间的消息头
    class KFClientHead : public KFNetHead
    {
    public:

    };

    // 服务器之间的消息头
    class KFServerHead : public KFNetHead
    {
    public:
        KFRoute _route;		// 路由信息
    };


客户端与服务器之间网络传送消息结构如下:  

	_length | _msgid | _child | _data |
	4字节   | 2字节   | 2字节  | proto消息序列化的数据, 长度为_length |

服务器与服务器之间网络传送消息结构如下:  

	_length | _msgid | _child | _route | _data |
	4字节   | 2字节   | 2字节  | 24字节 | proto消息序列化的数据, 长度为_length |

网络端接收到的数据长度是不确定的( 因为网络`mtu`一般是1500 字节),  
可能是1条完整消息, 可能是n条消息, 也可能是消息的一小半部分  
接收到的数据缓存在预留的buff中,  
然后根据接收到的数据, 强转成`KFNetHead指针`,读取`_length`长度数据, 根据`消息头长度 + _length`的长度,来解析接收到的消息数据.

如果是很大的消息, 会拆分很多个小消息分别发送.  
拆分的消息头id为 `CUT_MSGCHILDBEGIN`( 65535 )  
拆分的子消息id为 `CUT_MSGCHILD`( 65534 )  
拆分的消息个数放在消息头的`_child`中  
接收到拆包消息后, 需要判断接收到的消息个数是否满足`_child`个数, 否则等待继续接收.  
满足_child消息个数后, 将消息组合成原始消息给上层逻辑使用

!> 如果碰到恶意消息, 可以加一个定时器, 如果超过时间还没有后续消息,则断开连接 
