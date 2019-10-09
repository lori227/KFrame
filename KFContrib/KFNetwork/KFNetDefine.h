#ifndef __NET_DEFINE_H__
#define __NET_DEFINE_H__

#include "KFInclude.h"

namespace KFrame
{
    class KFNetConnector;
    namespace KFNetDefine
    {
        enum EConstDefine
        {
            // mtu = 1500 , data = 1472
            MaxMessageLength = 1024 * 5,
            MaxReqBuffLength = MaxMessageLength + 100,		// 投递给网络底层的bbuff大小
            MaxRecvBuffLength = 2 * MaxReqBuffLength,		// 接收消息的最大长度

            SerializeBuffLength = 1024 * 1024 * 20 + 100,	// 序列化消息的buff长度
            ConncectTime = 5000,							// 重连定时器
            PingTime = 20000,								// 20秒发送一个ping
            PingTimeout = 60000,							// 断线超时时间

            CUT_MSGCHILDBEGIN = 65535,		// 分割的头消息
            CUT_MSGCHILD = 65534,			// 分割的子消息

            ///////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////
            ConnectEvent = 1,		// 连接事件
            DisconnectEvent = 2,	// 断开事件
            FailedEvent = 3,		// 失败事件
            ErrorEvent = 4,			// 错误事件
            ShutEvent = 5,			// 销毁事件
            SendEvent = 6,			// 发送事件
            CloseEvent = 7,			// 关闭事件
        };
    }

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    // 网络事件
    class KFNetEventData
    {
    public:
        KFNetEventData()
        {
            _id = 0;
            _type = 0;
            _code = 0;
            _data = nullptr;
        }

        // 类型
        uint32 _type;

        // id
        uint64 _id;

        // 数据
        void* _data;

        // 错误码
        int32 _code;

        // 描述
        std::string _describe;
    };
}

#endif