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
        __REGISTER_MESSAGE__( KFItemMoveModule, KFMessageEnum::Player, KFMsg::MSG_MOVE_ITEM_REQ, KFMsg::MsgMoveItemReq, HandleMoveItemReq );
        __REGISTER_MESSAGE__( KFItemMoveModule, KFMessageEnum::Player, KFMsg::MSG_MOVE_ALL_ITEM_REQ, KFMsg::MsgMoveAllItemReq, HandleMoveAllItemReq );
        __REGISTER_MESSAGE__( KFItemMoveModule, KFMessageEnum::Player, KFMsg::MSG_SORT_ITEM_REQ, KFMsg::MsgSortItemReq, HandleSortItemReq );
    }

    void KFItemMoveModule::AfterLoad()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        if ( KFItemBagConfig::Instance()->_load_ok )
        {
            for ( auto& iter : KFItemBagConfig::Instance()->_settings._objects )
            {
                auto kfsetting = iter.second;
                for ( auto& tabname : kfsetting->_tab_list )
                {
                    __REGISTER_UPDATE_DATA_2__( __STRING__( item ), tabname, &KFItemMoveModule::OnUpdateItemTabIndexCallBack );
                }
            }
        }
    }

    void KFItemMoveModule::BeforeShut()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_ADD_LOGIC__( __STRING__( item ) );
        __UN_REMOVE_LOGIC__( __STRING__( item ) );
        for ( auto& iter : KFItemBagConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            for ( auto& tabname : kfsetting->_tab_list )
            {
                __UN_UPDATE_DATA_2__( __STRING__( item ), tabname );
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
        ItemIndexKey indexkey( player->GetKeyID(), kfparent->_data_setting->_name );
        auto kfbagindex = _player_item_index.Find( indexkey );
        if ( kfbagindex == nullptr )
        {
            return;
        }

        auto kfitemsetting = KFItemConfig::Instance()->FindSetting( kfdata->Get<uint32>( __STRING__( id ) ) );
        if ( kfitemsetting == nullptr )
        {
            return;
        }

        auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfitemsetting->_type );
        if ( kftypesetting == nullptr )
        {
            return;
        }

        for ( auto& tabname : kftypesetting->_tab_list )
        {
            auto kftabindex = kfbagindex->FindTab( tabname );
            if ( kftabindex == nullptr )
            {
                continue;
            }

            auto index = kfdata->Get<uint32>( tabname );
            index = kftabindex->AddItemIndex( key, index, kfbagindex->_max_count );
            if ( index != 0u )
            {
                kfdata->Set( tabname, index );
            }
        }
    }

    __KF_REMOVE_DATA_FUNCTION__( KFItemMoveModule::OnRemoveItemMoveLogic )
    {
        if ( key == 0u )
        {
            return;
        }

        ItemIndexKey indexkey( player->GetKeyID(), kfparent->_data_setting->_name );
        auto kfbagindex = _player_item_index.Find( indexkey );
        if ( kfbagindex == nullptr )
        {
            return;
        }

        auto kfitemsetting = KFItemConfig::Instance()->FindSetting( kfdata->Get<uint32>( __STRING__( id ) ) );
        if ( kfitemsetting == nullptr )
        {
            return;
        }

        auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfitemsetting->_type );
        if ( kftypesetting == nullptr )
        {
            return;
        }

        for ( auto& tabname : kftypesetting->_tab_list )
        {
            auto kftabindex = kfbagindex->FindTab( tabname );
            if ( kftabindex == nullptr )
            {
                continue;
            }

            auto index = kfdata->Get<uint32>( tabname );
            kftabindex->RemoveItemIndex( key, index );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFItemMoveModule::OnUpdateItemTabIndexCallBack )
    {
        ItemIndexKey indexkey( player->GetKeyID(), kfdata->GetParent()->_data_setting->_name );
        auto kfbagindex = _player_item_index.Find( indexkey );
        if ( kfbagindex == nullptr )
        {
            return;
        }

        auto kftabindex = kfbagindex->FindTab( kfdata->_data_setting->_name );
        if ( kftabindex == nullptr )
        {
            return;
        }

        kftabindex->UpdateItemIndex( key, oldvalue, newvalue );
    }

    __KF_PLAYER_ENTER_FUNCTION__( KFItemMoveModule::OnEnterItemMoveModule )
    {
        for ( auto& iter : KFItemBagConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_tab_list.empty() )
            {
                continue;
            }

            auto kfitembag = player->Find( iter.first );
            if ( kfitembag == nullptr )
            {
                continue;
            }

            InitItemEmptyIndexData( player, kfitembag, kfsetting );
        }
    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFItemMoveModule::OnLeaveItemMoveModule )
    {
        for ( auto& iter : KFItemBagConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_tab_list.empty() )
            {
                continue;
            }

            UnInitItemEmptyIndexData( player, iter.first );
        }
    }

    void KFItemMoveModule::InitItemEmptyIndexData( KFEntity* player, KFData* kfitembag, const KFItemBagSetting* kfbagsetting )
    {
        auto maxitemcount = _kf_item->GetItemBagMaxCount( player, kfitembag );

        // 初始化最大数量
        ItemIndexKey key( player->GetKeyID(), kfitembag->_data_setting->_name );
        auto kfbagindex = _player_item_index.Create( key );
        kfbagindex->InitMaxIndex( maxitemcount, kfbagsetting->_tab_list );

        std::list< std::tuple< KFData*, KFItemTabIndex* > > invalidlist;
        for ( auto kfitem = kfitembag->First(); kfitem != nullptr; kfitem = kfitembag->Next() )
        {
            auto kfitemsetting = KFItemConfig::Instance()->FindSetting( kfitem->Get<uint32>( __STRING__( id ) ) );
            if ( kfitemsetting == nullptr )
            {
                continue;
            }

            auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfitemsetting->_type );
            if ( kftypesetting == nullptr )
            {
                continue;
            }

            for ( auto& tabname : kftypesetting->_tab_list )
            {
                auto kftabindex = kfbagindex->FindTab( tabname );
                if ( kftabindex == nullptr )
                {
                    continue;
                }

                auto index = kfitem->Get<uint32>( kftabindex->_name );
                if ( index != 0u )
                {
                    kftabindex->RemoveEmpty( index, kfitem->GetKeyID() );
                }
                else
                {
                    invalidlist.push_back( std::make_tuple( kfitem, kftabindex ) );
                }
            }
        }

        // 如果存在没有索引的情况, 纠正数据
        for ( auto& tupledata : invalidlist )
        {
            auto kfitem = std::get<0>( tupledata );
            auto kftabindex = std::get<1>( tupledata );

            auto index = kftabindex->FindEmpty( kfitem->GetKeyID() );
            if ( index != 0u )
            {
                kfitem->Set( kftabindex->_name, index );
            }
        }
    }

    void KFItemMoveModule::UnInitItemEmptyIndexData( KFEntity* player, const std::string& name )
    {
        ItemIndexKey key( player->GetKeyID(), name );
        _player_item_index.Remove( key );
    }

    uint32 KFItemMoveModule::GetItemMaxIndex( KFEntity* player, KFData* kfitemrecord )
    {
        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfbagindex = _player_item_index.Find( key );
        if ( kfbagindex == nullptr )
        {
            return 0u;
        }

        return kfbagindex->GetMaxIndex();
    }

    void KFItemMoveModule::AddItemMaxIndex( KFEntity* player, KFData* kfitemrecord, uint32 count )
    {
        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfbagindex = _player_item_index.Find( key );
        if ( kfbagindex == nullptr )
        {
            return;
        }

        kfbagindex->AddMaxIndex( count );
    }

    void KFItemMoveModule::AddItemEmptyIndex( KFEntity* player, KFData* kfitembag, KFData* kfitem )
    {
        auto kfitemsetting = KFItemConfig::Instance()->FindSetting( kfitem->Get<uint32>( __STRING__( id ) ) );
        if ( kfitemsetting == nullptr )
        {
            return;
        }

        auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfitemsetting->_type );
        if ( kftypesetting == nullptr )
        {
            return;
        }

        ItemIndexKey key( player->GetKeyID(), kfitembag->_data_setting->_name );
        auto kfbagindex = _player_item_index.Find( key );
        if ( kfbagindex == nullptr )
        {
            return;
        }

        for ( auto& tabname : kftypesetting->_tab_list )
        {
            auto kftabindex = kfbagindex->FindTab( tabname );
            if ( kftabindex == nullptr )
            {
                continue;
            }

            auto index = kfitem->Get<uint32>( tabname );
            kftabindex->AddEmpty( index );
        }
    }

    KFData* KFItemMoveModule::FindIndexItem( KFEntity* player, KFData* kfitembag, uint32 index )
    {
        auto kfbagsetting = KFItemBagConfig::Instance()->FindSetting( kfitembag->_data_setting->_name );
        if ( kfbagsetting == nullptr || kfbagsetting->_tab_list.empty() )
        {
            return nullptr;
        }

        // 指定索引
        ItemIndexKey key( player->GetKeyID(), kfitembag->_data_setting->_name );
        auto kfbagindex = _player_item_index.Find( key );
        if ( kfbagindex == nullptr )
        {
            return nullptr;
        }

        auto kftabindex = kfbagindex->FindTab( kfbagsetting->_index_name );
        if ( kftabindex == nullptr )
        {
            return nullptr;
        }

        auto uuid = kftabindex->GetUUID( index );
        return kfitembag->Find( uuid );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsNeedUpdateAddFromSource( const KFItemBagSetting* kfbagsetting, const std::string& sourcebag )
    {
        return kfbagsetting->_update_add_from_source.find( sourcebag ) != kfbagsetting->_update_add_from_source.end();
    }

    bool IsNeedUpdateMoveToTarget( const KFItemBagSetting* kfbagsetting, const std::string& targetbag )
    {
        return kfbagsetting->_update_move_to_target.find( targetbag ) != kfbagsetting->_update_move_to_target.end();
    }

    bool IsNeedShowMoveToTarget( const KFItemBagSetting* kfbagsetting, const std::string& targetbag )
    {
        return kfbagsetting->_show_move_to_target.find( targetbag ) != kfbagsetting->_show_move_to_target.end();
    }

    uint32 GetOverlayCount( const KFItemSetting* kfsetting, const std::string& bagname )
    {
        return kfsetting->_overlay_count;
    }

    bool IsOverlayItem( const KFItemSetting* kfsetting )
    {
        return kfsetting->_overlay_count > 1u;
    }

    bool IsBagHaveTab( const KFItemBagSetting* kfbagsetting, const std::string& tabname )
    {
        return kfbagsetting->_tab_list.find( tabname ) != kfbagsetting->_tab_list.end();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFItemMoveModule::SplitItemLogic( KFEntity* player, const KFItemSetting* kfitemsetting,
            const KFItemBagSetting* kfsourcebagsetting, KFData* kfsourcebag, KFData* kfsourceitem, uint32 splitcount,
            const KFItemBagSetting* kftargetbagsetting, KFData* kftargetbag, uint32 splitindex )
    {
        // 扣除源物品数量
        MoveItemCountLogic( player, kfitemsetting, kfsourcebagsetting, kfsourceitem, splitcount, kftargetbagsetting, nullptr );

        // 添加新的道具
        auto kftargetitem = player->CreateData( kfsourceitem );
        kftargetitem->CopyFrom( kfsourceitem );
        auto uuid = KFGlobal::Instance()->STMakeUuid( __STRING__( item ) );
        kftargetitem->SetKeyID( uuid );
        kftargetitem->Set( __STRING__( count ), splitcount );
        kftargetitem->Set( kftargetbagsetting->_index_name, splitindex );

        auto isupdate = IsNeedUpdateAddFromSource( kftargetbagsetting, kfsourcebagsetting->_id );
        player->AddRecord( kftargetbag, uuid, kftargetitem, isupdate );

        // 添加客户端显示
        if ( IsNeedShowMoveToTarget( kfsourcebagsetting, kftargetbagsetting->_id ) )
        {
            MoveItemDataToShow( player, kfitemsetting, kfsourcebag, kftargetbag, splitcount );
        }

        return KFMsg::Ok;
    }

    uint32 KFItemMoveModule::MoveItemLogic( KFEntity* player, const KFItemSetting* kfitemsetting,
                                            const KFItemBagSetting* kfsourcebagsetting, KFData* kfsourcebag, KFData* kfsourceitem,
                                            const KFItemBagSetting* kftargetbagsetting, KFData* kftargetbag, uint32 targetindex )
    {
        // 如果背包相同
        if ( kfsourcebag == kftargetbag )
        {
            if ( targetindex == 0u || targetindex == kfsourceitem->Get<uint32>( kfsourcebagsetting->_index_name ) )
            {
                return KFMsg::ItemIndexError;
            }

            // 源背包索引
            player->UpdateObject( kfsourceitem, kfsourcebagsetting->_index_name, KFEnum::Set, targetindex );
            return KFMsg::Ok;
        }

        // 不同背包, 先移除源背包
        {
            auto isupdate = IsNeedUpdateMoveToTarget( kfsourcebagsetting, kftargetbagsetting->_id );
            player->MoveRecord( kfsourcebag, kfsourceitem->GetKeyID(), isupdate );
        }

        // 添加目标背包
        kfsourceitem->Set( kfsourcebagsetting->_index_name, targetindex );
        auto isupdate = IsNeedUpdateAddFromSource( kftargetbagsetting, kfsourcebagsetting->_id );
        player->AddRecord( kftargetbag, kfsourceitem->GetKeyID(), kfsourceitem, isupdate );

        // 添加客户端显示
        if ( IsNeedShowMoveToTarget( kfsourcebagsetting, kftargetbagsetting->_id ) )
        {
            MoveItemDataToShow( player, kfitemsetting, kfsourcebag, kftargetbag, kfsourceitem );
        }

        return KFMsg::Ok;
    }

    uint32 KFItemMoveModule::ExchangeItemLogic( KFEntity* player,
            const KFItemSetting* kfsourceitemsetting, const KFItemBagSetting* kfsourcebagsetting, KFData* kfsourcebag, KFData* kfsourceitem,
            const KFItemSetting* kftargetitemsetting, const KFItemBagSetting* kftargetbagsetting, KFData* kftargetbag, KFData* kftargetitem )
    {
        if ( kfsourceitem == kftargetitem )
        {
            return KFMsg::ItemIndexError;
        }

        auto sourceindex = kfsourceitem->Get<uint32>( kfsourcebagsetting->_index_name );
        auto targetindex = kftargetitem->Get<uint32>( kftargetbagsetting->_index_name );
        if ( kfsourcebag == kftargetbag )
        {
            // 背包相同, 直接更新索引
            player->UpdateObject( kfsourceitem, kfsourcebagsetting->_index_name, KFEnum::Set, targetindex );
            player->UpdateObject( kftargetitem, kftargetbagsetting->_index_name, KFEnum::Set, sourceindex );
            return KFMsg::Ok;
        }

        // 判断是否能移动
        if ( !CheckItemCanMove( kfsourceitemsetting, kfsourceitem->_data_setting->_name, kftargetitem->_data_setting->_name ) )
        {
            return KFMsg::ItemCanNotStore;
        }

        // 找到可以移动到的背包
        auto kffindrecord = FindCanMoveItemBag( player, kftargetitemsetting, kfsourceitem->_data_setting->_name, kftargetbag->_data_setting->_name );
        if ( kffindrecord == nullptr )
        {
            return KFMsg::ItemCanNotStore;
        }

        auto kffindbagsetting = KFItemBagConfig::Instance()->FindSetting( kffindrecord->_data_setting->_name );
        if ( kffindbagsetting == nullptr )
        {
            return KFMsg::ItemCanNotStore;
        }

        // 判断源道具数量
        auto sourceitemcount = kfsourceitem->Get<uint32>( __STRING__( count ) );
        auto maxsourceoverlaycount = GetOverlayCount( kfsourceitemsetting, kftargetbagsetting->_id );

        // 判断目标道具数量
        auto targetitemcount = kftargetitem->Get<uint32>( __STRING__( count ) );
        auto maxtargetoverlaycount = GetOverlayCount( kftargetitemsetting, kfsourcebagsetting->_id );

        if ( sourceitemcount > maxsourceoverlaycount && targetitemcount > maxtargetoverlaycount )
        {
            // 需要拆分, 所以需要判断格子数
            if ( _kf_item->CheckItemBagFull( player, kffindrecord ) )
            {
                return KFMsg::ItemBagIsFull;
            }

            // 需要拆分, 所以需要判断格子数
            if ( _kf_item->CheckItemBagFull( player, kftargetbag ) )
            {
                return KFMsg::ItemBagIsFull;
            }

            // 源道具拆分
            SplitItemLogic( player, kfsourceitemsetting, kfsourcebagsetting, kfsourcebag, kfsourceitem, maxsourceoverlaycount, kftargetbagsetting, kftargetbag, 0u );

            // 目标道具拆分
            SplitItemLogic( player, kftargetitemsetting, kftargetbagsetting, kftargetbag, kftargetbag, maxtargetoverlaycount, kffindbagsetting, kffindrecord, 0u );
        }
        else if ( sourceitemcount > maxsourceoverlaycount )
        {
            // 需要拆分, 所以需要判断格子数
            if ( _kf_item->CheckItemBagFull( player, kffindrecord ) )
            {
                return KFMsg::ItemBagIsFull;
            }

            // 移动目标道具
            MoveItemLogic( player, kftargetitemsetting, kftargetbagsetting, kftargetbag, kftargetitem, kffindbagsetting, kffindrecord, 0u );

            // 拆分源道具
            SplitItemLogic( player, kfsourceitemsetting, kfsourcebagsetting, kfsourcebag, kfsourceitem, maxsourceoverlaycount, kftargetbagsetting, kftargetbag, targetindex );
        }
        else if ( targetitemcount > maxtargetoverlaycount )
        {
            // 需要拆分, 所以需要判断格子数
            if ( _kf_item->CheckItemBagFull( player, kftargetbag ) )
            {
                return KFMsg::ItemBagIsFull;
            }

            // 移动源道具
            MoveItemLogic( player, kfsourceitemsetting, kfsourcebagsetting, kfsourcebag, kfsourceitem, kftargetbagsetting, kftargetbag, 0u );

            // 拆分目标道具
            SplitItemLogic( player, kftargetitemsetting, kftargetbagsetting, kftargetbag, kftargetitem, maxtargetoverlaycount, kffindbagsetting, kffindrecord, sourceindex );
        }
        else
        {
            if ( kffindrecord != kfsourcebag )
            {
                if ( _kf_item->CheckItemBagFull( player, kffindrecord ) )
                {
                    return KFMsg::ItemBagIsFull;
                }

                sourceindex = 0u;
            }
            else
            {
                AddItemEmptyIndex( player, kfsourcebag, kfsourceitem );
                AddItemEmptyIndex( player, kftargetbag, kftargetitem );
            }

            MoveItemLogic( player, kftargetitemsetting, kftargetbagsetting, kftargetbag, kftargetitem, kffindbagsetting, kffindrecord, sourceindex );
            MoveItemLogic( player, kfsourceitemsetting, kfsourcebagsetting, kfsourcebag, kfsourceitem, kftargetbagsetting, kftargetbag, targetindex );
        }

        return KFMsg::Ok;
    }

    uint32 KFItemMoveModule::MergeItemLogic( KFEntity* player, const KFItemSetting* kfitemsetting,
            const KFItemBagSetting* kfsourcebagsetting, KFData* kfsourceitem, uint32 mergecount,
            const KFItemBagSetting* kftargetbagsetting, KFData* kftargetitem )
    {
        if ( kfsourceitem == kftargetitem )
        {
            return KFMsg::ItemIndexError;
        }

        auto sourceitemcount = kfsourceitem->Get<uint32>( __STRING__( count ) );
        if ( sourceitemcount < mergecount )
        {
            mergecount = sourceitemcount;
        }

        // 已经达到最大
        auto targetitemcount = kftargetitem->Get<uint32>( __STRING__( count ) );
        auto maxtargetoverlaycount = GetOverlayCount( kfitemsetting, kftargetbagsetting->_id );
        auto canaddcount = CalcItemAddCount( mergecount, targetitemcount, maxtargetoverlaycount );
        if ( canaddcount == 0u )
        {
            return KFMsg::ItemOverlayCountMax;
        }

        // 移动数量
        MoveItemCountLogic( player, kfitemsetting, kfsourcebagsetting, kfsourceitem, canaddcount, kftargetbagsetting, kftargetitem );
        return KFMsg::Ok;
    }

    uint32 KFItemMoveModule::CalcItemAddCount( uint32 sourcecount, uint32 targetcount, uint32 maxcount )
    {
        if ( targetcount >= maxcount )
        {
            return 0u;
        }

        auto canaddcount = maxcount - targetcount;
        return sourcecount <= canaddcount ? sourcecount : canaddcount;
    }

    void KFItemMoveModule::MoveItemCountLogic( KFEntity* player, const KFItemSetting* kfitemsetting,
            const KFItemBagSetting* kfsourcebagsetting, KFData* kfsourceitem, uint32 movecount,
            const KFItemBagSetting* kftargetbagsetting, KFData* kftargetitem )
    {
        // 源背包
        auto ismoveupdate = IsNeedUpdateMoveToTarget( kfsourcebagsetting, kftargetbagsetting->_id );
        auto sourcecount = kfsourceitem->Get<uint32>( __STRING__( count ) );
        if ( sourcecount <= movecount )
        {
            player->RemoveRecord( kfsourceitem->GetParent(), kfsourceitem->GetKeyID(), ismoveupdate );
        }
        else
        {
            player->UpdateObject( kfsourceitem, __STRING__( count ), KFEnum::Dec, movecount, ismoveupdate );
        }

        // 目标背包
        if ( kftargetitem == nullptr )
        {
            return;
        }

        // 是否移动更新
        auto isaddupdate = IsNeedUpdateMoveToTarget( kftargetbagsetting, kfsourcebagsetting->_id );
        player->UpdateObject( kftargetitem, __STRING__( count ), KFEnum::Add, movecount, isaddupdate );

        // 添加显示
        if ( IsNeedShowMoveToTarget( kfsourcebagsetting, kftargetbagsetting->_id ) )
        {
            MoveItemDataToShow( player, kfitemsetting, kfsourceitem->GetParent(), kftargetitem->GetParent(), movecount );
        }
    }

    bool KFItemMoveModule::CheckItemCanMove( const KFItemSetting* kfsetting, const std::string& sourcename, const std::string& targetname )
    {
        if ( sourcename == targetname )
        {
            return true;
        }

        auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfsetting->_type );
        if ( kftypesetting == nullptr )
        {
            return false;
        }

        return kftypesetting->_move_bag_list.find( targetname ) != kftypesetting->_move_bag_list.end();
    }

    KFData* KFItemMoveModule::FindCanMoveItemBag( KFEntity* player, const KFItemSetting* kfsetting, const std::string& sourcename, const std::string& excludename )
    {
        auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfsetting->_type );
        if ( kftypesetting == nullptr )
        {
            return nullptr;
        }

        if ( kftypesetting->_move_bag_list.find( sourcename ) != kftypesetting->_move_bag_list.end() )
        {
            return player->Find( sourcename );
        }

        // 否则遍历可以移动到的包裹
        for ( auto& bagname : kftypesetting->_move_bag_list )
        {
            if ( bagname == excludename )
            {
                continue;
            }

            return player->Find( bagname );
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFItemMoveModule::MoveItem( KFEntity* player, KFData* kfsourcebag, KFData* kfsourceitem, KFData* kftargetbag, uint32 targetindex )
    {
        // 判断是否限制
        auto kfsourceitemsetting = KFItemConfig::Instance()->FindSetting( kfsourceitem->Get<uint32>( __STRING__( id ) ) );
        if ( kfsourceitemsetting == nullptr )
        {
            return KFMsg::ItemSettingNotExist;
        }

        // 判断能否移动
        if ( !CheckItemCanMove( kfsourceitemsetting, kfsourcebag->_data_setting->_name, kftargetbag->_data_setting->_name ) )
        {
            return KFMsg::ItemCanNotStore;
        }

        auto kfsourcebagsetting = KFItemBagConfig::Instance()->FindSetting( kfsourcebag->_data_setting->_name );
        auto kftargetbagsetting = KFItemBagConfig::Instance()->FindSetting( kftargetbag->_data_setting->_name );
        if ( kfsourcebagsetting == nullptr || kftargetbagsetting == nullptr )
        {
            return KFMsg::ItemCanNotStore;
        }

        uint32 result = KFMsg::Error;
        auto sourceitemcount = kfsourceitem->Get<uint32>( __STRING__( count ) );
        auto kftargetitem = FindIndexItem( player, kftargetbag, targetindex );
        if ( kftargetitem == nullptr )
        {
            if ( !_kf_item->CheckItemBagFull( player, kftargetbag ) )
            {
                // 移动到空格子上
                // 判断堆叠数量, 如果目标小于源堆叠, 并且源数量大于目标堆叠数量, 拆分物品
                auto maxtargetoverlaycount = GetOverlayCount( kfsourceitemsetting, kftargetbagsetting->_id );
                if ( sourceitemcount > maxtargetoverlaycount )
                {
                    // 拆分物品
                    result = SplitItemLogic( player, kfsourceitemsetting, kfsourcebagsetting, kfsourcebag, kfsourceitem, maxtargetoverlaycount, kftargetbagsetting, kftargetbag, targetindex );
                }
                else
                {
                    // 移动物品
                    result = MoveItemLogic( player, kfsourceitemsetting, kfsourcebagsetting, kfsourcebag, kfsourceitem, kftargetbagsetting, kftargetbag, targetindex );
                }
            }
            else
            {
                result = MoveItemDataLogic( player, kfsourceitemsetting, kfsourcebagsetting, kfsourcebag, kfsourceitem, kftargetbagsetting, kftargetbag );
            }
        }
        else
        {
            auto targetitemid = kftargetitem->Get<uint32>( kftargetitem->_data_setting->_config_key_name );
            auto kftargetitemsetting = KFItemConfig::Instance()->FindSetting( targetitemid );
            if ( kftargetitemsetting != nullptr )
            {
                // 判断道具是否能堆叠
                if ( CheckItemCanMerge( kfsourceitemsetting, kfsourceitem, kftargetitemsetting, kftargetitem ) )
                {
                    // 移动源物品的一定数量到目标物品上
                    result = MergeItemLogic( player, kfsourceitemsetting, kfsourcebagsetting, kfsourceitem, sourceitemcount, kftargetbagsetting, kftargetitem );
                }
                else
                {
                    // 直接交换
                    result = ExchangeItemLogic( player, kfsourceitemsetting, kfsourcebagsetting, kfsourcebag, kfsourceitem, kftargetitemsetting, kftargetbagsetting, kftargetbag, kftargetitem );
                }
            }
            else
            {
                result = KFMsg::ItemSettingNotExist;
            }
        }

        return result;
    }

    bool KFItemMoveModule::CheckItemCanMerge( const KFItemSetting* kfsourcesetting, KFData* kfsourceitem, const KFItemSetting* kftargetsetting, KFData* kftargetitem )
    {
        // 不同道具, 不能堆叠
        if ( kfsourcesetting->_id != kftargetsetting->_id || !IsOverlayItem( kfsourcesetting ) )
        {
            return false;
        }

        // 判断目标数量
        auto targetitemcount = kftargetitem->Get<uint32>( __STRING__( count ) );
        auto maxtargetoverlaycount = GetOverlayCount( kfsourcesetting, kftargetitem->_data_setting->_name );
        if ( targetitemcount < maxtargetoverlaycount )
        {
            return true;
        }

        // 目标满了, 如果源数据小于堆叠数量, 需要可以交换
        auto sourceitemcount = kfsourceitem->Get<uint32>( __STRING__( count ) );
        if ( sourceitemcount > maxtargetoverlaycount )
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
            result = MoveTabItem( kfentity, kfmsg->sourcename(), kfmsg->tabname(), kfmsg->sourceuuid(), kfmsg->targetindex() );
        }
        else
        {
            result = MoveBagItem( kfentity, kfmsg->sourcename(), kfmsg->sourceuuid(), kfmsg->targetname(), kfmsg->targetindex() );
        }
        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( kfentity, result );
        }
    }

    uint32 KFItemMoveModule::MoveBagItem( KFEntity* player, const std::string& sourcename, uint64 itemuuid, const std::string& targetname, uint32 targetindex )
    {
        auto kfsourcebagsetting = KFItemBagConfig::Instance()->FindSetting( sourcename );
        if ( kfsourcebagsetting != nullptr && !kfsourcebagsetting->_can_move )
        {
            return KFMsg::ItemBagCanNotMove;
        }

        auto kftargetbagsetting = KFItemBagConfig::Instance()->FindSetting( targetname );
        if ( kftargetbagsetting != nullptr && !kftargetbagsetting->_can_move )
        {
            return KFMsg::ItemBagCanNotMove;
        }

        // 判断背包属性
        auto kfsourcebag = player->Find( sourcename );
        auto kftargetbag = player->Find( targetname );
        if ( kfsourcebag == nullptr || kftargetbag == nullptr )
        {
            return KFMsg::ItemBagNameError;
        }

        auto maxindex = GetItemMaxIndex( player, kftargetbag );
        if ( targetindex > maxindex )
        {
            return KFMsg::ItemIndexError;
        }

        auto kfsourceitem = kfsourcebag->Find( itemuuid );
        if ( kfsourceitem == nullptr )
        {
            return KFMsg::ItemDataNotExist;
        }

        auto result = MoveItem( player, kfsourcebag, kfsourceitem, kftargetbag, targetindex );
        if ( result != KFMsg::Ok )
        {
            player->UpdateObject( kfsourceitem, kfsourcebagsetting->_index_name, KFEnum::Set, kfsourceitem->Get<uint32>( kfsourcebagsetting->_index_name ) );
        }

        return result;
    }

    uint32 KFItemMoveModule::MoveTabItem( KFEntity* player, const std::string& bagname, const std::string& tabname, uint64 itemuuid, uint32 targetindex )
    {
        auto kfitembag = player->Find( bagname );
        if ( kfitembag == nullptr )
        {
            return KFMsg::ItemBagNameError;
        }

        auto kfsourceitem = kfitembag->Find( itemuuid );
        if ( kfsourceitem == nullptr )
        {
            return KFMsg::ItemDataNotExist;
        }

        ItemIndexKey key( player->GetKeyID(), bagname );
        auto kfbagindex = _player_item_index.Create( key );
        auto kftabindex = kfbagindex->FindTab( tabname );
        if ( kftabindex == nullptr )
        {
            return KFMsg::ItemTabNotExist;
        }

        auto lastindex = kfsourceitem->Get<uint32>( kftabindex->_name );
        if ( lastindex == targetindex || targetindex == 0u || targetindex > kfbagindex->GetMaxIndex() )
        {
            return KFMsg::ItemIndexError;
        }

        // 判断目标是否道具
        auto targetuuid = kftabindex->GetUUID( targetindex );
        auto kftargetitem = kfitembag->Find( targetuuid );
        if ( kftargetitem == nullptr )
        {
            player->UpdateObject( kfsourceitem, tabname, KFEnum::Set, targetindex );
            return KFMsg::Ok;
        }

        auto sourceitemid = kfsourceitem->Get<uint32>( kfsourceitem->_data_setting->_config_key_name );
        auto targetitemid = kftargetitem->Get<uint32>( kftargetitem->_data_setting->_config_key_name );
        auto kfsourceitemsetting = KFItemConfig::Instance()->FindSetting( sourceitemid );
        auto kftargetitemsetting = KFItemConfig::Instance()->FindSetting( targetitemid );
        if ( kfsourceitemsetting == nullptr || kftargetitemsetting == nullptr )
        {
            return KFMsg::ItemSettingNotExist;
        }

        // 判断道具是否能堆叠
        if ( CheckItemCanMerge( kfsourceitemsetting, kfsourceitem, kftargetitemsetting, kftargetitem ) )
        {
            auto kfitembagsetting = KFItemBagConfig::Instance()->FindSetting( bagname );
            if ( kfitembagsetting == nullptr )
            {
                return KFMsg::ItemBagCanNotMove;
            }

            // 移动源物品的一定数量到目标物品上
            auto sourceitemcount = kfsourceitem->Get<uint32>( __STRING__( count ) );
            return MergeItemLogic( player, kfsourceitemsetting, kfitembagsetting, kfsourceitem, sourceitemcount, kfitembagsetting, kftargetitem );
        }

        // 交换位置
        player->UpdateObject( kfsourceitem, tabname, KFEnum::Set, targetindex );
        player->UpdateObject( kftargetitem, tabname, KFEnum::Set, lastindex );
        return KFMsg::Ok;
    }

    __KF_MESSAGE_FUNCTION__( KFItemMoveModule::HandleMoveAllItemReq, KFMsg::MsgMoveAllItemReq )
    {
        auto kfsourcebagsetting = KFItemBagConfig::Instance()->FindSetting( kfmsg->sourcename() );
        auto kftargetbagsetting = KFItemBagConfig::Instance()->FindSetting( kfmsg->targetname() );
        if ( kfsourcebagsetting == nullptr || kftargetbagsetting == nullptr || !kfsourcebagsetting->_can_move_all )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::ItemBagCanNotMove );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 全部拾取
        auto kfsourcerecord = kfentity->Find( kfmsg->sourcename() );
        auto kftargetrecord = kfentity->Find( kfmsg->targetname() );
        if ( kfsourcerecord == nullptr || kftargetrecord == nullptr )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::ItemBagNameError );
        }

        // 遍历出可以移动的道具
        std::map< KFData*, const KFItemSetting* > movelist;
        for ( auto kfitem = kfsourcerecord->First(); kfitem != nullptr; kfitem = kfsourcerecord->Next() )
        {
            auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
            auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
            if ( kfsetting == nullptr )
            {
                continue;
            }

            movelist[ kfitem ] = kfsetting;
        }

        for ( auto& iter : movelist )
        {
            auto result = MoveItemDataLogic( kfentity, iter.second, kfsourcebagsetting, kfsourcerecord, iter.first, kftargetbagsetting, kftargetrecord );
            if ( result != KFMsg::Ok )
            {
                return _kf_display->SendToClient( kfentity, result );
            }
        }
    }

    uint32 KFItemMoveModule::MoveItemDataLogic( KFEntity* player, const KFItemSetting* kfitemsetting,
            const KFItemBagSetting* kfsourcebagsetting, KFData* kfsourcerecord, KFData* kfsourceitem,
            const KFItemBagSetting* kftargetbagsetting, KFData* kftargetrecord )
    {
        // 判断是否能堆叠
        if ( IsOverlayItem( kfitemsetting ) )
        {
            auto sourcecount = kfsourceitem->Get<uint32>( __STRING__( count ) );
            auto maxoverlaycount = GetOverlayCount( kfitemsetting, kftargetrecord->_data_setting->_name );

            // 堆叠起来
            for ( auto kftargetitem = kftargetrecord->First(); kftargetitem != nullptr; kftargetitem = kftargetrecord->Next() )
            {
                auto itemid = kftargetitem->Get<uint32>( kftargetrecord->_data_setting->_config_key_name );
                if ( itemid != kfitemsetting->_id )
                {
                    continue;
                }

                auto targetcount = kftargetitem->Get<uint32>( __STRING__( count ) );
                auto movecount = CalcItemAddCount( sourcecount, targetcount, maxoverlaycount );
                if ( movecount == 0u )
                {
                    continue;
                }

                // 移动道具数量
                MoveItemCountLogic( player, kfitemsetting, kfsourcebagsetting, kfsourceitem, movecount, kftargetbagsetting, kftargetitem );
                sourcecount -= movecount;
                if ( sourcecount == 0u )
                {
                    return KFMsg::Ok;
                }
            }

            // 剩下的道具数量
            while ( sourcecount > maxoverlaycount )
            {
                if ( _kf_item->CheckItemBagFull( player, kftargetrecord ) )
                {
                    break;
                }

                SplitItemLogic( player, kfitemsetting, kfsourcebagsetting, kfsourcerecord, kfsourceitem, maxoverlaycount, kftargetbagsetting, kftargetrecord, 0u );
                sourcecount -= maxoverlaycount;
            }
        }

        // 不能堆叠或者剩下没有堆叠完的
        auto isfull = _kf_item->CheckItemBagFull( player, kftargetrecord );
        if ( !isfull )
        {
            MoveItemLogic( player, kfitemsetting, kfsourcebagsetting, kfsourcerecord, kfsourceitem, kftargetbagsetting, kftargetrecord, 0u );
        }

        return isfull ? KFMsg::ItemBagIsFull : KFMsg::Ok;
    }

    __KF_MESSAGE_FUNCTION__( KFItemMoveModule::HandleSortItemReq, KFMsg::MsgSortItemReq )
    {
        SortItem( kfentity, kfmsg->bagname(), kfmsg->tabname() );
    }

    void KFItemMoveModule::SortItem( KFEntity* player, const std::string& bagname, const std::string& tabname )
    {
        auto kfitemrecord = player->Find( bagname );
        if ( kfitemrecord == nullptr )
        {
            return;
        }

        auto kfitembagsetting = KFItemBagConfig::Instance()->FindSetting( bagname );
        if ( kfitembagsetting == nullptr || !IsBagHaveTab( kfitembagsetting, tabname ) )
        {
            return;
        }

        // 重置格子数量
        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfbagindex = _player_item_index.Create( key );
        auto kftabindex = kfbagindex->InitMaxIndex( tabname );

        // sort从小到大, 品质从大到小, id从小到大
        std::map<uint32, std::map<uint32, std::map<const KFItemSetting*, std::set<KFData*>>>> sortlist;
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            auto id = kfitem->Get<uint32>( kfitemrecord->_data_setting->_config_key_name );
            auto kfsetting = KFItemConfig::Instance()->FindSetting( id );
            if ( kfsetting == nullptr )
            {
                continue;
            }

            auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfsetting->_type );
            if ( kftypesetting == nullptr || kftypesetting->_tab_list.find( tabname ) == kftypesetting->_tab_list.end() )
            {
                continue;
            }

            sortlist[ kftypesetting->_sort ][ kfsetting->_quality ][ kfsetting ].insert( kfitem );
        }

        for ( auto& iter : sortlist )
        {
            auto& qualitylist = iter.second;
            for ( auto miter = qualitylist.rbegin(); miter != qualitylist.rend(); ++miter )
            {
                auto& itemlist = miter->second;
                for ( auto& qiter : itemlist )
                {
                    SortItem( player, qiter.first, kfitembagsetting, kftabindex, kfitemrecord, qiter.second );
                }
            }
        }
    }

    std::tuple<KFData*, uint32> FindMaxCountItem( uint32 maxoverlaycount, std::set< KFData* >& itemlist, KFData* kfminitem )
    {
        auto maxcount = 0u;
        KFData* kffind = nullptr;

        for ( auto kfitem : itemlist )
        {
            auto count = kfitem->Get<uint32>( __STRING__( count ) );
            if ( count < maxoverlaycount && count > maxcount && kfitem != kfminitem )
            {
                kffind = kfitem;
                maxcount = count;
            }
        }

        return std::make_tuple( kffind, maxcount );
    }

    std::tuple<KFData*, uint32> FindMinCountItem( uint32 maxoverlaycount, std::set< KFData* >& itemlist )
    {
        auto mincount = __MAX_UINT32__;
        KFData* kffind = nullptr;

        for ( auto kfitem : itemlist )
        {
            auto count = kfitem->Get<uint32>( __STRING__( count ) );
            if ( count < maxoverlaycount && count < mincount )
            {
                kffind = kfitem;
                mincount = count;
            }
        }

        return std::make_tuple( kffind, mincount );
    }

    KFData* PopMaxCountItem( uint32 maxoverlaycount, std::set< KFData* >& itemlist )
    {
        auto maxcount = 0u;
        KFData* kffind = nullptr;

        for ( auto kfitem : itemlist )
        {
            auto count = kfitem->Get<uint32>( __STRING__( count ) );
            if ( count == 0u || count >= maxoverlaycount )
            {
                kffind = kfitem;
                break;
            }

            if ( count > maxcount )
            {
                kffind = kfitem;
                maxcount = count;
            }
        }

        if ( kffind != nullptr )
        {
            itemlist.erase( kffind );
        }

        return kffind;
    }

    void KFItemMoveModule::SortItem( KFEntity* player, const KFItemSetting* kfitemsetting, const KFItemBagSetting* kfbagsetting,
                                     KFItemTabIndex* kftabindex, KFData* kfitemrecord, std::set<KFData*>& itemlist )
    {
        auto maxoverlaycount = GetOverlayCount( kfitemsetting, kfitemrecord->_data_setting->_name );
        if ( IsOverlayItem( kfitemsetting ) )
        {
            while ( true )
            {
                auto mincount = 0u;
                KFData* minitem = nullptr;
                std::tie( minitem, mincount ) = FindMinCountItem( maxoverlaycount, itemlist );
                if ( minitem == nullptr )
                {
                    break;
                }

                auto maxcount = 0u;
                KFData* maxitem = nullptr;
                std::tie( maxitem, maxcount ) = FindMaxCountItem( maxoverlaycount, itemlist, minitem );
                if ( maxitem == nullptr )
                {
                    break;
                }

                auto canaddcount = maxoverlaycount - maxcount;
                canaddcount = __MIN__( mincount, canaddcount );
                MoveItemCountLogic( player, kfitemsetting, kfbagsetting, minitem, canaddcount, kfbagsetting, maxitem );
                if ( canaddcount == mincount )
                {
                    itemlist.erase( minitem );
                }
            }
        }

        while ( true )
        {
            auto kfitem = PopMaxCountItem( maxoverlaycount, itemlist );
            if ( kfitem == nullptr )
            {
                break;
            }

            kfitem->Set( kftabindex->_name, 0u );
            auto index = kftabindex->FindEmpty( kfitem->GetKeyID() );
            player->UpdateObject( kfitem, kftabindex->_name, KFEnum::Set, index );
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFItemMoveModule::MoveItemDataToShow( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kftargetrecord, uint32 count )
    {
        StringUInt64 values;
        values[ __STRING__( count ) ] = count;
        values[ __STRING__( id ) ] = kfsetting->_id;
        player->AddDataToShow( kfsourcerecord->_data_setting->_name, 0u, __STRING__( item ), kfsetting->_id, values, true );
    }

    void KFItemMoveModule::MoveItemDataToShow( KFEntity* player, const KFItemSetting* kfsetting, KFData* kfsourcerecord, KFData* kftargetrecord, KFData* kfitem )
    {
        if ( IsOverlayItem( kfsetting ) )
        {
            player->AddDataToShow( kfsourcerecord->_data_setting->_name, 0u, kfitem );
        }
        else
        {
            auto count = kfitem->Get( __STRING__( count ) );
            MoveItemDataToShow( player, kfsetting, kfsourcerecord, kftargetrecord, count );
        }
    }
}