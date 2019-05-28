#ifndef __NET_MESSAGE_H__
#define __NET_MESSAGE_H__

#include "KFRouter.h"

#pragma pack( 1 )

namespace KFrame
{
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
        KFNetMessage( uint32 length );
        ~KFNetMessage();

        static KFNetMessage* Create( uint32 length );
        void Release();

        static uint32 HeadLength();
        //////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////
        void CopyData( const int8* data, uint32 length );

        // 复制消息
        void CopyFrom( KFNetMessage* message );
        void CopyFrom( const Route& route, uint32 msgid, const int8* data, uint32 length );
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
