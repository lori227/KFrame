#include "KFSkinModule.hpp"
#include "KFSkinConfig.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFSkinModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_skin_config, true );
    }

    void KFSkinModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->RegisterAddDataFunction( __KF_STRING__( skin ), this, &KFSkinModule::OnAddSkinCallBack );
        _kf_component->RegisterRemoveDataFunction( __KF_STRING__( skin ), this, &KFSkinModule::OnRemoveSkinCallBack );
        //////////////////////////////////////////////////////////////////////////////////////////////////

        _kf_component->RegisterAddElementFunction( __KF_STRING__( skin ), this, &KFSkinModule::AddSkinElement );
        //////////////////////////////////////////////////////////////////////////////////////////////////

        _kf_player->RegisterEnterFunction( this, &KFSkinModule::OnEnterStartSkinTimer );
        _kf_player->RegisterLeaveFunction( this, &KFSkinModule::OnLeaveStopSkinTimer );

        //////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFSkinModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__( _kf_skin_config );

        _kf_component->UnRegisterAddDataFunction( this, __KF_STRING__( skin ) );
        _kf_component->UnRegisterRemoveDataFunction( this, __KF_STRING__( skin ) );
        _kf_component->UnRegisterAddElementFunction( __KF_STRING__( skin ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////

        _kf_player->UnRegisterEnterFunction( this );
        _kf_player->UnRegisterLeaveFunction( this );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        //__UNREGISTER_MESSAGE__( KFMsg::MSG_DRESS_SKIN_REQ );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFSkinModule::OnEnterStartSkinTimer( KFEntity* player )
    {
        CheckStartSkinTimer( player );
    }

    void KFSkinModule::OnLeaveStopSkinTimer( KFEntity* player )
    {
        __UNREGISTER_OBJECT_TIMER__( player->GetKeyID() );
    }

    void KFSkinModule::CheckStartSkinTimer( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto kfskinrecord = kfobject->FindData( __KF_STRING__( skin ) );

        // 检查所有皮肤, 找到时间最少的一个
        auto _min_valid_time = __MAX_UINT64__;

        auto kfskin = kfskinrecord->FirstData();
        while ( kfskin != nullptr )
        {
            auto validtime = kfskin->GetValue( __KF_STRING__( time ) );
            if ( validtime != 0 && validtime < _min_valid_time )
            {
                _min_valid_time = validtime;
            }

            kfskin = kfskinrecord->NextData();
        }

        if ( _min_valid_time == __MAX_UINT64__ )
        {
            return;
        }

        // 如果已经有超时皮肤
        if ( _min_valid_time < KFGlobal::Instance()->_real_time )
        {
            _min_valid_time = KFGlobal::Instance()->_real_time;
        }

        // 转换成间隔时间(毫秒) 启动定时器
        auto intervaltime = ( _min_valid_time - KFGlobal::Instance()->_real_time + 1 ) * 1000;
        __REGISTER_LIMIT_TIMER__( player->GetKeyID(), intervaltime, 1, &KFSkinModule::OnTimerCheckSkinValidTime );
    }

    void KFSkinModule::RemoveInvalidTimeSkin( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto kfskinrecord = kfobject->FindData( __KF_STRING__( skin ) );

        // 查找时间皮肤
        std::map< KFData*, uint64 > timeskins;
        auto kfskin = kfskinrecord->FirstData();
        while ( kfskin != nullptr )
        {
            auto validtime = kfskin->GetValue( __KF_STRING__( time ) );
            if ( validtime != 0 )
            {
                timeskins.insert( std::make_pair( kfskin, validtime ) );
            }

            kfskin = kfskinrecord->NextData();
        }

        // 删除时间皮肤
        for ( auto& iter : timeskins )
        {
            auto* kfclothes = iter.first;
            if ( KFGlobal::Instance()->_real_time < iter.second )
            {
                continue;
            }

            // 删除皮肤
            auto keyid = kfclothes->GetKeyID();
            player->RemoveData( kfskinrecord, keyid );
        }
    }

    __KF_TIMER_FUNCTION__( KFSkinModule::OnTimerCheckSkinValidTime )
    {
        auto player = _kf_player->FindPlayer( objectid, __FUNC_LINE__ );
        if ( player == nullptr )
        {
            return;
        }

        // 删除过期的皮肤
        RemoveInvalidTimeSkin( player );

        // 检查是否需要开启定时器
        CheckStartSkinTimer( player );
    }

    __KF_ADD_ELEMENT_FUNCTION__( KFSkinModule::AddSkinElement )
    {
        if ( !kfelement->IsObject() )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_parent->_data );
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id!", kfelement->_parent->_data );
        }

        auto kfsetting = _kf_skin_config->FindSkinSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] skinsetting = null!", kfelement->_parent->_data );
        }

        auto kfobject = player->GetData();

        // 判断是否存在该皮肤
        auto kfskin = kfparent->FindData( kfelementobject->_config_id );
        if ( kfskin == nullptr )
        {
            // 不存在, 创建新的皮肤
            kfskin = _kf_kernel->CreateObject( kfparent->GetDataSetting() );

            // 设置时间
            auto elementtime = kfelementobject->CalcUInt64( __KF_STRING__( time ), multiple );
            if ( elementtime != _invalid_int )
            {
                kfskin->OperateValue( __KF_STRING__( time ), kfelementobject->_operate, KFGlobal::Instance()->_real_time + elementtime );
            }

            // 添加皮肤
            player->AddData( kfparent, kfelementobject->_config_id, kfskin );
        }
        else
        {
            // 存在, 判断有效时间
            auto datatime = kfskin->GetValue( __KF_STRING__( time ) );
            auto elementtime = kfelementobject->CalcUInt64( __KF_STRING__( time ), multiple );
            if ( datatime != _invalid_int )
            {
                // 时限皮肤
                if ( elementtime == _invalid_int )
                {
                    // 永久皮肤, 直接把时间更新成永久
                    player->UpdateData( kfskin, __KF_STRING__( time ), KFEnum::Set, _invalid_int );
                }
                else
                {
                    // 添加皮肤时间
                    player->UpdateData( kfskin, __KF_STRING__( time ), KFEnum::Add, elementtime );
                }
            }
            else
            {
                // 永久皮肤
                // 按照规则换算成其他奖励
            }
        }
    }

    __KF_ADD_DATA_FUNCTION__( KFSkinModule::OnAddSkinCallBack )
    {
        auto validtime = kfdata->GetValue( __KF_STRING__( time ) );
        if ( validtime == _invalid_int )
        {
            return;
        }

        // 时限皮肤 开启定时器
        CheckStartSkinTimer( player );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFSkinModule::OnRemoveSkinCallBack )
    {
        // 发送邮件, 通知客户端
        //_kf_mail->SendMail( player, kfsetting->_mail_id, nullptr );
    }

}