#include "KFNetConnector.h"
#include "KFDefine.h"
#include "KFNetServices.h"

namespace KFrame
{
    KFNetConnector::KFNetConnector()
    {
        _last_recv_time = 0;
        _last_send_time = 0;
        _net_services = nullptr;
    }

    KFNetConnector::~KFNetConnector()
    {
        _net_services = nullptr;
        for ( auto& iter : _delay_queue )
        {
            for ( auto message : iter.second )
            {
                message->Release();
            }
        }
        _delay_queue.clear();
    }

    void KFNetConnector::InitConnector( uint64 id, KFNetServices* netservices, const KFNetCompressEncrypt* compressencrypt )
    {
        _net_services = netservices;
        _net_compress_encrypt = compressencrypt;
        _last_recv_time = _net_services->_now_time;
        _last_send_time = _net_services->_now_time;

        // 消息头长度
        uint32 headlength = ( netservices->_message_type == KFMessageEnum::Server ? sizeof( KFServerHead ) : sizeof( KFClientHead ) );
        InitSession( id, netservices->_queue_size, headlength );
    }

    KFNetMessage* KFNetConnector::PopMessage()
    {
        auto message = _recv_queue.Front();
        if ( message == nullptr )
        {
            return nullptr;
        }

        KFNetMessage* retmessage = nullptr;
        switch ( message->_head._msgid )
        {
        case KFNetDefine::MsgChildBegin:	// 子消息头
            retmessage = PopMultiMessage( message );
            break;
        case KFNetDefine::MsgChild:			// 如果取到的是子消息, 直接丢掉
            _recv_queue.PopRemove();
            break;
        default:		// 不是拆包消息, 直接返回
            retmessage = PopSingleMessage( message );
            break;
        }

        return retmessage;
    }

    // 弹出一个消息
    KFNetMessage* KFNetConnector::PopNetMessage()
    {
        auto message = PopMessage();
        if ( message != nullptr )
        {
            // 收到消息时间
            _last_recv_time = _net_services->_now_time;

            // ping 消息不处理
            if ( message->_head._msgid == 0 )
            {
                message = nullptr;
            }
        }
        else
        {
            if ( _is_connected )
            {
                // 计算收到消息时间, 超过60秒没有消息, 则认为是断线了
                if ( _last_recv_time + KFNetDefine::PingTimeout < _net_services->_now_time )
                {
                    // 防止多次调用
                    _last_recv_time = _net_services->_now_time;

                    // 断线处理
                    _net_services->SendEventToServices( this, KFNetDefine::DisconnectEvent );
                    __LOG_DEBUG__( "net connector=[{}:{}] message timeout", _session_id, _object_id );
                }
            }
        }

        return message;
    }

    KFNetMessage* KFNetConnector::PopSingleMessage( KFNetMessage* message )
    {
        KFNetMessage* retmessage = nullptr;
        if ( message->_head._length + sizeof( KFNetMessage ) <= _net_services->_max_serialize_buff_length )
        {
            retmessage = reinterpret_cast< KFNetMessage* >( _net_services->_serialize_buff );
            retmessage->_data = _net_services->_serialize_buff + sizeof( KFNetMessage );
            retmessage->CopyFrom( message );
        }

        _recv_queue.PopRemove();
        return retmessage;
    }

