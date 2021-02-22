#include "KFDefine.h"
#include "KFNetSession.hpp"
#include "KFNetEvent.hpp"
#include "KFNetConnector.hpp"
#include "KFNetService.hpp"
#include "uv.h"

namespace KFrame
{
    KFNetSession::KFNetSession()
    {
        _is_connected = false;
        _is_shutdown = false;
        _is_sending = false;
        _uv_write = __NEW_OBJECT__( uv_write_t );
    }

    KFNetSession::~KFNetSession()
    {
        __DELETE_OBJECT__( _uv_write );
    }

    void KFNetSession::InitSession( uint64 id, uint32 queue_count, uint32 header_length )
    {
        _session_id = id;
        _uv_write->data = this;
        _message_head_length = header_length;

        auto extend_count = IsServerSession() ? queue_count : 0u;

        _send_queue.InitQueue( queue_count, extend_count );
        _recv_queue.InitQueue( queue_count, extend_count );
    }

    bool KFNetSession::IsConnected() const
    {
        return _is_connected;
    }

    bool KFNetSession::IsNeedToSend()
    {
        // 已经在发送 断开连接 已经关闭
        if ( _is_sending || !_is_connected || _is_shutdown  )
        {
            return false;
        }

        return true;
    }

    bool KFNetSession::IsServerSession()
    {
        return _session_id == _object_id;
    }

    void KFNetSession::StartSendMessage()
    {
        if ( !_is_connected || _is_shutdown || _is_sending )
        {
            return;
        }

        SendQueueMessage();
    }

    void KFNetSession::SendBuffer( char* buffer, uint32 length )
    {
        _is_sending = true;

        uv_buf_t buff;
        buff.base = buffer;
        buff.len = length;

        _uv_write->data = this;
        uv_write( _uv_write, _uv_stream, &buff, 1, OnSendCallBack );
    }

    void KFNetSession::OnSendCallBack( uv_write_t* uv_write, int32 status )
    {
        auto net_session = reinterpret_cast< KFNetSession* >( uv_write->data );
        if ( status != 0 )
        {
            net_session->OnDisconnect( status, __FUNC_LINE__ );
        }
        else
        {
            net_session->OnSendOK();
            net_session->StartSendMessage();
        }
    }

    void KFNetSession::OnSendOK()
    {
        _send_length = 0;
        _is_sending = false;
    }

    uint32 KFNetSession::SendQueueMessage()
    {
        if ( _send_length == 0 )
        {
            do
            {
                // 消息队列为空
                auto message = _send_queue.Front();
                if ( message == nullptr )
                {
                    break;
                }

                // 超过最大长度
                if ( !CheckBufferLength( _send_length, message->_header._length ) )
                {
                    break;
                }

                memcpy( _req_send_buffer + _send_length, &message->_header, _message_head_length );
                _send_length += _message_head_length;

                if ( message->_header._length > 0 )	// 不是空消息
                {
                    memcpy( _req_send_buffer + _send_length, message->_data, message->_header._length );
                    _send_length += message->_header._length;
                }

                // 释放内存
                _send_queue.Pop();
            } while ( true );
        }

        if ( _send_length != 0 )
        {
            SendBuffer( _req_send_buffer, _send_length );
        }

        return _send_length;
    }

