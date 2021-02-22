#ifndef __NET_MESSAGE_H__
#define __NET_MESSAGE_H__

#include "KFRoute.h"
#include "KFMemory/KFMalloc.h"

#pragma pack( 1 )

namespace KFrame
{
    //#define __USE_MESSAGE_FLAG__
    ///////////////////////////////////////////////////////////////////////////////////////////
    class KFNetHeader
    {
    public:
        // 消息长度
        uint32 _length = 0u;

        // 消息类型
        uint16 _msg_id = 0u;

        // 子消息个数
        uint16 _child = 0u;

#ifdef __USE_MESSAGE_FLAG__
        // 消息标记 ( 0x1 压缩, 0x2 加密  0x3 压缩+加密 )
        uint16 _flag = 0u;
#endif
    };

    // 客户端与服务器之间的消息头
    class KFClientHeader : public KFNetHeader
    {
    public:

    };

    // 服务器之间的消息头
    class KFServerHeader : public KFNetHeader
    {
    public:
        Route _route;		// 路由信息
    };
    ///////////////////////////////////////////////////////////////////////////////////////////
    // 消息基类
    class KFNetMessage
    {
    public:
        KFNetMessage( uint32 length )
        {
            _header._length = length;
            if ( _header._length > 0 )
            {
                _data = __KF_MALLOC__( int8, length );
            }
        }

        ~KFNetMessage()
        {
            if ( _data != nullptr )
            {
                __KF_FREE__( int8, _data );
            }

            _data = nullptr;
            _header._length = 0;
        }

        static uint32 HeaderLength()
        {
            return sizeof( _header );
        }
        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        inline void CopyData( const int8* data, uint32 length )
        {
            _header._length = length;
            if ( length == 0 || data == nullptr )
            {
                return;
            }

            memcpy( _data, data, length );
        }

        // 复制消息
        inline void CopyFrom( KFNetMessage* message )
        {
            _header = message->_header;
            if ( message->_header._length > 0 )
            {
                CopyData( message->_data, message->_header._length );
            }
        }

        inline void CopyFrom( const Route& route, uint32 msg_id, const int8* data, uint32 length, uint16 flag )
        {
            _header._route = route;
            _header._msg_id = msg_id;
#ifdef __USE_MESSAGE_FLAG__
            _header._flag = flag;
#endif
            CopyData( data, length );
        }
        ///////////////////////////////////////////////////////////////////////////////
    public:
        // 消息头
        KFServerHeader _header;

        // 消息数据
        int8* _data = nullptr;
    };

    /////////////////////////////////////////////////////////////////////////////
}

#pragma pack()
#endif
