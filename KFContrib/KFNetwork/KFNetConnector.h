#ifndef __NET_CONNECTOR_H__
#define __NET_CONNECTOR_H__

#include "KFNetSession.h"

namespace KFrame
{
    class KFNetServices;
    class KFNetConnector : public KFNetSession
    {
    public:
        KFNetConnector();
        ~KFNetConnector();

        // 初始化
        void InitConnector( uint64 id, KFNetServices* netservices, const KFNetCompressEncrypt* compressencrypt );

        // 弹出一个收到消息
        KFNetMessage* PopNetMessage();

        // 发送消息
        bool SendNetMessage( uint32 msgid, const char* data, uint32 length, uint32 delay = 0u );
        bool SendNetMessage( uint64 recvid, uint32 msgid, const char* data, uint32 length, uint32 delay = 0u );

        // 执行消息处理
        void RunUpdate( KFMessageFunction& netfunction, uint32 maxcount );

    protected:
        // 发送单一消息
        bool SendSingleMessage( uint64 recvid, uint32 msgid, const char* data, uint32 length, uint16 flag, uint32 delay );

        // 发送拆包消息
        bool SendMultiMessage( uint64 recvid, uint32 msgid, const char* data, uint32 length, uint16 flag, uint32 delay );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        KFNetMessage* PopMessage();
        KFNetMessage* PopSingleMessage( KFNetMessage* message );
        KFNetMessage* PopMultiMessage( KFNetMessage* message );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        // 发送ping逻辑
        void RunSendPingMessage();

        // 发送延迟消息
        void RunSendDelayMessage();
        bool PushSendMessage( KFNetMessage* message, uint32 delay );

        // 处理消息
        void RunMessage( KFMessageFunction& netfunction, uint32 maxcount );

    protected:
        // 网络服务
        KFNetServices* _net_services;

        // 加密压缩类型
        const KFNetCompressEncrypt* _net_compress_encrypt = nullptr;

        // 上一次通讯时间
        uint64 _last_send_time;
        uint64 _last_recv_time;

        // 延迟发送的消息队列
        std::map< uint64, std::list< KFNetMessage*> > _delay_queue;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif