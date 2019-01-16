#ifndef __KF_PLAYERT_INTERFACE_H__
#define __KF_PLAYERT_INTERFACE_H__

#include "KFrame.h"
#include "KFKernel/KFComponent.h"

namespace KFrame
{
    typedef std::function< void( uint32, KFEntity*, const std::string&, uint64 ) > KFAfterSetNameFunction;
    class KFPlayerInterface : public KFModule
    {
    public:
        // 绑定加载玩家数据回调函数
        template< class T >
        void BindAfterLoadFunction( T* object, void ( T::*handle )( uint32, const KFMsg::PBLoginData*, KFMsg::PBObject* ) )
        {
            KFLoadPlayerFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            SetAfterLoadFunction( function );
        }

        // 取消绑定
        template< class T >
        void UnBindAfterLoadFunction( T* object )
        {
            KFLoadPlayerFunction function = nullptr;
            SetAfterLoadFunction( function );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class T >
        void BindAfterQueryFunction( T* object, void ( T::*handle )( uint32, uint64, KFMsg::PBObject* ) )
        {
            KFQueryPlayerFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );
            SetAfterQueryFunction( function );
        }

        // 取消绑定
        template< class T >
        void UnBindAfterQueryFunction( T* object )
        {
            KFQueryPlayerFunction function = nullptr;
            SetAfterQueryFunction( function );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class T >
        void BindAfterSetNameFunction( T* object, void ( T::*handle )( uint32, KFEntity*, const std::string&, uint64 ) )
        {
            KFAfterSetNameFunction function = std::bind( handle, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            SetAfterSetNameFunction( function );
        }

        // 取消绑定
        template< class T >
        void UnBindAfterSetNameFunction( T* object )
        {
            KFAfterSetNameFunction function = nullptr;
            SetAfterSetNameFunction( function );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class T >
        void RegisterInitDataFunction( T* object, void ( T::*handle )( KFEntity* player ) )
        {
            KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
            AddInitDataFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterInitDataFunction( T* object )
        {
            RemoveInitDataFunction( typeid( T ).name() );
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class T >
        void RegisterUnInitDataFunction( T* object, void ( T::*handle )( KFEntity* player ) )
        {
            KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
            AddUnInitDataFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterUnInitDataFunction( T* object )
        {
            RemoveUnInitDataFunction( typeid( T ).name() );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 逻辑函数
        template< class T >
        void RegisterRunDataFunction( T* object, void ( T::*handle )( KFEntity* player ) )
        {
            KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
            AddRunDataFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterRunDataFunction( T* object )
        {
            RemoveRunDataFunction( typeid( T ).name() );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 重置函数
        template< class T >
        void RegisterResetFunction( T* object, void ( T::*handle )( KFEntity* player ) )
        {
            KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
            AddResetFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterResetFunction( T* object )
        {
            RemoveResetFunction( typeid( T ).name() );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 登录函数
        template< class T >
        void RegisterEnterFunction( T* object, void ( T::*handle )( KFEntity* player ) )
        {
            KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
            AddEnterFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterEnterFunction( T* object )
        {
            RemoveEnterFunction( typeid( T ).name() );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 离开函数
        template< class T >
        void RegisterLeaveFunction( T* object, void ( T::*handle )( KFEntity* player ) )
        {
            KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
            AddLeaveFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterLeaveFunction( T* object )
        {
            RemoveLeaveFunction( typeid( T ).name() );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 新玩家首次登陆函数
        template< class T >
        void RegisterNewPlayerFunction( T* object, void ( T::*handle )( KFEntity* player ) )
        {
            KFEntityFunction function = std::bind( handle, object, std::placeholders::_1 );
            AddNewPlayerFunction( typeid( T ).name(), function );
        }

        template< class T >
        void UnRegisterNewPlayerFunction( T* object )
        {
            RemoveNewPlayerFunction( typeid( T ).name() );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        // 查找玩家
        virtual KFEntity* FindPlayer( uint64 playerid ) = 0;
        virtual KFEntity* FindPlayer( uint64 playerid, const char* function, uint32 line ) = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        // 加载玩家数据
        virtual bool LoadPlayer( const KFMsg::PBLoginData* pblogin ) = 0;

        // 保存玩家
        virtual void SavePlayer( KFEntity* player ) = 0;

        // 查询玩家
        virtual bool QueryPlayer( uint64 sendid, uint64 playerid ) = 0;

        // 遍历玩家
        virtual KFEntity* FirstPlayer() = 0;
        virtual KFEntity* NextPlayer() = 0;

        // 删除玩家
        virtual void RemovePlayer( uint64 playerid ) = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        // 设置名字
        virtual bool SetName( uint64 playerid, const std::string& oldname, const std::string& newname, uint64 itemguid ) = 0;

        // 判断操作频率
        virtual bool CheckOperateFrequently( KFEntity* player, uint32 time ) = 0;

    protected:
        virtual void SetAfterLoadFunction( KFLoadPlayerFunction& function ) = 0;
        virtual void SetAfterQueryFunction( KFQueryPlayerFunction& function ) = 0;
        virtual void SetAfterSetNameFunction( KFAfterSetNameFunction& function ) = 0;

        virtual void AddInitDataFunction( const std::string& moudle, KFEntityFunction& function ) = 0;
        virtual void RemoveInitDataFunction( const std::string& moudle ) = 0;

        virtual void AddUnInitDataFunction( const std::string& moudle, KFEntityFunction& function ) = 0;
        virtual void RemoveUnInitDataFunction( const std::string& moudle ) = 0;

        virtual void AddRunDataFunction( const std::string& moudle, KFEntityFunction& function ) = 0;
        virtual void RemoveRunDataFunction( const std::string& moudle ) = 0;

        virtual void AddResetFunction( const std::string& moudle, KFEntityFunction& function ) = 0;
        virtual void RemoveResetFunction( const std::string& moudle ) = 0;

        virtual void AddEnterFunction( const std::string& moudle, KFEntityFunction& function ) = 0;
        virtual void RemoveEnterFunction( const std::string& moudle ) = 0;

        virtual void AddLeaveFunction( const std::string& moudle, KFEntityFunction& function ) = 0;
        virtual void RemoveLeaveFunction( const std::string& moudle ) = 0;

        virtual void AddNewPlayerFunction( const std::string& moudle, KFEntityFunction& function ) = 0;
        virtual void RemoveNewPlayerFunction( const std::string& moudle ) = 0;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_player, KFPlayerInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __CLIENT_PROTO_PARSE__( msgtype ) \
    __PROTO_PARSE__( msgtype ); \
    auto playerid = __ROUTE_RECV_ID__; \
    auto player = _kf_player->FindPlayer( playerid, __FUNC_LINE__ );\
    if ( player == nullptr )\
    {\
        return;\
    }

#define __SERVER_PROTO_PARSE__( msgtype ) \
    __PROTO_PARSE__( msgtype ); \
    auto player = _kf_player->FindPlayer( kfmsg.playerid() );\
    if ( player == nullptr )\
    {\
        return;\
    }

#define __ROUTE_PROTO_PARSE__( msgtype ) \
    __PROTO_PARSE__( msgtype ); \
    auto playerid = __ROUTE_RECV_ID__; \
    auto player = _kf_player->FindPlayer( playerid );\
    if ( player == nullptr )\
    {\
        return;\
    }
}

#endif