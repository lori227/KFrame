#include "KFLeaveModule.h"
#include "KFLeaveConfig.h"

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
        __KF_REMOVE_CONFIG__();
        _kf_player->UnRegisterLeaveFunction( this );
    }

    void KFLeaveModule::LeaveGameWorld( KFEntity* player )
    {
        // 最后下线时间 和 总在线时间
        auto kfglobal = KFGlobal::Instance();
        player->UpdateData( __KF_STRING__( lastofflinetime ), KFOperateEnum::Set, kfglobal->_real_time );

        auto kfobject = player->GetData();
        auto onlinetime = kfobject->GetValue<uint64>( __KF_STRING__( onlinetime ) );
        if ( onlinetime != _invalid_int && kfglobal->_real_time > onlinetime )
        {
            player->UpdateData( __KF_STRING__( totalonlinetime ), KFOperateEnum::Add, kfglobal->_real_time - onlinetime );
        }
        //////////////////////////////////////////////////////////////////////////
        auto playerid = player->GetKeyID();
        auto kfnoteparent = kfobject->FindData( __KF_STRING__( note ) );
        for ( auto& iter : _kf_leave_config->_kf_leave_setting )
        {
            auto kfsetting = &iter;
            if ( kfsetting->_note_id != _invalid_int )
            {
                auto notevalue = kfnoteparent->GetValue( kfsetting->_note_id, __KF_STRING__( value ) );
                if ( notevalue != _invalid_int )
                {
                    continue;
                }

                // 设置属性
                player->UpdateData( kfnoteparent, kfsetting->_note_id, __KF_STRING__( value ), KFOperateEnum::Set, 1 );
            }

            // 调用脚本
            _kf_lua->Call( kfsetting->_lua_file, kfsetting->_lua_function, playerid );
        }
    }
}