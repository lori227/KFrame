#ifndef __NET_MESSAGE_H__
#define __NET_MESSAGE_H__

#include "KFRouter.h"
#include "KFMemory/KFMalloc.h"

#pragma pack( 1 )

namespace KFrame
{
    //#define __USE_MESSAGE_FLAG__
    ///////////////////////////////////////////////////////////////////////////////////////////
    class KFNetHead
    {
    public:
        // 消息长度
        uint32 _length = 0u;

        // 消息类型
        uint16 _msgid = 0u;

        // 子消息个数
        uint16 _child = 0u;

#ifdef __USE_MESSAGE_FLAG__
        // 消息标记 ( 0x1 压缩, 0x2 加密  0x3 压缩+加密 )
        uint16 _flag = 0u;
#endif
    };

    // 客户端与服务器之间的消息头
    class KFClientHead : public KFNetHead
    {
    public:

    };

    // 服务器之间的消息头
    class KFServerHead : public KFNetHead
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
            _head._length = length;
            if ( _head._length > 0 )
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
            _head._length = 0;
        }

        static KFNetMessage* Create( uint32 length )
        {
            return __KF_NEW__( KFNetMessage, length );
        }

        inline void Release()
        {
            __KF_DELETE__( KFNetMessage, this );
        }

        static uint32 HeadLength()
        {
            return sizeof( _head );
        }
        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        inline void CopyData( const int8* data, uint32 length )
        {
            _head._length = length;
            if ( length == 0 || data == nullptr )
            {
                return;
            }

            memcpy( _data, data, length );
        }

        // 复制消息
        inline void CopyFrom( KFNetMessage* message )
        {
            _head = message->_head;
            if ( message->_head._length > 0 )
            {
                CopyData( message->_data, message->_head._length );
            }
        }

        inline void CopyFrom( const Route& route, uint32 msgid, const int8* data, uint32 length, uint16 flag )
        {
            _head._route = route;
            _head._msgid = msgid;
#ifdef __USE_MESSAGE_FLAG__
            _head._flag = flag;
#endif
            CopyData( data, length );
        }
        ///////////////////////////////////////////////////////////////////////////////
    public:
        // 消息头
        KFServerHead _head;

        // 消息数据
        int8* _data = nullptr;
    };

    /////////////////////////////////////////////////////////////////////////////
}

#pragma pack()
#endif
