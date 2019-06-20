#include "KFLeaveModule.hpp"
#include "KFLeaveConfig.hpp"

namespace KFrame
{
    void KFLeaveModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_leave_config, true );
    }

    void KFLeaveModule::BeforeRun()
    {
        _kf_player->RegisterLeaveFunction( this, &KFLeaveModule::LeaveGameWorld );
    }

    void KFLeaveModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__( _kf_leave_config );
        _kf_player->UnRegisterLeaveFunction( this );
    }

    void KFLeaveModule::LeaveGameWorld( KFEntity* player )
    {
        auto kfobject = player->GetData();
        //////////////////////////////////////////////////////////////////////////
        auto playerid = player->GetKeyID();
        auto kfnoteparent = kfobject->FindData( __KF_STRING__( note ) );
        for ( auto& iter : _kf_leave_config->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_id != _invalid_int )
            {
                auto notevalue = kfnoteparent->GetValue( kfsetting->_id, __KF_STRING__( value ) );
                if ( notevalue != _invalid_int )
                {
                    continue;
                }

                // 设置属性
                player->UpdateData( kfnoteparent, kfsetting->_id, __KF_STRING__( value ), KFEnum::Set, 1 );
            }

            // 调用脚本
            _kf_lua->Call( kfsetting->_lua_file, kfsetting->_lua_function, playerid );
        }
    }
}