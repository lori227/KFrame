#ifndef __KF_PLAYERT_INTERFACE_H__
#define __KF_PLAYERT_INTERFACE_H__

#include "KFrame.h"
#include "KFKernel/KFComponent.h"

namespace KFrame
{
    class KFPlayerInterface : public KFModule
    {
    public:
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 逻辑函数
        template<class T>
        void RegisterRunDataFunction( T* module, void ( T::*handle )( EntityPtr ) )
        {
            KFEntityFunction function = std::bind( handle, module, std::placeholders::_1 );
            AddRunDataFunction( module, function );
        }

        template<class T>
        void UnRegisterRunDataFunction( T* module )
        {
            RemoveRunDataFunction( module );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////
        // 逻辑函数
        template<class T>
        void RegisterAfterRunDataFunction( T* module, void ( T::*handle )( EntityPtr ) )
        {
            KFEntityFunction function = std::bind( handle, module, std::placeholders::_1 );
            AddAfterRunDataFunction( module, function );
        }

        template<class T>
        void UnRegisterAfterRunDataFunction( T* module )
        {
            RemoveAfterRunDataFunction( module );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////

        // 重置函数
        template<class T>
        void RegisterResetFunction( T* module, void ( T::*handle )( EntityPtr ) )
        {
            KFEntityFunction function = std::bind( handle, module, std::placeholders::_1 );
            AddResetFunction( module, function );
        }

        template<class T>
        void UnRegisterResetFunction( T* module )
        {
            RemoveResetFunction( module );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////
        // 登录函数
        template<class T>
        void RegisterBeforeEnterFunction( T* module, void ( T::*handle )( EntityPtr ) )
        {
            KFEntityFunction function = std::bind( handle, module, std::placeholders::_1 );
            AddBeforeEnterFunction( module, function );
        }

        template<class T>
        void UnRegisterBeforeEnterFunction( T* module )
        {
            RemoveBeforeEnterFunction( module );
        }
        // 登录函数
        template<class T>
        void RegisterEnterFunction( T* module, void ( T::*handle )( EntityPtr ) )
        {
            KFEntityFunction function = std::bind( handle, module, std::placeholders::_1 );
            AddEnterFunction( module, function );
        }

        template<class T>
        void UnRegisterEnterFunction( T* module )
        {
            RemoveEnterFunction( module );
        }

        // 登录函数
        template<class T>
        void RegisterAfterEnterFunction( T* module, void ( T::*handle )( EntityPtr ) )
        {
            KFEntityFunction function = std::bind( handle, module, std::placeholders::_1 );
            AddAfterEnterFunction( module, function );
        }

        template<class T>
        void UnRegisterAfterEnterFunction( T* module )
        {
            RemoveAfterEnterFunction( module );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        // 离开函数
        template<class T>
        void RegisterLeaveFunction( T* module, void ( T::*handle )( EntityPtr ) )
        {
            KFEntityFunction function = std::bind( handle, module, std::placeholders::_1 );
            AddLeaveFunction( module, function );
        }

        template<class T>
        void UnRegisterLeaveFunction( T* module )
        {
            RemoveLeaveFunction( module );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////
        // 新玩家首次登陆函数
        template<class T>
        void RegisterNewPlayerFunction( T* module, void ( T::*handle )( EntityPtr ) )
        {
            KFEntityFunction function = std::bind( handle, module, std::placeholders::_1 );
            AddNewPlayerFunction( module, function );
        }

        template<class T>
        void UnRegisterNewPlayerFunction( T* module )
        {
            RemoveNewPlayerFunction( module );
        }

        // 创建角色
        template<class T>
        void RegisterCreateRoleFunction( T* module, void ( T::*handle )( EntityPtr ) )
        {
            KFEntityFunction function = std::bind( handle, module, std::placeholders::_1 );
            AddCreateRoleFunction( module, function );
        }

        template<class T>
        void UnRegisterCreateRoleFunction( T* module )
        {
            RemoveCreateRoleFunction( module );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        // 玩家登录
        virtual EntityPtr Login( const KFMsg::PBLoginData* login_data, const KFMsg::PBObject* player_data ) = 0;

        // 重新登录
        virtual EntityPtr ReLogin( uint64 player_id, uint64 gate_id ) = 0;

        // 玩家登出
        virtual void Logout() = 0;
        virtual void Logout( uint64 player_id ) = 0;
        virtual void Logout( EntityPtr player ) = 0;

        // 查找玩家
        virtual EntityPtr FindPlayer( uint64 player_id ) = 0;

        // 发送消息
        virtual bool SendToClient( EntityPtr player, uint32 msg_id, ::google::protobuf::Message* message, uint32 delay = 0u ) = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        virtual void AddRunDataFunction( KFModule* module, KFEntityFunction& function ) = 0;
        virtual void RemoveRunDataFunction( KFModule* module ) = 0;

        virtual void AddAfterRunDataFunction( KFModule* module, KFEntityFunction& function ) = 0;
        virtual void RemoveAfterRunDataFunction( KFModule* module ) = 0;

        virtual void AddResetFunction( KFModule* module, KFEntityFunction& function ) = 0;
        virtual void RemoveResetFunction( KFModule* module ) = 0;

        virtual void AddBeforeEnterFunction( KFModule* module, KFEntityFunction& function ) = 0;
        virtual void RemoveBeforeEnterFunction( KFModule* module ) = 0;

        virtual void AddEnterFunction( KFModule* module, KFEntityFunction& function ) = 0;
        virtual void RemoveEnterFunction( KFModule* module ) = 0;

        virtual void AddAfterEnterFunction( KFModule* module, KFEntityFunction& function ) = 0;
        virtual void RemoveAfterEnterFunction( KFModule* module ) = 0;

        virtual void AddLeaveFunction( KFModule* module, KFEntityFunction& function ) = 0;
        virtual void RemoveLeaveFunction( KFModule* module ) = 0;

        virtual void AddNewPlayerFunction( KFModule* module, KFEntityFunction& function ) = 0;
        virtual void RemoveNewPlayerFunction( KFModule* module ) = 0;

        virtual void AddCreateRoleFunction( KFModule* module, KFEntityFunction& function ) = 0;
        virtual void RemoveCreateRoleFunction( KFModule* module ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_player, KFPlayerInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __FIND_PLAYER_BY_ID__ \
    auto player_id = kfmsg->playerid(); \
    auto player = _kf_player->FindPlayer( player_id );\
    if ( player == nullptr )\
    {\
        return;\
    }\
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_PLAYER_ENTER_FUNCTION__( function ) void function( EntityPtr player )
#define __REGISTER_PLAYER_ENTER__( function ) _kf_player->RegisterEnterFunction( this, function )
#define __UN_PLAYER_ENTER__() _kf_player->UnRegisterEnterFunction( this )

#define __REGISTER_PLAYER_BEFORE_ENTER__( function ) _kf_player->RegisterBeforeEnterFunction( this, function )
#define __UN_PLAYER_BEFORE_ENTER__() _kf_player->UnRegisterBeforeEnterFunction( this )

#define __REGISTER_PLAYER_AFTER_ENTER__( function ) _kf_player->RegisterAfterEnterFunction( this, function )
#define __UN_PLAYER_AFTER_ENTER__() _kf_player->UnRegisterAfterEnterFunction( this )

#define __KF_PLAYER_LEAVE_FUNCTION__( function ) void function( EntityPtr player )
#define __REGISTER_PLAYER_LEAVE__( function ) _kf_player->RegisterLeaveFunction( this, function )
#define __UN_PLAYER_LEAVE__() _kf_player->UnRegisterLeaveFunction( this )

#define __KF_PLAYER_RESET_FUNCTION__( function ) void function( EntityPtr player )
#define __REGISTER_PLAYER_RESET__( function ) _kf_player->RegisterResetFunction( this, function )
#define __UN_PLAYER_RESET__() _kf_player->UnRegisterResetFunction( this )

#define __KF_PLAYER_RUN_FUNCTION__( function ) void function( EntityPtr player )
#define __REGISTER_PLAYER_RUN__( function ) _kf_player->RegisterRunDataFunction( this, function )
#define __UN_PLAYER_RUN__() _kf_player->UnRegisterRunDataFunction( this )

#define __KF_PLAYER_AFTER_RUN_FUNCTION__( function ) void function( EntityPtr player )
#define __REGISTER_PLAYER_AFTER_RUN__( function ) _kf_player->RegisterAfterRunDataFunction( this, function )
#define __UN_PLAYER_AFTER_RUN__() _kf_player->UnRegisterAfterRunDataFunction( this )

#define __KF_NEW_PLAYER_FUNCTION__( function ) void function( EntityPtr player )
#define __REGISTER_NEW_PLAYER__( function ) _kf_player->RegisterNewPlayerFunction( this, function )
#define __UN_NEW_PLAYER__() _kf_player->UnRegisterNewPlayerFunction( this )

#define __KF_CREATE_ROLE_FUNCTION__( function ) void function( EntityPtr player )
#define __REGISTER_CREATE_ROLE__( function ) _kf_player->RegisterCreateRoleFunction( this, function )
#define __UN_CREATE_ROLE__() _kf_player->UnRegisterCreateRoleFunction( this )

}

#endif