    bool KFNetSession::CheckBufferLength( uint32 total_length, uint32 message_length )
    {
        if ( total_length + _message_head_length + message_length <= KFNetDefine::MaxReqBuffLength )
        {
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFNetSession::OnRecvData( const char* buffer, uint32 length )
    {
        // 接受总长度
        _recv_length += length;

        // 处理消息
        ParseBuffToMessage();
    }

    void KFNetSession::ParseBuffToMessage()
    {
        // 长度不足一个消息头
        uint32 now_position = 0;
        auto net_header = CheckRecvBuffValid( now_position );
        if ( net_header == nullptr )
        {
            return;
        }

        while ( _recv_length >= ( now_position + _message_head_length + net_header->_length ) )
        {
            auto recv_message = __MAKE_SHARED__( KFNetMessage, net_header->_length );
            memcpy( &recv_message->_header, net_header, _message_head_length );

            now_position += _message_head_length;
            if ( net_header->_length > 0 )
            {
                recv_message->CopyData( _req_recv_buffer + now_position, net_header->_length );
                now_position += net_header->_length;
            }

            AddRecvMessage( recv_message );

            // 检查消息的有效性
            net_header = CheckRecvBuffValid( now_position );
            if ( net_header == nullptr )
            {
                break;
            }
        }

        // 设置长度
        _recv_length -= __MIN__( _recv_length, now_position );
        if ( _recv_length > 0 && now_position > 0 )
        {
            memmove( _req_recv_buffer, _req_recv_buffer + now_position, _recv_length );
        }
    }

    KFNetHeader* KFNetSession::CheckRecvBuffValid( uint32 position )
    {
        if ( _recv_length < ( position + _message_head_length ) )
        {
            return nullptr;
        }

        auto net_header = reinterpret_cast< KFNetHeader* >( _req_recv_buffer + position );

        // 收到的消息长度有错误
        if ( net_header->_length > KFNetDefine::MaxMessageLength )
        {
            _recv_length = 0;
            return nullptr;
        }

        return net_header;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFNetSession::CloseSession()
    {
        _is_connected = false;
        if ( _uv_stream != nullptr )
        {
            uv_read_stop( _uv_stream );
        }
    }

    void KFNetSession::OnConnect( uv_stream_t* uv_stream )
    {
        _is_connected = true;
        _uv_stream = uv_stream;

        // 开始接受消息
        StartRecvData();
    }

    void KFNetSession::StartRecvData()
    {
        if ( !_is_connected || _is_shutdown )
        {
            return;
        }

        _uv_stream->data = this;
        uv_read_start( _uv_stream, AllocRecvBuffer, OnRecvCallBack );
    }

    void KFNetSession::AllocRecvBuffer( uv_handle_t* handle, size_t size, uv_buf_t* uv_buffer )
    {
        auto net_session = reinterpret_cast< KFNetSession* >( handle->data );

        uv_buffer->base = net_session->_req_recv_buffer + net_session->_recv_length;
        uv_buffer->len = KFNetDefine::MaxRecvBuffLength - net_session->_recv_length;
    }

    void KFNetSession::OnRecvCallBack( uv_stream_t* uv_stream, ssize_t length, const uv_buf_t* uv_buffer )
    {
        auto net_session = reinterpret_cast< KFNetSession* >( uv_stream->data );
        if ( length < 0 )
        {
            // 错误, 断开连接
            net_session->OnDisconnect( static_cast< int32 >( length ), __FUNC_LINE__ );
        }
        else
        {
            // 接收数据
            net_session->OnRecvData( uv_buffer->base, static_cast< uint32 >( length ) );
            net_session->StartRecvData();
        }
    }

    void KFNetSession::OnDisconnect( int32 code, const char* function, uint32 line )
    {
        _is_connected = false;
        if ( IsServerSession() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "session[{}:{}] disconnect[{}:{}]", _session_id, KFAppId::ToString( _session_id ), code, uv_err_name( code ) );
        }
        else
        {
            __LOG_INFO_FUNCTION__( function, line, "session[{}:{}] disconnect[{}:{}]", _session_id, _object_id, code, uv_err_name( code ) );
        }
    }

    bool KFNetSession::AddSendMessage( std::shared_ptr<KFNetMessage>& message )
    {
        auto ok = _send_queue.Push( message );
        if ( !ok )
        {
            if ( !_is_send_queue_full )
            {
                _is_send_queue_full = true;

                if ( IsServerSession() )
                {
                    __LOG_ERROR__( "session[{}:{}] send msg_id[{}] failed", _session_id, KFAppId::ToString( _session_id ), message->_header._msg_id );
                }
                else
                {
                    __LOG_ERROR__( "session[{}:{}] send msg_id[{}] failed", _session_id, _object_id, message->_header._msg_id );
                }
            }
        }
        else
        {
            _is_send_queue_full = false;
        }

        return ok;
    }

    bool KFNetSession::AddRecvMessage( std::shared_ptr<KFNetMessage>& message )
    {
        auto ok = _recv_queue.Push( message );
        if ( !ok )
        {
            if ( !_is_recv_queue_full )
            {
                _is_recv_queue_full = true;

                if ( IsServerSession() )
                {
                    __LOG_ERROR__( "session[{}:{}] recv msg_id[{}] failed", _session_id, KFAppId::ToString( _session_id ), message->_header._msg_id );
                }
                else
                {
                    __LOG_ERROR__( "session[{}:{}] recv msg_id[{}] failed", _session_id, _object_id, message->_header._msg_id );
                }
            }
        }
        else
        {
            _is_recv_queue_full = false;
        }

        return ok;
    }

    void KFNetSession::SendServiceEvent( uint32 event_type )
    {
        if ( _is_shutdown )
        {
            return;
        }

        _net_service->SendEventToService( event_type, shared_from_this() );
    }

    void KFNetSession::HandleServiceEvent( uint32 event_type )
    {
        if ( _is_shutdown )
        {
            return;
        }

        switch ( event_type )
        {
        case KFNetDefine::ConnectEvent:
            StartSession();
            break;
        case KFNetDefine::SendEvent:
            StartSendMessage();
            break;
        case KFNetDefine::CloseEvent:
            CloseSession();
            break;
        case KFNetDefine::DisconnectEvent:
            OnDisconnect( 0, __FUNC_LINE__ );
            break;
        default:
            break;
        }
    }
}

