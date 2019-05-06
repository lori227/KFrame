#include "KFHeroModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFHeroModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_hero_config, true );
    }

    void KFHeroModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->RegisterAddDataFunction( __KF_STRING__( hero ), this, &KFHeroModule::OnAddHeroCallBack );
        _kf_component->RegisterRemoveDataFunction( __KF_STRING__( hero ), this, &KFHeroModule::OnRemoveHeroCallBack );
        _kf_component->RegisterAddElementFunction( __KF_STRING__( hero ), this, &KFHeroModule::AddHeroElement );
        //////////////////////////////////////////////////////////////////////////////////////////////////

        _kf_player->RegisterEnterFunction( this, &KFHeroModule::OnEnterStartHeroTimer );
        _kf_player->RegisterLeaveFunction( this, &KFHeroModule::OnLeaveStopHeroTimer );

        //////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFHeroModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__( _kf_hero_config );

        _kf_component->UnRegisterAddDataFunction( this, __KF_STRING__( hero ) );
        _kf_component->UnRegisterRemoveDataFunction( this, __KF_STRING__( hero ) );
        _kf_component->UnRegisterAddElementFunction( __KF_STRING__( hero ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////

        _kf_player->UnRegisterEnterFunction( this );
        _kf_player->UnRegisterLeaveFunction( this );
        //////////////////////////////////////////////////////////////////////////////////////////////////
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFHeroModule::OnEnterStartHeroTimer( KFEntity* player )
    {
        CheckStartHeroTimer( player );
    }

    void KFHeroModule::OnLeaveStopHeroTimer( KFEntity* player )
    {
        __UNREGISTER_OBJECT_TIMER__( player->GetKeyID() );
    }

    void KFHeroModule::CheckStartHeroTimer( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto kfherorecord = kfobject->FindData( __KF_STRING__( hero ) );

        // 检查所有英雄, 找到时间最少的一个
        auto _min_valid_time = __MAX_UINT64__;

        auto kfhero = kfherorecord->FirstData();
        while ( kfhero != nullptr )
        {
            auto validtime = kfhero->GetValue( __KF_STRING__( time ) );
            if ( validtime != _invalid_int && validtime < _min_valid_time )
            {
                _min_valid_time = validtime;
            }

            kfhero = kfherorecord->NextData();
        }

        if ( _min_valid_time == __MAX_UINT64__ )
        {
            return;
        }

        // 如果已经有超时的英雄
        if ( _min_valid_time < KFGlobal::Instance()->_real_time )
        {
            _min_valid_time = KFGlobal::Instance()->_real_time;
        }

        // 转换成间隔时间(毫秒) 启动定时器
        auto intervaltime = ( _min_valid_time - KFGlobal::Instance()->_real_time + 1 ) * 1000;
        _kf_timer->RegisterLimitTimer( player->GetKeyID(), intervaltime, 1, this, &KFHeroModule::OnTimerCheckHeroValidTime );
    }

    void KFHeroModule::RemoveInvalidTimeHero( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto kfherorecord = kfobject->FindData( __KF_STRING__( hero ) );

        // 查找时间英雄
        std::map< KFData*, uint64 > timehero;
        auto kfhero = kfherorecord->FirstData();
        while ( kfhero != nullptr )
        {
            auto validtime = kfhero->GetValue( __KF_STRING__( time ) );
            if ( validtime != 0 )
            {
                timehero.insert( std::make_pair( kfhero, validtime ) );
            }

            kfhero = kfherorecord->NextData();
        }

        // 删除时间英雄
        for ( auto& iter : timehero )
        {
            auto* kfhero = iter.first;
            if ( KFGlobal::Instance()->_real_time < iter.second )
            {
                continue;
            }

            // 删除英雄
            auto heroid = kfhero->GetKeyID();
            player->RemoveData( kfherorecord, heroid );
        }
    }

    __KF_TIMER_FUNCTION__( KFHeroModule::OnTimerCheckHeroValidTime )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        // 删除过期的英雄
        RemoveInvalidTimeHero( player );

        // 检查是否需要开启定时器
        CheckStartHeroTimer( player );
    }

    __KF_ADD_ELEMENT_FUNCTION__( KFHeroModule::AddHeroElement )
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

        auto kfsetting = _kf_hero_config->FindHeroSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "[{}] hero setting = null!", kfelement->_parent->_data );
        }

        // 判断是否存在该英雄
        auto kfhero = kfparent->FindData( kfelementobject->_config_id );
        if ( kfhero == nullptr )
        {
            // 不存在, 创建
            kfhero = _kf_kernel->CreateObject( kfparent->GetDataSetting() );

            // 添加英雄
            player->AddData( kfparent, kfelementobject->_config_id, kfhero );
        }
        else
        {
            // 存在, 判断有效时间
            auto datatime = kfhero->GetValue( __KF_STRING__( time ) );
            auto elementtime = kfelementobject->CalcUInt64( __KF_STRING__( time ), multiple );
            if ( datatime != _invalid_int )
            {
                if ( elementtime == _invalid_int )
                {
                    // 永久英雄, 直接把时间更新成永久
                    player->UpdateData( kfhero, __KF_STRING__( time ), KFEnum::Set, _invalid_int );
                }
                else
                {
                    // 时限英雄
                    player->UpdateData( kfhero, __KF_STRING__( time ), KFEnum::Add, elementtime );
                }
            }
            else
            {
                // 按照规则换算成碎片
            }
        }
    }

    __KF_ADD_DATA_FUNCTION__( KFHeroModule::OnAddHeroCallBack )
    {
        auto validtime = kfdata->GetValue< uint64 >( __KF_STRING__( time ) );
        if ( validtime == _invalid_int )
        {
            return;
        }

        // 时间道具 开启有效时间
        CheckStartHeroTimer( player );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFHeroModule::OnRemoveHeroCallBack )
    {
        auto kfsetting = _kf_hero_config->FindHeroSetting( static_cast< uint32 >( key ) );
        if ( kfsetting == nullptr )
        {
            return;
        }

        // 发送邮件

    }
}