#include "KFDefine.h"
#include "KFNetConnector.hpp"
#include "KFNetService.hpp"

namespace KFrame
{
    KFNetConnector::KFNetConnector()
    {
        _last_recv_time = 0;
        _last_send_time = 0;
        _net_service = nullptr;
    }

    KFNetConnector::~KFNetConnector()
    {
        _delay_queue.clear();
        _net_service = nullptr;
    }

    void KFNetConnector::InitConnector( uint64 id, KFNetService* net_service, const KFNetCompressEncrypt* compress_encrypt )
    {
        _net_service = net_service;
        _net_compress_encrypt = compress_encrypt;
        _last_recv_time = _net_service->_now_time;
        _last_send_time = _net_service->_now_time;

        // 消息头长度
        uint32 header_length = ( net_service->_message_type == KFMessageEnum::Server ? sizeof( KFServerHeader ) : sizeof( KFClientHeader ) );
        InitSession( id, net_service->_queue_size, header_length );
    }

    // 弹出一个消息
    std::shared_ptr<KFNetMessage> KFNetConnector::PickNetMessage()
    {
        auto message = PopMessage();
        if ( message != nullptr )
        {
            // 收到消息时间
            _last_recv_time = _net_service->_now_time;
            if ( message->_header._msg_id == 0 )
            {
                // ping 消息不处理, 继续取下一个消息
                message = PopMessage();
            }
        }
        else
        {
            if ( _is_connected )
            {
                // 计算收到消息时间, 超过60秒没有消息, 则认为是断线了
                if ( _last_recv_time + KFNetDefine::PingTimeout < _net_service->_now_time )
                {
                    // 防止多次调用
                    _last_recv_time = _net_service->_now_time;
                    SendServiceEvent( KFNetDefine::DisconnectEvent );
                }
            }
        }

        return message;
    }

    std::shared_ptr<KFNetMessage> KFNetConnector::PopMessage()
    {
        auto message = _recv_queue.Front();
        if ( message == nullptr )
        {
            return nullptr;
        }

        switch ( message->_header._msg_id )
        {
        case KFNetDefine::MsgChildBegin:	// 子消息头
            message = PopMultiMessage( message );
            break;
        case KFNetDefine::MsgChild:			// 如果取到的是子消息, 直接丢掉
            _recv_queue.Pop();
            message = nullptr;
            break;
        default:		// 不是拆包消息, 直接返回
            break;
        }

        message->_header._route._send_id = _object_id;
        message->_header._route._server_id = _session_id;
        return message;
    }


    std::shared_ptr<KFNetMessage> KFNetConnector::PopMultiMessage( std::shared_ptr<KFNetMessage>& message )
    {
        if ( message->_data == nullptr || message->_header._length < sizeof( KFNetHeader ) )
        {
            // 消息异常, 直接丢弃
            _recv_queue.Pop();
            return nullptr;
        }

        // 如果超出了最大的队列长度
        auto child_count = message->_header._child;
        if ( child_count >= _recv_queue.Capacity() )
        {
            _recv_queue.Pop();
            return nullptr;
        }

        // 这里的子消息没有包括消息头
        auto queue_size = _recv_queue.Size();
        if ( queue_size < ( child_count + 1u ) )
        {
            return nullptr;
        }

        // 重新计算buff大小
        auto net_header = reinterpret_cast<KFNetHeader*>( message->_data );
        auto ret_message = __MAKE_SHARED__( KFNetMessage, net_header->_length );

        // 先将消息头拷贝过去
        memcpy( &ret_message->_header, message->_data, message->_header._length );

        // 合并子消息
        auto copy_length = 0u;
        auto left_length = net_header->_length;
        for ( auto i = 0u; i < child_count; ++i )
        {
            auto child_message = _recv_queue.Pop();
            if ( child_message == nullptr || left_length < child_message->_header._length )
            {
                return nullptr;
            }

            // 如果不是子消息头和子消息, 直接返回当前消息, 前面的消息直接丢弃
            if ( child_message->_header._msg_id != KFNetDefine::MsgChild && child_message->_header._msg_id != KFNetDefine::MsgChildBegin )
            {
                return child_message;
            }

            memcpy( ret_message->_data + copy_length, child_message->_data, child_message->_header._length );
            copy_length += child_message->_header._length;
            left_length -= child_message->_header._length;
        }

        return ret_message;
    }

    // 添加一个发送消息
    bool KFNetConnector::SendSingleMessage( uint64 recv_id, uint32 msg_id, const char* data, uint32 length, uint16 flag, uint32 delay )
    {
        auto net_message = __MAKE_SHARED__( KFNetMessage, length );

        Route route( 0, 0, recv_id );
        net_message->CopyFrom( route, msg_id, data, length, flag );
        return PushSendMessage( net_message, delay );
    }

