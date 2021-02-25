#ifndef __KF_DISPLAY_INTERFACE_H__
#define __KF_DISPLAY_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFDisplayInterface : public KFModule
    {
    public:
        // 发送结果提示到客户端
        // Gate/Proxy ==> Client
        template<class...Args>
        inline void SendToClient( uint64 player_id, uint32 result, Args&& ... args )
        {
            StringList params;
            FormatParam( params, std::forward< Args >( args )... );
            SendToClient( player_id, result, params );
        }

        // 发送结果提示到客户端
        // Game ==> Client
        template<class...Args>
        inline void SendToClient( EntityPtr player, uint32 result, Args&& ... args )
        {
            StringList params;
            FormatParam( params, std::forward< Args >( args )... );
            SendToClient( player, result, params );
        }

        // 发送结果提示到客户端
        // Game ==> Client
        template<class...Args>
        inline void DelayToClient( EntityPtr player, uint32 result, Args&& ... args )
        {
            StringList params;
            FormatParam( params, std::forward< Args >( args )... );
            DelayToClient( player, result, params );
        }

        // 发送结果提示到客户端
        // Route ==> Player
        template<class...Args>
        inline void SendToPlayer( uint64 server_id, uint64 player_id, uint32 result, Args&& ... args )
        {
            StringList params;
            FormatParam( params, std::forward< Args >( args )... );
            SendToPlayer( server_id, player_id, result, params );
        }

        // 发送结果提示到客户端
        // Route ==> Player
        template<class...Args>
        inline void SendToPlayer( const Route& route, uint32 result, Args&& ... args )
        {
            StringList params;
            FormatParam( params, std::forward< Args >( args )... );
            SendToPlayer( route, result, params );
        }

        // 发送结果提示到客户端
        // Game ==> Player
        template<class...Args>
        inline void SendToPlayer( DataPtr basic_data, uint32 result, Args&& ... args )
        {
            StringList params;
            FormatParam( params, std::forward< Args >( args )... );
            SendToPlayer( basic_data, result, params );
        }

        // 发送结果提示到客户端
        // Game ==> Client
        template<class...Args>
        inline void SendToGroup( EntityPtr player, uint32 result, Args&& ... args )
        {
            StringList params;
            FormatParam( params, std::forward< Args >( args )... );
            SendToGroup( player, result, params );
        }

    protected:
        // 格式化参数
        inline void FormatParam( StringList& params ) {}

        template< class T, class...Args >
        inline void FormatParam( StringList& params, T head, Args&& ... args )
        {
            params.push_back( KFUtility::ToString<T>( head ) );
            FormatParam( params, args... );
        }

    private:
        // Gate/Proxy ==> Client
        virtual void SendToClient( uint64 player_id, uint32 result, StringList& params ) = 0;

        // Game ==> Client
        virtual void SendToClient( EntityPtr player, uint32 result, StringList& params ) = 0;
        virtual void DelayToClient( EntityPtr player, uint32 result, StringList& params ) = 0;

        // Game ==> Client
        virtual void SendToGroup( EntityPtr player, uint32 result, StringList& params ) = 0;

        // Game ==> Route ==> Game
        virtual void SendToPlayer( DataPtr basic_data, uint32 result, StringList& params ) = 0;
        virtual void SendToPlayer( const Route& route, uint32 result, StringList& params ) = 0;
        virtual void SendToPlayer( uint64 server_id, uint64 player_id, uint32 result, StringList& params ) = 0;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_display, KFDisplayInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

#endif