    KFNetMessage* KFNetConnector::PopMultiMessage( KFNetMessage* message )
    {
        if ( message->_data == nullptr || message->_head._length < sizeof( KFNetHead ) )
        {
            // 消息异常, 直接丢弃
            _recv_queue.PopRemove();
            return nullptr;
        }

        // 如果超出了最大的队列长度
        auto childcount = message->_head._child;
        if ( childcount >= _recv_queue.Capacity() )
        {
            _recv_queue.PopRemove();
            return nullptr;
        }

        // 这里的子消息没有包括消息头
        auto queuesize = _recv_queue.Size();
        if ( queuesize < ( childcount + 1u ) )
        {
            return nullptr;
        }

        // 重新计算buff大小
        auto nethead = reinterpret_cast< KFNetHead* >( message->_data );
        auto totallength = nethead->_length + static_cast< uint32 >( sizeof( KFNetMessage ) );
        auto buffaddress = _net_services->GetBuffAddress( nethead->_msgid, totallength );
        if ( _net_services->_max_serialize_buff_length < totallength )
        {
            // 长度异常, 直接丢弃
            _recv_queue.PopRemove();
            return nullptr;
        }

        // 先将消息头拷贝过去
        memcpy( buffaddress, message->_data, message->_head._length );
        _recv_queue.PopRemove();

        auto retmessage = reinterpret_cast< KFNetMessage* >( buffaddress );
        retmessage->_data = buffaddress + sizeof( KFNetMessage );

        // 合并子消息
        auto copylength = 0u;
        auto leftlength = _net_services->_max_serialize_buff_length - sizeof( KFNetMessage );
        for ( auto i = 0u; i < childcount; ++i )
        {
            auto childmessage = _recv_queue.Front();

            // 不是子消息, 直接返回null
            if ( childmessage == nullptr || childmessage->_head._msgid != KFNetDefine::MsgChild )
            {
                return nullptr;
            }

            // 长度不足, 返回null
            if ( leftlength < childmessage->_head._length )
            {
                _recv_queue.PopRemove();
                return nullptr;
            }

            memcpy( retmessage->_data + copylength, childmessage->_data, childmessage->_head._length );
            copylength += childmessage->_head._length;
            leftlength -= childmessage->_head._length;

            _recv_queue.PopRemove();
        }

        retmessage->_head._route._send_id = _object_id;
        retmessage->_head._route._server_id = _session_id;
        return retmessage;
    }

    // 添加一个发送消息
    bool KFNetConnector::SendSingleMessage( uint64 recvid, uint32 msgid, const char* data, uint32 length, uint16 flag, uint32 delay )
    {
        auto netmessage = KFNetMessage::Create( length );

        Route route( 0, 0, recvid );
        netmessage->CopyFrom( route, msgid, data, length, flag );
        return PushSendMessage( netmessage, delay );
    }

    bool KFNetConnector::SendMultiMessage( uint64 recvid, uint32 msgid, const char* data, uint32 length, uint16 flag, uint32 delay )
    {
        // 子消息个数
        uint32 datalength = length;
        uint32 messagecount = ( datalength + KFNetDefine::MaxMessageLength - 1 ) / KFNetDefine::MaxMessageLength;

        // 消息头
        KFServerHead head;
        head._msgid = msgid;
        head._length = length;
#ifdef __USE_MESSAGE_FLAG__
        head._flag = flag;
#endif
        head._route._recv_id = recvid;

        // 子消息头
        auto headmessage = KFNetMessage::Create( KFNetMessage::HeadLength() );
        headmessage->_head._child = messagecount;
        headmessage->CopyFrom( head._route, KFNetDefine::MsgChildBegin, reinterpret_cast< int8*>( &head ), KFNetMessage::HeadLength(), 0 );
        if ( !PushSendMessage( headmessage, delay ) )
        {
            return false;
        }

        // 子消息
        uint32 copydatalength = 0;
        for ( auto i = 0u; i < messagecount; ++i )
        {
            //  需要发送的数据长度
            auto sendlength = __MIN__( datalength, KFNetDefine::MaxMessageLength );

            // 消息拷贝
            auto childmessage = KFNetMessage::Create( sendlength );
            childmessage->_head._child = i + 1;
            childmessage->CopyFrom( head._route, KFNetDefine::MsgChild, data + copydatalength, sendlength, 0 );
            if ( !PushSendMessage( childmessage, delay ) )
            {
                return false;
            }

            // 游标设置
            datalength -= sendlength;
            copydatalength += sendlength;
        }

        return true;
    }

