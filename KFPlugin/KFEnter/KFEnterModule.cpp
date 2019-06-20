#include "KFEnterModule.hpp"
#include "KFEnterConfig.hpp"

namespace KFrame
{
    void KFEnterModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_enter_config, true );
    }

    void KFEnterModule::BeforeRun()
    {
        _kf_player->RegisterEnterFunction( this, &KFEnterModule::EnterGameWorld );
    }

    void KFEnterModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__( _kf_enter_config );
        _kf_player->UnRegisterEnterFunction( this );
    }

    void KFEnterModule::EnterGameWorld( KFEntity* player )
    {
        // 上线时间
        auto kfobject = player->GetData();
        ////////////////////////////////////////////////////////////////////////////////
        auto playerid = player->GetKeyID();
        auto kfnoteparent = kfobject->FindData( __KF_STRING__( note ) );
        for ( auto& iter : _kf_enter_config->_settings._objects )
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