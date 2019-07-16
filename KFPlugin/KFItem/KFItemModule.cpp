#include "KFItemModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFItemModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_item_config, true );
    }

    void KFItemModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->RegisterAddDataFunction( __KF_STRING__( item ), this, &KFItemModule::OnAddItemCallBack );
        _kf_component->RegisterRemoveDataFunction( __KF_STRING__( item ), this, &KFItemModule::OnRemoveItemCallBack );
        _kf_component->RegisterUpdateDataFunction( __KF_STRING__( item ), __KF_STRING__( count ), this, &KFItemModule::OnItemCountUpdateCallBack );
        //////////////////////////////////////////////////////////////////////////////////////////////////

        _kf_component->RegisterCheckAddElementFunction( __KF_STRING__( item ), this, &KFItemModule::CheckAddItemElement );
        _kf_component->RegisterAddElementFunction( __KF_STRING__( item ), this, &KFItemModule::AddItemElement );
        _kf_component->RegisterCheckRemoveElementFunction( __KF_STRING__( item ), this, &KFItemModule::CheckRemoveItemElement );
        _kf_component->RegisterRemoveElementFunction( __KF_STRING__( item ), this, &KFItemModule::RemoveItemElement );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        _kf_player->RegisterEnterFunction( this, &KFItemModule::OnEnterItemModule );
        _kf_player->RegisterLeaveFunction( this, &KFItemModule::OnLeaveItemModule );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_SPLIT_ITEM_REQ, &KFItemModule::HandleSplitItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_MERGE_ITEM_REQ, &KFItemModule::HandleMergeItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_MOVE_ITEM_REQ, &KFItemModule::HandleMoveItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_USE_ITEM_REQ, &KFItemModule::HandleUseItemReq );
    }

    void KFItemModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __KF_REMOVE_CONFIG__( _kf_item_config );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        _kf_component->UnRegisterAddDataFunction( this, __KF_STRING__( item ) );
        _kf_component->UnRegisterRemoveDataFunction( this, __KF_STRING__( item ) );
        _kf_component->UnRegisterUpdateDataFunction( this, __KF_STRING__( item ), __KF_STRING__( count ) );

        _kf_component->UnRegisterCheckAddElementFunction( __KF_STRING__( item ) );
        _kf_component->UnRegisterAddElementFunction( __KF_STRING__( item ) );
        _kf_component->UnRegisterCheckRemoveElementFunction( __KF_STRING__( item ) );
        _kf_component->UnRegisterRemoveElementFunction( __KF_STRING__( item ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        _kf_player->UnRegisterEnterFunction( this );
        _kf_player->UnRegisterLeaveFunction( this );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_SPLIT_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_MERGE_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_MOVE_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_USE_ITEM_REQ );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFItemModule::IsItemBagFull( KFData* kfitemrecord )
    {
        auto _option = _kf_option->FindOption( "maxitemcount", kfitemrecord->_data_setting->_name );

        auto count = kfitemrecord->Size();
        return count >= _option->_uint32_value;
    }

    __KF_CHECK_ADD_ELEMENT_FUNCTION__( KFItemModule::CheckAddItemElement )
    {
        if ( !kfelement->IsObject() )
        {
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        auto kfsetting = _kf_item_config->FindSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            return false;
        }

        // 放入背包的类型
        if ( !kfsetting->_add_type.empty() && kfsetting->_add_type != kfparent->_data_setting->_name )
        {
            kfparent = player->GetData()->FindData( kfsetting->_add_type );
            if ( kfparent == nullptr )
            {
                return false;
            }
        }

        return !IsItemBagFull( kfparent );
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

        auto kfsetting = _kf_item_config->FindSetting( kfelementobject->_config_id );
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

        // 放入背包的类型
        if ( !kfsetting->_add_type.empty() && kfsetting->_add_type != kfparent->_data_setting->_name )
        {
            kfparent = player->GetData()->FindData( kfsetting->_add_type );
            if ( kfparent == nullptr )
            {
                __LOG_ERROR_FUNCTION__( function, line, "item addtype=[{}] no data!", kfsetting->_add_type );
                return std::make_tuple( KFDataDefine::Show_None, nullptr );
            }
        }

        // 添加道具
        if ( kfsetting->_overlay_type == KFItemEnum::OverlayByTime )
        {
            // 时间叠加道具
            // 获得时间
            auto itemtime = kfelementobject->GetValue( kfparent->_class_setting, __KF_STRING__( time ), multiple );
            if ( itemtime == 0 )
            {
                __LOG_ERROR_FUNCTION__( function, line, "time item id=[{}] count = 0!", kfelementobject->_config_id );
                return std::make_tuple( KFDataDefine::Show_None, nullptr );
            }

            itemtime *= itemcount;
            AddOverlayTimeItem( player, kfparent, kfelementobject, kfsetting, itemtime );
        }
        else if ( kfsetting->_overlay_type == KFItemEnum::OverlayByCount )
        {
            // 数量叠加道具
            do
            {
                itemcount = AddOverlayCountItem( player, kfparent, kfelementobject, kfsetting, itemcount );
            } while ( itemcount > _invalid_int );
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
            AddNewTimeItem( player, kfparent, kfelementobject, kfsetting, time );
        }
    }

    void KFItemModule::AddNewTimeItem( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint32 time )
    {
        auto datasetting = kfparent->_data_setting;
        auto kfitem = _kf_kernel->CreateObject( datasetting );

        // count
        kfitem->SetValue( __KF_STRING__( count ), 1u );

        // id
        kfitem->SetValue( datasetting->_config_key_name, kfsetting->_id );

        // time
        kfitem->SetValue( __KF_STRING__( time ), KFGlobal::Instance()->_real_time + time );

        // uuid
        auto uuid = KFGlobal::Instance()->MakeUUID( KFMsg::UUidItem );

        // 添加道具
        player->AddData( kfparent, uuid, kfitem );
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
        return AddNewCountItem( player, kfparent, kfelementobject, kfsetting, count );
    }

    uint32 KFItemModule::AddNewCountItem( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint32 count )
    {
        auto datasetting = kfparent->_data_setting;
        auto kfitem = _kf_kernel->CreateObject( datasetting );

        // count
        auto addcount = __MIN__( count, kfsetting->_overlay_count );
        kfitem->SetValue( __KF_STRING__( count ), addcount );

        // id
        kfitem->SetValue( datasetting->_config_key_name, kfsetting->_id );

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
        auto itemtime = kfdata->GetValue< uint64 >();
        if ( itemtime != 0u )
        {
            StartItemCheckTimer( player );
        }
    }

    void KFItemModule::OnEnterItemModule( KFEntity* player )
    {
        StartItemCheckTimer( player );
    }

    void KFItemModule::StartItemCheckTimer( KFEntity* player )
    {
        KFData* kftimeitem = nullptr;
        uint64 mintime = __MAX_UINT64__;

        auto kfitemrecord = player->GetData()->FindData( __KF_STRING__( item ) );
        for ( auto kfitem = kfitemrecord->FirstData(); kfitem != nullptr; kfitem = kfitemrecord->NextData() )
        {
            auto time = kfitem->GetValue<uint64>( __KF_STRING__( time ) );
            if ( time != 0u && time < mintime )
            {
                mintime = time;
                kftimeitem = kfitem;
            }
        }

        auto kfstoragerecord = player->GetData()->FindData( __KF_STRING__( storage ) );
        if ( kfstoragerecord != nullptr )
        {
            for ( auto kfitem = kfstoragerecord->FirstData(); kfitem != nullptr; kfitem = kfstoragerecord->NextData() )
            {
                auto time = kfitem->GetValue<uint64>( __KF_STRING__( time ) );
                if ( time != 0u && time < mintime )
                {
                    mintime = time;
                    kftimeitem = kfitem;
                }
            }
        }

        if ( kftimeitem != nullptr )
        {
            // 启动定时器
            auto intervaltime = 1u;
            auto nowtime = KFGlobal::Instance()->_real_time;
            if ( mintime > nowtime )
            {
                intervaltime = ( mintime - nowtime + 1 ) * 1000;
            }
            __LIMIT_TIMER_1__( player->GetKeyID(), intervaltime, 1, &KFItemModule::OnTimerRemoveTimeItem );
        }
    }

    __KF_TIMER_FUNCTION__( KFItemModule::OnTimerRemoveTimeItem )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        std::map< KFData*, KFData* > removes;
        auto kfitemrecord = player->GetData()->FindData( __KF_STRING__( item ) );
        for ( auto kfitem = kfitemrecord->FirstData(); kfitem != nullptr; kfitem = kfitemrecord->NextData() )
        {
            auto time = kfitem->GetValue<uint64>( __KF_STRING__( time ) );
            if ( time != 0u && time <= KFGlobal::Instance()->_real_time )
            {
                removes[ kfitem ] = kfitemrecord;
            }
        }

        auto kfstoragerecord = player->GetData()->FindData( __KF_STRING__( storage ) );
        if ( kfstoragerecord != nullptr )
        {
            for ( auto kfitem = kfstoragerecord->FirstData(); kfitem != nullptr; kfitem = kfstoragerecord->NextData() )
            {
                auto time = kfitem->GetValue<uint64>( __KF_STRING__( time ) );
                if ( time != 0u && time <= KFGlobal::Instance()->_real_time )
                {
                    removes[ kfitem ] = kfstoragerecord;
                }
            }
        }

        for ( auto& iter : removes )
        {
            player->RemoveData( iter.second, iter.first->GetKeyID() );
        }

        // 重新启动定时器
        StartItemCheckTimer( player );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFItemModule::OnRemoveItemCallBack )
    {
        auto itemid = kfdata->GetValue( kfdata->_data_setting->_config_key_name );
        auto kfsetting = _kf_item_config->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return;
        }

        auto count = kfdata->GetValue<uint32>( __KF_STRING__( count ) );
        CallItemLuaFunction( player, KFItemEnum::RemoveFunction, kfsetting, __MAX__( count, 1 ) );
    }

    void KFItemModule::OnLeaveItemModule( KFEntity* player )
    {
        __UN_TIMER_1__( player->GetKeyID() );
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

        if ( IsItemBagFull( kftargetrecord ) )
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

        // 扣除数量
        player->UpdateData( kfsourceitem, __KF_STRING__( count ), KFEnum::Dec, kfmsg.sourcecount() );

        auto kftargetitem = _kf_kernel->CreateObject( kfsourceitem->_data_setting );
        kftargetitem->CopyFrom( kfsourceitem );
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

        auto kfsetting = _kf_item_config->FindSetting( sourceid );
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

        if ( IsItemBagFull( kftargetrecord ) )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagFull );
        }

        auto ok = player->MoveData( kfsourcerecord, kfmsg.sourceuuid(), kftargetrecord );
        if ( !ok )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemMoveFailed );
        }
    }

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
        auto kfsetting = _kf_item_config->FindSetting( itemid );
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

        }

        return true;
    }

    bool KFItemModule::UseScriptItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting )
    {
        auto ok = CallItemLuaFunction( player, KFItemEnum::UseFunction, kfsetting, 1 );
        return ok;
    }
}