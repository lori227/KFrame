#include "KFDropModule.hpp"
#include "KFProtocol/KFProtocol.h"


namespace KFrame
{
    void KFDropModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_ADD_ELEMENT__( __STRING__( drop ), &KFDropModule::AddDropElement );
        __REGISTER_DROP_LOGIC__( __STRING__( data ), &KFDropModule::OnDropDataElement );
    }

    void KFDropModule::BeforeShut()
    {
        __UN_ADD_ELEMENT__( __STRING__( drop ) );
        __UN_DROP_LOGIC__( __STRING__( data ) );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_ELEMENT_FUNCTION__( KFDropModule::AddDropElement )
    {
        auto kfelement = kfresult->_element;
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfelement->_data_name );
            return false;
        }
#ifdef __KF_DEBUG__
        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", kfelement->_data_name );
            return false;
        }

        auto count = kfelementobject->CalcValue( kfparent->_data_setting, kfparent->_data_setting->_value_key_name );
        Drop( player, kfelementobject->_config_id, __MAX__( count, 1u ), __STRING__( command ), 0u, __FUNC_LINE__ );
#endif

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_DROP_LOGIC_FUNCTION__( KFDropModule::OnDropDataElement )
    {
        player->AddElement( &dropdata->_elements, _default_multiple, modulename, moduleid, function, line );
    }
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
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    const DropDataList& KFDropModule::Drop( KFEntity* player, uint32 dropid, const std::string& modulename, uint64 moduleid, const char* function, uint32 line )
    {
        return Drop( player, dropid, 1u, modulename, moduleid, function, line );
    }

    const DropDataList& KFDropModule::Drop( KFEntity* player, const UInt32Vector& droplist, const std::string& modulename, uint64 moduleid, const char* function, uint32 line )
    {
        static DropDataList _drop_data_list;
        _drop_data_list.clear();

        for ( auto dropid : droplist )
        {
            RandDropLogic( player, dropid, 1u, _drop_data_list, function, line );
        }

        // 执行掉落
        ExecuteDropLogic( player, _drop_data_list, modulename, moduleid, function, line );
        return _drop_data_list;
    }

    const DropDataList& KFDropModule::Drop( KFEntity* player, uint32 dropid, uint32 count, const std::string& modulename, uint64 moduleid, const char* function, uint32 line )
    {
        static DropDataList _drop_data_list;
        _drop_data_list.clear();

        // 随机掉落数据
        RandDropLogic( player, dropid, count, _drop_data_list, function, line );

        // 执行掉落
        ExecuteDropLogic( player, _drop_data_list, modulename, moduleid, function, line );
        return _drop_data_list;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFDropModule::ExecuteDropLogic( KFEntity* player, const DropDataList& dropdatalist, const std::string& modulename, uint64 moduleid, const char* function, uint32 line )
    {
        player->AddDataToShow( modulename, moduleid );

        // 执行掉落逻辑
        for ( auto iter = dropdatalist.begin(); iter != dropdatalist.end(); ++iter )
        {
            auto dropdata = *iter;
            auto kffunction = _drop_logic_function.Find( dropdata->_logic_name );
            if ( kffunction != nullptr )
            {
                kffunction->_function( player, dropdata, modulename, moduleid, function, line );
            }
            else
            {
                __LOG_ERROR_FUNCTION__( function, line, "drop logicname=[{}] no function!", dropdata->_logic_name );
            }
        }

        // 直接同步掉落属性
        player->SyncDataToClient();
    }

#ifdef __KF_DEBUG__
    void KFDropModule::SendDropDataToClient( KFEntity* player, uint32 dropid, uint32 count, const DropDataList& droplist )
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
            pbdata->set_logicname( dropdata->_logic_name );
        }

        _kf_player->SendToClient( player, KFMsg::MSG_DEBUG_SHOW_DROP, &show );
    }
#endif // __KF_DEBUG__
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFDropModule::RandDropLogic( KFEntity* player, uint32 dropid, uint32 count, DropDataList& outlist, const char* function, uint32 line )
    {
        __LOG_INFO_FUNCTION__( function, line, "player=[{}] drop=[{}] count=[{}]", player->GetKeyID(), dropid, count );
        auto kfsetting = KFDropGroupConfig::Instance()->FindSetting( dropid );
        if ( kfsetting == nullptr )
        {
            _kf_display->SendToClient( player, KFMsg::DropSettingError, dropid );
            return __LOG_ERROR_FUNCTION__( function, line, "dropid=[{}] can't find setting", dropid );
        }

        DropDataList dropdatalist;
        for ( auto i = 0u; i < count; ++i )
        {
            RandDropLogic( player, kfsetting, dropdatalist );
        }

#ifdef __KF_DEBUG__
        SendDropDataToClient( player, dropid, count, dropdatalist );
#endif

        outlist.splice( outlist.end(), dropdatalist );
    }

    void KFDropModule::RandDropLogic( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist )
    {
        if ( kfsetting->_is_drop_count )
        {
            player->UpdateData( __STRING__( drop ), kfsetting->_id, __STRING__( value ), KFEnum::Add, 1u );
        }

        // 必掉的列表
        UInt32Set excludedropdata;
        for ( auto kfdropweight : kfsetting->_necessary_list._objects )
        {
            // 判断条件
            auto ok = _kf_condition->CheckCondition( player, &kfdropweight->_conditions );
            if ( ok )
            {
                RandDropData( player, kfsetting, outlist, kfdropweight, excludedropdata, __FUNC_LINE__ );
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
            __LOG_ERROR__( "drop=[{}] conditiontype=[{}] error", kfsetting->_id, kfsetting->_condition_type );
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
            UInt32Set excludedropdata;
            RandDropData( player, kfsetting, outlist, kfdropweight, excludedropdata, __FUNC_LINE__ );
        }
    }

    void KFDropModule::RandDropDataByProbability( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const UInt32Set& excludelist )
    {
        UInt32Set excludedropdata;
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
            if ( KFGlobal::Instance()->RandCheck( KFRandEnum::TenThousand, kfdropweight->_weight ) )
            {
                RandDropData( player, kfsetting, outlist, kfdropweight, excludedropdata, __FUNC_LINE__ );
            }
        }
    }

    void KFDropModule::RandDropData( KFEntity* player, const KFDropSetting* kfsetting, DropDataList& outlist, const KFDropGroupWeight* kfdropweight, UInt32Set& excludedatalist, const char* function, uint32 line )
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

        auto kfdropdataweight = kfdropweight->_drop_data_setting->_drop_data_list.Rand( excludedatalist, true );
        if ( kfdropdataweight == nullptr )
        {
            return;
        }

        auto dropdata = &kfdropdataweight->_drop_data;
        if ( !dropdata->IsValid() )
        {
            _kf_display->SendToClient( player, KFMsg::DropValueError, dropdata->_drop_data_id );
            return __LOG_ERROR_FUNCTION__( function, line, "dropgroup=[{}] dropdata=[{}] value=0", kfsetting->_id, dropdata->_drop_data_id );
        }

        if ( dropdata->_logic_name == __STRING__( drop ) )
        {
            // 如果是掉落的话, 继续执行掉落逻辑
            RandDropLogic( player, dropdata->GetValue(), 1u, outlist, function, line );
        }
        else
        {
            outlist.push_back( dropdata );
        }

        // 添加排除项
        if ( kfsetting->_is_exclude )
        {
            excludedatalist.insert( kfdropdataweight->_id );
        }
    }
}