#include "KFItemModule.hpp"

namespace KFrame
{
    namespace KFItemEnum
    {
        enum EConstDefine
        {
            /////////////////////////////////////////////////////////////////////////////////////////////////////////
            // 物品类型
            None = 0,		// 无类型
            Normal = 1,		// 普通道具
            Gift = 2,		// 礼包, 抽奖道具
            Medicine = 3,	// 药品道具
            Weapon = 4,		// 武器道具
            Material = 5,	// 材料道具
            Script = 101,	// 脚本道具(不是真正的道具)
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            // 脚本调用类型
            AddFunction = 1,	// 添加物品调用
            UseFunction = 2,	// 使用物品调用
            RemoveFunction = 3,	// 删除物品调用
            MaxFunction = 4,	// 最大值
            /////////////////////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////////////////////////
            OverlayByCount = 1,	// 数量叠加
            OverlayByTime = 2,	// 时间叠加
            /////////////////////////////////////////////////////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////////////////////////
        };
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void KFItemModule::BeforeRun()
    {
        _component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_GET_CONFIG_VALUE__( __STRING__( item ), &KFItemModule::GetItemTotalCount );
        __REGISTER_ADD_DATA_1__( __STRING__( item ), &KFItemModule::OnAddItemCallBack );
        __REGISTER_UPDATE_DATA_2__( __STRING__( item ), __STRING__( count ), &KFItemModule::OnItemCountUpdateCallBack );
        __REGISTER_UPDATE_DATA_2__( __STRING__( item ), __STRING__( time ), &KFItemModule::OnItemTimeUpdateCallBack );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_CHECK_ADD_ELEMENT__( __STRING__( item ), &KFItemModule::CheckAddItemElement );
        __REGISTER_ADD_ELEMENT__( __STRING__( item ), &KFItemModule::AddItemElement );
        __REGISTER_CHECK_REMOVE_ELEMENT__( __STRING__( item ), &KFItemModule::CheckRemoveItemElement );
        __REGISTER_REMOVE_ELEMENT__( __STRING__( item ), &KFItemModule::RemoveItemElement );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_PLAYER_ENTER__( &KFItemModule::OnEnterItemModule );
        __REGISTER_PLAYER_LEAVE__( &KFItemModule::OnLeaveItemModule );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_REMOVE_ITEM_REQ, &KFItemModule::HandleRemoveItemReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_REMOVE_ITEM_COUNT_REQ, &KFItemModule::HandleRemoveItemCountReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_SELL_ITEM_REQ, &KFItemModule::HandleSellItemReq );
    }