    bool KFNetConnector::SendMultiMessage( uint64 recv_id, uint32 msg_id, const char* data, uint32 length, uint16 flag, uint32 delay )
    {
        // 子消息个数
        uint32 data_length = length;
        uint32 message_count = ( data_length + KFNetDefine::MaxMessageLength - 1 ) / KFNetDefine::MaxMessageLength;

        // 消息头
        KFServerHeader header;
        header._msg_id = msg_id;
        header._length = length;
#ifdef __USE_MESSAGE_FLAG__
        header._flag = flag;
#endif
        header._route._recv_id = recv_id;

        // 子消息头
        auto header_message = __MAKE_SHARED__( KFNetMessage, KFNetMessage::HeaderLength() );
        header_message->_header._child = message_count;
        header_message->CopyFrom( header._route, KFNetDefine::MsgChildBegin, reinterpret_cast<int8*>( &header ), KFNetMessage::HeaderLength(), 0 );
        if ( !PushSendMessage( header_message, delay ) )
        {
            return false;
        }

        // 子消息
        uint32 copy_data_length = 0;
        for ( auto i = 0u; i < message_count; ++i )
        {
            //  需要发送的数据长度
            auto send_length = __MIN__( data_length, KFNetDefine::MaxMessageLength );

            // 消息拷贝
            auto child_message = __MAKE_SHARED__( KFNetMessage, send_length );
            child_message->_header._child = i + 1;
            child_message->CopyFrom( header._route, KFNetDefine::MsgChild, data + copy_data_length, send_length, 0 );
            if ( !PushSendMessage( child_message, delay ) )
            {
                return false;
            }

            // 游标设置
            data_length -= send_length;
            copy_data_length += send_length;
        }

        return true;
    }

    bool KFNetConnector::SendNetMessage( uint32 msg_id, const char* data, uint32 length, uint32 delay /* = 0u */ )
    {
        return SendNetMessage( 0, msg_id, data, length, delay );
    }

    bool KFNetConnector::SendNetMessage( uint64 recv_id, uint32 msg_id, const char* data, uint32 length, uint32 delay /* = 0u */ )
    {
        // 消息压缩, 加密
        auto tuple_data = _net_service->Encode( _net_compress_encrypt, data, length );
        auto send_data = std::get<0>( tuple_data );
        auto send_length = std::get<1>( tuple_data );
        auto send_flag = std::get<2>( tuple_data );

        bool ok = false;
        if ( length <= KFNetDefine::MaxMessageLength )
        {
            ok = SendSingleMessage( recv_id, msg_id, send_data, send_length, send_flag, delay );
        }
        else
        {
            ok = SendMultiMessage( recv_id, msg_id, send_data, send_length, send_flag, delay );
        }

        // 发送消息
        _last_send_time = _net_service->_now_time;
        if ( IsNeedToSend() )
        {
            SendServiceEvent( KFNetDefine::SendEvent );
        }

        return ok;
    }

    void KFNetConnector::RunUpdate( uint32 max_count )
    {
        // 已经关闭了, 不处理消息
        if ( _is_shutdown )
        {
            return;
        }

        // 发送延迟消息
        RunSendDelayMessage();

        // 处理消息
        RunMessage( max_count );

        // ping 消息
        RunSendPingMessage();
    }

    void KFNetConnector::RunSendPingMessage()
    {
        // 20秒没有消息通讯, 发送一次ping消息
        // keeplive 经常会失灵, 很久才检测到断开 问题待查
        if ( _last_send_time + KFNetDefine::PingTime > _net_service->_now_time )
        {
            return;
        }

        SendNetMessage( 0, nullptr, 0 );
    }

    void KFNetConnector::RunMessage( uint32 max_count )
    {
        for ( int i = 0u; i < max_count; ++i )
        {
            auto message = PickNetMessage();
            if ( message == nullptr )
            {
                break;
            }

            // 消息解压缩,解密
#ifdef __USE_MESSAGE_FLAG__
            auto tuple_data = _net_service->Decode( _net_compress_encrypt, message->_data, message->_header._length, message->_header._flag );
#else
            auto tuple_data = _net_service->Decode( _net_compress_encrypt, message->_data, message->_header._length, 0 );
#endif
            auto ret_data = std::get<0>( tuple_data );
            auto ret_length = std::get<1>( tuple_data );

            // 处理回调函数
            _net_service->_net_function( message->_header._route, message->_header._msg_id, ret_data, ret_length );
        }
    }

    void KFNetConnector::RunSendDelayMessage()
    {
        if ( _delay_queue.empty() )
        {
            return;
        }

        bool have_message = false;
        for ( auto iter = _delay_queue.begin(); iter != _delay_queue.end(); )
        {
            if ( _net_service->_now_time >= iter->first )
            {
                have_message = true;
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

        if ( have_message && IsNeedToSend() )
        {
            SendServiceEvent( KFNetDefine::SendEvent );
        }
    }

    bool KFNetConnector::PushSendMessage( std::shared_ptr<KFNetMessage>& message, uint32 delay )
    {
        auto ok = true;
        if ( delay == 0u )
        {
            ok = AddSendMessage( message );
        }
        else
        {
            _delay_queue[ _net_service->_now_time + delay ].push_back( message );
        }

        return ok;
    }
}
