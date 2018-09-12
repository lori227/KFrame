#include "KFNetSession.h"
#include "KFDefine.h"
#include "KFNetEvent.h"
#include "KFNetConnector.h"

namespace KFrame
{
    KFNetSession::KFNetSession()
    {
        _send_length = 0;
        _receive_length = 0;
        _uv_stream = nullptr;
        _is_connected = false;
        _is_shutdown = false;
        _is_sending = false;
        _is_send_queue_full = false;
        _is_recv_queue_full = false;

        memset( _receive_buff, 0, sizeof( _receive_buff ) );
        memset( _req_recv_buffer, 0, sizeof( _req_recv_buffer ) );
        memset( _req_send_buffer, 0, sizeof( _req_send_buffer ) );
    }

    KFNetSession::~KFNetSession()
    {

    }

    void KFNetSession::InitSession( uint32 id, uint32 queuecount )
    {
        _session_id = id;
        _object_id = id;
        _uv_write.data = this;
        _send_queue.InitQueue( queuecount );
        _recv_queue.InitQueue( queuecount );
    }

    bool KFNetSession::IsConnected() const
    {
        return _is_connected;
    }

    bool KFNetSession::IsNeedSend()
    {
        // 断开连接
        if ( !_is_connected )
        {
            return false;
        }

        // 已经关闭
        if ( _is_shutdown )
        {
            return false;
        }

        // 已经在发送
        if ( _is_sending )
        {
            return false;
        }

        return true;
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

        _uv_write.data = this;
        uv_write( &_uv_write, _uv_stream, &buff, 1, OnSendCallBack );
    }

    void KFNetSession::OnSendCallBack( uv_write_t* uvwrite, int32 status )
    {
        auto netsession = reinterpret_cast< KFNetSession* >( uvwrite->data );
        if ( status != 0 )
        {
            netsession->OnDisconnect( __FUNCTION__, status );
        }
        else
        {
            netsession->OnSendOK();
            netsession->StartSendMessage();
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
                if ( !CheckBufferLength( _send_length, message->_length ) )
                {
                    break;
                }

                memcpy( _req_send_buffer + _send_length, message, KFNetMessage::HeadLength() );
                _send_length += KFNetMessage::HeadLength();

                if ( message->_length > 0 )	// 不是空消息
                {
                    memcpy( _req_send_buffer + _send_length, message->_data, message->_length );
                    _send_length += message->_length;
                }

                // 释放内存
                _send_queue.PopRemove();
            } while ( true );
        }

        if ( _send_length != 0 )
        {
            SendBuffer( _req_send_buffer, _send_length );
        }