    void KFItemModule::BeforeShut()
    {
        __UN_TIMER_0__();
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_GET_CONFIG_VALUE__( __STRING__( item ) );
        __UN_ADD_DATA_1__( __STRING__( item ) );
        __UN_UPDATE_DATA_2__( __STRING__( item ), __STRING__( count ) );
        __UN_UPDATE_DATA_2__( __STRING__( item ), __STRING__( time ) );

        __UN_CHECK_ADD_ELEMENT__( __STRING__( item ) );
        __UN_ADD_ELEMENT__( __STRING__( item ) );
        __UN_CHECK_REMOVE_ELEMENT__( __STRING__( item ) );
        __UN_REMOVE_ELEMENT__( __STRING__( item ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_PLAYER_ENTER__();
        __UN_PLAYER_LEAVE__();
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_REMOVE_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_REMOVE_ITEM_COUNT_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SELL_ITEM_REQ );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFItemModule::BindItemInitFunction( uint32 item_type, KFModule* module, KFItemInitFunction& function )
    {
        auto function_type_list = _item_init_function.Create( item_type );
        auto function_data = function_type_list->Create( module );
        function_data->SetFunction( module, function );
    }

    void KFItemModule::UnBindItemInitFunction( uint32 item_type, KFModule* module )
    {
        auto function_type_list = _item_init_function.Find( item_type );
        if ( function_type_list != nullptr )
        {
            function_type_list->Remove( module );
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleRemoveItemReq, KFMsg::MsgRemoveItemReq )
    {
        __LOG_INFO__( "player=[{}] remove item=[{}:{}]", player->GetKeyID(), kfmsg->sourcename(), kfmsg->uuid() );

        if ( kfmsg->uuid() == 0u )
        {
            player->ClearRecord( kfmsg->sourcename() );
        }
        else
        {
            player->RemoveRecord( kfmsg->sourcename(), kfmsg->uuid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleRemoveItemCountReq, KFMsg::MsgRemoveItemCountReq )
    {
        __LOG_INFO__( "player=[{}] remove item count=[{}:{}]", player->GetKeyID(), kfmsg->itemid(), kfmsg->count() );

        auto item_record = FindItemBag( player, kfmsg->itemid() );
        RemoveItem( player, item_record, kfmsg->itemid(), kfmsg->count() );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_GET_CONFIG_VALUE_FUNCTION__( KFItemModule::GetItemTotalCount )
    {
        auto item_record = FindItemBag( player, id );
        if ( item_record == nullptr )
        {
            return 0u;
        }

        return GetItemCount( player, item_record, id, max_value );
    }

    uint32 KFItemModule::GetItemCount( EntityPtr player, DataPtr item_record, uint32 item_id, uint32 max_count /* = __MAX_UINT32__ */ )
    {
        auto total_count = 0u;
        for ( auto item_data = item_record->First(); item_data != nullptr; item_data = item_record->Next() )
        {
            if ( item_id != item_data->Get<uint32>( __STRING__( id ) ) )
            {
                continue;
            }

            total_count += item_data->Get<uint32>( __STRING__( count ) );
            if ( total_count >= max_count )
            {
                break;
            }
        }

        return total_count;
    }

    __KF_CHECK_ADD_ELEMENT_FUNCTION__( KFItemModule::CheckAddItemElement )
    {
        if ( !element->IsObject() )
        {
            return false;
        }

        // 计算物品数量
        auto element_object = std::dynamic_pointer_cast<KFElementObject>( element );
        auto item_count = element_object->CalcValue( parent_data->_data_setting, __STRING__( count ), multiple );
        if ( item_count == _invalid_int || element_object->_config_id == _invalid_int )
        {
            return false;
        }


        auto item_record = FindItemBag( player, element_object->_config_id );
        if ( item_record != nullptr )
        {
            parent_data = item_record;
        }

        auto setting = KFItemConfig::Instance()->FindSetting( element_object->_config_id );
        if ( setting == nullptr )
        {
            return false;
        }

        return !CheckItemBagFull( player, parent_data, setting, item_count );
    }

    bool KFItemModule::CheckItemBagFull( EntityPtr player, DataPtr item_record, std::shared_ptr<const KFItemSetting> setting, uint32 item_count )
    {
        auto max_size = GetItemBagMaxCount( player, item_record );
        auto max_overlay_count = setting->_overlay_count;

        // 计算占用格子数
        auto current_size = item_record->Size();
        uint32 use_size = ( item_count + max_overlay_count - 1 ) / max_overlay_count;
        auto left_size = ( max_size > current_size ) ? ( max_size - current_size ) : 0u;
        if ( left_size >= use_size )
        {
            return false;
        }

        if ( setting->_overlay_count > 1u )
        {
            // 剩余格子可叠加物品数
            auto can_add_count = left_size * max_overlay_count;
            for ( auto item_data = item_record->First(); item_data != nullptr; item_data = item_record->Next() )
            {
                if ( setting->_id != item_data->Get( __STRING__( id ) ) )
                {
                    continue;
                }

                auto old_count = item_data->Get( __STRING__( count ) );
                can_add_count += max_overlay_count - __MIN__( old_count, max_overlay_count );
                if ( can_add_count >= item_count )
                {
                    return false;
                }
            }
        }

        return true;
    }


    uint32 KFItemModule::CalcItemAddCount( uint32 source_count, uint32 target_count, uint32 max_count )
    {
        if ( target_count >= max_count )
        {
            return 0u;
        }

        auto can_add_count = max_count - target_count;
        return source_count <= can_add_count ? source_count : can_add_count;
    }

    uint32 KFItemModule::GetCanAddItemCount( EntityPtr player, uint32 item_id, uint32 item_count )
    {
        auto setting = KFItemConfig::Instance()->FindSetting( item_id );
        if ( setting == nullptr )
        {
            __LOG_ERROR__( "item=[{}] no setting", setting->_id );
            return 0u;
        }

        auto item_record = FindItemBag( player, item_id );
        if ( item_record == nullptr )
        {
            return 0u;
        }

        auto max_size = GetItemBagMaxCount( player, item_record );
        auto max_overlay_count = setting->_overlay_count;

        // 计算占用格子数
        auto current_size = item_record->Size();
        uint32 use_size = ( item_count + max_overlay_count - 1 ) / max_overlay_count;
        auto left_size = ( max_size > current_size ) ? ( max_size - current_size ) : 0u;
        if ( left_size >= use_size )
        {
            return item_count;
        }

        // 不能叠加
        if ( setting->_overlay_count == 1u )
        {
            return left_size;
        }

        // 剩余格子可添加物品数
        auto can_add_count = left_size * max_overlay_count;
        for ( auto item_data = item_record->First(); item_data != nullptr; item_data = item_record->Next() )
        {
            if ( setting->_id != item_data->Get<uint32>( __STRING__( id ) ) )
            {
                continue;
            }

            auto old_count = item_data->Get( __STRING__( count ) );
            can_add_count += max_overlay_count - __MIN__( old_count, max_overlay_count );
            if ( can_add_count >= item_count )
            {
                return item_count;
            }
        }

        return can_add_count;
    }

    uint32 KFItemModule::GetItemBagMaxCount( EntityPtr player, DataPtr item_record )
    {
        auto item_bag_setting = KFItemBagConfig::Instance()->FindSetting( item_record->_data_setting->_name );
        if ( item_bag_setting == nullptr )
        {
            return item_record->_data_setting->_int_max_value;
        }

        return item_bag_setting->_max_count;
    }

    uint32 KFItemModule::GetItemEmptyCount( EntityPtr player, DataPtr item_record )
    {
        auto max_count = GetItemBagMaxCount( player, item_record );
        return max_count - item_record->Size();
    }

    bool KFItemModule::CheckItemBagFull( EntityPtr player, DataPtr item_record )
    {
        auto max_count = GetItemBagMaxCount( player, item_record );
        return item_record->Size() >= max_count;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_ELEMENT_FUNCTION__( KFItemModule::AddItemElement )
    {
        if ( !element_result->_element->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", element_result->_element->_data_name );
            return false;
        }

        auto element_object = std::dynamic_pointer_cast<KFElementObject>( element_result->_element );
        if ( element_object->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", element_result->_element->_data_name );
            return false;
        }

        auto item_setting = KFItemConfig::Instance()->FindSetting( element_object->_config_id );
        if ( item_setting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "id=[{}] itemsetting = null", element_object->_config_id );
            _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist, element_object->_config_id );
            return false;
        }

        auto item_type_setting = KFItemTypeConfig::Instance()->FindSetting( item_setting->_type );
        if ( item_type_setting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item_type=[{}] typesetting = null", item_setting->_type );
            _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist, element_object->_config_id );
            return false;
        }

        // 查找背包数据
        parent_data = FindFinalItemBag( player, parent_data, item_setting, item_type_setting, 0u );

        // 计算物品数量
        auto item_count = element_object->CalcValue( parent_data->_data_setting, __STRING__( count ), element_result->_multiple );
        if ( item_count == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] count = 0", element_object->_config_id );
            return false;
        }

        // 判断堆叠类型
        if ( item_setting->_overlay_type == KFItemEnum::OverlayByTime )
        {
            // 计算时间
            auto item_time = element_object->CalcValue( parent_data->_data_setting, __STRING__( time ), element_result->_multiple );
            if ( item_time == 0u )
            {
                __LOG_ERROR_FUNCTION__( function, line, "time item id=[{}] count = 0", element_object->_config_id );
                return false;
            }

            AddOverlayTimeItem( player, parent_data, element_result, item_setting, item_type_setting, item_count, item_time );
        }
        else if ( item_setting->_overlay_type == KFItemEnum::OverlayByCount )
        {
            // 按数量叠加道具
            if ( item_setting->_overlay_count > 1u )
            {
                AddOverlayCountItem( player, parent_data, element_result, item_setting, item_type_setting, item_count );
            }
            else
            {
                AddNotOverlayItem( player, parent_data, element_result, item_setting, item_type_setting, item_count );
            }
        }

        return true;
    }

    void KFItemModule::AddOverlayTimeItem( EntityPtr player, DataPtr item_record, KFElementResult* element_result,
                                           std::shared_ptr<const KFItemSetting> item_setting, std::shared_ptr<const KFItemTypeSetting> item_type_setting, uint32 count, uint32 time )
    {
        std::list<DataPtr> find_item_list;
        item_record->Find( __STRING__( id ), item_setting->_id, find_item_list, false );

        DataPtr item_data = nullptr;
        if ( !find_item_list.empty() )
        {
            item_data = find_item_list.front();
            player->UpdateObject( item_data, __STRING__( time ), KFEnum::Add, count * time );
        }
        else
        {
            item_data = AddNewItemData( player, item_record, item_setting, item_type_setting, count, time );
        }

        AddItemResult( element_result, item_setting->_id, item_data );
    }

    void KFItemModule::AddOverlayCountItem( EntityPtr player, DataPtr item_record, KFElementResult* element_result,
                                            std::shared_ptr<const KFItemSetting> item_setting, std::shared_ptr<const KFItemTypeSetting> item_type_setting, uint32 count )
    {
        auto max_overlay_count = item_setting->_overlay_count;

        // 查找相同道具的id
        auto total_add_count = 0u;
        for ( auto item_data = item_record->First(); item_data != nullptr; item_data = item_record->Next() )
        {
            if ( item_setting->_id != item_data->Get<uint32>( __STRING__( id ) ) )
            {
                continue;
            }

            auto old_count = item_data->Get( __STRING__( count ) );
            auto add_count = CalcItemAddCount( count, old_count, max_overlay_count );
            if ( add_count == 0u )
            {
                continue;
            }

            // 更新数量
            player->UpdateObject( item_data, __STRING__( count ), KFEnum::Add, add_count );
            count -= add_count;
            total_add_count += add_count;
            if ( count == 0u )
            {
                break;
            }
        }

        // 显示添加数量
        if ( total_add_count != 0u )
        {
            AddItemResult( element_result, item_record->_data_setting->_name, item_setting->_id, total_add_count );
        }

        // 添加完成, 直接返回
        if ( count == 0u )
        {
            return;
        }

        // 添加新物品
        do
        {
            auto item_data = AddNewItemData( player, item_record, item_setting, item_type_setting, count, 0u );
            AddItemResult( element_result, item_data->_data_setting->_name, item_setting->_id, item_data->Get<uint32>( __STRING__( count ) ) );
        } while ( count > 0u );
    }

    void KFItemModule::AddNotOverlayItem( EntityPtr player, DataPtr item_record, KFElementResult* element_result,
                                          std::shared_ptr<const KFItemSetting> item_setting, std::shared_ptr<const KFItemTypeSetting> item_type_setting, uint32 count )
    {
        // 添加新物品
        do
        {
            auto item_data = AddNewItemData( player, item_record, item_setting, item_type_setting, count, 0u );
            AddItemResult( element_result, item_setting->_id, item_data );
        } while ( count > 0u );
    }

    DataPtr KFItemModule::AddNewItemData( EntityPtr player, DataPtr item_record,
                                          std::shared_ptr<const KFItemSetting> item_setting, std::shared_ptr<const KFItemTypeSetting> item_type_setting, uint32& count, uint32 time )
    {
        auto max_overlay_count = item_setting->_overlay_count;
        auto add_count = __MIN__( count, max_overlay_count );

        // 判断满包情况, 需要添加另外的背包
        item_record = FindFinalItemBag( player, item_record, item_setting, item_type_setting, add_count );
        auto item_data = player->CreateData( item_record );

        // id
        item_data->Operate( __STRING__( id ), KFEnum::Set, item_setting->_id );

        // count
        item_data->Operate( __STRING__( count ), KFEnum::Set, add_count );

        // time
        if ( time != 0u )
        {
            item_data->Operate( __STRING__( time ), KFEnum::Set, KFGlobal::Instance()->_real_time + time * count );
        }

        // 初始化数据
        CallItemInitFunction(  player, item_data, item_setting );

        // uuid
        auto uuid = KFGlobal::Instance()->STMakeUuid( __STRING__( item ) );
        item_data->Operate( __STRING__( uuid ), KFEnum::Set, uuid );
        
        // 添加新的物品
        count -= add_count;
        player->AddRecord( item_record, uuid, item_data );
        return item_data;
    }

    void KFItemModule::CallItemInitFunction( EntityPtr player, DataPtr item_data, std::shared_ptr<const KFItemSetting> item_setting )
    {
        auto function_type_list = _item_init_function.Find( item_setting->_type );
        if ( function_type_list == nullptr )
        {
            return;
        }

        for ( auto& iter : function_type_list->_objects )
        {
            iter.second->Call( player, item_data, item_setting );
        }
    }

    DataPtr KFItemModule::CreateItem( EntityPtr player, uint32 item_id, DataPtr item_data, const char* function, uint32 line )
    {
        // 没有传入item, 创建一个新的item
        if ( item_data == nullptr )
        {
            auto item_record = FindItemBag( player, item_id );
            if ( item_record == nullptr )
            {
                return nullptr;
            }

            item_data = player->CreateData( item_record );
            if ( item_data == nullptr )
            {
                return nullptr;
            }
        }

        auto setting = KFItemConfig::Instance()->FindSetting( item_id );
        if ( setting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item=[{}] can't find setting", item_id );
            _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist, item_id );
            return nullptr;
        }

        // id
        item_data->Operate( __STRING__( id ), KFEnum::Set, setting->_id );

        // count
        item_data->Operate( __STRING__( count ), KFEnum::Set, 1u );

        // uuid
        auto uuid = KFGlobal::Instance()->STMakeUuid( __STRING__( item ) );
        item_data->Operate( __STRING__( uuid ), KFEnum::Set, setting->_id  );

        // 初始化数据
        CallItemInitFunction( player, item_data, setting );
        return item_data;
    }

    DataPtr KFItemModule::FindItemBag( EntityPtr player, DataPtr item_data )
    {
        auto item_id = item_data->Get<uint32>( __STRING__( id ) );
        return FindItemBag( player, item_id );
    }

    DataPtr KFItemModule::FindItemBag( EntityPtr player, uint32 item_id )
    {
        auto item_setting = KFItemConfig::Instance()->FindSetting( item_id );
        if ( item_setting == nullptr )
        {
            __LOG_ERROR__( "item=[{}] can't find item setting", item_id );
            return nullptr;
        }

        auto item_type_setting = KFItemTypeConfig::Instance()->FindSetting( item_setting->_type );
        if ( item_type_setting == nullptr )
        {
            __LOG_ERROR__( "item=[{}] can't find type=[{}] setting", item_id, item_setting->_type );
            return nullptr;
        }

        auto item_record = player->Find( item_type_setting->_obtain_bag );
        if ( item_record == nullptr )
        {
            __LOG_ERROR__( "bag=[{}] can't itembag record", item_type_setting->_obtain_bag );
            return nullptr;
        }

        return FindFinalItemBag( player, item_record, item_setting, item_type_setting, 0u );
    }


    DataPtr KFItemModule::FindFinalItemBag( EntityPtr player, DataPtr item_record, std::shared_ptr<const KFItemSetting> item_setting, std::shared_ptr<const KFItemTypeSetting> item_type_setting, uint32 item_count )
    {
        {
            auto find_item_record = player->Find( item_type_setting->_obtain_bag );
            if ( find_item_record != nullptr )
            {
                item_record = find_item_record;
            }
            else
            {
                __LOG_ERROR__( "item=[{}] type=[{}] can't find item bag", item_setting->_id, item_setting->_type );
            }
        }

        if ( item_count > 0u )
        {
            // 判断包裹是否满了
            auto item_bag_setting = KFItemBagConfig::Instance()->FindSetting( item_record->_data_setting->_name );
            if ( item_bag_setting != nullptr && !item_bag_setting->_extend_bag.empty() )
            {
                auto is_full = CheckItemBagFull( player, item_record, item_setting, item_count );
                if ( is_full )
                {
                    auto find_item_record = player->Find( item_bag_setting->_extend_bag );
                    if ( find_item_record != nullptr )
                    {
                        item_record = find_item_record;
                    }
                }
            }
        }

        return item_record;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_UPDATE_DATA_FUNCTION__( KFItemModule::OnItemCountUpdateCallBack )
    {
        if ( new_value > 0u )
        {
            return;
        }

        // 数量为0, 删除道具
        player->RemoveRecord( data->GetParent()->GetParent()->GetDataPtr(), key );
    }

    const std::list<DataPtr>& KFItemModule::FindRemoveItemList( EntityPtr player, DataPtr item_record, std::shared_ptr<const KFItemSetting> setting, uint32 item_count )
    {
        std::list<DataPtr> item_record_list;
        item_record_list.push_back( item_record );

        auto item_bag_setting = KFItemBagConfig::Instance()->FindSetting( item_record->_data_setting->_name );
        if ( item_bag_setting != nullptr && !item_bag_setting->_use_child_bag.empty() )
        {
            for ( auto& child_bag_name : item_bag_setting->_use_child_bag )
            {
                auto child_item_record = player->Find( child_bag_name );
                if ( child_item_record != nullptr )
                {
                    item_record_list.push_back( child_item_record );
                }
            }
        }

        static std::list<DataPtr> _result_list;
        _result_list.clear();

        auto total_count = 0u;
        for ( auto find_item_record : item_record_list )
        {
            for ( auto item_data = find_item_record->First(); item_data != nullptr; item_data = find_item_record->Next() )
            {
                if ( setting->_id != item_data->Get( __STRING__( id  ) ) )
                {
                    continue;
                }

                _result_list.push_back( item_data );
                total_count += item_data->Get<uint32>( __STRING__( count ) );
                if ( total_count >= item_count )
                {
                    return _result_list;
                }
            }
        }

        _result_list.clear();
        return _result_list;
    }

    __KF_CHECK_REMOVE_ELEMENT_FUNCTION__( KFItemModule::CheckRemoveItemElement )
    {
        if ( !element->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", element->_data_name );
            return false;
        }

        auto element_object = std::dynamic_pointer_cast<KFElementObject>( element );
        if ( element_object->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", element->_data_name );
            return false;
        }

        // 判断数量
        auto item_count = element_object->CalcValue( parent_data->_data_setting, __STRING__( count ), multiple );
        if ( item_count == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] count = 0", element_object->_config_id );
            return false;
        }

        auto setting = KFItemConfig::Instance()->FindSetting( element_object->_config_id );
        if ( setting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] setting = nullptr", element_object->_config_id );
            _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist, element_object->_config_id );
            return false;
        }

        auto& item_list = FindRemoveItemList( player, parent_data, setting, item_count );
        return !item_list.empty();
    }

