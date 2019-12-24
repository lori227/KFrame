#include "KFDropModule.hpp"
#ifdef __KF_DEBUG__
    #include "KFProtocol/KFProtocol.h"
#endif // __KF_DEBUG__


namespace KFrame
{
    void KFDropModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_ADD_ELEMENT__( __STRING__( drop ), &KFDropModule::AddDropElement );
    }

    void KFDropModule::BeforeShut()
    {
        __UN_ADD_ELEMENT__( __STRING__( drop ) );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_ELEMENT_FUNCTION__( KFDropModule::AddDropElement )
    {
        auto kfelement = kfresult->_element;
        if ( !kfelement->IsObject() )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_data_name );
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id!", kfelement->_data_name );
        }

        auto count = kfelementobject->CalcValue( kfparent->_class_setting, kfparent->_data_setting->_value_key_name, 1.0f );
        Drop( player, kfelementobject->_config_id, __MAX__( count, 1u ), __STRING__( command ), __FUNC_LINE__ );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFDropModule::BindDropLogicFunction( const std::string& dataname, KFDropLogicFunction& function )
    {
        auto kffucntion = _drop_logic_function.Create( dataname );
        kffucntion->_function = function;
    }

    void KFDropModule::UnRegisterDropLogicFunction( const std::string& dataname )
    {
        _drop_logic_function.Remove( dataname );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __KF_DEBUG__
    void KFDropModule::SendDropDataToClient( KFEntity* player, uint32 dropid, uint32 count, DropDataList& droplist )
    {
        KFMsg::MsgDebugShowDrop show;
        show.set_dropid( dropid );
        show.set_count( count );

        for ( auto dropdata : droplist )
        {
            auto pbdata = show.add_dropdata();
            pbdata->set_dataid( dropdata->_drop_data_id );
            pbdata->set_dataindex( dropdata->_data_index );
            pbdata->set_dataname( dropdata->_data_name );
        }

        _kf_player->SendToClient( player, KFMsg::MSG_DEBUG_SHOW_DROP, &show );
    }
#endif // __KF_DEBUG__
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    DropDataList& KFDropModule::Drop( KFEntity* player, uint32 dropid, const char* function, uint32 line )
    {
        return Drop( player, dropid, 1u, _invalid_string, function, line );
    }

    DropDataList& KFDropModule::Drop( KFEntity* player, uint32 dropid, const std::string& modulename, const char* function, uint32 line )
    {
        return Drop( player, dropid, 1u, modulename, function, line );
    }

    DropDataList& KFDropModule::Drop( KFEntity* player, const UInt32Vector& droplist, const std::string& modulename, const char* function, uint32 line )
    {
        static DropDataList _dropdatalist;
        _dropdatalist.clear();

        for ( auto iter : droplist )
        {
            auto& data = Drop( player, iter, 1u, modulename, function, line );
            _dropdatalist.splice( _dropdatalist.end(), data );
        }

        return _dropdatalist;
    }

    DropDataList& KFDropModule::Drop( KFEntity* player, uint32 dropid, uint32 count, const std::string& modulename, const char* function, uint32 line )
    {
        auto& drops = DropLogic( player, dropid, count, function, line );
#ifdef __KF_DEBUG__
        SendDropDataToClient( player, dropid, count, drops );
#endif // __KF_DEBUG__

        if ( !modulename.empty() )
        {
            player->AddDataToShow( modulename );
        }

        for ( auto iter = drops.begin(); iter != drops.end(); )
        {
            auto dropdata = *iter;
            auto kffunction = _drop_logic_function.Find( dropdata->_data_name );
            if ( kffunction != nullptr )
            {
                iter = drops.erase( iter );
                kffunction->_function( player, dropdata, function, line );
            }
            else
            {
                ++iter;
                player->AddElement( &dropdata->_elements, function, line );
            }
        }

        return drops;
    }

    DropDataList& KFDropModule::DropLogic( KFEntity* player, uint32 dropid, uint32 count, const char* function, uint32 line )
    {
        static DropDataList _drops;
        _drops.clear();

        // 掉落逻辑
        DropLogic( player, dropid, count, _drops, function, line );
        return _drops;
    }

    void KFDropModule::DropLogic( KFEntity* player, uint32 dropid, uint32 count, DropDataList& outlist, const char* function, uint32 line )
    {
        if ( dropid == 0u )
        {
            return;
        }

        __LOG_INFO_FUNCTION__( function, line, "player=[{}] drop=[{}] count=[{}]", player->GetKeyID(), dropid, count );
        auto kfsetting = KFDropGroupConfig::Instance()->FindSetting( dropid );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "dropid=[{}] can't find setting!", dropid );
        }

        for ( auto i = 0u; i < count; ++i )
        {
            DropLogic( player, kfsetting, outlist );
        }
    }

    void KFDropModule::DropLogic( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist )
    {
        if ( kfsetting->_is_drop_count )
        {
            player->UpdateData( __STRING__( drop ), kfsetting->_id, __STRING__( value ), KFEnum::Add, 1u );
        }

        // 必掉的列表
        for ( auto kfdropweight : kfsetting->_necessary_list._objects )
        {
            // 判断条件
            auto ok = _kf_condition->CheckCondition( player, &kfdropweight->_conditions );
            if ( ok )
            {
                RandDropData( player, kfsetting, outlist, kfdropweight, __FUNC_LINE__ );
            }
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机的列表
        switch ( kfsetting->_condition_type )
        {
        case KFDropSetting::MutexCondition:
            DropMutexCondition( player, kfsetting, outlist );
            break;
        case KFDropSetting::OverlayCondition:
            DropOverlayCondition( player, kfsetting, outlist );
            break;
        default:
            __LOG_ERROR__( "drop=[{}] conditiontype=[{}] error!", kfsetting->_id, kfsetting->_condition_type );
            break;
        }
    }

    void KFDropModule::DropMutexCondition( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist )
    {
        bool ishaveconditiondrop = false;
        UInt32Set excludelist;
        for ( auto kfdropweight : kfsetting->_rand_list._weight_data )
        {
            auto ok = _kf_condition->CheckCondition( player, &kfdropweight->_conditions );
            if ( ok )
            {
                if ( !kfdropweight->_conditions.IsEmpty() )
                {
                    // 设置已经有了条件掉落
                    ishaveconditiondrop = true;
                }
                else
                {
                    // 如果是无条件掉落, 并且已经有了条件掉落, 排除这个无条件掉落
                    if ( ishaveconditiondrop )
                    {
                        excludelist.insert( kfdropweight->_id );
                    }
                }
            }
            else
            {
                excludelist.insert( kfdropweight->_id );
            }
        }

        RandDropDataList( player, kfsetting, outlist, excludelist );
    }

    void KFDropModule::DropOverlayCondition( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist )
    {
        UInt32Set excludelist;
        for ( auto kfdropweight : kfsetting->_rand_list._weight_data )
        {
            auto ok = _kf_condition->CheckCondition( player, &kfdropweight->_conditions );
            if ( !ok )
            {
                excludelist.insert( kfdropweight->_id );
            }
        }

        RandDropDataList( player, kfsetting, outlist, excludelist );
    }

    void KFDropModule::RandDropDataList( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const UInt32Set& excludelist )
    {
        switch ( kfsetting->_rand_type )
        {
        case KFRandEnum::Weight:
            RandDropDataByWeight( player, kfsetting, outlist, excludelist );
            break;
        case KFRandEnum::Probability:
            RandDropDataByProbability( player, kfsetting, outlist, excludelist );
            break;
        default:
            __LOG_ERROR__( "drop=[{}] randtype=[{}] error", kfsetting->_id, kfsetting->_rand_type );
            break;
        }
    }

    void KFDropModule::RandDropDataByWeight( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const UInt32Set& excludelist )
    {
        auto kfdropweight = kfsetting->_rand_list.Rand( excludelist, true );
        if ( kfdropweight != nullptr )
        {
            RandDropData( player, kfsetting, outlist, kfdropweight, __FUNC_LINE__ );
        }
    }

    void KFDropModule::RandDropDataByProbability( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const UInt32Set& excludelist )
    {
        for ( auto kfdropweight : kfsetting->_rand_list._weight_data )
        {
            if ( kfdropweight->_weight == 0u )
            {
                continue;
            }

            // 判断不在列表中
            if ( excludelist.find( kfdropweight->_id ) != excludelist.end() )
            {
                continue;
            }

            // 判断概率
            auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::TenThousand );
            if ( rand >= kfdropweight->_weight )
            {
                continue;
            }

            RandDropData( player, kfsetting, outlist, kfdropweight, __FUNC_LINE__ );
        }
    }

    void KFDropModule::RandDropData( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const KFDropGroupWeight* kfdropweight, const char* function, uint32 line )
    {
        // 如果需要重置
        if ( kfdropweight->_is_clear_var && kfsetting->_is_drop_count )
        {
            player->UpdateData( __STRING__( drop ), kfsetting->_id, __STRING__( value ), KFEnum::Set, 0u );
        }

        if ( kfdropweight->_drop_data_setting == nullptr )
        {
            return;
        }

        auto kfdropdataweight = kfdropweight->_drop_data_setting->_drop_data_list.Rand();
        if ( kfdropdataweight == nullptr )
        {
            return;
        }

        auto dropdata = &kfdropdataweight->_drop_data;
        if ( dropdata->_max_value == 0u )
        {
            return;
        }

        if ( dropdata->_data_name == __STRING__( drop ) )
        {
            // 如果是掉落的话, 继续执行掉落逻辑
            DropLogic( player, dropdata->GetValue(), 1u, outlist, function, line );
        }
        else
        {
            outlist.push_back( dropdata );
        }
    }
}