#include "KFItemModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFItemModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        __REGISTER_ADD_DATA__( __KF_STRING__( item ), &KFItemModule::OnAddItemCallBack );
        __REGISTER_REMOVE_DATA__( __KF_STRING__( item ), &KFItemModule::OnRemoveItemCallBack );
        __REGISTER_UPDATE_DATA_2__( __KF_STRING__( item ), __KF_STRING__( count ), &KFItemModule::OnItemCountUpdateCallBack );
        __REGISTER_UPDATE_DATA_2__( __KF_STRING__( item ), __KF_STRING__( time ), &KFItemModule::OnItemTimeUpdateCallBack );
        //////////////////////////////////////////////////////////////////////////////////////////////////
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
        __REGISTER_MESSAGE__( KFMsg::MSG_USE_ITEM_REQ, &KFItemModule::HandleUseItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_EXCHANGE_ITEM_REQ, &KFItemModule::HandleExchangeItemReq );
    }

    void KFItemModule::BeforeShut()
    {
        __UN_TIMER_0__();
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_ADD_DATA__( __KF_STRING__( item ) );
        __UN_REMOVE_DATA__( __KF_STRING__( item ) );
        __UN_UPDATE_DATA_2__( __KF_STRING__( item ), __KF_STRING__( count ) );
        __UN_UPDATE_DATA_2__( __KF_STRING__( item ), __KF_STRING__( time ) );

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
        __UN_MESSAGE__( KFMsg::MSG_USE_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_EXCHANGE_ITEM_REQ );
    }

    void KFItemModule::PrepareRun()
    {
        _item_data_list.clear();
        _item_data_list = _kf_component->GetDataList( __KF_STRING__( item ) );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

        // 计算物品数量
        auto itemcount = kfelementobject->GetValue( kfparent->_class_setting, __KF_STRING__( count ), multiple );
        if ( itemcount == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] count = 0!", kfelementobject->_config_id );
            return std::make_tuple( KFDataDefine::Show_None, nullptr );
        }

        // 添加道具
        switch ( kfsetting->_overlay_type )
        {
        case KFItemEnum::OverlayByTime:		// 时间叠加道具
        {
            auto itemtime = kfelementobject->GetValue( kfparent->_class_setting, __KF_STRING__( time ), multiple );
            if ( itemtime == 0 )
            {
                __LOG_ERROR_FUNCTION__( function, line, "time item id=[{}] count = 0!", kfelementobject->_config_id );
                return std::make_tuple( KFDataDefine::Show_None, nullptr );
            }

            itemtime *= itemcount;
            AddOverlayTimeItem( player, kfparent, kfelementobject, kfsetting, itemtime );
        }
        break;
        case KFItemEnum::OverlayByCount:	// 数量叠加道具
        {
            do
            {
                itemcount = AddOverlayCountItem( player, kfparent, kfelementobject, kfsetting, itemcount );
            } while ( itemcount > _invalid_int );
        }
        break;
        default:
            break;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CallItemLuaFunction( player, KFItemEnum::AddFunction, kfsetting, itemcount );
        return std::make_tuple( KFDataDefine::Show_Element, kfparent );
    }

    void KFItemModule::AddOverlayTimeItem( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint32 time )
    {
        std::list< KFData* > finditem;
        kfparent->FindData( kfparent->_data_setting->_config_key_name, kfsetting->_id, finditem, false );

        if ( !finditem.empty() )
        {
            auto kfitem = finditem.front();
            player->UpdateData( kfitem, __KF_STRING__( time ), KFEnum::Add, time );
        }
        else
        {
            AddNewItemData( player, kfparent, kfelementobject, kfsetting, 1u, time );
        }
    }

    uint32 KFItemModule::AddOverlayCountItem( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint32 count )
    {
        auto datasetting = kfparent->_data_setting;

        // 叠加数量 > 1
        if ( kfsetting->_overlay_count > 1u )
        {
            std::list< KFData* > finditem;
            kfparent->FindData( datasetting->_config_key_name, kfsetting->_id, finditem, true );

            // 判断是否可以叠加完
            for ( auto kfitem : finditem )
            {
                auto oldcount = kfitem->GetValue( __KF_STRING__( count ) );
                oldcount = __MIN__( oldcount, kfsetting->_overlay_count );
                auto canadd = kfsetting->_overlay_count - oldcount;
                auto addcount = __MIN__( canadd, count );
                if ( addcount == _invalid_int )
                {
                    continue;
                }

                // 更新数量
                player->UpdateData( kfitem, __KF_STRING__( count ), KFEnum::Add, addcount );

                count -= addcount;
                if ( count == _invalid_int )
                {
                    return _invalid_int;
                }
            }
        }

        // 添加新的物品
        return AddNewItemData( player, kfparent, kfelementobject, kfsetting, count, 0u );
    }

    uint32 KFItemModule::AddNewItemData( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint32 count, uint32 time )
    {
        auto datasetting = kfparent->_data_setting;
        auto kfitem = _kf_kernel->CreateObject( datasetting );

        // count
        auto addcount = __MIN__( count, kfsetting->_overlay_count );
        kfitem->SetValue( __KF_STRING__( count ), addcount );

        // id
        kfitem->SetValue( datasetting->_config_key_name, kfsetting->_id );

        // time
        if ( time != 0u )
        {
            kfitem->SetValue( __KF_STRING__( time ), KFGlobal::Instance()->_real_time + time );
        }

        // 位置
        auto index = FindItemEmptyIndex( player, kfparent );
        if ( index != 0u )
        {
            kfitem->SetValue( __KF_STRING__( index ), index );
        }

        // uuid
        auto uuid = KFGlobal::Instance()->MakeUUID( KFMsg::UUidItem );

        // 添加新的物品
        player->AddData( kfparent, uuid, kfitem );
        return count - addcount;
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
        auto itemcount = kfelementobject->GetValue( kfparent->_class_setting, __KF_STRING__( count ), multiple );
        if ( itemcount == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] count = 0!", kfelementobject->_config_id );
            return false;
        }

        // 获取item列表
        std::list< KFData* > finditem;
        kfparent->FindData( kfparent->_data_setting->_config_key_name, kfelementobject->_config_id, finditem, true );
        if ( finditem.empty() )
        {
            return false;
        }

        auto totalcount = 0u;
        for ( auto kfitem : finditem )
        {
            // 累计数量
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

        auto itemcount = kfelementobject->GetValue( kfparent->_class_setting, __KF_STRING__( count ), multiple );
        if ( itemcount == _invalid_int )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] count = 0!", kfelementobject->_config_id );
        }

        // 获取item列表
        std::list< KFData* > finditem;
        kfparent->FindData( kfparent->_data_setting->_config_key_name, kfelementobject->_config_id, finditem, true );
        if ( finditem.empty() )
        {
            return;
        }

        auto totalcount = 0u;
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

        if ( !kfsetting->IsCanStore( kfmsg.targetname() ) )
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
        auto uuid = KFGlobal::Instance()->MakeUUID( KFMsg::UUidItem );
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
        if ( kfsourceitem == nullptr || kftargetitem == nullptr )
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

        auto canaddcount = kfsetting->_overlay_count - targetcount;
        auto sourcecount = kfsourceitem->GetValue<uint32>( __KF_STRING__( count ) );
        canaddcount = __MIN__( sourcecount, canaddcount );

        player->UpdateData( kfsourceitem, __KF_STRING__( count ), KFEnum::Dec, canaddcount );
        player->UpdateData( kftargetitem, __KF_STRING__( count ), KFEnum::Add, canaddcount );
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

        if ( kftargetrecord->IsFull() )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagFull );
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

        if ( !kfsetting->IsCanStore( kfmsg.targetname() ) )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotStore );
        }

        // 索引判断
        if ( kftargetrecord->_data_setting->_int_logic_value == __USE_ITEM_INDEX__ )
        {
            // 同背包不能相同的索引
            auto index = kfmsg.targetindex();
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

            // 原背包索引
            AddItemEmptyIndex( player, kfsourceitem );

            // 设置新的索引
            kfsourceitem->SetValue( __KF_STRING__( index ), index );
        }

        auto kfitem = player->MoveData( kfsourcerecord, kfmsg.sourceuuid(), kftargetrecord );
        if ( kfitem != nullptr )
        {
            RemoveItemEmptyIndex( player, kfitem );
        }
        else
        {
            _kf_display->SendToClient( player, KFMsg::ItemMoveFailed );
        }
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

                if ( !kfsetting->IsCanStore( kfmsg.targetname() ) )
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

                if ( !kfsetting->IsCanStore( kfmsg.sourcename() ) )
                {
                    return _kf_display->SendToClient( player, KFMsg::ItemCanNotStore );
                }
            }

            AddItemEmptyIndex( player, kfsourceitem );
            AddItemEmptyIndex( player, kftargetitem );

            kfsourceitem->SetValue( __KF_STRING__( index ), targetindex );
            kfsourceitem = player->MoveData( kfsourcerecord, kfmsg.sourceuuid(), kftargetrecord );
            RemoveItemEmptyIndex( player, kfsourceitem );

            kftargetitem->SetValue( __KF_STRING__( index ), sourceindex );
            kftargetitem = player->MoveData( kftargetrecord, kfmsg.targetuuid(), kfsourcerecord );
            RemoveItemEmptyIndex( player, kftargetitem );
        }
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
    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleUseItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUseItemReq );

        // 判断是否有这个道具
        auto kfitemrecord = player->GetData()->FindData( kfmsg.name() );
        auto kfitem = kfitemrecord->FindData( kfmsg.uuid() );
        if ( kfitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        auto itemid = kfitem->GetValue<uint32>( kfitemrecord->_data_setting->_config_key_name  );
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist, itemid );
        }

        if ( !kfsetting->CheckCanUse() )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotUse );
        }

        auto ok = UseItem( player, kfitem, kfsetting );
        if ( !ok )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemUseFailed );
        }

        // 扣除数量
        auto usecount = kfitem->GetValue<uint32>( __KF_STRING__( usecount ) );
        if ( usecount + 1u >= kfsetting->_use_count )
        {
            player->UpdateData( kfitem, __KF_STRING__( count ), KFEnum::Dec, 1u );
        }
        else
        {
            player->UpdateData( kfitem, __KF_STRING__( usecount ), KFEnum::Add, 1u );
        }
    }

    bool KFItemModule::UseItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting )
    {
        switch ( kfsetting->_type )
        {
        case KFItemEnum::GiftBag:	// 礼包
            return UseGiftBagItem( player, kfitem, kfsetting );
            break;
        case KFItemEnum::Script:	// 脚本
            return UseScriptItem( player, kfitem, kfsetting );
            break;
        default:
            break;
        }

        return false;
    }

    bool KFItemModule::UseGiftBagItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting )
    {
        // 判断各类资源是否满了

        if ( !kfsetting->_rewards.IsEmpty() )
        {
            player->AddElement( &kfsetting->_rewards, true, __FUNC_LINE__ );
        }

        if ( kfsetting->_drop_id != 0u )
        {
            _kf_drop->DropElement( player, kfsetting->_drop_id, 1u, true, __FUNC_LINE__ );
        }

        return true;
    }

    bool KFItemModule::UseScriptItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting )
    {
        auto ok = CallItemLuaFunction( player, KFItemEnum::UseFunction, kfsetting, 1u );
        return ok;
    }
}