    bool KFNetConnector::SendNetMessage( uint32 msgid, const char* data, uint32 length, uint32 delay /* = 0u */ )
    {
        return SendNetMessage( 0, msgid, data, length, delay );
    }

    bool KFNetConnector::SendNetMessage( uint64 recvid, uint32 msgid, const char* data, uint32 length, uint32 delay /* = 0u */ )
    {
        // 消息压缩, 加密
        auto rettuple = _net_services->Encode( _net_compress_encrypt, data, length );
        auto senddata = std::get<0>( rettuple );
        auto sendlength = std::get<1>( rettuple );
        auto sendflag = std::get<2>( rettuple );

        bool ok = false;
        if ( length <= KFNetDefine::MaxMessageLength )
        {
            ok = SendSingleMessage( recvid, msgid, senddata, sendlength, sendflag, delay );
        }
        else
        {
            ok = SendMultiMessage( recvid, msgid, senddata, sendlength, sendflag, delay );
        }

        // 发送消息
        _last_send_time = _net_services->_now_time;
        if ( IsNeedSend() )
        {
            _net_services->SendEventToServices( this, KFNetDefine::SendEvent );
        }

        return ok;
    }

    void KFNetConnector::RunUpdate( KFMessageFunction& netfunction, uint32 maxcount )
    {
        // 已经关闭了, 不处理消息
        if ( _is_shutdown )
        {
            return;
        }

        // 发送延迟消息
        RunSendDelayMessage();

        // 处理消息
        RunMessage( netfunction, maxcount );

        // ping 消息
        RunSendPingMessage();
    }

    void KFNetConnector::RunSendPingMessage()
    {
        // 20秒没有消息通讯, 发送一次ping消息
        // keeplive 经常会失灵, 很久才检测到断开 问题待查
        if ( _last_send_time + KFNetDefine::PingTime > _net_services->_now_time )
        {
            return;
        }

        SendNetMessage( 0, nullptr, 0 );
    }

    void KFNetConnector::RunMessage( KFMessageFunction& netfunction, uint32 maxcount )
    {
        auto messagecount = _invalid_int;
        auto message = PopNetMessage();
        while ( message != nullptr )
        {
            // 消息解压缩,解密
#ifdef __USE_MESSAGE_FLAG__
            auto rettuple = _net_services->Decode( _net_compress_encrypt, message->_data, message->_head._length, message->_head._flag );
#else
            auto rettuple = _net_services->Decode( _net_compress_encrypt, message->_data, message->_head._length, 0 );
#endif
            auto retdata = std::get<0>( rettuple );
            auto retlength = std::get<1>( rettuple );

            // 处理回调函数
            netfunction( message->_head._route, message->_head._msgid, retdata, retlength );

            // 每次处理200个消息
            ++messagecount;
            if ( messagecount >= maxcount )
            {
                break;
            }
            message = PopNetMessage();
        }
    }

    void KFNetConnector::RunSendDelayMessage()
    {
        if ( _delay_queue.empty() )
        {
            return;
        }

        bool havemessage = false;
        for ( auto iter = _delay_queue.begin(); iter != _delay_queue.end(); )
        {
            if ( _net_services->_now_time >= iter->first )
            {
                havemessage = true;
                for ( auto message : iter->second )
                {
                    AddSendMessage( message );
                }
                iter = _delay_queue.erase( iter );
            }
            else
            {
                ++iter;
            }
        }

        if ( havemessage && IsNeedSend() )
        {
            _net_services->SendEventToServices( this, KFNetDefine::SendEvent );
        }
    }

    bool KFNetConnector::PushSendMessage( KFNetMessage* message, uint32 delay )
    {
        auto ok = true;
        if ( delay == 0u )
        {
            ok = AddSendMessage( message );
        }
        else
        {
            _delay_queue[ _net_services->_now_time + delay ].push_back( message );
        }

        return ok;
    }
}
