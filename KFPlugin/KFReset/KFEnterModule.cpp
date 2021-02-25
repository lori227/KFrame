#include "KFEnterModule.hpp"

namespace KFrame
{
    void KFEnterModule::BeforeRun()
    {
        __REGISTER_PLAYER_ENTER__( &KFEnterModule::EnterGameWorld );
    }

    void KFEnterModule::BeforeShut()
    {
        __UN_PLAYER_ENTER__();
    }

    __KF_PLAYER_ENTER_FUNCTION__( KFEnterModule::EnterGameWorld )
    {
        // 上线时间
        player->Operate( __STRING__( logintime ), KFEnum::Set, KFGlobal::Instance()->_real_time );
        ////////////////////////////////////////////////////////////////////////////////
        auto player_id = player->GetKeyID();
        auto note_record = player->Find( __STRING__( note ) );
        for ( auto& iter : KFEnterConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            if ( setting->_id != _invalid_int )
            {
                auto note_value = note_record->Get( setting->_id, __STRING__( value ) );
                if ( note_value != _invalid_int )
                {
                    continue;
                }

                // 设置属性
                player->UpdateRecord( note_record, setting->_id, __STRING__( value ), KFEnum::Set, 1 );
            }

            // 调用脚本
            _kf_lua->Call( setting->_lua_file, setting->_lua_function, player_id );
        }
    }
}