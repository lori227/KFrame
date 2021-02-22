#ifndef __NET_CONNECTOR_H__
#define __NET_CONNECTOR_H__

#include "KFNetSession.hpp"

namespace KFrame
{
    class KFNetConnector : public KFNetSession
    {
    public:
        KFNetConnector();
        virtual ~KFNetConnector();

        // 初始化
        void InitConnector( uint64 id, KFNetService* net_service, const KFNetCompressEncrypt* compress_encrypt );

        // 弹出一个收到消息
        std::shared_ptr<KFNetMessage> PickNetMessage();

        // 发送消息
        bool SendNetMessage( uint32 msg_id, const char* data, uint32 length, uint32 delay = 0u );
        bool SendNetMessage( uint64 recv_id, uint32 msg_id, const char* data, uint32 length, uint32 delay = 0u );

        // 执行消息处理
        void RunUpdate( uint32 max_count );

    protected:
        // 发送单一消息
        bool SendSingleMessage( uint64 recv_id, uint32 msg_id, const char* data, uint32 length, uint16 flag, uint32 delay );

        // 发送拆包消息
        bool SendMultiMessage( uint64 recv_id, uint32 msg_id, const char* data, uint32 length, uint16 flag, uint32 delay );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        std::shared_ptr<KFNetMessage> PopMessage();
        std::shared_ptr<KFNetMessage> PopMultiMessage( std::shared_ptr<KFNetMessage>& message );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        // 发送ping逻辑
        void RunSendPingMessage();

        // 发送延迟消息
        void RunSendDelayMessage();
        bool PushSendMessage( std::shared_ptr<KFNetMessage>& message, uint32 delay );

        // 处理消息
        void RunMessage( uint32 max_count );

    protected:
        // 加密压缩类型
        const KFNetCompressEncrypt* _net_compress_encrypt = nullptr;

        // 上一次通讯时间
        uint64 _last_send_time = 0u;
        uint64 _last_recv_time = 0u;

        // 延迟发送的消息队列
        std::map<uint64, std::list<std::shared_ptr<KFNetMessage>>> _delay_queue;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif