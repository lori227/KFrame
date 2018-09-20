#include "KFNetConnector.h"
#include "KFDefine.h"
#include "KFNetServices.h"

namespace KFrame
{
    KFNetConnector::KFNetConnector()
    {
        _net_services = nullptr;
    }

    KFNetConnector::~KFNetConnector()
    {
        _net_services = nullptr;
    }

    void KFNetConnector::InitConnector( uint32 id, KFNetServices* netservices )
    {
        InitSession( id, netservices->_queue_size );
        _net_services = netservices;
    }

    // 弹出一个消息
    KFNetMessage* KFNetConnector::PopMessage()
    {
        auto message = _recv_queue.Front();
        if ( message == nullptr )
        {
            return nullptr;
        }

        KFNetMessage* retmessage = nullptr;
        switch ( message->_msgid )
        {
        case KFNetDefine::CUT_MSGCHILDBEGIN:	// 子消息头
        {
            // 这里的子消息没有包括消息头
            auto childcount = message->_child;
            auto queuesize = _recv_queue.Size();
            if ( queuesize >= ( childcount + 1u ) )
            {
                // 不能强转成KFNetMessage, headmessage->_data的长度只包括KFNetHead

                // 重新计算buff大小
                auto tempmessage = reinterpret_cast< KFNetHead* >( message->_data );
                auto totallength = tempmessage->_length + static_cast< uint32 >( sizeof( KFNetMessage ) );
                auto buffaddress = _net_services->GetBuffAddress( tempmessage->_msgid, totallength );

                if ( _net_services->_buff_length < totallength )
                {
                    // 长度异常, 直接丢弃
                    _recv_queue.PopRemove();
                }
                else
                {
                    // 先将消息头拷贝过去
                    memcpy( buffaddress, message->_data, message->_length );
                    _recv_queue.PopRemove();

                    retmessage = reinterpret_cast< KFNetMessage* >( buffaddress );
                    retmessage->_data = buffaddress + sizeof( KFNetMessage );

                    // 合并子消息
                    uint32 copylength = 0u;
                    auto leftlength = _net_services->_buff_length - sizeof( KFNetMessage );

                    for ( uint32 i = 0u; i < childcount; ++i )
                    {
                        auto childmessage = _recv_queue.Front();

                        // 不是子消息, 直接返回null
                        if ( childmessage == nullptr || childmessage->_msgid != KFNetDefine::CUT_MSGCHILD )
                        {
                            return nullptr;
                        }

                        // 长度不足, 返回null
                        if ( leftlength < childmessage->_length )
                        {
                            _recv_queue.PopRemove();
                            return nullptr;
                        }

                        memcpy( retmessage->_data + copylength, childmessage->_data, childmessage->_length );
                        copylength += childmessage->_length;
                        leftlength -= childmessage->_length;

                        _recv_queue.PopRemove();
                    }
                }
            }
            else
            {
                // 如果超出了最大的队列长度
                if ( childcount >= _recv_queue.Capacity() )
                {
                    _recv_queue.PopRemove();
                }
            }
        }
        break;
        case KFNetDefine::CUT_MSGCHILD:		// 如果取到的是子消息, 直接丢掉
        {
            _recv_queue.PopRemove();
        }
        break;
        default:	// 不是拆包消息, 直接返回
        {
            if ( message->_length + sizeof( KFNetMessage ) <= _net_services->_buff_length )
            {
                retmessage = reinterpret_cast< KFNetMessage* >( _net_services->_buff_address );
                retmessage->_data = _net_services->_buff_address + sizeof( KFNetMessage );
                retmessage->CopyFrom( message );
            }

            _recv_queue.PopRemove();
        }
        break;
        }

        return retmessage;
    }

    // 添加一个发送消息
    bool KFNetConnector::SendSingleMessage( uint32 objectid, uint32 msgid, const char* data, uint32 length )
    {
        auto netmessage = KFNetMessage::Create( length );

        KFGuid kfguid( 0, objectid );
        netmessage->CopyFrom( kfguid, msgid, data, length );
        return AddSendMessage( netmessage );
    }

    bool KFNetConnector::SendMultiMessage( uint32 objectid, uint32 msgid, const char* data, uint32 length )
    {
        auto ok = true;

        // 子消息个数
        uint32 datalength = length;
        uint32 messagecount = ( datalength + KFNetDefine::MaxMessageLength - 1 ) / KFNetDefine::MaxMessageLength;

        // 消息头
        KFGuid kfguid( 0, objectid );
        auto message = reinterpret_cast< KFNetMessage* >( _net_services->_buff_address );
        message->CopyFrom( kfguid, msgid, nullptr, length );

        // 子消息头
        auto headmessage = KFNetMessage::Create( KFNetMessage::HeadLength() );
        headmessage->_child = messagecount;
        headmessage->CopyFrom( kfguid, KFNetDefine::CUT_MSGCHILDBEGIN, _net_services->_buff_address, KFNetMessage::HeadLength() );
        if ( !AddSendMessage( headmessage ) )
        {
            ok = false;
        }

        // 子消息
        uint32 copydatalength = 0;
        for ( auto i = 0u; i < messagecount; ++i )
        {
            //  需要发送的数据长度
            auto sendlength = __MIN__( datalength, KFNetDefine::MaxMessageLength );

            // 消息拷贝
            auto childmessage = KFNetMessage::Create( sendlength );
            childmessage->_child = i + 1;
            childmessage->CopyFrom( kfguid, KFNetDefine::CUT_MSGCHILD, data + copydatalength, sendlength );
            if ( !AddSendMessage( childmessage ) )
            {
                ok = false;
            }

            // 游标设置
            datalength -= sendlength;
            copydatalength += sendlength;
        }

        return ok;
    }

    KFNetMessage* KFNetConnector::PopNetMessage()
    {
        auto message = PopMessage();
        if ( message != nullptr )
        {
            // 解密
        }

        return message;
    }

    bool KFNetConnector::SendNetMessage( uint32 msgid, const char* data, uint32 length )
    {
        return SendNetMessage( 0, msgid, data, length );
    }

    bool KFNetConnector::SendNetMessage( uint32 objectid, uint32 msgid, const char* data, uint32 length )
    {
        bool ok = false;

        // 小于发送的最大buff, 直接添加消息
        if ( length <= KFNetDefine::MaxMessageLength )
        {
            ok = SendSingleMessage( objectid, msgid, data, length );
        }
        else
        {
            ok = SendMultiMessage( objectid, msgid, data, length );
        }

        // 发送消息
        _net_services->SendNetMessage( this );

        return ok;
    }
}