        return _send_length;
    }

    bool KFNetSession::CheckBufferLength( uint32 totallength, uint32 messagelength )
    {
        if ( totallength + KFNetMessage::HeadLength() + messagelength <= KFNetDefine::MaxReqBuffLength )
        {
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFNetSession::OnRecvData( const char* buffer, uint32 length )
    {
        // 消息长度增加
        if ( _receive_length + length > KFNetDefine::MaxRecvBuffLength )
        {
            __LOG_CRITICAL__( KFLogEnum::System, "recv length[{}:{}] error", _receive_length, length );
            _receive_length = 0;
        }

        memcpy( _receive_buff + _receive_length, buffer, length );
        _receive_length += length;

        // 处理消息
        ParseBuffToMessage();
    }

    void KFNetSession::ParseBuffToMessage()
    {
        // 长度不足一个消息头
        uint32 nowposition = 0;
        auto nethead = CheckReciveBuffValid( nowposition );
        if ( nethead == nullptr )
        {
            return;
        }

        while ( _receive_length >= ( nowposition + KFNetMessage::HeadLength() + nethead->_length ) )
        {
            auto recvmessage = KFNetMessage::Create( nethead->_length );
            memcpy( recvmessage, nethead, KFNetMessage::HeadLength() );

            nowposition += KFNetMessage::HeadLength();
            if ( nethead->_length > 0 )
            {
                recvmessage->CopyData( _receive_buff + nowposition, nethead->_length );
                nowposition += nethead->_length;
            }

            AddRecvMessage( recvmessage );

            // 检查消息的有效性
            nethead = CheckReciveBuffValid( nowposition );
            if ( nethead == nullptr )
            {
                break;
            }
        }

        // 设置长度
        _receive_length -= __MIN__( _receive_length, nowposition );

        // 移动消息buff
        if ( _receive_length > 0 && nowposition > 0 )
        {
            memmove( _receive_buff, _receive_buff + nowposition, _receive_length );
        }
    }

    KFNetHead* KFNetSession::CheckReciveBuffValid( uint32 position )
    {
        if ( _receive_length < ( position + KFNetMessage::HeadLength() ) )
        {
            return nullptr;
        }

        auto nethead = reinterpret_cast< KFNetHead* >( _receive_buff + position );

        // 收到的消息长度有错误
        if ( nethead->_msgid == 0 || nethead->_length > KFNetDefine::MaxMessageLength )
        {
            _receive_length = 0;
            __LOG_CRITICAL__( KFLogEnum::System, "recv msgid[{}] length[{}] position[{}] error",
                              nethead->_msgid, nethead->_length, position );
            return nullptr;
        }

        return nethead;
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

    void KFNetSession::OnConnect( uv_stream_t* uvstream )
    {
        _is_connected = true;
        _uv_stream = uvstream;

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

    void KFNetSession::AllocRecvBuffer( uv_handle_t* handle, size_t size, uv_buf_t* pbuffer )
    {
        auto netsession = reinterpret_cast< KFNetSession* >( handle->data );

        pbuffer->base = netsession->_req_recv_buffer;
        pbuffer->len = sizeof( netsession->_req_recv_buffer );
    }

    void KFNetSession::OnRecvCallBack( uv_stream_t* uvstream, ssize_t length, const uv_buf_t* pbuffer )
    {
        auto netsession = reinterpret_cast< KFNetSession* >( uvstream->data );
        if ( length < 0 )
        {
            return netsession->OnDisconnect( __FUNCTION__, static_cast< uint32 >( length ) );
        }

        // 连接状态接受消息
        netsession->OnRecvData( pbuffer->base, static_cast< uint32 >( length ) );
        netsession->StartRecvData();
    }

    void KFNetSession::OnDisconnect( const char* error, int32 code )
    {
        _is_connected = false;
        __LOG_DEBUG__( KFLogEnum::Net, "session[{}:{}] disconnect[{}:{}]!", _session_id, _object_id, error, code );
    }

    bool KFNetSession::AddSendMessage( KFNetMessage* message )
    {
        message->_guid._head_id = _object_id;
        bool ok = _send_queue.PushObject( message );
        if ( !ok )
        {
            if ( !_is_send_queue_full )
            {
                _is_send_queue_full = true;
                __LOG_CRITICAL__( KFLogEnum::System, "add send msgid[{}] guid[{}:{}] failed!",
                                  message->_msgid, __KF_HEAD_ID__( message->_guid ), __KF_DATA_ID__( message->_guid ) );
            }
        }
        else
        {
            _is_send_queue_full = false;
        }

        return ok;
    }

    bool KFNetSession::AddRecvMessage( KFNetMessage* message )
    {
        message->_guid._head_id = _object_id;
        auto ok = _recv_queue.PushObject( message );
        if ( !ok )
        {
            if ( !_is_recv_queue_full )
            {
                _is_recv_queue_full = true;
                __LOG_CRITICAL__( KFLogEnum::System, "add recv msgid[{}] guid[{}:{}] failed!",
                                  message->_msgid, __KF_HEAD_ID__( message->_guid ), __KF_DATA_ID__( message->_guid ) );
            }
        }
        else
        {
            _is_recv_queue_full = false;
        }

        return ok;
    }

}

