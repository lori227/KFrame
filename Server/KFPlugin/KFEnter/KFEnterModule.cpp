#include "KFEnterModule.h"
#include "KFEnterConfig.h"
#include "KFKernel/KFKernelInterface.h"

namespace KFrame
{
    KFEnterModule::KFEnterModule()
    {

    }

    KFEnterModule::~KFEnterModule()
    {
    }

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
        __KF_REMOVE_CONFIG__();
        _kf_player->UnRegisterEnterFunction( this );
    }

    void KFEnterModule::EnterGameWorld( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto kfnoteparent = kfobject->FindData( KFField::_note );
        if ( kfnoteparent == nullptr )
        {
            return;
        }

        auto playerid = player->GetKeyID();
        for ( auto& iter : _kf_enter_config->_kf_enter_setting )
        {
            auto setting = &iter;

            auto notevalue = kfnoteparent->GetValue( setting->_note_id, KFField::_value );
            if ( notevalue != _invalid_int )
            {
                continue;
            }

            // 设置属性
            player->UpdateData( kfnoteparent, setting->_note_id, KFField::_value, KFOperateEnum::Set, 1 );

            // 调用脚本
            _kf_lua->CallFunction( setting->_lua_file, setting->_lua_function, playerid );
        }
    }
}