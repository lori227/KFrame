#include "KFHeroModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFHeroModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_ADD_DATA_1__( __STRING__( hero ), &KFHeroModule::OnAddHeroCallBack );
        __REGISTER_REMOVE_DATA_1__( __STRING__( hero ), &KFHeroModule::OnRemoveHeroCallBack );
        __REGISTER_ADD_ELEMENT__( __STRING__( hero ), &KFHeroModule::AddHeroElement );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_ENTER_PLAYER__( &KFHeroModule::OnEnterStartHeroTimer );
        __REGISTER_LEAVE_PLAYER__( &KFHeroModule::OnLeaveStopHeroTimer );
        //////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFHeroModule::BeforeShut()
    {
        __UN_ADD_DATA_1__( __STRING__( hero ) );
        __UN_REMOVE_DATA_1__( __STRING__( hero ) );
        __UN_ADD_ELEMENT__( __STRING__( hero ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();
        //////////////////////////////////////////////////////////////////////////////////////////////////
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFHeroModule::OnEnterStartHeroTimer( EntityPtr player )
    {
        CheckStartHeroTimer( player );
    }

    void KFHeroModule::OnLeaveStopHeroTimer( EntityPtr player )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    void KFHeroModule::CheckStartHeroTimer( EntityPtr player )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );

        // 检查所有英雄, 找到时间最少的一个
        auto _min_valid_time = __MAX_UINT64__;

        auto kfhero = kfherorecord->First();
        while ( kfhero != nullptr )
        {
            auto validtime = kfhero->Get( __STRING__( time ) );
            if ( validtime != _invalid_int && validtime < _min_valid_time )
            {
                _min_valid_time = validtime;
            }

            kfhero = kfherorecord->Next();
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
        __LIMIT_TIMER_1__( player->GetKeyID(), intervaltime, 1, &KFHeroModule::OnTimerCheckHeroValidTime );
    }

    void KFHeroModule::RemoveInvalidTimeHero( EntityPtr player )
    {
        auto kfherorecord = player->Find( __STRING__( hero ) );

        // 查找时间英雄
        std::unordered_map< DataPtr, uint64 > timehero;
        auto kfhero = kfherorecord->First();
        while ( kfhero != nullptr )
        {
            auto validtime = kfhero->Get( __STRING__( time ) );
            if ( validtime != 0 )
            {
                timehero.insert( std::make_pair( kfhero, validtime ) );
            }

            kfhero = kfherorecord->Next();
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
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_data_name );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id!", kfelement->_data_name );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        auto kfsetting = KFHeroConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, " hero id=[{}] setting = null!", kfelementobject->_config_id );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        // 判断是否存在该英雄
        auto kfhero = kfparent->Find( kfelementobject->_config_id );
        if ( kfhero == nullptr )
        {
            // 不存在, 创建
            kfhero = player->CreateData( kfparent );

            // 添加英雄
            player->AddData( kfparent, kfelementobject->_config_id, kfhero );
        }
        else
        {
            // 存在, 判断有效时间
            auto datatime = kfhero->Get( __STRING__( time ) );
            auto elementtime = kfelementobject->CalcValue( kfhero->_class_setting, __STRING__( time ), multiple );
            if ( datatime != _invalid_int )
            {
                if ( elementtime == _invalid_int )
                {
                    // 永久英雄, 直接把时间更新成永久
                    player->UpdateData( kfhero, __STRING__( time ), KFEnum::Set, _invalid_int );
                }
                else
                {
                    // 时限英雄
                    player->UpdateData( kfhero, __STRING__( time ), KFEnum::Add, elementtime );
                }
            }
            else
            {
                // 按照规则换算成碎片
            }
        }

        return std::make_tuple( KFDataDefine::Show_Element, kfhero );
    }

    __KF_ADD_DATA_FUNCTION__( KFHeroModule::OnAddHeroCallBack )
    {
        auto validtime = kfdata->Get< uint64 >( __STRING__( time ) );
        if ( validtime == _invalid_int )
        {
            return;
        }

        // 时间道具 开启有效时间
        CheckStartHeroTimer( player );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFHeroModule::OnRemoveHeroCallBack )
    {
        auto kfsetting = KFHeroConfig::Instance()->FindSetting( static_cast< uint32 >( key ) );
        if ( kfsetting == nullptr )
        {
            return;
        }

        // 发送邮件

    }
}