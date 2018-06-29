#ifndef __KF_DISPLAY_INTERFACE_H__
#define __KF_DISPLAY_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFData;
    class KFEntity;
    class KFDisplayInterface : public KFModule
    {
    public:
        // 发送结果提示到客户端
        // Gate/Proxy ==> Client
        template< class...Args >
        inline void SendToClient( uint32 playerid, uint32 result, Args&& ... args )
        {
            ListString params;
            FormatParam( params, std::forward< Args >( args )... );
            SendToClient( playerid, result, params );
        }

        // 发送结果提示到客户端
        // Game ==> Client
        template< class...Args >
        inline void SendToClient( KFEntity* player, uint32 result, Args&& ... args )
        {
            ListString params;
            FormatParam( params, std::forward< Args >( args )... );
            SendToClient( player, result, params );
        }

        // 发送结果提示到客户端
        // Cluster ==> Game
        template< class...Args >
        inline void SendToGame( uint32 serverid, uint32 playerid, uint32 result, Args&& ... args )
        {
            ListString params;
            FormatParam( params, std::forward< Args >( args )... );
            SendToGame( serverid, playerid, result, params );
        }

        // 发送结果提示到客户端
        // Game ==> Route ==> Game
        template< class...Args >
        inline void SendToPlayer( uint32 serverid, uint32 playerid, uint32 result, Args&& ... args )
        {
            ListString params;
            FormatParam( params, std::forward< Args >( args )... );
            SendToPlayer( serverid, playerid, result, params );
        }

        // 发送结果提示到客户端
        // Game ==> Route ==> Game
        template< class...Args >
        inline void SendToPlayer( KFData* kfbasic, uint32 result, Args&& ... args )
        {
            ListString params;
            FormatParam( params, std::forward< Args >( args )... );
            SendToPlayer( kfbasic, result, params );
        }

        // 发送结果提示到客户端
        // Game ==> Client
        template< class...Args >
        inline void SendToGroup( KFEntity* player, uint32 result, Args&& ... args )
        {
            ListString params;
            FormatParam( params, std::forward< Args >( args )... );
            SendToGroup( player, result, params );
        }

    protected:
        // 格式化参数
        inline void FormatParam( ListString& params ) {}

        template< class T, class...Args >
        inline void FormatParam( ListString& params, T head, Args&& ... args )
        {
            params.push_back( KFUtility::ToString< T >( head ) );
            FormatParam( params, args... );
        }

    private:
        // Gate/Proxy ==> Client
        virtual void SendToClient( uint32 playerid, uint32 result, ListString& params ) = 0;

        // Game ==> Client
        virtual void SendToClient( KFEntity* player, uint32 result, ListString& params ) = 0;

        // Game ==> Client
        virtual void SendToGroup( KFEntity* player, uint32 result, ListString& params ) = 0;

        // Cluster ==> Game
        virtual void SendToGame( uint32 serverid, uint32 playerid, uint32 result, ListString& params ) = 0;

        // Game ==> Route ==> Game
        virtual void SendToPlayer( uint32 serverid, uint32 playerid, uint32 result, ListString& params ) = 0;
        virtual void SendToPlayer( KFData* kfbasic, uint32 result, ListString& params ) = 0;

    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_display, KFDisplayInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

#endif