    __KF_REMOVE_ELEMENT_FUNCTION__( KFItemModule::RemoveItemElement )
    {
        if ( !element_result->_element->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", element_result->_element->_data_name );
            return false;
        }

        auto element_object = std::dynamic_pointer_cast<KFElementObject>( element_result->_element );
        if ( element_object->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", element_result->_element->_data_name );
            return false;
        }

        auto item_count = element_object->GetValue( __STRING__( count ) );
        if ( item_count == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] count = 0", element_object->_config_id );
            return false;
        }

        auto setting = KFItemConfig::Instance()->FindSetting( element_object->_config_id );
        if ( setting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] can't find setting", element_object->_config_id );
            return false;
        }

        // 删除道具数量
        auto& item_list = FindRemoveItemList( player, parent_data, setting, item_count );
        for ( auto item_data : item_list )
        {
            auto remove_count = RemoveItemCount( player, item_data, item_count );
            item_count -= remove_count;
            element_result->AddResult( setting->_id, item_data->_data_setting->_name, __STRING__( count ), remove_count );
            if ( item_count == _invalid_int )
            {
                break;
            }
        }

        return true;
    }

    uint32 KFItemModule::RemoveItemCount( EntityPtr player, DataPtr item_data, uint32 item_count )
    {
        auto old_count = item_data->Get<uint32>( __STRING__( count ) );
        if ( item_count >= old_count )
        {
            item_count = old_count;
            player->RemoveRecord( item_data->GetParent()->GetDataPtr(), item_data->GetKeyID() );
        }
        else
        {
            player->UpdateObject( item_data, __STRING__( count ), KFEnum::Dec, item_count );
        }

        return item_count;
    }

    void KFItemModule::RemoveItem( EntityPtr player, DataPtr item_record, uint32 item_id, uint32 item_count )
    {
        if ( item_count == 0u )
        {
            return;
        }

        auto find_count = 0u;
        std::list<DataPtr> find_item_list;
        for ( auto item_data = item_record->First(); item_data != nullptr; item_data = item_record->Next() )
        {
            if ( item_id != item_data->Get<uint32>( __STRING__( id ) ) )
            {
                continue;
            }

            find_item_list.push_back( item_data );
            find_count += item_data->Get<uint32>( __STRING__( count ) );
            if ( find_count >= item_count )
            {
                break;
            }
        }

        for ( auto item_data : find_item_list )
        {
            auto remove_count = RemoveItemCount( player, item_data, item_count );
            item_count -= remove_count;
            if ( item_count == _invalid_int )
            {
                break;
            }
        }
    }

    __KF_ADD_DATA_FUNCTION__( KFItemModule::OnAddItemCallBack )
    {
        auto item_time = data->Get<uint64>();
        if ( item_time != 0u )
        {
            StartItemCheckTimer( player, data );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFItemModule::OnItemTimeUpdateCallBack )
    {
        if ( new_value == 0u )
        {
            // 如果变成永久道具, 取消定时器
            __UN_TIMER_2__( player->GetKeyID(), key );
        }
        else
        {
            // 更新定时器
            StartItemCheckTimer( player, data->GetParent()->GetDataPtr() );
        }
    }

    __KF_PLAYER_ENTER_FUNCTION__( KFItemModule::OnEnterItemModule )
    {
        for ( auto& iter : KFItemBagConfig::Instance()->_setting_list._objects )
        {
            auto item_record = player->Find( iter.first );
            for ( auto item_data = item_record->First(); item_data != nullptr; item_data = item_record->Next() )
            {
                auto time = item_data->Get<uint64>( __STRING__( time ) );
                if ( time != 0u )
                {
                    StartItemCheckTimer( player, item_data );
                }
            }
        }
    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFItemModule::OnLeaveItemModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    void KFItemModule::StartItemCheckTimer( EntityPtr player, DataPtr item_data )
    {
        auto interval_time = 1000u;
        auto now_time = KFGlobal::Instance()->_real_time;
        auto item_time = item_data->Get<uint64>( __STRING__( time ) );
        if ( item_time > now_time )
        {
            interval_time = ( item_time - now_time + 1 ) * 1000;
        }

        __COUNT_TIMER_2__( player->GetKeyID(), item_data->GetKeyID(), interval_time, 1, &KFItemModule::OnTimerRemoveTimeItem );
    }

    __KF_TIMER_FUNCTION__( KFItemModule::OnTimerRemoveTimeItem )
    {
        auto player = _kf_player->FindPlayer( object_id );
        if ( player == nullptr )
        {
            return;
        }

        auto tuple_data = FindItem( player, data_id );
        auto item_record = std::get<0>( tuple_data );
        auto item_data = std::get<1>( tuple_data );
        if ( item_record != nullptr && item_data != nullptr )
        {
            player->RemoveRecord( item_record, data_id );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::tuple<DataPtr, DataPtr> KFItemModule::FindItem( EntityPtr player, uint64 item_uuid )
    {
        for ( auto& iter : KFItemBagConfig::Instance()->_setting_list._objects )
        {
            auto item_record = player->Find( iter.first );
            auto item_data = item_record->Find( item_uuid );
            if ( item_data != nullptr )
            {
                return std::make_tuple( item_record, item_data );
            }
        }

        return std::make_tuple( nullptr, nullptr );
    }

    void KFItemModule::AddItemResult( KFElementResult* element_result, uint32 item_id, DataPtr item_data )
    {
        element_result->AddResult( item_id, item_data );

        // 判断是否需要显示
        auto setting = KFItemBagConfig::Instance()->FindSetting( item_data->_data_setting->_name );
        if ( setting != nullptr && !setting->_show_when_add )
        {
            element_result->_is_need_show = false;
        }
    }

    void KFItemModule::AddItemResult( KFElementResult* element_result, const std::string& item_name, uint32 item_id, uint32 count )
    {
        element_result->AddResult( item_id, item_name, __STRING__( count ), count );

        // 判断是否需要显示
        auto setting = KFItemBagConfig::Instance()->FindSetting( item_name );
        if ( setting != nullptr && !setting->_show_when_add )
        {
            element_result->_is_need_show = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleSellItemReq, KFMsg::MsgSellItemReq )
    {
        auto item_record = entity->Find( kfmsg->name() );
        auto item_data = item_record->Find( kfmsg->uuid() );
        if ( item_data == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::ItemDataNotExist );
        }

        auto item_id = item_data->Get<uint32>( __STRING__( id ) );
        auto setting = KFItemConfig::Instance()->FindSetting( item_id );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::ItemSettingNotExist );
        }

        if ( setting->_sell.IsEmpty() )
        {
            return _kf_display->SendToClient( entity, KFMsg::ItemCanNotSell );
        }

        auto count = item_data->Get<uint32>( __STRING__( count ) );
        if ( kfmsg->count() == 0u || count < kfmsg->count() )
        {
            return _kf_display->SendToClient( entity, KFMsg::ItemSellCountError );
        }

        // 更新道具数量
        entity->UpdateObject( item_data, __STRING__( count ), KFEnum::Dec, kfmsg->count() );

        // 添加道具
        entity->AddElement( &setting->_sell, kfmsg->count(), __STRING__( sell ), item_id, __FUNC_LINE__ );

        KFMsg::MsgSellItemAck ack;
        ack.set_itemid( item_id );
        ack.set_count( kfmsg->count() );
        _kf_player->SendToClient( entity, KFMsg::MSG_SELL_ITEM_ACK, &ack );
    }
}