#include "KFItemMoveModule.hpp"

namespace KFrame
{
    void KFItemMoveModule::BeforeRun()
    {
        __REGISTER_ADD_LOGIC__( __STRING__( item ), &KFItemMoveModule::OnAddItemMoveLogic );
        __REGISTER_REMOVE_LOGIC__( __STRING__( item ), &KFItemMoveModule::OnRemoveItemMoveLogic );

        __REGISTER_PLAYER_AFTER_ENTER__( &KFItemMoveModule::OnEnterItemMoveModule );
        __REGISTER_PLAYER_LEAVE__( &KFItemMoveModule::OnLeaveItemMoveModule );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_MOVE_ITEM_REQ, &KFItemMoveModule::HandleMoveItemReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_MOVE_ALL_ITEM_REQ, &KFItemMoveModule::HandleMoveAllItemReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_SORT_ITEM_REQ, &KFItemMoveModule::HandleSortItemReq );
    }

    void KFItemMoveModule::AfterLoad()
    {
        _component = _kf_kernel->FindComponent( __STRING__( player ) );
        if ( KFItemBagConfig::Instance()->_load_ok )
        {
            for ( auto& iter : KFItemBagConfig::Instance()->_setting_list._objects )
            {
                auto setting = iter.second;
                for ( auto& tab_name : setting->_tab_list )
                {
                    __REGISTER_UPDATE_DATA_2__( __STRING__( item ), tab_name, &KFItemMoveModule::OnUpdateItemTabIndexCallBack );
                }
            }
        }
    }

    void KFItemMoveModule::BeforeShut()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_ADD_LOGIC__( __STRING__( item ) );
        __UN_REMOVE_LOGIC__( __STRING__( item ) );
        for ( auto& iter : KFItemBagConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            for ( auto& tab_name : setting->_tab_list )
            {
                __UN_UPDATE_DATA_2__( __STRING__( item ), tab_name );
            }
        }

        __UN_PLAYER_AFTER_ENTER__();
        __UN_PLAYER_LEAVE__();
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_MOVE_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_MOVE_ALL_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SORT_ITEM_REQ );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_DATA_FUNCTION__( KFItemMoveModule::OnAddItemMoveLogic )
    {
        // 添加道具, 设置格子索引
        ItemIndexKey index_key( player->GetKeyID(), parent_data->_data_setting->_name );
        auto bag_index = _player_item_index.Find( index_key );
        if ( bag_index == nullptr )
        {
            return;
        }

        auto item_setting = KFItemConfig::Instance()->FindSetting( data->Get<uint32>( __STRING__( id ) ) );
        if ( item_setting == nullptr )
        {
            return;
        }

        auto item_type_setting = KFItemTypeConfig::Instance()->FindSetting( item_setting->_type );
        if ( item_type_setting == nullptr )
        {
            return;
        }

        for ( auto& tab_name : item_type_setting->_tab_list )
        {
            auto tab_index = bag_index->FindTab( tab_name );
            if ( tab_index == nullptr )
            {
                continue;
            }

            auto index = data->Get<uint32>( tab_name );
            index = tab_index->AddItemIndex( key, index, bag_index->_max_count );
            if ( index != 0u )
            {
                data->Operate( tab_name, KFEnum::Set, index );
            }
        }
    }

    __KF_REMOVE_DATA_FUNCTION__( KFItemMoveModule::OnRemoveItemMoveLogic )
    {
        if ( key == 0u )
        {
            return;
        }

        ItemIndexKey index_key( player->GetKeyID(), parent_data->_data_setting->_name );
        auto bag_index = _player_item_index.Find( index_key );
        if ( bag_index == nullptr )
        {
            return;
        }

        auto item_setting = KFItemConfig::Instance()->FindSetting( data->Get<uint32>( __STRING__( id ) ) );
        if ( item_setting == nullptr )
        {
            return;
        }

        auto item_type_setting = KFItemTypeConfig::Instance()->FindSetting( item_setting->_type );
        if ( item_type_setting == nullptr )
        {
            return;
        }

        for ( auto& tab_name : item_type_setting->_tab_list )
        {
            auto tab_index = bag_index->FindTab( tab_name );
            if ( tab_index == nullptr )
            {
                continue;
            }

            auto index = data->Get<uint32>( tab_name );
            tab_index->RemoveItemIndex( key, index );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFItemMoveModule::OnUpdateItemTabIndexCallBack )
    {
        ItemIndexKey index_key( player->GetKeyID(), data->GetParent()->_data_setting->_name );
        auto bag_index = _player_item_index.Find( index_key );
        if ( bag_index == nullptr )
        {
            return;
        }

        auto tab_index = bag_index->FindTab( data->_data_setting->_name );
        if ( tab_index == nullptr )
        {
            return;
        }

        tab_index->UpdateItemIndex( key, old_value, new_value );
    }

    __KF_PLAYER_ENTER_FUNCTION__( KFItemMoveModule::OnEnterItemMoveModule )
    {
        for ( auto& iter : KFItemBagConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            if ( setting->_tab_list.empty() )
            {
                continue;
            }

            auto item_record = player->Find( iter.first );
            if ( item_record == nullptr )
            {
                continue;
            }

            InitItemEmptyIndexData( player, item_record, setting );
        }
    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFItemMoveModule::OnLeaveItemMoveModule )
    {
        for ( auto& iter : KFItemBagConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            if ( setting->_tab_list.empty() )
            {
                continue;
            }

            UnInitItemEmptyIndexData( player, iter.first );
        }
    }

    void KFItemMoveModule::InitItemEmptyIndexData( EntityPtr player, DataPtr item_record, std::shared_ptr<const KFItemBagSetting> item_bag_setting )
    {
        auto max_item_count = _kf_item->GetItemBagMaxCount( player, item_record );

        // 初始化最大数量
        ItemIndexKey key( player->GetKeyID(), item_record->_data_setting->_name );
        auto bag_index = _player_item_index.Create( key );
        bag_index->InitMaxIndex( max_item_count, item_bag_setting->_tab_list );

        std::list<std::tuple<DataPtr, std::shared_ptr<KFItemTabIndex>>> invalid_list;
        for ( auto item_data = item_record->First(); item_data != nullptr; item_data = item_record->Next() )
        {
            auto item_setting = KFItemConfig::Instance()->FindSetting( item_data->Get<uint32>( __STRING__( id ) ) );
            if ( item_setting == nullptr )
            {
                continue;
            }

            auto item_type_setting = KFItemTypeConfig::Instance()->FindSetting( item_setting->_type );
            if ( item_type_setting == nullptr )
            {
                continue;
            }

            for ( auto& tab_name : item_type_setting->_tab_list )
            {
                auto tab_index = bag_index->FindTab( tab_name );
                if ( tab_index == nullptr )
                {
                    continue;
                }

                auto index = item_data->Get<uint32>( tab_index->_name );
                if ( index != 0u )
                {
                    tab_index->RemoveEmpty( index, item_data->GetKeyID() );
                }
                else
                {
                    invalid_list.push_back( std::make_tuple( item_data, tab_index ) );
                }
            }
        }

        // 如果存在没有索引的情况, 纠正数据
        for ( auto& tuple_data : invalid_list )
        {
            auto item_data = std::get<0>( tuple_data );
            auto tab_index = std::get<1>( tuple_data );

            auto index = tab_index->FindEmpty( item_data->GetKeyID() );
            if ( index != 0u )
            {
                item_data->Operate( tab_index->_name, KFEnum::Set, index );
            }
        }
    }

    void KFItemMoveModule::UnInitItemEmptyIndexData( EntityPtr player, const std::string& name )
    {
        ItemIndexKey key( player->GetKeyID(), name );
        _player_item_index.Remove( key );
    }

    uint32 KFItemMoveModule::GetItemMaxIndex( EntityPtr player, DataPtr item_record )
    {
        ItemIndexKey key( player->GetKeyID(), item_record->_data_setting->_name );
        auto bag_index = _player_item_index.Find( key );
        if ( bag_index == nullptr )
        {
            return 0u;
        }

        return bag_index->GetMaxIndex();
    }

    void KFItemMoveModule::AddItemMaxIndex( EntityPtr player, DataPtr item_record, uint32 count )
    {
        ItemIndexKey key( player->GetKeyID(), item_record->_data_setting->_name );
        auto bag_index = _player_item_index.Find( key );
        if ( bag_index == nullptr )
        {
            return;
        }

        bag_index->AddMaxIndex( count );
    }

    void KFItemMoveModule::AddItemEmptyIndex( EntityPtr player, DataPtr item_record, DataPtr item_data )
    {
        auto item_setting = KFItemConfig::Instance()->FindSetting( item_data->Get<uint32>( __STRING__( id ) ) );
        if ( item_setting == nullptr )
        {
            return;
        }

        auto item_type_setting = KFItemTypeConfig::Instance()->FindSetting( item_setting->_type );
        if ( item_type_setting == nullptr )
        {
            return;
        }

        ItemIndexKey key( player->GetKeyID(), item_record->_data_setting->_name );
        auto bag_index = _player_item_index.Find( key );
        if ( bag_index == nullptr )
        {
            return;
        }

        for ( auto& tab_name : item_type_setting->_tab_list )
        {
            auto tab_index = bag_index->FindTab( tab_name );
            if ( tab_index == nullptr )
            {
                continue;
            }

            auto index = item_data->Get<uint32>( tab_name );
            tab_index->AddEmpty( index );
        }
    }

    DataPtr KFItemMoveModule::FindIndexItem( EntityPtr player, DataPtr item_record, uint32 index )
    {
        auto item_bag_setting = KFItemBagConfig::Instance()->FindSetting( item_record->_data_setting->_name );
        if ( item_bag_setting == nullptr || item_bag_setting->_tab_list.empty() )
        {
            return nullptr;
        }

        // 指定索引
        ItemIndexKey key( player->GetKeyID(), item_record->_data_setting->_name );
        auto bag_index = _player_item_index.Find( key );
        if ( bag_index == nullptr )
        {
            return nullptr;
        }

        auto tab_index = bag_index->FindTab( item_bag_setting->_index_name );
        if ( tab_index == nullptr )
        {
            return nullptr;
        }

        auto uuid = tab_index->GetUUID( index );
        return item_record->Find( uuid );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsNeedUpdateAddFromSource( std::shared_ptr<const KFItemBagSetting> item_bag_setting, const std::string& source_bag_name )
    {
        return item_bag_setting->_update_add_from_source.find( source_bag_name ) != item_bag_setting->_update_add_from_source.end();
    }

    bool IsNeedUpdateMoveToTarget( std::shared_ptr<const KFItemBagSetting> item_bag_setting, const std::string& target_bag_name )
    {
        return item_bag_setting->_update_move_to_target.find( target_bag_name ) != item_bag_setting->_update_move_to_target.end();
    }

    bool IsNeedShowMoveToTarget( std::shared_ptr<const KFItemBagSetting> item_bag_setting, const std::string& target_bag_name )
    {
        return item_bag_setting->_show_move_to_target.find( target_bag_name ) != item_bag_setting->_show_move_to_target.end();
    }

    uint32 GetOverlayCount( std::shared_ptr<const KFItemSetting> setting, const std::string& bag_name )
    {
        return setting->_overlay_count;
    }

    bool IsOverlayItem( std::shared_ptr<const KFItemSetting> setting )
    {
        return setting->_overlay_count > 1u;
    }

    bool IsBagHaveTab( std::shared_ptr<const KFItemBagSetting> item_bag_setting, const std::string& tab_name )
    {
        return item_bag_setting->_tab_list.find( tab_name ) != item_bag_setting->_tab_list.end();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFItemMoveModule::SplitItemLogic( EntityPtr player, std::shared_ptr<const KFItemSetting> item_setting,
            std::shared_ptr<const KFItemBagSetting> source_bag_setting, DataPtr source_record, DataPtr source_item, uint32 split_count,
            std::shared_ptr<const KFItemBagSetting> target_bag_setting, DataPtr target_record, uint32 split_index )
    {
        // 扣除源物品数量
        MoveItemCountLogic( player, item_setting, source_bag_setting, source_item, split_count, target_bag_setting, nullptr );

        // 添加新的道具
        auto target_item = player->CreateData( source_item );
        target_item->CopyFrom( source_item );
        auto uuid = KFGlobal::Instance()->STMakeUuid( __STRING__( item ) );
        target_item->SetKeyID( uuid );
        target_item->Operate( __STRING__( count ), KFEnum::Set, split_count );
        target_item->Operate( target_bag_setting->_index_name, KFEnum::Set, split_index );

        auto is_update = IsNeedUpdateAddFromSource( target_bag_setting, source_bag_setting->_id );
        player->AddRecord( target_record, uuid, target_item, is_update );

        // 添加客户端显示
        if ( IsNeedShowMoveToTarget( source_bag_setting, target_bag_setting->_id ) )
        {
            MoveItemDataToShow( player, item_setting, source_record, target_record, split_count );
        }

        return KFMsg::Ok;
    }

    uint32 KFItemMoveModule::MoveItemLogic( EntityPtr player, std::shared_ptr<const KFItemSetting> item_setting,
                                            std::shared_ptr<const KFItemBagSetting> source_bag_setting, DataPtr source_record, DataPtr source_item,
                                            std::shared_ptr<const KFItemBagSetting> target_bag_setting, DataPtr target_record, uint32 target_index )
    {
        // 如果背包相同
        if ( source_record == target_record )
        {
            if ( target_index == 0u || target_index == source_item->Get<uint32>( source_bag_setting->_index_name ) )
            {
                return KFMsg::ItemIndexError;
            }

            // 源背包索引
            player->UpdateObject( source_item, source_bag_setting->_index_name, KFEnum::Set, target_index );
            return KFMsg::Ok;
        }

        // 不同背包, 先移除源背包
        {
            auto is_update = IsNeedUpdateMoveToTarget( source_bag_setting, target_bag_setting->_id );
            player->MoveRecord( source_record, source_item->GetKeyID(), is_update );
        }

        // 添加目标背包
        source_item->Operate( source_bag_setting->_index_name, KFEnum::Set, target_index );
        auto is_update = IsNeedUpdateAddFromSource( target_bag_setting, source_bag_setting->_id );
        player->AddRecord( target_record, source_item->GetKeyID(), source_item, is_update );

        // 添加客户端显示
        if ( IsNeedShowMoveToTarget( source_bag_setting, target_bag_setting->_id ) )
        {
            MoveItemDataToShow( player, item_setting, source_record, target_record, source_item );
        }

        return KFMsg::Ok;
    }

    uint32 KFItemMoveModule::ExchangeItemLogic( EntityPtr player,
            std::shared_ptr<const KFItemSetting> source_item_setting, std::shared_ptr<const KFItemBagSetting> source_bag_setting, DataPtr source_record, DataPtr source_item,
            std::shared_ptr<const KFItemSetting> target_item_setting, std::shared_ptr<const KFItemBagSetting> target_bag_setting, DataPtr target_record, DataPtr target_item )
    {
        if ( source_item == target_item )
        {
            return KFMsg::ItemIndexError;
        }

        auto source_index = source_item->Get<uint32>( source_bag_setting->_index_name );
        auto target_index = target_item->Get<uint32>( target_bag_setting->_index_name );
        if ( source_record == target_record )
        {
            // 背包相同, 直接更新索引
            player->UpdateObject( source_item, source_bag_setting->_index_name, KFEnum::Set, target_index );
            player->UpdateObject( target_item, target_bag_setting->_index_name, KFEnum::Set, source_index );
            return KFMsg::Ok;
        }

        // 判断是否能移动
        if ( !CheckItemCanMove( source_item_setting, source_item->_data_setting->_name, target_item->_data_setting->_name ) )
        {
            return KFMsg::ItemCanNotStore;
        }

        // 找到可以移动到的背包
        auto find_item_record = FindCanMoveItemBag( player, target_item_setting, source_item->_data_setting->_name, target_record->_data_setting->_name );
        if ( find_item_record == nullptr )
        {
            return KFMsg::ItemCanNotStore;
        }

        auto find_bag_setting = KFItemBagConfig::Instance()->FindSetting( find_item_record->_data_setting->_name );
        if ( find_bag_setting == nullptr )
        {
            return KFMsg::ItemCanNotStore;
        }

        // 判断源道具数量
        auto source_item_count = source_item->Get<uint32>( __STRING__( count ) );
        auto max_source_overlay_count = GetOverlayCount( source_item_setting, target_bag_setting->_id );

        // 判断目标道具数量
        auto target_item_count = target_item->Get<uint32>( __STRING__( count ) );
        auto max_target_overlay_count = GetOverlayCount( target_item_setting, source_bag_setting->_id );

        if ( source_item_count > max_source_overlay_count && target_item_count > max_target_overlay_count )
        {
            // 需要拆分, 所以需要判断格子数
            if ( _kf_item->CheckItemBagFull( player, find_item_record ) )
            {
                return KFMsg::ItemBagIsFull;
            }

            // 需要拆分, 所以需要判断格子数
            if ( _kf_item->CheckItemBagFull( player, target_record ) )
            {
                return KFMsg::ItemBagIsFull;
            }

            // 源道具拆分
            SplitItemLogic( player, source_item_setting, source_bag_setting, source_record, source_item, max_source_overlay_count, target_bag_setting, target_record, 0u );

            // 目标道具拆分
            SplitItemLogic( player, target_item_setting, target_bag_setting, target_record, target_record, max_target_overlay_count, find_bag_setting, find_item_record, 0u );
        }
        else if ( source_item_count > max_source_overlay_count )
        {
            // 需要拆分, 所以需要判断格子数
            if ( _kf_item->CheckItemBagFull( player, find_item_record ) )
            {
                return KFMsg::ItemBagIsFull;
            }

            // 移动目标道具
            MoveItemLogic( player, target_item_setting, target_bag_setting, target_record, target_item, find_bag_setting, find_item_record, 0u );

            // 拆分源道具
            SplitItemLogic( player, source_item_setting, source_bag_setting, source_record, source_item, max_source_overlay_count, target_bag_setting, target_record, target_index );
        }
        else if ( target_item_count > max_target_overlay_count )
        {
            // 需要拆分, 所以需要判断格子数
            if ( _kf_item->CheckItemBagFull( player, target_record ) )
            {
                return KFMsg::ItemBagIsFull;
            }

            // 移动源道具
            MoveItemLogic( player, source_item_setting, source_bag_setting, source_record, source_item, target_bag_setting, target_record, 0u );

            // 拆分目标道具
            SplitItemLogic( player, target_item_setting, target_bag_setting, target_record, target_item, max_target_overlay_count, find_bag_setting, find_item_record, source_index );
        }
        else
        {
            if ( find_item_record != source_record )
            {
                if ( _kf_item->CheckItemBagFull( player, find_item_record ) )
                {
                    return KFMsg::ItemBagIsFull;
                }

                source_index = 0u;
            }
            else
            {
                AddItemEmptyIndex( player, source_record, source_item );
                AddItemEmptyIndex( player, target_record, target_item );
            }

            MoveItemLogic( player, target_item_setting, target_bag_setting, target_record, target_item, find_bag_setting, find_item_record, source_index );
            MoveItemLogic( player, source_item_setting, source_bag_setting, source_record, source_item, target_bag_setting, target_record, target_index );
        }

        return KFMsg::Ok;
    }

    uint32 KFItemMoveModule::MergeItemLogic( EntityPtr player, std::shared_ptr<const KFItemSetting> item_setting,
            std::shared_ptr<const KFItemBagSetting> source_bag_setting, DataPtr source_item, uint32 merge_count,
            std::shared_ptr<const KFItemBagSetting> target_bag_setting, DataPtr target_item )
    {
        if ( source_item == target_item )
        {
            return KFMsg::ItemIndexError;
        }

        auto source_item_count = source_item->Get<uint32>( __STRING__( count ) );
        if ( source_item_count < merge_count )
        {
            merge_count = source_item_count;
        }

        // 已经达到最大
        auto target_item_count = target_item->Get<uint32>( __STRING__( count ) );
        auto max_target_overlay_count = GetOverlayCount( item_setting, target_bag_setting->_id );
        auto can_add_count = CalcItemAddCount( merge_count, target_item_count, max_target_overlay_count );
        if ( can_add_count == 0u )
        {
            return KFMsg::ItemOverlayCountMax;
        }

        // 移动数量
        MoveItemCountLogic( player, item_setting, source_bag_setting, source_item, can_add_count, target_bag_setting, target_item );
        return KFMsg::Ok;
    }

    uint32 KFItemMoveModule::CalcItemAddCount( uint32 source_count, uint32 target_count, uint32 max_count )
    {
        if ( target_count >= max_count )
        {
            return 0u;
        }

        auto can_add_count = max_count - target_count;
        return source_count <= can_add_count ? source_count : can_add_count;
    }

    void KFItemMoveModule::MoveItemCountLogic( EntityPtr player, std::shared_ptr<const KFItemSetting> item_setting,
            std::shared_ptr<const KFItemBagSetting> source_bag_setting, DataPtr source_item, uint32 move_count,
            std::shared_ptr<const KFItemBagSetting> target_bag_setting, DataPtr target_item )
    {
        // 源背包
        auto is_move_update = IsNeedUpdateMoveToTarget( source_bag_setting, target_bag_setting->_id );
        auto source_count = source_item->Get<uint32>( __STRING__( count ) );
        if ( source_count <= move_count )
        {
            player->RemoveRecord( source_item->GetParent()->GetDataPtr(), source_item->GetKeyID(), is_move_update );
        }
        else
        {
            player->UpdateObject( source_item, __STRING__( count ), KFEnum::Dec, move_count, is_move_update );
        }

        // 目标背包
        if ( target_item == nullptr )
        {
            return;
        }

        // 是否移动更新
        auto is_add_update = IsNeedUpdateMoveToTarget( target_bag_setting, source_bag_setting->_id );
        player->UpdateObject( target_item, __STRING__( count ), KFEnum::Add, move_count, is_add_update );

        // 添加显示
        if ( IsNeedShowMoveToTarget( source_bag_setting, target_bag_setting->_id ) )
        {
            MoveItemDataToShow( player, item_setting, source_item->GetParent()->GetDataPtr(), target_item->GetParent()->GetDataPtr(), move_count );
        }
    }

    bool KFItemMoveModule::CheckItemCanMove( std::shared_ptr<const KFItemSetting> setting, const std::string& source_bag_name, const std::string& target_bag_name )
    {
        if ( source_bag_name == target_bag_name )
        {
            return true;
        }

        auto item_type_setting = KFItemTypeConfig::Instance()->FindSetting( setting->_type );
        if ( item_type_setting == nullptr )
        {
            return false;
        }

        return item_type_setting->_move_bag_list.find( target_bag_name ) != item_type_setting->_move_bag_list.end();
    }

    DataPtr KFItemMoveModule::FindCanMoveItemBag( EntityPtr player, std::shared_ptr<const KFItemSetting> setting, const std::string& source_bag_name, const std::string& exclude_bag_name )
    {
        auto item_type_setting = KFItemTypeConfig::Instance()->FindSetting( setting->_type );
        if ( item_type_setting == nullptr )
        {
            return nullptr;
        }

        if ( item_type_setting->_move_bag_list.find( source_bag_name ) != item_type_setting->_move_bag_list.end() )
        {
            return player->Find( source_bag_name );
        }

        // 否则遍历可以移动到的包裹
        for ( auto& bag_name : item_type_setting->_move_bag_list )
        {
            if ( bag_name == exclude_bag_name )
            {
                continue;
            }

            return player->Find( bag_name );
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFItemMoveModule::MoveItem( EntityPtr player, DataPtr source_record, DataPtr source_item, DataPtr target_record, uint32 target_index )
    {
        // 判断是否限制
        auto source_item_setting = KFItemConfig::Instance()->FindSetting( source_item->Get<uint32>( __STRING__( id ) ) );
        if ( source_item_setting == nullptr )
        {
            return KFMsg::ItemSettingNotExist;
        }

        // 判断能否移动
        if ( !CheckItemCanMove( source_item_setting, source_record->_data_setting->_name, target_record->_data_setting->_name ) )
        {
            return KFMsg::ItemCanNotStore;
        }

        auto source_bag_setting = KFItemBagConfig::Instance()->FindSetting( source_record->_data_setting->_name );
        auto target_bag_setting = KFItemBagConfig::Instance()->FindSetting( target_record->_data_setting->_name );
        if ( source_bag_setting == nullptr || target_bag_setting == nullptr )
        {
            return KFMsg::ItemCanNotStore;
        }

        uint32 result = KFMsg::Error;
        auto source_item_count = source_item->Get<uint32>( __STRING__( count ) );
        auto target_item = FindIndexItem( player, target_record, target_index );
        if ( target_item == nullptr )
        {
            if ( !_kf_item->CheckItemBagFull( player, target_record ) )
            {
                // 移动到空格子上
                // 判断堆叠数量, 如果目标小于源堆叠, 并且源数量大于目标堆叠数量, 拆分物品
                auto max_target_overlay_count = GetOverlayCount( source_item_setting, target_bag_setting->_id );
                if ( source_item_count > max_target_overlay_count )
                {
                    // 拆分物品
                    result = SplitItemLogic( player, source_item_setting, source_bag_setting, source_record, source_item, max_target_overlay_count, target_bag_setting, target_record, target_index );
                }
                else
                {
                    // 移动物品
                    result = MoveItemLogic( player, source_item_setting, source_bag_setting, source_record, source_item, target_bag_setting, target_record, target_index );
                }
            }
            else
            {
                result = MoveItemDataLogic( player, source_item_setting, source_bag_setting, source_record, source_item, target_bag_setting, target_record );
            }
        }
        else
        {
            auto target_item_id = target_item->Get<uint32>( target_item->_data_setting->_config_key_name );
            auto target_item_setting = KFItemConfig::Instance()->FindSetting( target_item_id );
            if ( target_item_setting != nullptr )
            {
                // 判断道具是否能堆叠
                if ( CheckItemCanMerge( source_item_setting, source_item, target_item_setting, target_item ) )
                {
                    // 移动源物品的一定数量到目标物品上
                    result = MergeItemLogic( player, source_item_setting, source_bag_setting, source_item, source_item_count, target_bag_setting, target_item );
                }
                else
                {
                    // 直接交换
                    result = ExchangeItemLogic( player, source_item_setting, source_bag_setting, source_record, source_item, target_item_setting, target_bag_setting, target_record, target_item );
                }
            }
            else
            {
                result = KFMsg::ItemSettingNotExist;
            }
        }

        return result;
    }

    bool KFItemMoveModule::CheckItemCanMerge( std::shared_ptr<const KFItemSetting> source_item_setting, DataPtr source_item, std::shared_ptr<const KFItemSetting> target_item_setting, DataPtr target_item )
    {
        // 不同道具, 不能堆叠
        if ( source_item_setting->_id != target_item_setting->_id || !IsOverlayItem( source_item_setting ) )
        {
            return false;
        }

        // 判断目标数量
        auto target_item_count = target_item->Get<uint32>( __STRING__( count ) );
        auto max_target_overlay_count = GetOverlayCount( source_item_setting, target_item->_data_setting->_name );
        if ( target_item_count < max_target_overlay_count )
        {
            return true;
        }

        // 目标满了, 如果源数据小于堆叠数量, 需要可以交换
        auto source_item_count = source_item->Get<uint32>( __STRING__( count ) );
        if ( source_item_count > max_target_overlay_count )
        {
            return true;
        }

        return false;
    }

    __KF_MESSAGE_FUNCTION__( KFItemMoveModule::HandleMoveItemReq, KFMsg::MsgMoveItemReq )
    {
        uint32 result = KFMsg::Error;
        if ( kfmsg->sourcename() == kfmsg->targetname() )
        {
            result = MoveTabItem( player, kfmsg->sourcename(), kfmsg->tabname(), kfmsg->sourceuuid(), kfmsg->targetindex() );
        }
        else
        {
            result = MoveBagItem( player, kfmsg->sourcename(), kfmsg->sourceuuid(), kfmsg->targetname(), kfmsg->targetindex() );
        }
        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, result );
        }
    }

    uint32 KFItemMoveModule::MoveBagItem( EntityPtr player, const std::string& source_bag_name, uint64 item_uuid, const std::string& target_bag_name, uint32 target_index )
    {
        auto source_bag_setting = KFItemBagConfig::Instance()->FindSetting( source_bag_name );
        if ( source_bag_setting != nullptr && !source_bag_setting->_can_move )
        {
            return KFMsg::ItemBagCanNotMove;
        }

        auto target_bag_setting = KFItemBagConfig::Instance()->FindSetting( target_bag_name );
        if ( target_bag_setting != nullptr && !target_bag_setting->_can_move )
        {
            return KFMsg::ItemBagCanNotMove;
        }

        // 判断背包属性
        auto source_record = player->Find( source_bag_name );
        auto target_record = player->Find( target_bag_name );
        if ( source_record == nullptr || target_record == nullptr )
        {
            return KFMsg::ItemBagNameError;
        }

        auto max_index = GetItemMaxIndex( player, target_record );
        if ( target_index > max_index )
        {
            return KFMsg::ItemIndexError;
        }

        auto source_item = source_record->Find( item_uuid );
        if ( source_item == nullptr )
        {
            return KFMsg::ItemDataNotExist;
        }

        auto result = MoveItem( player, source_record, source_item, target_record, target_index );
        if ( result != KFMsg::Ok )
        {
            player->UpdateObject( source_item, source_bag_setting->_index_name, KFEnum::Set, source_item->Get<uint32>( source_bag_setting->_index_name ) );
        }

        return result;
    }

    uint32 KFItemMoveModule::MoveTabItem( EntityPtr player, const std::string& bag_name, const std::string& tab_name, uint64 item_uuid, uint32 target_index )
    {
        auto item_record = player->Find( bag_name );
        if ( item_record == nullptr )
        {
            return KFMsg::ItemBagNameError;
        }

        auto source_item = item_record->Find( item_uuid );
        if ( source_item == nullptr )
        {
            return KFMsg::ItemDataNotExist;
        }

        ItemIndexKey key( player->GetKeyID(), bag_name );
        auto bag_index = _player_item_index.Create( key );
        auto tab_index = bag_index->FindTab( tab_name );
        if ( tab_index == nullptr )
        {
            return KFMsg::ItemTabNotExist;
        }

        auto last_index = source_item->Get<uint32>( tab_index->_name );
        if ( last_index == target_index || target_index == 0u || target_index > bag_index->GetMaxIndex() )
        {
            return KFMsg::ItemIndexError;
        }

        // 判断目标是否道具
        auto target_uuid = tab_index->GetUUID( target_index );
        auto target_item = item_record->Find( target_uuid );
        if ( target_item == nullptr )
        {
            player->UpdateObject( source_item, tab_name, KFEnum::Set, target_index );
            return KFMsg::Ok;
        }

        auto source_item_id = source_item->Get<uint32>( source_item->_data_setting->_config_key_name );
        auto target_item_id = target_item->Get<uint32>( target_item->_data_setting->_config_key_name );
        auto source_item_setting = KFItemConfig::Instance()->FindSetting( source_item_id );
        auto target_item_setting = KFItemConfig::Instance()->FindSetting( target_item_id );
        if ( source_item_setting == nullptr || target_item_setting == nullptr )
        {
            return KFMsg::ItemSettingNotExist;
        }

        // 判断道具是否能堆叠
        if ( CheckItemCanMerge( source_item_setting, source_item, target_item_setting, target_item ) )
        {
            auto item_bag_setting = KFItemBagConfig::Instance()->FindSetting( bag_name );
            if ( item_bag_setting == nullptr )
            {
                return KFMsg::ItemBagCanNotMove;
            }

            // 移动源物品的一定数量到目标物品上
            auto source_item_count = source_item->Get<uint32>( __STRING__( count ) );
            return MergeItemLogic( player, source_item_setting, item_bag_setting, source_item, source_item_count, item_bag_setting, target_item );
        }

        // 交换位置
        player->UpdateObject( source_item, tab_name, KFEnum::Set, target_index );
        player->UpdateObject( target_item, tab_name, KFEnum::Set, last_index );
        return KFMsg::Ok;
    }

    __KF_MESSAGE_FUNCTION__( KFItemMoveModule::HandleMoveAllItemReq, KFMsg::MsgMoveAllItemReq )
    {
        auto source_bag_setting = KFItemBagConfig::Instance()->FindSetting( kfmsg->sourcename() );
        auto target_bag_setting = KFItemBagConfig::Instance()->FindSetting( kfmsg->targetname() );
        if ( source_bag_setting == nullptr || target_bag_setting == nullptr || !source_bag_setting->_can_move_all )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagCanNotMove );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 全部拾取
        auto source_record = player->Find( kfmsg->sourcename() );
        auto target_record = player->Find( kfmsg->targetname() );
        if ( source_record == nullptr || target_record == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        // 遍历出可以移动的道具
        std::map<DataPtr, std::shared_ptr<const KFItemSetting>> move_list;
        for ( auto item_data = source_record->First(); item_data != nullptr; item_data = source_record->Next() )
        {
            auto item_id = item_data->Get<uint32>( item_data->_data_setting->_config_key_name );
            auto setting = KFItemConfig::Instance()->FindSetting( item_id );
            if ( setting == nullptr )
            {
                continue;
            }

            move_list[ item_data ] = setting;
        }

        for ( auto& iter : move_list )
        {
            auto result = MoveItemDataLogic( player, iter.second, source_bag_setting, source_record, iter.first, target_bag_setting, target_record );
            if ( result != KFMsg::Ok )
            {
                return _kf_display->SendToClient( player, result );
            }
        }
    }

    uint32 KFItemMoveModule::MoveItemDataLogic( EntityPtr player, std::shared_ptr<const KFItemSetting> item_setting,
            std::shared_ptr<const KFItemBagSetting> source_bag_setting, DataPtr source_record, DataPtr source_item,
            std::shared_ptr<const KFItemBagSetting> target_bag_setting, DataPtr target_record )
    {
        // 判断是否能堆叠
        if ( IsOverlayItem( item_setting ) )
        {
            auto source_count = source_item->Get<uint32>( __STRING__( count ) );
            auto max_overlay_count = GetOverlayCount( item_setting, target_record->_data_setting->_name );

            // 堆叠起来
            for ( auto target_item = target_record->First(); target_item != nullptr; target_item = target_record->Next() )
            {
                auto item_id = target_item->Get<uint32>( target_record->_data_setting->_config_key_name );
                if ( item_id != item_setting->_id )
                {
                    continue;
                }

                auto target_count = target_item->Get<uint32>( __STRING__( count ) );
                auto move_count = CalcItemAddCount( source_count, target_count, max_overlay_count );
                if ( move_count == 0u )
                {
                    continue;
                }

                // 移动道具数量
                MoveItemCountLogic( player, item_setting, source_bag_setting, source_item, move_count, target_bag_setting, target_item );
                source_count -= move_count;
                if ( source_count == 0u )
                {
                    return KFMsg::Ok;
                }
            }

            // 剩下的道具数量
            while ( source_count > max_overlay_count )
            {
                if ( _kf_item->CheckItemBagFull( player, target_record ) )
                {
                    break;
                }

                SplitItemLogic( player, item_setting, source_bag_setting, source_record, source_item, max_overlay_count, target_bag_setting, target_record, 0u );
                source_count -= max_overlay_count;
            }
        }

        // 不能堆叠或者剩下没有堆叠完的
        auto is_full = _kf_item->CheckItemBagFull( player, target_record );
        if ( !is_full )
        {
            MoveItemLogic( player, item_setting, source_bag_setting, source_record, source_item, target_bag_setting, target_record, 0u );
        }

        return is_full ? KFMsg::ItemBagIsFull : KFMsg::Ok;
    }

    __KF_MESSAGE_FUNCTION__( KFItemMoveModule::HandleSortItemReq, KFMsg::MsgSortItemReq )
    {
        SortItem( player, kfmsg->bagname(), kfmsg->tabname() );
    }

    void KFItemMoveModule::SortItem( EntityPtr player, const std::string& bag_name, const std::string& tab_name )
    {
        auto item_record = player->Find( bag_name );
        if ( item_record == nullptr )
        {
            return;
        }

        auto item_bag_setting = KFItemBagConfig::Instance()->FindSetting( bag_name );
        if ( item_bag_setting == nullptr || !IsBagHaveTab( item_bag_setting, tab_name ) )
        {
            return;
        }

        // 重置格子数量
        ItemIndexKey key( player->GetKeyID(), item_record->_data_setting->_name );
        auto bag_index = _player_item_index.Create( key );
        auto tab_index = bag_index->InitMaxIndex( tab_name );

        // sort从小到大, 品质从大到小, id从小到大
        std::map<uint32, std::map<uint32, std::map<std::shared_ptr<const KFItemSetting>, std::set<DataPtr>>>> sort_list;
        for ( auto item_data = item_record->First(); item_data != nullptr; item_data = item_record->Next() )
        {
            auto id = item_data->Get<uint32>( item_record->_data_setting->_config_key_name );
            auto setting = KFItemConfig::Instance()->FindSetting( id );
            if ( setting == nullptr )
            {
                continue;
            }

            auto item_type_setting = KFItemTypeConfig::Instance()->FindSetting( setting->_type );
            if ( item_type_setting == nullptr || item_type_setting->_tab_list.find( tab_name ) == item_type_setting->_tab_list.end() )
            {
                continue;
            }

            sort_list[ item_type_setting->_sort ][ setting->_quality ][ setting ].insert( item_data );
        }

        for ( auto& sort_iter : sort_list )
        {
            auto& quality_list = sort_iter.second;
            for ( auto item_iter = quality_list.rbegin(); item_iter != quality_list.rend(); ++item_iter )
            {
                auto& item_list = item_iter->second;
                for ( auto& iter : item_list )
                {
                    SortItem( player, iter.first, item_bag_setting, tab_index, item_record, iter.second );
                }
            }
        }
    }

    std::tuple<DataPtr, uint32> FindMaxCountItem( uint32 max_overlay_count, std::set<DataPtr>& item_list, DataPtr min_count_item )
    {
        auto max_count = 0u;
        DataPtr find_item = nullptr;

        for ( auto item_data : item_list )
        {
            auto count = item_data->Get<uint32>( __STRING__( count ) );
            if ( count < max_overlay_count && count > max_count && item_data != min_count_item )
            {
                find_item = item_data;
                max_count = count;
            }
        }

        return std::make_tuple( find_item, max_count );
    }

    std::tuple<DataPtr, uint32> FindMinCountItem( uint32 max_overlay_count, std::set<DataPtr>& item_list )
    {
        auto min_count = __MAX_UINT32__;
        DataPtr find_item = nullptr;

        for ( auto item_data : item_list )
        {
            auto count = item_data->Get<uint32>( __STRING__( count ) );
            if ( count < max_overlay_count && count < min_count )
            {
                find_item = item_data;
                min_count = count;
            }
        }

        return std::make_tuple( find_item, min_count );
    }

    DataPtr PopMaxCountItem( uint32 max_overlay_count, std::set< DataPtr >& item_list )
    {
        auto max_count = 0u;
        DataPtr find_item = nullptr;

        for ( auto item_data : item_list )
        {
            auto count = item_data->Get<uint32>( __STRING__( count ) );
            if ( count == 0u || count >= max_overlay_count )
            {
                find_item = item_data;
                break;
            }

            if ( count > max_count )
            {
                find_item = item_data;
                max_count = count;
            }
        }

        if ( find_item != nullptr )
        {
            item_list.erase( find_item );
        }

        return find_item;
    }

    void KFItemMoveModule::SortItem( EntityPtr player, std::shared_ptr<const KFItemSetting> item_setting, std::shared_ptr<const KFItemBagSetting> item_bag_setting,
                                     std::shared_ptr<KFItemTabIndex> tab_index, DataPtr item_record, std::set<DataPtr>& item_list )
    {
        auto max_overlay_count = GetOverlayCount( item_setting, item_record->_data_setting->_name );
        if ( IsOverlayItem( item_setting ) )
        {
            while ( true )
            {
                auto min_count = 0u;
                DataPtr min_count_item = nullptr;
                std::tie( min_count_item, min_count ) = FindMinCountItem( max_overlay_count, item_list );
                if ( min_count_item == nullptr )
                {
                    break;
                }

                auto max_count = 0u;
                DataPtr max_count_item = nullptr;
                std::tie( max_count_item, max_count ) = FindMaxCountItem( max_overlay_count, item_list, min_count_item );
                if ( max_count_item == nullptr )
                {
                    break;
                }

                auto can_add_count = max_overlay_count - max_count;
                can_add_count = __MIN__( min_count, can_add_count );
                MoveItemCountLogic( player, item_setting, item_bag_setting, min_count_item, can_add_count, item_bag_setting, max_count_item );
                if ( can_add_count == min_count )
                {
                    item_list.erase( min_count_item );
                }
            }
        }

        while ( true )
        {
            auto item_data = PopMaxCountItem( max_overlay_count, item_list );
            if ( item_data == nullptr )
            {
                break;
            }

            item_data->Operate( tab_index->_name, KFEnum::Set, 0u );
            auto index = tab_index->FindEmpty( item_data->GetKeyID() );
            player->UpdateObject( item_data, tab_index->_name, KFEnum::Set, index );
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFItemMoveModule::MoveItemDataToShow( EntityPtr player, std::shared_ptr<const KFItemSetting> setting, DataPtr source_record, DataPtr target_record, uint32 count )
    {
        StringUInt64 values;
        values[__STRING__( count )] = count;
        values[__STRING__( id )] = setting->_id;
        player->AddDataToShow( source_record->_data_setting->_name, 0u, __STRING__( item ), setting->_id, values, true );
    }

    void KFItemMoveModule::MoveItemDataToShow( EntityPtr player, std::shared_ptr<const KFItemSetting> setting, DataPtr source_record, DataPtr target_record, DataPtr item_data )
    {
        if ( IsOverlayItem( setting ) )
        {
            player->AddDataToShow( source_record->_data_setting->_name, 0u, item_data );
        }
        else
        {
            auto count = item_data->Get( __STRING__( count ) );
            MoveItemDataToShow( player, setting, source_record, target_record, count );
        }
    }
}