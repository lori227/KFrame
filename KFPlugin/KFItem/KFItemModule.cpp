#include "KFItemModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFItemModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        __REGISTER_GET_CONFIG_VALUE__( __KF_STRING__( item ), &KFItemModule::GetItemTotalCount );
        __REGISTER_ADD_DATA_1__( __KF_STRING__( item ), &KFItemModule::OnAddItemCallBack );
        __REGISTER_REMOVE_DATA_1__( __KF_STRING__( item ), &KFItemModule::OnRemoveItemCallBack );
        __REGISTER_UPDATE_DATA_2__( __KF_STRING__( item ), __KF_STRING__( count ), &KFItemModule::OnItemCountUpdateCallBack );
        __REGISTER_UPDATE_DATA_2__( __KF_STRING__( item ), __KF_STRING__( time ), &KFItemModule::OnItemTimeUpdateCallBack );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_CHECK_ADD_ELEMENT__( __KF_STRING__( item ), &KFItemModule::CheckAddItemElement );
        __REGISTER_ADD_ELEMENT__( __KF_STRING__( item ), &KFItemModule::AddItemElement );
        __REGISTER_CHECK_REMOVE_ELEMENT__( __KF_STRING__( item ), &KFItemModule::CheckRemoveItemElement );
        __REGISTER_REMOVE_ELEMENT__( __KF_STRING__( item ), &KFItemModule::RemoveItemElement );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_ENTER_PLAYER__( &KFItemModule::OnEnterItemModule );
        __REGISTER_LEAVE_PLAYER__( &KFItemModule::OnLeaveItemModule );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_SPLIT_ITEM_REQ, &KFItemModule::HandleSplitItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_MERGE_ITEM_REQ, &KFItemModule::HandleMergeItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_MOVE_ITEM_REQ, &KFItemModule::HandleMoveItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_MOVE_ALL_ITEM_REQ, &KFItemModule::HandleMoveAllItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXCHANGE_ITEM_REQ, &KFItemModule::HandleExchangeItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CLEAN_ITEM_REQ, &KFItemModule::HandleCleanItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SORT_ITEM_REQ, &KFItemModule::HandleSortItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REMOVE_ITEM_REQ, &KFItemModule::HandleRemoveItemReq );
    }

    void KFItemModule::BeforeShut()
    {
        __UN_TIMER_0__();
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_GET_CONFIG_VALUE__( __KF_STRING__( item ) );
        __UN_ADD_DATA_1__( __KF_STRING__( item ) );
        __UN_REMOVE_DATA_1__( __KF_STRING__( item ) );
        __UN_UPDATE_DATA_2__( __KF_STRING__( item ), __KF_STRING__( count ) );
        __UN_UPDATE_DATA_2__( __KF_STRING__( item ), __KF_STRING__( time ) );

        __UN_CHECK_ADD_ELEMENT__( __KF_STRING__( item ) );
        __UN_ADD_ELEMENT__( __KF_STRING__( item ) );
        __UN_CHECK_REMOVE_ELEMENT__( __KF_STRING__( item ) );
        __UN_REMOVE_ELEMENT__( __KF_STRING__( item ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_SPLIT_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_MERGE_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_MOVE_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_MOVE_ALL_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXCHANGE_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_CLEAN_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SORT_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_REMOVE_ITEM_REQ );
    }

    void KFItemModule::PrepareRun()
    {
        _item_data_list.clear();
        _item_data_list = _kf_component->GetDataList( __KF_STRING__( item ) );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_GET_CONFIG_VALUE_FUNCTION__( KFItemModule::GetItemTotalCount )
    {
        auto kfsetting = KFItemConfig::Instance()->FindSetting( id );
        if ( kfsetting == nullptr )
        {
            return 0u;
        }

        auto kfitemrecord = FindItemRecord( player, kfsetting, 0u );
        if ( kfitemrecord == nullptr )
        {
            return 0u;
        }

        // 获取item列表
        std::list< KFData* > finditem;
        kfitemrecord->FindData( kfitemrecord->_data_setting->_config_key_name, id, finditem, true );

        auto totalcount = 0u;
        for ( auto kfitem : finditem )
        {
            totalcount += kfitem->GetValue<uint32>( __KF_STRING__( count ) );
        }

        return totalcount;
    }

    __KF_CHECK_ADD_ELEMENT_FUNCTION__( KFItemModule::CheckAddItemElement )
    {
        if ( !kfelement->IsObject() )
        {
            return false;
        }

        // 计算物品数量
        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        auto itemcount = kfelementobject->CalcValue( kfparent->_class_setting, __KF_STRING__( count ), multiple );
        if ( itemcount == _invalid_int || kfelementobject->_config_id == _invalid_int )
        {
            return false;
        }

        auto kfsetting = KFItemConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            return false;
        }

        kfparent = FindItemRecord( player, kfsetting, 0u );
        if ( kfparent == nullptr )
        {
            return false;
        }

        return !CheckItemRecordFull( kfparent, kfsetting, itemcount );
    }

    bool KFItemModule::CheckItemRecordFull( KFData* kfitemrecrod, const KFItemSetting* kfsetting, uint32 itemcount )
    {
        // 计算占用格子数
        uint32 usesize = ( itemcount + kfsetting->_overlay_count - 1 ) / kfsetting->_overlay_count;
        auto leftsize = kfitemrecrod->MaxSize() - kfitemrecrod->Size();
        if ( leftsize >= usesize )
        {
            return false;
        }

        // 不能叠加
        if ( !kfsetting->IsOverlay() )
        {
            return true;
        }

        // 可叠加物品
        auto canaddcount = 0u;
        std::list< KFData* > finditem;
        kfitemrecrod->FindData( kfitemrecrod->_data_setting->_config_key_name, kfsetting->_id, finditem, true );
        for ( auto kfitem : finditem )
        {
            auto oldcount = kfitem->GetValue( __KF_STRING__( count ) );
            canaddcount += kfsetting->_overlay_count - __MIN__( oldcount, kfsetting->_overlay_count );
            if ( canaddcount >= itemcount )
            {
                return false;
            }
        }

        return true;
    }

    void KFItemModule::AddItemDataToShow( KFEntity* player, uint64 id, uint64 count )
    {
        player->AddDataToShow( __KF_STRING__( item ), id, __KF_STRING__( count ), count );
    }

    __KF_ADD_ELEMENT_FUNCTION__( KFItemModule::AddItemElement )
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

        auto kfsetting = KFItemConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "id=[{}] itemsetting = null!", kfelementobject->_config_id );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        // 查找背包数据
        kfparent = FindItemRecord( player, kfsetting, 0u );
        if ( kfparent == nullptr )
        {
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        // 计算物品数量
        auto itemcount = kfelementobject->CalcValue( kfparent->_class_setting, __KF_STRING__( count ), multiple );
        if ( itemcount == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] count = 0!", kfelementobject->_config_id );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        auto addcount = 0u;
        auto leftcount = itemcount;
        uint32 showdata = KFDataDefine::Show_None;
        if ( kfsetting->_overlay_type == KFItemEnum::OverlayByTime )
        {
            // 计算时间
            auto itemtime = kfelementobject->CalcValue( kfparent->_class_setting, __KF_STRING__( time ), multiple );
            if ( itemtime == 0u )
            {
                __LOG_ERROR_FUNCTION__( function, line, "time item id=[{}] count = 0!", kfelementobject->_config_id );
                return std::make_tuple( KFDataDefine::Show_None, nullptr );
            }

            std::tie( addcount, leftcount, showdata ) = AddOverlayTimeItem( player, kfparent, kfelementobject, kfsetting, itemcount, itemtime );
            if ( showdata == KFDataDefine::Show_Data )
            {
                AddItemDataToShow( player, kfsetting->_id, addcount );
            }
        }
        else if ( kfsetting->_overlay_type == KFItemEnum::OverlayByCount )
        {
            // 数量叠加道具
            do
            {
                std::tie( addcount, leftcount, showdata ) = AddOverlayCountItem( player, kfparent, kfelementobject, kfsetting, leftcount );
                if ( showdata == KFDataDefine::Show_Data )
                {
                    AddItemDataToShow( player, kfsetting->_id, addcount );
                }
            } while ( leftcount > 0u );
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CallItemLuaFunction( player, KFItemEnum::AddFunction, kfsetting, itemcount );
        return std::make_tuple( KFDataDefine::Show_None, nullptr );
    }

    std::tuple<uint32, uint32, uint32> KFItemModule::AddOverlayTimeItem( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint32 count, uint32 time )
    {
        std::list< KFData* > finditem;
        kfparent->FindData( kfparent->_data_setting->_config_key_name, kfsetting->_id, finditem, false );

        if ( !finditem.empty() )
        {
            auto kfitem = finditem.front();
            player->UpdateData( kfitem, __KF_STRING__( time ), KFEnum::Add, count * time );
            return std::make_tuple( count, 0, KFDataDefine::Show_Data );
        }

        return AddNewItemData( player, kfparent, kfelementobject, kfsetting, count, time );
    }

    std::tuple<uint32, uint32, uint32> KFItemModule::AddOverlayCountItem( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint32 count )
    {
        // 叠加数量 > 1
        if ( kfsetting->IsOverlay() )
        {
            std::list< KFData* > finditem;
            kfparent->FindData( kfparent->_data_setting->_config_key_name, kfsetting->_id, finditem, true );

            // 判断是否可以叠加完
            auto totaladdcount = 0u;
            for ( auto kfitem : finditem )
            {
                auto oldcount = kfitem->GetValue( __KF_STRING__( count ) );
                auto addcount = CalcItemAddCount( count, oldcount, kfsetting->_overlay_count );
                if ( addcount == 0u )
                {
                    continue;
                }

                // 更新数量
                player->UpdateData( kfitem, __KF_STRING__( count ), KFEnum::Add, addcount );

                count -= addcount;
                totaladdcount += addcount;
                if ( count == 0u )
                {
                    break;
                }
            }

            // 添加显示
            AddItemDataToShow( player, kfsetting->_id, totaladdcount );
            if ( count == 0u )
            {
                return std::make_tuple( totaladdcount, 0, KFDataDefine::Show_None );
            }
        }

        // 添加新的物品
        return AddNewItemData( player, kfparent, kfelementobject, kfsetting, count, 0u );
    }

    std::tuple<uint32, uint32, uint32> KFItemModule::AddNewItemData( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint32 count, uint32 time )
    {
        uint32 showelement = KFDataDefine::Show_Data;
        // 判断满包情况, 需要添加另外的背包, 加入满包不显示( 测试特殊要求 )
        auto addcount = __MIN__( count, kfsetting->_overlay_count );
        auto kfnewparent = FindItemRecord( player, kfsetting, addcount );
        if ( kfnewparent != nullptr && kfnewparent != kfparent )
        {
            kfparent = kfnewparent;
            showelement = KFDataDefine::Show_None;
        }

        auto datasetting = kfparent->_data_setting;
        auto kfitem = _kf_kernel->CreateObject( datasetting );

        // count
        kfitem->SetValue( __KF_STRING__( count ), addcount );

        // id
        kfitem->SetValue( datasetting->_config_key_name, kfsetting->_id );

        // time
        if ( time != 0u )
        {
            kfitem->SetValue( __KF_STRING__( time ), KFGlobal::Instance()->_real_time + time * count );
        }

        // 位置
        auto index = FindItemEmptyIndex( player, kfparent );
        if ( index != 0u )
        {
            kfitem->SetValue( __KF_STRING__( index ), index );
        }

        // uuid
        auto uuid = KFGlobal::Instance()->STMakeUUID( __KF_STRING__( item ) );

        // 添加新的物品
        player->AddData( kfparent, uuid, kfitem );
        return std::make_tuple( addcount, count - addcount, showelement );
    }

    KFData* KFItemModule::FindItemRecord( KFEntity* player, const KFItemSetting* kfsetting, uint32 itemcount )
    {
        auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfsetting->_type );
        if ( kftypesetting == nullptr )
        {
            __LOG_ERROR__( "item=[{}] type=[{}] no type setting!", kfsetting->_id, kfsetting->_type );
            return nullptr;
        }

        KFData* kfitemrecord = nullptr;
        auto kfobject = player->GetData();
        auto status = kfobject->GetValue<uint32>( __KF_STRING__( basic ), __KF_STRING__( status ) );
        if ( status == KFMsg::ExploreStatus || status == KFMsg::PVEStatus )
        {
            kfitemrecord = kfobject->FindData( kftypesetting->_bag_name );

            // 需要判断包满情况
            if ( kfitemrecord != nullptr && itemcount > 0u && !kftypesetting->_extend_name.empty() )
            {
                auto isfull = CheckItemRecordFull( kfitemrecord, kfsetting, itemcount );
                if ( isfull )
                {
                    kfitemrecord = kfobject->FindData( kftypesetting->_extend_name );
                }
            }
        }
        else
        {
            kfitemrecord = kfobject->FindData( kftypesetting->_store_name );
        }

        if ( kfitemrecord == nullptr )
        {
            __LOG_ERROR__( "item=[{}] store=[{}] bag=[{}] extend=[{}] status=[{}] error!",
                           kfsetting->_id, kftypesetting->_store_name, kftypesetting->_bag_name, kftypesetting->_extend_name, status );
        }

        return kfitemrecord;
    }

    __KF_UPDATE_DATA_FUNCTION__( KFItemModule::OnItemCountUpdateCallBack )
    {
        if ( newvalue > _invalid_int )
        {
            return;
        }

        // 数量为0, 删除道具
        player->RemoveData( kfdata->GetParent()->GetParent(), key );
    }

    bool KFItemModule::CallItemLuaFunction( KFEntity* player, uint32 functiontype, const KFItemSetting* kfsetting, uint32 itemcount )
    {
        if ( kfsetting->_lua_file.empty() )
        {
            return false;
        }

        auto& luafunction = kfsetting->GetFunction( functiontype );
        if ( luafunction.empty() )
        {
            return false;
        }

        auto playerid = player->GetKeyID();
        auto result = _kf_lua->Call( kfsetting->_lua_file, luafunction, playerid, kfsetting->_id, itemcount );
        return result != 0u;
    }

    __KF_CHECK_REMOVE_ELEMENT_FUNCTION__( KFItemModule::CheckRemoveItemElement )
    {
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_data_name );
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id!", kfelement->_data_name );
            return false;
        }

        // 判断数量
        auto itemcount = kfelementobject->CalcValue( kfparent->_class_setting, __KF_STRING__( count ), multiple );
        if ( itemcount == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] count = 0!", kfelementobject->_config_id );
            return false;
        }

        auto kfsetting = KFItemConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] setting = nullptr!", kfelementobject->_config_id );
            return false;
        }

        kfparent = FindItemRecord( player, kfsetting, 0u );
        if ( kfparent == nullptr )
        {
            return false;
        }

        // 累计数量
        auto totalcount = 0u;
        for ( auto kfitem = kfparent->FirstData(); kfitem != nullptr; kfitem = kfparent->NextData() )
        {
            auto itemid = kfitem->GetValue( kfparent->_data_setting->_config_key_name );
            if ( itemid != kfsetting->_id )
            {
                continue;
            }

            totalcount += kfitem->GetValue<uint32>( __KF_STRING__( count ) );
            if ( totalcount >= itemcount )
            {
                return true;
            }
        }

        return false;
    }

    __KF_REMOVE_ELEMENT_FUNCTION__( KFItemModule::RemoveItemElement )
    {
        if ( !kfelement->IsObject() )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_data_name );
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id!", kfelement->_data_name );
        }

        auto itemcount = kfelementobject->CalcValue( kfparent->_class_setting, __KF_STRING__( count ), multiple );
        if ( itemcount == _invalid_int )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] count = 0!", kfelementobject->_config_id );
        }

        auto kfsetting = KFItemConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            return;
        }

        kfparent = FindItemRecord( player, kfsetting, 0u );
        if ( kfparent == nullptr )
        {
            return;
        }

        std::list< KFData* > finditem;
        kfparent->FindData( kfparent->_data_setting->_config_key_name, kfelementobject->_config_id, finditem, true );
        for ( auto kfitem : finditem )
        {
            auto removecount = __MIN__( itemcount, kfitem->GetValue<uint32>( __KF_STRING__( count ) ) );
            player->UpdateData( kfitem, __KF_STRING__( count ), KFEnum::Dec, removecount );
            itemcount -= removecount;
            if ( itemcount == _invalid_int )
            {
                break;
            }
        }
    }

    __KF_ADD_DATA_FUNCTION__( KFItemModule::OnAddItemCallBack )
    {
        // 保存格子信息
        RemoveItemEmptyIndex( player, kfdata );

        auto itemtime = kfdata->GetValue< uint64 >();
        if ( itemtime != 0u )
        {
            StartItemCheckTimer( player, kfdata );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFItemModule::OnItemTimeUpdateCallBack )
    {
        if ( newvalue == 0u )
        {
            // 如果变成永久道具, 取消定时器
            __UN_TIMER_2__( player->GetKeyID(), key );
        }
        else
        {
            // 更新定时器
            StartItemCheckTimer( player, kfdata->GetParent() );
        }
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFItemModule::OnEnterItemModule )
    {
        for ( auto& dataname : _item_data_list )
        {
            auto kfitemrecord = player->GetData()->FindData( dataname );
            for ( auto kfitem = kfitemrecord->FirstData(); kfitem != nullptr; kfitem = kfitemrecord->NextData() )
            {
                auto time = kfitem->GetValue<uint64>( __KF_STRING__( time ) );
                if ( time != 0u )
                {
                    StartItemCheckTimer( player, kfitem );
                }
            }

            InitItemEmptyIndexData( player, kfitemrecord );
        }
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFItemModule::OnLeaveItemModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
        for ( auto& dataname : _item_data_list )
        {
            UnInitItemEmptyIndexData( player, dataname );
        }
    }

    void KFItemModule::StartItemCheckTimer( KFEntity* player, KFData* kfitem )
    {
        auto intervaltime = 1000u;
        auto nowtime = KFGlobal::Instance()->_real_time;
        auto itemtime = kfitem->GetValue<uint64>( __KF_STRING__( time ) );
        if ( itemtime > nowtime )
        {
            intervaltime = ( itemtime - nowtime + 1 ) * 1000;
        }

        __LIMIT_TIMER_2__( player->GetKeyID(), kfitem->GetKeyID(), intervaltime, 1, &KFItemModule::OnTimerRemoveTimeItem );
    }

    __KF_TIMER_FUNCTION__( KFItemModule::OnTimerRemoveTimeItem )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        for ( auto& dataname : _item_data_list )
        {
            auto ok = player->RemoveData( dataname, subid );
            if ( ok )
            {
                break;
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleRemoveItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRemoveItemReq );

        __LOG_INFO__( "player=[{}] remove item=[{}:{}]", playerid, kfmsg.sourcename(), kfmsg.uuid() );

        if ( kfmsg.uuid() == 0u )
        {
            player->RemoveData( kfmsg.sourcename() );
        }
        else
        {
            player->RemoveData( kfmsg.sourcename(), kfmsg.uuid() );
        }
    }

    __KF_REMOVE_DATA_FUNCTION__( KFItemModule::OnRemoveItemCallBack )
    {
        // 清空索引
        AddItemEmptyIndex( player, kfdata );

        auto itemid = kfdata->GetValue( kfdata->_data_setting->_config_key_name );
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return;
        }

        auto count = kfdata->GetValue<uint32>( __KF_STRING__( count ) );
        CallItemLuaFunction( player, KFItemEnum::RemoveFunction, kfsetting, __MAX__( count, 1 ) );
    }

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleSplitItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSplitItemReq );

        auto kfobject = player->GetData();
        auto kfsourcerecord = kfobject->FindData( kfmsg.sourcename() );
        auto kftargetrecord = kfobject->FindData( kfmsg.targetname() );
        if ( kfsourcerecord == nullptr || kftargetrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        if ( kftargetrecord->IsFull() )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagFull );
        }

        auto kfsourceitem = kfsourcerecord->FindData( kfmsg.sourceuuid() );
        if ( kfsourceitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        auto count = kfsourceitem->GetValue<uint32>( __KF_STRING__( count ) );
        if ( count <= kfmsg.sourcecount() || kfmsg.sourcecount() == 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemSplitCountError );
        }

        // 判断是否限制
        auto itemid = kfsourceitem->GetValue<uint32>( kfsourceitem->_data_setting->_config_key_name );
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist );
        }

        if ( !CheckItemCanMove( kfsetting, kfmsg.sourcename(), kfmsg.targetname() ) )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotStore );
        }

        // 索引判断
        auto index = kfmsg.targetindex();
        if ( kftargetrecord->_data_setting->_int_logic_value == __USE_ITEM_INDEX__ )
        {
            // 同背包不能相同的索引
            if ( kfmsg.sourcename() == kfmsg.targetname() )
            {
                auto sourceindex = kfsourceitem->GetValue<uint32>( __KF_STRING__( index ) );
                if ( sourceindex == index )
                {
                    return;
                }
            }

            if ( index != 0u )
            {
                if ( index > kftargetrecord->_data_setting->_int_max_value )
                {
                    return _kf_display->SendToClient( player, KFMsg::ItemIndexError );
                }
            }
            else
            {
                index = FindItemEmptyIndex( player, kftargetrecord );
            }
        }

        // 扣除数量
        player->UpdateData( kfsourceitem, __KF_STRING__( count ), KFEnum::Dec, kfmsg.sourcecount() );

        auto kftargetitem = _kf_kernel->CreateObject( kfsourceitem->_data_setting );
        kftargetitem->CopyFrom( kfsourceitem );
        kftargetitem->SetValue( __KF_STRING__( index ), index );
        kftargetitem->SetValue( __KF_STRING__( count ), kfmsg.sourcecount() );

        // 添加新的道具
        auto uuid = KFGlobal::Instance()->STMakeUUID( __KF_STRING__( item ) );
        player->AddData( kftargetrecord, uuid, kftargetitem );
    }

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleMergeItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgMergeItemReq );

        auto kfobject = player->GetData();
        auto kfsourcerecord = kfobject->FindData( kfmsg.sourcename() );
        auto kftargetrecord = kfobject->FindData( kfmsg.targetname() );
        if ( kfsourcerecord == nullptr || kftargetrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        auto kfsourceitem = kfsourcerecord->FindData( kfmsg.sourceuuid() );
        auto kftargetitem = kftargetrecord->FindData( kfmsg.targetuuid() );
        if ( kfsourceitem == nullptr || kftargetitem == nullptr || kfsourceitem == kftargetitem )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        auto sourceid = kfsourceitem->GetValue<uint32>( kfsourceitem->_data_setting->_config_key_name );
        auto targetid = kftargetitem->GetValue<uint32>( kftargetitem->_data_setting->_config_key_name );
        if ( sourceid != targetid )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemMergeIdError );
        }

        auto kfsetting = KFItemConfig::Instance()->FindSetting( sourceid );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist, sourceid );
        }

        auto targetcount = kftargetitem->GetValue<uint32>( __KF_STRING__( count ) );
        if ( targetcount >= kfsetting->_overlay_count )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemOverlayCountMax );
        }

        auto sourcecount = kfsourceitem->GetValue<uint32>( __KF_STRING__( count ) );
        auto canaddcount = CalcItemAddCount( sourcecount, targetcount, kfsetting->_overlay_count );

        player->UpdateData( kfsourceitem, __KF_STRING__( count ), KFEnum::Dec, canaddcount );
        player->UpdateData( kftargetitem, __KF_STRING__( count ), KFEnum::Add, canaddcount );

        if ( kfmsg.showclient() )
        {
            AddItemDataToShow( player, kfsetting->_id, canaddcount );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleMoveItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgMoveItemReq );

        auto kfobject = player->GetData();
        auto kfsourcerecord = kfobject->FindData( kfmsg.sourcename() );
        auto kftargetrecord = kfobject->FindData( kfmsg.targetname() );
        if ( kfsourcerecord == nullptr || kftargetrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        auto kfsourceitem = kfsourcerecord->FindData( kfmsg.sourceuuid() );
        if ( kfsourceitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        // 判断是否限制
        auto itemid = kfsourceitem->GetValue<uint32>( kfsourceitem->_data_setting->_config_key_name );
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist );
        }

        if ( !CheckItemCanMove( kfsetting, kfmsg.sourcename(), kfmsg.targetname() ) )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotStore );
        }

        auto itemcount = kfsourceitem->GetValue<uint32>( __KF_STRING__( count ) );
        auto result = MoveItem( player, kfsourcerecord, kfsourceitem, kfmsg.sourceuuid(), kftargetrecord, kfmsg.targetindex() );
        if ( result == KFMsg::Ok )
        {
            if ( kfmsg.showclient() )
            {
                AddItemDataToShow( player, kfsetting->_id, itemcount );
            }
        }
        else
        {
            _kf_display->SendToClient( player, result );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleMoveAllItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgMoveAllItemReq );

        auto kfobject = player->GetData();
        auto kfsourcerecord = kfobject->FindData( kfmsg.sourcename() );
        auto kftargetrecord = kfobject->FindData( kfmsg.targetname() );
        if ( kfsourcerecord == nullptr || kftargetrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        // 遍历出可以移动的道具
        std::list< std::tuple< KFData*, const KFItemSetting* > > movelist;
        for ( auto kfitem = kfsourcerecord->FirstData(); kfitem != nullptr; kfitem = kfsourcerecord->NextData() )
        {
            auto itemid = kfitem->GetValue<uint32>( kfitem->_data_setting->_config_key_name );
            auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
            if ( kfsetting == nullptr )
            {
                continue;
            }

            auto ok = CheckItemCanMove( kfsetting, kfmsg.sourcename(), kfmsg.targetname() );
            if ( !ok )
            {
                continue;
            }

            movelist.push_back( std::make_tuple( kfitem, kfsetting ) );
        }

        for ( auto& tupledata : movelist )
        {
            KFData* kfsourceitem = nullptr;
            const KFItemSetting* kfsetting = nullptr;
            std::tie( kfsourceitem, kfsetting ) = tupledata;
            auto itemcount = MoveItemData( player, kfsourcerecord, kfsourceitem, kftargetrecord, kfsetting );
            if ( itemcount != 0u && kfmsg.showclient() )
            {
                AddItemDataToShow( player, kfsetting->_id, itemcount );
            }
        }
    }

    uint32 KFItemModule::MoveItem( KFEntity* player, KFData* kfsourcerecord, KFData* kfsourceitem, uint64 sourceuuid, KFData* kftargetrecord, uint32 targetindex )
    {
        if ( kftargetrecord->IsFull() )
        {
            return KFMsg::ItemBagFull;
        }

        // 索引判断
        if ( kftargetrecord->_data_setting->_int_logic_value == __USE_ITEM_INDEX__ )
        {
            // 同背包不能相同的索引
            if ( targetindex != 0u )
            {
                if ( targetindex > kftargetrecord->_data_setting->_int_max_value )
                {
                    return KFMsg::ItemIndexError;
                }

                // 判断该索引是否有道具
                std::list< KFData* > itemlist;
                kftargetrecord->FindData( __KF_STRING__( index ), targetindex, itemlist, false );
                if ( !itemlist.empty() )
                {
                    return KFMsg::ItemMoveFailed;
                }
            }
            else
            {
                targetindex = FindItemEmptyIndex( player, kftargetrecord );
            }

            // 原背包索引
            AddItemEmptyIndex( player, kfsourceitem );
            if ( kfsourcerecord == kftargetrecord )
            {
                player->UpdateData( kfsourceitem, __KF_STRING__( index ), KFEnum::Set, targetindex );
                RemoveItemEmptyIndex( player, kfsourceitem );
                return KFMsg::Ok;
            }

            // 设置新的索引
            kfsourceitem->SetValue( __KF_STRING__( index ), targetindex );
        }

        // 移动
        auto kfitem = player->MoveData( kfsourcerecord, sourceuuid, kftargetrecord );
        if ( kfitem != nullptr )
        {
            RemoveItemEmptyIndex( player, kfitem );
        }

        return KFMsg::Ok;
    }


    uint32 KFItemModule::MoveItemData( KFEntity* player, KFData* kfsourcerecord, KFData* kfsourceitem, KFData* kftargetrecord, const KFItemSetting* kfsetting )
    {
        auto totalmovecount = 0u;
        auto sourcecount = kfsourceitem->GetValue<uint32>( __KF_STRING__( count ) );

        // 判断是否能堆叠
        if ( !kfsetting->IsOverlay() )
        {
            // 不能堆叠, 直接移动
            auto result = MoveItem( player, kfsourcerecord, kfsourceitem, kfsourceitem->GetKeyID(), kftargetrecord, 0u );
            if ( result == KFMsg::Ok )
            {
                totalmovecount = sourcecount;
            }
        }
        else
        {
            // 找到可以堆叠的道具
            std::list< KFData* > outlist;
            kftargetrecord->FindData( kfsourceitem->_data_setting->_config_key_name, kfsetting->_id, outlist, true );

            // 堆叠起来
            for ( auto kftargetitem : outlist )
            {
                auto targetcount = kftargetitem->GetValue<uint32>( __KF_STRING__( count ) );
                auto movecount = CalcItemAddCount( sourcecount, targetcount, kfsetting->_overlay_count );

                player->UpdateData( kfsourceitem, __KF_STRING__( count ), KFEnum::Dec, movecount );
                player->UpdateData( kftargetitem, __KF_STRING__( count ), KFEnum::Add, movecount );

                sourcecount -= movecount;
                totalmovecount += movecount;
                if ( sourcecount == 0u )
                {
                    break;
                }
            }

            if ( sourcecount > 0u )
            {
                // 剩下的没有合并完, 直接移动
                auto result = MoveItem( player, kfsourcerecord, kfsourceitem, kfsourceitem->GetKeyID(), kftargetrecord, 0u );
                if ( result == KFMsg::Ok )
                {
                    totalmovecount += sourcecount;
                }
            }
        }

        return totalmovecount;
    }

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleExchangeItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgExchangeItemReq );

        auto kfobject = player->GetData();
        auto kfsourcerecord = kfobject->FindData( kfmsg.sourcename() );
        auto kftargetrecord = kfobject->FindData( kfmsg.targetname() );
        if ( kfsourcerecord == nullptr || kftargetrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        auto kfsourceitem = kfsourcerecord->FindData( kfmsg.sourceuuid() );
        auto kftargetitem = kftargetrecord->FindData( kfmsg.targetuuid() );
        if ( kfsourceitem == nullptr || kftargetitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        auto sourceindex = kfsourceitem->GetValue<uint32>( __KF_STRING__( index ) );
        auto targetindex = kftargetitem->GetValue<uint32>( __KF_STRING__( index ) );

        if ( kfmsg.sourcename() == kfmsg.targetname() )
        {
            // 交换索引
            player->UpdateData( kfsourceitem, __KF_STRING__( index ), KFEnum::Set, targetindex );
            player->UpdateData( kftargetitem, __KF_STRING__( index ), KFEnum::Set, sourceindex );
        }
        else
        {
            // 判断存放是否限制
            {
                auto itemid = kfsourceitem->GetValue<uint32>( kfsourceitem->_data_setting->_config_key_name );
                auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
                if ( kfsetting == nullptr )
                {
                    return _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist );
                }

                if ( !CheckItemCanMove( kfsetting, kfmsg.sourcename(), kfmsg.targetname() ) )
                {
                    return _kf_display->SendToClient( player, KFMsg::ItemCanNotStore );
                }
            }

            // 判断存放是否限制
            {
                auto itemid = kftargetitem->GetValue<uint32>( kftargetitem->_data_setting->_config_key_name );
                auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
                if ( kfsetting == nullptr )
                {
                    return _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist );
                }

                if ( !CheckItemCanMove( kfsetting, kfmsg.targetname(), kfmsg.sourcename() ) )
                {
                    return _kf_display->SendToClient( player, KFMsg::ItemCanNotStore );
                }
            }

            // 先空出索引
            AddItemEmptyIndex( player, kfsourceitem );
            AddItemEmptyIndex( player, kftargetitem );

            // 交换
            kfsourceitem->SetValue( __KF_STRING__( index ), targetindex );
            kfsourceitem = player->MoveData( kfsourcerecord, kfmsg.sourceuuid(), kftargetrecord );
            RemoveItemEmptyIndex( player, kfsourceitem );

            kftargetitem->SetValue( __KF_STRING__( index ), sourceindex );
            kftargetitem = player->MoveData( kftargetrecord, kfmsg.targetuuid(), kfsourcerecord );
            RemoveItemEmptyIndex( player, kftargetitem );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleCleanItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgCleanItemReq );

        CleanItem( player, kfmsg.sourcename() );
    }

    void KFItemModule::CleanItem( KFEntity* player, const std::string& name )
    {
        auto kfobject = player->GetData();
        auto kfsourcerecord = kfobject->FindData( name );
        if ( kfsourcerecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        // 遍历出需要自动清空的道具
        std::list< std::tuple< KFData*, const KFItemSetting*, const KFItemTypeSeting* > > cleanlist;
        for ( auto kfitem = kfsourcerecord->FirstData(); kfitem != nullptr; kfitem = kfsourcerecord->NextData() )
        {
            auto itemid = kfitem->GetValue<uint32>( kfitem->_data_setting->_config_key_name );
            auto kfitemsetting = KFItemConfig::Instance()->FindSetting( itemid );
            if ( kfitemsetting == nullptr )
            {
                continue;
            }

            auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfitemsetting->_type );
            if ( kftypesetting == nullptr || !kftypesetting->_is_auto_store )
            {
                continue;
            }

            cleanlist.push_back( std::make_tuple( kfitem, kfitemsetting, kftypesetting ) );
        }

        for ( auto& tupledata : cleanlist )
        {
            KFData* kfsourceitem = nullptr;
            const KFItemSetting* kfitemsetting = nullptr;
            const KFItemTypeSeting* kftypesetting = nullptr;
            std::tie( kfsourceitem, kfitemsetting, kftypesetting ) = tupledata;
            auto kftargetrecord = kfobject->FindData( kftypesetting->_store_name );
            if ( kftargetrecord != nullptr )
            {
                MoveItemData( player, kfsourcerecord, kfsourceitem, kftargetrecord, kfitemsetting );
            }
        }
    }

    std::tuple<KFData*, uint32> FindMaxCountItem( const KFItemSetting* kfsetting, std::set< KFData* >& itemlist )
    {
        auto maxcount = 0u;
        KFData* kffind = nullptr;

        for ( auto kfitem : itemlist )
        {
            auto count = kfitem->GetValue<uint32>( __KF_STRING__( count ) );
            if ( count < kfsetting->_overlay_count && count > maxcount )
            {
                kffind = kfitem;
                maxcount = count;
            }
        }

        return std::make_tuple( kffind, maxcount );
    }

    std::tuple<KFData*, uint32> FindMinCountItem( const KFItemSetting* kfsetting, std::set< KFData* >& itemlist )
    {
        auto mincount = __MAX_UINT32__;
        KFData* kffind = nullptr;

        for ( auto kfitem : itemlist )
        {
            auto count = kfitem->GetValue<uint32>( __KF_STRING__( count ) );
            if ( count < kfsetting->_overlay_count && count < mincount )
            {
                kffind = kfitem;
                mincount = count;
            }
        }

        return std::make_tuple( kffind, mincount );
    }

    KFData* PopMaxCountItem( const KFItemSetting* kfsetting, std::set< KFData* >& itemlist )
    {
        auto maxcount = 0u;
        KFData* kffind = nullptr;

        for ( auto kfitem : itemlist )
        {
            auto count = kfitem->GetValue<uint32>( __KF_STRING__( count ) );
            if ( count >= kfsetting->_overlay_count  )
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

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleSortItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSortItemReq );

        for ( auto i = 0; i < kfmsg.sourcename_size(); ++i )
        {
            SortItem( player, kfmsg.sourcename( i ) );
        }

        _kf_display->SendToClient( player, KFMsg::ItemSortOk );
    }

    void KFItemModule::SortItem( KFEntity* player, const std::string& name )
    {
        auto kfobject = player->GetData();
        auto kfitemrecord = kfobject->FindData( name );
        if ( kfitemrecord == nullptr || kfitemrecord->_data_setting->_int_logic_value != __USE_ITEM_INDEX__ )
        {
            return;
        }

        // 重置格子数量
        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Create( key );
        kfindex->InitMaxIndex( kfitemrecord->_data_setting->_int_max_value );

        std::map<uint32, std::set<KFData*>> sortlist;
        for ( auto kfitem = kfitemrecord->FirstData(); kfitem != nullptr; kfitem = kfitemrecord->NextData() )
        {
            auto id = kfitem->GetValue<uint32>( kfitemrecord->_data_setting->_config_key_name );
            sortlist[ id ].insert( kfitem );
        }

        for ( auto& miter : sortlist )
        {
            auto kfsetting = KFItemConfig::Instance()->FindSetting( miter.first );
            if ( kfsetting == nullptr )
            {
                continue;
            }

            if ( kfsetting->IsOverlay() )
            {
                while ( true )
                {
                    auto mincount = 0u;
                    KFData* minitem = nullptr;
                    std::tie( minitem, mincount ) = FindMinCountItem( kfsetting, miter.second );
                    if ( minitem == nullptr )
                    {
                        break;
                    }

                    auto maxcount = 0u;
                    KFData* maxitem = nullptr;
                    std::tie( maxitem, maxcount ) = FindMaxCountItem( kfsetting, miter.second );
                    if ( maxitem == nullptr )
                    {
                        break;
                    }

                    if ( maxitem == minitem )
                    {
                        break;
                    }

                    auto canaddcount = kfsetting->_overlay_count - maxcount;
                    canaddcount = __MIN__( mincount, canaddcount );

                    player->UpdateData( maxitem, __KF_STRING__( count ), KFEnum::Add, canaddcount );
                    player->UpdateData( minitem, __KF_STRING__( count ), KFEnum::Dec, canaddcount );
                    if ( canaddcount == mincount )
                    {
                        miter.second.erase( minitem );
                    }
                }
            }

            while ( true )
            {
                auto kfitem = PopMaxCountItem( kfsetting, miter.second );
                if ( kfitem == nullptr )
                {
                    break;
                }

                auto index = kfindex->FindEmpty();
                player->UpdateData( kfitem, __KF_STRING__( index ), KFEnum::Set, index );

            }
        }
    }

    uint32 KFItemModule::CalcItemAddCount( uint32 sourcecount, uint32 targetcount, uint32 maxcount )
    {
        if ( targetcount >= maxcount )
        {
            return 0u;
        }

        auto canaddcount = maxcount - targetcount;
        return sourcecount <= canaddcount ? sourcecount : canaddcount;
    }

    bool KFItemModule::CheckItemCanMove( const KFItemSetting* kfsetting, const std::string& sourcename, const std::string& targetname )
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

        // 额外的背包, 并且是探索背包
        if ( sourcename == kftypesetting->_extend_name && targetname == kftypesetting->_bag_name )
        {
            return true;
        }

        return kftypesetting->_move_name_list.find( targetname ) != kftypesetting->_move_name_list.end();
    }

    void KFItemModule::InitItemEmptyIndexData( KFEntity* player, KFData* kfitemrecord )
    {
        if ( kfitemrecord->_data_setting->_int_logic_value != __USE_ITEM_INDEX__ )
        {
            return;
        }

        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Create( key );
        kfindex->InitMaxIndex( kfitemrecord->_data_setting->_int_max_value );

        std::list< KFData* > invalid;
        for ( auto kfitem = kfitemrecord->FirstData(); kfitem != nullptr; kfitem = kfitemrecord->NextData() )
        {
            auto index = kfitem->GetValue<uint32>( __KF_STRING__( index ) );
            if ( index == 0u )
            {
                invalid.push_back( kfitem );
            }
            else
            {
                kfindex->RemoveEmpty( index );
            }
        }

        // 如果存在没有索引的情况, 纠正数据
        if ( !invalid.empty() )
        {
            for ( auto kfitem : invalid )
            {
                auto index = kfindex->FindEmpty();
                if ( index == 0u )
                {
                    break;
                }

                kfitem->SetValue( __KF_STRING__( index ), index );
            }
        }
    }

    void KFItemModule::UnInitItemEmptyIndexData( KFEntity* player, const std::string& name )
    {
        ItemIndexKey key( player->GetKeyID(), name );
        _player_item_index.Remove( key );
    }

    uint32 KFItemModule::FindItemEmptyIndex( KFEntity* player, KFData* kfitemrecord )
    {
        if ( kfitemrecord->_data_setting->_int_logic_value != __USE_ITEM_INDEX__ ||
                kfitemrecord->IsFull() )
        {
            return 0u;
        }

        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Find( key );
        return kfindex->FindEmpty();
    }

    void KFItemModule::RemoveItemEmptyIndex( KFEntity* player, KFData* kfitem )
    {
        auto kfitemrecord = kfitem->GetParent();
        if ( kfitemrecord->_data_setting->_int_logic_value != __USE_ITEM_INDEX__ )
        {
            return;
        }

        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Find( key );

        auto index = kfitem->GetValue<uint32>( __KF_STRING__( index ) );
        kfindex->RemoveEmpty( index );
    }

    void KFItemModule::AddItemEmptyIndex( KFEntity* player, KFData* kfitem )
    {
        auto kfitemrecord = kfitem->GetParent();
        if ( kfitemrecord->_data_setting->_int_logic_value != __USE_ITEM_INDEX__ )
        {
            return;
        }

        ItemIndexKey key( player->GetKeyID(), kfitemrecord->_data_setting->_name );
        auto kfindex = _player_item_index.Find( key );

        auto index = kfitem->GetValue<uint32>( __KF_STRING__( index ) );
        kfindex->AddEmpty( index );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}