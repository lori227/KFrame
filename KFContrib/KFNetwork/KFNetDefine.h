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

            ConncectTime = 5000,			// 重连定时器
            SerializeBuffLength = 1024 * 1024 * 20 + 100,	// 序列化消息的buff长度

            CUT_MSGCHILDBEGIN = 65535,		// 分割的头消息
            CUT_MSGCHILD = 65534,			// 分割的子消息

            ///////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////
            ConnectEvent = 1,		// 连接事件
            DisconnectEvent = 2,	// 关闭事件
            FailedEvent = 3,		// 失败事件
            ErrorEvent = 4,			// 错误事件
            ShutEvent = 5,			// 销毁时间
        };
    }

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    // 网络事件
    class KFEventData
    {
    public:
        KFEventData()
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