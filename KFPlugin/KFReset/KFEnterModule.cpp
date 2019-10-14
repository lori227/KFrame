#include "KFEnterModule.hpp"

namespace KFrame
{
    void KFEnterModule::BeforeRun()
    {
        __REGISTER_ENTER_PLAYER__( &KFEnterModule::EnterGameWorld );
    }

    void KFEnterModule::BeforeShut()
    {
        __UN_ENTER_PLAYER__();
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFEnterModule::EnterGameWorld )
    {
        // 上线时间
        ////////////////////////////////////////////////////////////////////////////////
        auto playerid = player->GetKeyID();
        auto kfnoterecord = player->Find( __KF_STRING__( note ) );
        for ( auto& iter : KFEnterConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_id != _invalid_int )
            {
                auto notevalue = kfnoterecord->Get( kfsetting->_id, __KF_STRING__( value ) );
                if ( notevalue != _invalid_int )
                {
                    continue;
                }

                // 设置属性
                player->UpdateData( kfnoterecord, kfsetting->_id, __KF_STRING__( value ), KFEnum::Set, 1 );
            }

            // 调用脚本
            _kf_lua->Call( kfsetting->_lua_file, kfsetting->_lua_function, playerid );
        }
    }
}