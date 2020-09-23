#include "KFLeaveModule.hpp"

namespace KFrame
{
    void KFLeaveModule::BeforeRun()
    {
        __REGISTER_PLAYER_LEAVE__( &KFLeaveModule::LeaveGameWorld );
    }

    void KFLeaveModule::BeforeShut()
    {
        __UN_PLAYER_LEAVE__();
    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFLeaveModule::LeaveGameWorld )
    {
        // 更新总时间
        auto logintime = player->Get( __STRING__( logintime ) );
        player->Operate( __STRING__( totaltime ), KFEnum::Add, KFGlobal::Instance()->_real_time - logintime );
        ///////////////////////////////////////////////////////////////////////////////////////////
        auto playerid = player->GetKeyID();
        auto kfnoteparent = player->Find( __STRING__( note ) );
        for ( auto& iter : KFLeaveConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_id != _invalid_int )
            {
                auto notevalue = kfnoteparent->Get( kfsetting->_id, __STRING__( value ) );
                if ( notevalue != _invalid_int )
                {
                    continue;
                }

                // 设置属性
                player->UpdateRecordData( kfnoteparent, kfsetting->_id, __STRING__( value ), KFEnum::Set, 1 );
            }

            // 调用脚本
            _kf_lua->Call( kfsetting->_lua_file, kfsetting->_lua_function, playerid );
        }
    }
}