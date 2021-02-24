#include "KFDropModule.hpp"
#include "KFProtocol/KFProtocol.h"


namespace KFrame
{
    void KFDropModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_ADD_ELEMENT__( __STRING__( drop ), &KFDropModule::AddDropElement );
        __REGISTER_DROP_LOGIC__( __STRING__( data ), &KFDropModule::OnDropDataElement );
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_DEPLOY_FUNCTION__( __STRING__( dropopen ), &KFDropModule::OnDeployDropOpen );
        __REGISTER_DEPLOY_FUNCTION__( __STRING__( dropclose ), &KFDropModule::OnDeployDropClose );
    }

    void KFDropModule::BeforeShut()
    {
        __UN_ADD_ELEMENT__( __STRING__( drop ) );
        __UN_DROP_LOGIC__( __STRING__( data ) );
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_DEPLOY_FUNCTION__( __STRING__( dropopen ) );
        __UN_DEPLOY_FUNCTION__( __STRING__( dropclose ) );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_ELEMENT_FUNCTION__( KFDropModule::AddDropElement )
    {
        auto element = kfresult->_element;
        if ( !element->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", element->_data_name );
            return false;
        }
        
#ifdef __KF_DEBUG__
        auto element_object = std::static_pointer_cast<KFElementObject>( element );
        if ( element_object->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", element->_data_name );
            return false;
        }

        auto count = element_object->CalcValue( kfparent->_data_setting, kfparent->_data_setting->_value_key_name );
        Drop( player, element_object->_config_id, __MAX__( count, 1u ), __STRING__( command ), 0u, __FUNC_LINE__ );
#endif

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_DROP_LOGIC_FUNCTION__( KFDropModule::OnDropDataElement )
    {
        player->AddElement( &drop_data->_elements, _default_multiple, module_name, module_id, function, line );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFDropModule::BindDropLogicFunction( const std::string& logic_name, KFModule* module, KFDropLogicFunction& function )
    {
        auto function_data = _drop_logic_function.Create( logic_name );
        function_data->SetFunction( module, function );
    }

    void KFDropModule::UnRegisterDropLogicFunction( const std::string& logic_name )
    {
        _drop_logic_function.Remove( logic_name );
    }

    void KFDropModule::SetDropLogicOpen( const std::string& logic_name, bool is_open )
    {
        auto function_data = _drop_logic_function.Find( logic_name );
        if ( function_data != nullptr )
        {
            function_data->SetOpen( is_open );
        }
    }

    __KF_DEPLOY_FUNCTION__( KFDropModule::OnDeployDropOpen )
    {
        SetDropLogicOpen( param, true );
    }

    __KF_DEPLOY_FUNCTION__( KFDropModule::OnDeployDropClose )
    {
        SetDropLogicOpen( param, false );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    const DropDataList& KFDropModule::Drop( EntityPtr player, uint32 drop_id, const std::string& module_name, uint64 module_id, const char* function, uint32 line )
    {
        return Drop( player, drop_id, 1u, module_name, module_id, function, line );
    }

    const DropDataList& KFDropModule::Drop( EntityPtr player, const UInt32Vector& drop_list, const std::string& module_name, uint64 module_id, const char* function, uint32 line )
    {
        // 逻辑里面可能还会触发drop, 所以这里不能使用static
        DropDataList drop_data_list;
        for ( auto drop_id : drop_list )
        {
            RandDropLogic( player, drop_id, 1u, drop_data_list, function, line );
        }

        // 执行掉落
        ExecuteDropLogic( player, drop_data_list, module_name, module_id, function, line );

        // 返回列表
        static DropDataList _drop_data_list;
        _drop_data_list.swap( drop_data_list );
        return _drop_data_list;
    }

    const DropDataList& KFDropModule::Drop( EntityPtr player, uint32 drop_id, uint32 count, const std::string& module_name, uint64 module_id, const char* function, uint32 line )
    {
        // 逻辑里面可能还会触发drop, 所以这里不能使用static
        DropDataList drop_data_list;
        RandDropLogic( player, drop_id, count, drop_data_list, function, line );

        // 执行掉落
        ExecuteDropLogic( player, drop_data_list, module_name, module_id, function, line );

        // 返回列表
        static DropDataList _drop_data_list;
        _drop_data_list.swap( drop_data_list );
        return _drop_data_list;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFDropModule::ExecuteDropLogic( EntityPtr player, const DropDataList& drop_data_list, const std::string& module_name, uint64 module_id, const char* function, uint32 line )
    {
        player->SetDataShowModule( module_name, module_id );

        // 执行掉落逻辑
        for ( auto drop_data : drop_data_list )
        {
            auto function_data = _drop_logic_function.Find( drop_data->_logic_name );
            if ( function_data != nullptr )
            {
                function_data->Call( player, drop_data, module_name, module_id, function, line );
            }
            else
            {
                __LOG_ERROR_FUNCTION__( function, line, "drop logic_name=[{}] no function!", drop_data->_logic_name );
            }
        }

        // 直接同步掉落属性
        player->SyncDataToClient();
    }

#ifdef __KF_DEBUG__
    void KFDropModule::SendDropDataToClient( EntityPtr player, uint32 drop_id, uint32 count, const DropDataList& drop_list )
    {
        KFMsg::MsgDebugShowDrop show;
        show.set_dropid( drop_id );
        show.set_count( count );

        for ( auto drop_data : drop_list )
        {
            auto show_data = show.add_dropdata();
            show_data->set_dataid( drop_data->_drop_data_id );
            show_data->set_dataindex( drop_data->_data_index );
            show_data->set_data_name( drop_data->_data_name );
            show_data->set_logicname( drop_data->_logic_name );
        }

        _kf_player->SendToClient( player, KFMsg::MSG_DEBUG_SHOW_DROP, &show );
    }
#endif // __KF_DEBUG__
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFDropModule::RandDropLogic( EntityPtr player, uint32 drop_id, uint32 count, DropDataList& out_list, const char* function, uint32 line )
    {
        __LOG_INFO_FUNCTION__( function, line, "player=[{}] drop=[{}] count=[{}]", player->GetKeyID(), drop_id, count );
        auto setting = KFDropGroupConfig::Instance()->FindSetting( drop_id );
        if ( setting == nullptr )
        {
            _kf_display->SendToClient( player, KFMsg::DropSettingError, drop_id );
            return __LOG_ERROR_FUNCTION__( function, line, "drop_id=[{}] can't find setting", drop_id );
        }

        DropDataList drop_data_list;
        for ( auto i = 0u; i < count; ++i )
        {
            RandDropLogic( player, setting, drop_data_list );
        }

#ifdef __KF_DEBUG__
        SendDropDataToClient( player, drop_id, count, drop_data_list );
#endif

        out_list.splice( out_list.end(), drop_data_list );
    }

    void KFDropModule::RandDropLogic( EntityPtr player, std::shared_ptr<const KFDropSetting> setting, DropDataList& out_list )
    {
        if ( setting->_is_drop_count )
        {
            player->UpdateRecord( __STRING__( drop ), setting->_id, __STRING__( value ), KFEnum::Add, 1u );
        }

        // 必掉的列表
        UInt32Set exclude_drop_data;
        for ( auto drop_weight : setting->_necessary_list._objects )
        {
            // 判断条件
            auto ok = _kf_condition->CheckStaticCondition( player, drop_weight->_conditions );
            if ( ok )
            {
                RandDropData( player, setting, out_list, drop_weight, exclude_drop_data, __FUNC_LINE__ );
            }
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 随机的列表
        switch ( setting->_condition_type )
        {
        case KFDropSetting::MutexCondition:
            DropMutexCondition( player, setting, out_list );
            break;
        case KFDropSetting::OverlayCondition:
            DropOverlayCondition( player, setting, out_list );
            break;
        default:
            __LOG_ERROR__( "drop=[{}] conditiontype=[{}] error", setting->_id, setting->_condition_type );
            break;
        }
    }

    void KFDropModule::DropMutexCondition( EntityPtr player, std::shared_ptr<const KFDropSetting> setting, DropDataList& out_list )
    {
        bool is_have_condition_drop = false;
        UInt32Set exclude_list;
        for ( auto drop_weight : setting->_rand_list._weight_data )
        {
            auto ok = _kf_condition->CheckStaticCondition( player, drop_weight->_conditions );
            if ( ok )
            {
                if ( !drop_weight->_conditions->IsEmpty() )
                {
                    // 设置已经有了条件掉落
                    is_have_condition_drop = true;
                }
                else
                {
                    // 如果是无条件掉落, 并且已经有了条件掉落, 排除这个无条件掉落
                    if ( is_have_condition_drop )
                    {
                        exclude_list.insert( drop_weight->_value );
                    }
                }
            }
            else
            {
                exclude_list.insert( drop_weight->_value );
            }
        }

        RandDropDataList( player, setting, out_list, exclude_list );
    }

    void KFDropModule::DropOverlayCondition( EntityPtr player, std::shared_ptr<const KFDropSetting> setting, DropDataList& out_list )
    {
        UInt32Set exclude_list;
        for ( auto drop_weight : setting->_rand_list._weight_data )
        {
            auto ok = _kf_condition->CheckStaticCondition( player, drop_weight->_conditions );
            if ( !ok )
            {
                exclude_list.insert( drop_weight->_value );
            }
        }

        RandDropDataList( player, setting, out_list, exclude_list );
    }

    void KFDropModule::RandDropDataList( EntityPtr player, std::shared_ptr<const KFDropSetting> setting, DropDataList& out_list, const UInt32Set& exclude_list )
    {
        switch ( setting->_rand_type )
        {
        case KFRandEnum::Weight:
            RandDropDataByWeight( player, setting, out_list, exclude_list );
            break;
        case KFRandEnum::Probability:
            RandDropDataByProbability( player, setting, out_list, exclude_list );
            break;
        default:
            __LOG_ERROR__( "drop=[{}] randtype=[{}] error", setting->_id, setting->_rand_type );
            break;
        }
    }

    void KFDropModule::RandDropDataByWeight( EntityPtr player, std::shared_ptr<const KFDropSetting> setting, DropDataList& out_list, const UInt32Set& exclude_list )
    {
        auto drop_weight = setting->_rand_list.RandExcludeList( exclude_list );
        if ( drop_weight != nullptr )
        {
            UInt32Set exclude_drop_data;
            RandDropData( player, setting, out_list, drop_weight, exclude_drop_data, __FUNC_LINE__ );
        }
    }

    void KFDropModule::RandDropDataByProbability( EntityPtr player, std::shared_ptr<const KFDropSetting> setting, DropDataList& out_list, const UInt32Set& exclude_list )
    {
        UInt32Set exclude_drop_data;
        for ( auto drop_weight : setting->_rand_list._weight_data )
        {
            if ( drop_weight->_weight == 0u )
            {
                continue;
            }

            // 判断不在列表中
            if ( exclude_list.find( drop_weight->_value ) != exclude_list.end() )
            {
                continue;
            }

            // 判断概率
            if ( KFGlobal::Instance()->RandCheck( KFRandEnum::TenThousand, drop_weight->_weight ) )
            {
                RandDropData( player, setting, out_list, drop_weight, exclude_drop_data, __FUNC_LINE__ );
            }
        }
    }

    void KFDropModule::RandDropData( EntityPtr player, std::shared_ptr<const KFDropSetting> setting, DropDataList& out_list,
                                     std::shared_ptr<const KFDropGroupWeight> drop_weight, UInt32Set& exclude_data_list, const char* function, uint32 line )
    {
        // 如果需要重置
        if ( drop_weight->_is_clear_var && setting->_is_drop_count )
        {
            player->UpdateRecord( __STRING__( drop ), setting->_id, __STRING__( value ), KFEnum::Set, 0u );
        }

        if ( drop_weight->_drop_data_setting == nullptr )
        {
            return;
        }

        auto drop_data_weight = drop_weight->_drop_data_setting->_drop_data_list.RandExcludeList( exclude_data_list );
        if ( drop_data_weight == nullptr )
        {
            return;
        }

        auto drop_data = &drop_data_weight->_drop_data;
        if ( !drop_data->IsValid() )
        {
            _kf_display->SendToClient( player, KFMsg::DropValueError, drop_data->_drop_data_id );
            return __LOG_ERROR_FUNCTION__( function, line, "dropgroup=[{}] dropdata=[{}] value=0", setting->_id, drop_data->_drop_data_id );
        }

        if ( drop_data->_logic_name == __STRING__( drop ) )
        {
            // 如果是掉落的话, 继续执行掉落逻辑
            RandDropLogic( player, drop_data->GetValue(), 1u, out_list, function, line );
        }
        else
        {
            out_list.push_back( drop_data );
        }

        // 添加排除项
        if ( setting->_is_exclude )
        {
            exclude_data_list.insert( drop_data_weight->_value );
        }
    }
}