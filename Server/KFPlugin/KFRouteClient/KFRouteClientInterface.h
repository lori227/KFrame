#ifndef __KF_ROUTE_CLIENT_INTERFACE_H__
#define __KF_ROUTE_CLIENT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFRouteClientInterface : public KFModule
    {
    public:
        // 转发消息
        virtual bool SendToRoute( uint64 serverid, uint64 playerid, uint32 msgid, ::google::protobuf::Message* message ) = 0;
        virtual bool SendToRoute( uint64 serverid, uint64 playerid, uint32 msgid, const char* data, uint32 length ) = 0;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册连接成功函数
        template< class T >
        void RegisterTransmitFunction( T* object, bool ( T::*handle )( const KFId& kfid, uint32 msgid, const char* data, uint32 length ) )
        {
            KFTransmitFunction function = std::bind( handle, object,
                                          std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            SetTransmitFunction( function );
        }

        // 卸载
        virtual void UnRegisterTransmitFunction() = 0;
    protected:

        virtual void SetTransmitFunction( KFTransmitFunction& function ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_route, KFRouteClientInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif