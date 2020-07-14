#include "KFItemModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFItemModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_GET_CONFIG_VALUE__( __STRING__( item ), &KFItemModule::GetItemTotalCount );
        __REGISTER_ADD_DATA_1__( __STRING__( item ), &KFItemModule::OnAddItemCallBack );
        __REGISTER_REMOVE_DATA_1__( __STRING__( item ), &KFItemModule::OnRemoveItemCallBack );
        __REGISTER_UPDATE_DATA_2__( __STRING__( item ), __STRING__( count ), &KFItemModule::OnItemCountUpdateCallBack );
        __REGISTER_UPDATE_DATA_2__( __STRING__( item ), __STRING__( time ), &KFItemModule::OnItemTimeUpdateCallBack );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_CHECK_ADD_ELEMENT__( __STRING__( item ), &KFItemModule::CheckAddItemElement );
        __REGISTER_ADD_ELEMENT__( __STRING__( item ), &KFItemModule::AddItemElement );
        __REGISTER_CHECK_REMOVE_ELEMENT__( __STRING__( item ), &KFItemModule::CheckRemoveItemElement );
        __REGISTER_REMOVE_ELEMENT__( __STRING__( item ), &KFItemModule::RemoveItemElement );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_ENTER_PLAYER__( &KFItemModule::OnEnterItemModule );
        __REGISTER_LEAVE_PLAYER__( &KFItemModule::OnLeaveItemModule );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_REMOVE_ITEM_REQ, &KFItemModule::HandleRemoveItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REMOVE_ITEM_COUNT_REQ, &KFItemModule::HandleRemoveItemCountReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SELL_ITEM_REQ, &KFItemModule::HandleSellItemReq );
    }

    void KFItemModule::BeforeShut()
    {
        __UN_TIMER_0__();
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_GET_CONFIG_VALUE__( __STRING__( item ) );
        __UN_ADD_DATA_1__( __STRING__( item ) );
        __UN_REMOVE_DATA_1__( __STRING__( item ) );
        __UN_UPDATE_DATA_2__( __STRING__( item ), __STRING__( count ) );
        __UN_UPDATE_DATA_2__( __STRING__( item ), __STRING__( time ) );

        __UN_CHECK_ADD_ELEMENT__( __STRING__( item ) );
        __UN_ADD_ELEMENT__( __STRING__( item ) );
        __UN_CHECK_REMOVE_ELEMENT__( __STRING__( item ) );
        __UN_REMOVE_ELEMENT__( __STRING__( item ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_REMOVE_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_REMOVE_ITEM_COUNT_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SELL_ITEM_REQ );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFItemModule::BindInitItemFunction( uint32 itemtype, KFItemFunction& function )
    {
        auto kffunction = _init_item_function.Create( itemtype );
        kffunction->_function = function;
    }

    void KFItemModule::UnRegisteInitItemFunction( uint32 itemtype )
    {
        _init_item_function.Remove( itemtype );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleRemoveItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRemoveItemReq );

        __LOG_INFO__( "player=[{}] remove item=[{}:{}]", playerid, kfmsg.sourcename(), kfmsg.uuid() );
        if ( kfmsg.uuid() == 0u )
        {
            player->CleanData( kfmsg.sourcename() );
        }
        else
        {
            player->RemoveData( kfmsg.sourcename(), kfmsg.uuid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleRemoveItemCountReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRemoveItemCountReq );

        __LOG_INFO__( "player=[{}] remove itemcount=[{}:{}]", playerid, kfmsg.itemid(), kfmsg.count() );

        auto kfitemrecord = FindItemRecord( player, kfmsg.itemid() );
        RemoveItem( player, kfitemrecord, kfmsg.itemid(), kfmsg.count() );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_GET_CONFIG_VALUE_FUNCTION__( KFItemModule::GetItemTotalCount )
    {
        auto kfsetting = KFItemConfig::Instance()->FindSetting( id );
        if ( kfsetting == nullptr )
        {
            return 0u;
        }

        auto kfitemrecord = player->Find( KFItemBagConfig::Instance()->_default_bag_name );
        kfitemrecord = FindItemRecord( player, kfitemrecord, kfsetting, 0u );
        return GetItemCount( player, kfitemrecord, kfsetting->_id, maxvalue );
    }

    uint32 KFItemModule::GetItemCount( KFEntity* player, KFData* kfitemrecord, uint32 itemid, uint32 maxcount /* = __MAX_UINT32__ */ )
    {
        auto totalcount = 0u;
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            auto id = kfitem->Get<uint32>( kfitemrecord->_data_setting->_config_key_name );
            if ( itemid != id )
            {
                continue;
            }

            totalcount += kfitem->Get<uint32>( __STRING__( count ) );
            if ( totalcount >= maxcount )
            {
                break;
            }
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
        auto itemcount = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( count ), multiple );
        if ( itemcount == _invalid_int || kfelementobject->_config_id == _invalid_int )
        {
            return false;
        }

        auto kfsetting = KFItemConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            return false;
        }

        kfparent = FindItemRecord( player, kfparent, kfsetting, 0u );
        return !CheckItemRecordFull( player, kfparent, kfsetting, itemcount );
    }

    bool KFItemModule::CheckItemRecordFull( KFEntity* player, KFData* kfitemrecord, const KFItemSetting* kfsetting, uint32 itemcount )
    {
        auto maxsize = GetItemRecordMaxCount( player, kfitemrecord );
        auto maxoverlaycount = kfsetting->GetOverlayCount( kfitemrecord->_data_setting->_name );

        // 计算占用格子数
        auto cursize = kfitemrecord->Size();
        uint32 usesize = ( itemcount + maxoverlaycount - 1 ) / maxoverlaycount;
        auto leftsize = ( maxsize > cursize ) ? ( maxsize - cursize ) : 0u;
        if ( leftsize >= usesize )
        {
            return false;
        }

        if ( kfsetting->IsOverlay() )
        {
            // 剩余格子可叠加物品数
            auto canaddcount = leftsize * maxoverlaycount;
            for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
            {
                auto itemid = kfitem->Get<uint32>( kfitemrecord->_data_setting->_config_key_name );
                if ( itemid != kfsetting->_id )
                {
                    continue;
                }

                auto oldcount = kfitem->Get( __STRING__( count ) );
                canaddcount += maxoverlaycount - __MIN__( oldcount, maxoverlaycount );
                if ( canaddcount >= itemcount )
                {
                    return false;
                }
            }
        }

        return true;
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

    uint32 KFItemModule::GetCanAddItemCount( KFEntity* player, uint32 itemid, uint32 itemcount )
    {
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR__( "item=[{}] no setting", kfsetting->_id );
            return 0u;
        }

        auto kfitemrecord = FindItemRecord( player, itemid );
        if ( kfitemrecord == nullptr )
        {
            return 0u;
        }

        auto maxsize = GetItemRecordMaxCount( player, kfitemrecord );
        auto maxoverlaycount = kfsetting->GetOverlayCount( kfitemrecord->_data_setting->_name );

        // 计算占用格子数
        auto cursize = kfitemrecord->Size();
        uint32 usesize = ( itemcount + maxoverlaycount - 1 ) / maxoverlaycount;
        auto leftsize = ( maxsize > cursize ) ? ( maxsize - cursize ) : 0u;
        if ( leftsize >= usesize )
        {
            return itemcount;
        }

        // 不能叠加
        if ( !kfsetting->IsOverlay() )
        {
            return leftsize;
        }

        // 剩余格子可添加物品数
        auto canaddcount = leftsize * maxoverlaycount;
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            auto itemid = kfitem->Get<uint32>( kfitemrecord->_data_setting->_config_key_name );
            if ( itemid != kfsetting->_id )
            {
                continue;
            }

            auto oldcount = kfitem->Get( __STRING__( count ) );
            canaddcount += maxoverlaycount - __MIN__( oldcount, maxoverlaycount );
            if ( canaddcount >= itemcount )
            {
                return itemcount;
            }
        }

        return canaddcount;
    }

    uint32 KFItemModule::GetItemRecordMaxCount( KFEntity* player, KFData* kfitemrecord )
    {
        auto defaultmaxcount = kfitemrecord->_data_setting->_int_max_value;
        auto technologymaxcount = player->Get<uint32>( __STRING__( effect ), kfitemrecord->_data_setting->_name );
        return defaultmaxcount + technologymaxcount;
    }

    uint32 KFItemModule::GetItemEmptyCount( KFEntity* player, KFData* kfitemrecord )
    {
        auto maxcount = GetItemRecordMaxCount( player, kfitemrecord );
        return maxcount - kfitemrecord->Size();
    }

    bool KFItemModule::IsItemRecordFull( KFEntity* player, KFData* kfitemrecord )
    {
        auto maxcount = GetItemRecordMaxCount( player, kfitemrecord );
        return kfitemrecord->Size() >= maxcount;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_ELEMENT_FUNCTION__( KFItemModule::AddItemElement )
    {
        auto kfelement = kfresult->_element;
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfelement->_data_name );
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", kfelement->_data_name );
            return false;
        }

        auto kfsetting = KFItemConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "id=[{}] itemsetting = null", kfelementobject->_config_id );
            _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist, kfelementobject->_config_id );
            return false;
        }

        // 查找背包数据
        kfparent = FindItemRecord( player, kfparent, kfsetting, 0u );

        // 计算物品数量
        auto itemcount = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( count ), kfresult->_multiple );
        if ( itemcount == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] count = 0", kfelementobject->_config_id );
            return false;
        }

        // 添加道具调用脚本
        CallItemLuaFunction( player, KFItemEnum::AddFunction, kfsetting, itemcount );

        // 自动打开的礼包
        if ( kfsetting->IsAutoUse() )
        {
            auto kfitembagsetting = KFItemBagConfig::Instance()->FindSetting( kfparent->_data_setting->_name );
            if ( kfitembagsetting != nullptr && kfitembagsetting->_is_auto_use_gift )
            {
                AddItemResult( kfresult, kfparent->_data_setting->_name, kfsetting->_id, itemcount );
                player->AddElement( &kfsetting->_reward, itemcount, _invalid_string, _invalid_int, __FUNC_LINE__ );
                return true;
            }
        }

        // 判断堆叠类型
        if ( kfsetting->_overlay_type == KFItemEnum::OverlayByTime )
        {
            // 计算时间
            auto itemtime = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( time ), kfresult->_multiple );
            if ( itemtime == 0u )
            {
                __LOG_ERROR_FUNCTION__( function, line, "time item id=[{}] count = 0", kfelementobject->_config_id );
                return false;
            }

            AddOverlayTimeItem( player, kfparent, kfresult, kfsetting, itemcount, itemtime );
        }
        else if ( kfsetting->_overlay_type == KFItemEnum::OverlayByCount )
        {
            // 按数量叠加道具
            if ( kfsetting->IsOverlay() )
            {
                AddOverlayCountItem( player, kfparent, kfresult, kfsetting, itemcount );
            }
            else
            {
                AddNotOverlayItem( player, kfparent, kfresult, kfsetting, itemcount );
            }
        }

        return true;
    }

    void KFItemModule::AddOverlayTimeItem( KFEntity* player, KFData* kfparent, KFElementResult* kfresult, const KFItemSetting* kfsetting, uint32 count, uint32 time )
    {
        std::list< KFData* > finditem;
        kfparent->Find( kfparent->_data_setting->_config_key_name, kfsetting->_id, finditem, false );

        KFData* kfitem = nullptr;
        if ( !finditem.empty() )
        {
            kfitem = finditem.front();
            player->UpdateData( kfitem, __STRING__( time ), KFEnum::Add, count * time );
        }
        else
        {
            kfitem = AddNewItemData( player, kfparent, kfsetting, count, time );
        }

        AddItemResult( kfresult, kfsetting->_id, kfitem );
    }

    void KFItemModule::AddOverlayCountItem( KFEntity* player, KFData* kfparent, KFElementResult* kfresult, const KFItemSetting* kfsetting, uint32 count )
    {
        auto maxoverlaycount = kfsetting->GetOverlayCount( kfparent->_data_setting->_name );

        // 查找相同道具的id
        auto totaladdcount = 0u;
        for ( auto kfitem = kfparent->First(); kfitem != nullptr; kfitem = kfparent->Next() )
        {
            auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
            if ( itemid != kfsetting->_id )
            {
                continue;
            }

            auto oldcount = kfitem->Get( __STRING__( count ) );
            auto addcount = CalcItemAddCount( count, oldcount, maxoverlaycount );
            if ( addcount == 0u )
            {
                continue;
            }

            // 更新数量
            player->UpdateData( kfitem, __STRING__( count ), KFEnum::Add, addcount );
            count -= addcount;
            totaladdcount += addcount;
            if ( count == 0u )
            {
                break;
            }
        }

        // 显示添加数量
        if ( totaladdcount != 0u )
        {
            AddItemResult( kfresult, kfparent->_data_setting->_name, kfsetting->_id, totaladdcount );
        }

        // 添加完成, 直接返回
        if ( count == 0u )
        {
            return;
        }

        // 添加新物品
        do
        {
            auto kfitem = AddNewItemData( player, kfparent, kfsetting, count, 0u );
            AddItemResult( kfresult, kfitem->_data_setting->_name, kfsetting->_id, kfitem->Get<uint32>( __STRING__( count ) ) );
        } while ( count > 0u );
    }

    void KFItemModule::AddNotOverlayItem( KFEntity* player, KFData* kfparent, KFElementResult* kfresult, const KFItemSetting* kfsetting, uint32 count )
    {
        // 添加新物品
        do
        {
            auto kfitem = AddNewItemData( player, kfparent, kfsetting, count, 0u );
            AddItemResult( kfresult, kfsetting->_id, kfitem );
        } while ( count > 0u );
    }

    KFData* KFItemModule::AddNewItemData( KFEntity* player, KFData* kfparent, const KFItemSetting* kfsetting, uint32& count, uint32 time )
    {
        auto maxoverlaycount = kfsetting->GetOverlayCount( kfparent->_data_setting->_name );
        auto addcount = __MIN__( count, maxoverlaycount );

        // 判断满包情况, 需要添加另外的背包, 加入满包不显示( 测试特殊要求 )
        auto kfnewparent = FindItemRecord( player, kfparent, kfsetting, addcount );
        if ( kfnewparent != kfparent )
        {
            kfparent = kfnewparent;
            maxoverlaycount = kfsetting->GetOverlayCount( kfparent->_data_setting->_name );
        }

        auto kfitem = player->CreateData( kfparent );

        // count
        kfitem->Set( __STRING__( count ), addcount );

        // id
        kfitem->Set( kfparent->_data_setting->_config_key_name, kfsetting->_id );

        // time
        if ( time != 0u )
        {
            kfitem->Set( __STRING__( time ), KFGlobal::Instance()->_real_time + time * count );
        }

        // 初始化数据
        auto kffunction = _init_item_function.Find( kfsetting->_type );
        if ( kffunction != nullptr )
        {
            kffunction->_function( player, kfitem, kfsetting );
        }

        // uuid
        auto uuid = KFGlobal::Instance()->STMakeUuid( __STRING__( item ) );

        // 添加新的物品
        count -= addcount;
        player->AddData( kfparent, uuid, kfitem );
        return kfitem;
    }

    KFData* KFItemModule::CreateItem( KFEntity* player, uint32 itemid, KFData* kfitem, const char* function, uint32 line )
    {
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item=[{}] can't find setting", itemid );
            _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist, itemid );
            return kfitem;
        }

        if ( kfitem == nullptr )
        {
            kfitem = player->CreateData( KFItemBagConfig::Instance()->_default_bag_name );
            if ( kfitem == nullptr )
            {
                return kfitem;
            }
        }

        // count
        kfitem->Set( __STRING__( count ), 1u );

        // id
        kfitem->Set( kfitem->_data_setting->_config_key_name, kfsetting->_id );

        // uuid
        kfitem->SetKeyID( KFGlobal::Instance()->STMakeUuid( __STRING__( item ) ) );

        // 初始化数据
        auto kffunction = _init_item_function.Find( kfsetting->_type );
        if ( kffunction != nullptr )
        {
            kffunction->_function( player, kfitem, kfsetting );
        }

        return kfitem;
    }

    KFData* KFItemModule::FindItemRecord( KFEntity* player, uint32 itemid )
    {
        auto kfitemrecord = player->Find( KFItemBagConfig::Instance()->_default_bag_name );
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return kfitemrecord;
        }

        return FindItemRecord( player, kfitemrecord, kfsetting, 0u );
    }

    KFData* KFItemModule::FindItemRecord( KFEntity* player, KFData* kfitem )
    {
        auto kfsetting = KFItemConfig::Instance()->FindSetting( kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name ) );
        if ( kfsetting == nullptr )
        {
            return kfitem->GetParent();
        }

        return FindItemRecord( player, kfitem->GetParent(), kfsetting, kfitem->Get<uint32>( kfitem->_data_setting->_value_key_name ) );
    }

    KFData* KFItemModule::FindItemRecord( KFEntity* player, KFData* kfitemrecord, const KFItemSetting* kfitemsetting, uint32 itemcount )
    {
        auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfitemsetting->_type );
        if ( kftypesetting == nullptr )
        {
            __LOG_ERROR__( "item=[{}] type=[{}] no type setting", kfitemsetting->_id, kfitemsetting->_type );
            return kfitemrecord;
        }

        auto status = player->GetStatus();
        auto& bagname = kftypesetting->GetBagName( status );
        if ( bagname.empty() )
        {
            __LOG_ERROR__( "item=[{}] type=[{}] status=[{}] can't find bagname", kfitemsetting->_id, kfitemsetting->_type, status );
            return kfitemrecord;
        }

        auto kffindrecord = player->Find( bagname );
        if ( itemcount > 0u )
        {
            auto kfbagsetting = KFItemBagConfig::Instance()->FindSetting( bagname );
            if ( kfbagsetting != nullptr && !kfbagsetting->_extend_bag_name.empty() )
            {
                auto isfull = CheckItemRecordFull( player, kffindrecord, kfitemsetting, itemcount );
                if ( isfull )
                {
                    kffindrecord = player->Find( kfbagsetting->_extend_bag_name );
                }
            }
        }

        return kffindrecord;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_UPDATE_DATA_FUNCTION__( KFItemModule::OnItemCountUpdateCallBack )
    {
        if ( newvalue > 0u )
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

    bool KFItemModule::FindUseRemoveItem( KFEntity* player, KFData* kfparent, const KFItemSetting* kfsetting, uint32 itemcount, std::list<KFData*>& itemlist )
    {
        std::list<KFData*> itemrecordlist;

        kfparent = FindItemRecord( player, kfparent, kfsetting, 0u );
        itemrecordlist.push_back( kfparent );

        auto kfbagsetting = KFItemBagConfig::Instance()->FindSetting( kfparent->_data_setting->_name );
        if ( kfbagsetting != nullptr && !kfbagsetting->_use_child_bag_name.empty() )
        {
            auto kfitemrecord = player->Find( kfbagsetting->_use_child_bag_name );
            if ( kfitemrecord != nullptr )
            {
                itemrecordlist.push_back( kfitemrecord );
            }
        }

        auto totalcount = 0u;
        for ( auto kfitemrecord : itemrecordlist )
        {
            for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
            {
                auto itemid = kfitem->Get( kfitemrecord->_data_setting->_config_key_name );
                if ( itemid != kfsetting->_id )
                {
                    continue;
                }

                itemlist.push_back( kfitem );
                totalcount += kfitem->Get<uint32>( __STRING__( count ) );
                if ( totalcount >= itemcount )
                {
                    return true;
                }
            }
        }

        return false;
    }

    __KF_CHECK_REMOVE_ELEMENT_FUNCTION__( KFItemModule::CheckRemoveItemElement )
    {
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfelement->_data_name );
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", kfelement->_data_name );
            return false;
        }

        // 判断数量
        auto itemcount = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( count ), multiple );
        if ( itemcount == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] count = 0", kfelementobject->_config_id );
            return false;
        }

        auto kfsetting = KFItemConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] setting = nullptr", kfelementobject->_config_id );
            _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist, kfelementobject->_config_id );
            return false;
        }

        std::list<KFData*> itemlist;
        return FindUseRemoveItem( player, kfparent, kfsetting, itemcount, itemlist );
    }

    __KF_REMOVE_ELEMENT_FUNCTION__( KFItemModule::RemoveItemElement )
    {
        auto kfelement = kfresult->_element;
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object", kfelement->_data_name );
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id", kfelement->_data_name );
            return false;
        }

        auto itemcount = kfelementobject->GetValue( __STRING__( count ) );
        if ( itemcount == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] count = 0", kfelementobject->_config_id );
            return false;
        }

        auto kfsetting = KFItemConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] can't find setting", kfelementobject->_config_id );
            return false;
        }

        // 删除道具数量
        std::list<KFData*> itemlist;
        FindUseRemoveItem( player, kfparent, kfsetting, itemcount, itemlist );
        for ( auto kfitem : itemlist )
        {
            auto removecount = RemoveItemCount( player, kfitem, itemcount );
            itemcount -= removecount;
            kfresult->AddResult( kfsetting->_id, kfitem->_data_setting->_name, __STRING__( count ), removecount );
            if ( itemcount == _invalid_int )
            {
                break;
            }
        }

        return true;
    }

    uint32 KFItemModule::RemoveItemCount( KFEntity* player, KFData* kfitem, uint32 itemcount )
    {
        auto oldcount = kfitem->Get<uint32>( __STRING__( count ) );
        if ( itemcount >= oldcount )
        {
            itemcount = oldcount;
            player->RemoveData( kfitem->GetParent(), kfitem->GetKeyID() );
        }
        else
        {
            player->UpdateData( kfitem, __STRING__( count ), KFEnum::Dec, itemcount );
        }

        return itemcount;
    }

    void KFItemModule::AddItem( KFEntity* player, KFData* kfitemrecord, uint32 itemid, uint32 itemcount, uint32 itemindex, bool callback )
    {
        if ( IsItemRecordFull( player, kfitemrecord ) )
        {
            return;
        }

        auto kfitemsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfitemsetting == nullptr )
        {
            return;
        }

        while ( itemcount > kfitemsetting->_overlay_count )
        {
            AddItem( player, kfitemrecord, itemid, kfitemsetting->_overlay_count, 0u, callback );

            itemcount -= kfitemsetting->_overlay_count;
        }

        auto kfitem = player->CreateData( kfitemrecord );

        kfitem->Set( kfitemrecord->_data_setting->_config_key_name, itemid );
        kfitem->Set( __STRING__( count ), itemcount );
        if ( itemindex > 0u )
        {
            kfitem->Set( __STRING__( index ), itemindex );
        }

        // 初始化数据
        auto kffunction = _init_item_function.Find( kfitemsetting->_type );
        if ( kffunction != nullptr )
        {
            kffunction->_function( player, kfitem, kfitemsetting );
        }

        auto uuid = KFGlobal::Instance()->STMakeUuid( __STRING__( item ) );

        player->AddData( kfitemrecord, uuid, kfitem, callback );
    }

    void KFItemModule::RemoveItem( KFEntity* player, KFData* kfitemrecord, uint32 itemid, uint32 itemcount )
    {
        if ( itemcount == 0u )
        {
            return;
        }

        auto findcount = 0u;
        std::list< KFData* > finditem;
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            auto id = kfitem->Get<uint32>( kfitemrecord->_data_setting->_config_key_name );
            if ( id != itemid )
            {
                continue;
            }

            finditem.push_back( kfitem );
            findcount += kfitem->Get<uint32>( __STRING__( count ) );
            if ( findcount >= itemcount )
            {
                break;
            }
        }

        for ( auto kfitem : finditem )
        {
            auto removecount = RemoveItemCount( player, kfitem, itemcount );
            itemcount -= removecount;
            if ( itemcount == _invalid_int )
            {
                break;
            }
        }
    }

    uint32 KFItemModule::RandRemoveItem( KFEntity* player, KFData* kfitemrecord, uint32& itemcount )
    {
        itemcount = 0u;
        std::vector< KFData* > itemlist;
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            itemlist.push_back( kfitem );
        }

        auto count = itemlist.size();
        if ( count == 0u )
        {
            return 0u;
        }

        auto index = KFGlobal::Instance()->RandRatio( count );
        auto kfitem = itemlist.at( index );

        auto itemid = kfitem->Get<uint32>( kfitemrecord->_data_setting->_config_key_name );
        itemcount = kfitem->Get<uint32>( __STRING__( count ) );
        player->RemoveData( kfitemrecord, kfitem->GetKeyID() );

        return itemid;
    }

    __KF_ADD_DATA_FUNCTION__( KFItemModule::OnAddItemCallBack )
    {
        auto itemtime = kfdata->Get< uint64 >();
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
        for ( auto& iter : KFItemBagConfig::Instance()->_settings._objects )
        {
            auto kfitemrecord = player->Find( iter.first );
            for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
            {
                auto time = kfitem->Get<uint64>( __STRING__( time ) );
                if ( time != 0u )
                {
                    StartItemCheckTimer( player, kfitem );
                }
            }
        }
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFItemModule::OnLeaveItemModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    void KFItemModule::StartItemCheckTimer( KFEntity* player, KFData* kfitem )
    {
        auto intervaltime = 1000u;
        auto nowtime = KFGlobal::Instance()->_real_time;
        auto itemtime = kfitem->Get<uint64>( __STRING__( time ) );
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

        for ( auto& iter : KFItemBagConfig::Instance()->_settings._objects )
        {
            auto ok = player->RemoveData( iter.first, subid );
            if ( ok )
            {
                break;
            }
        }
    }

    __KF_REMOVE_DATA_FUNCTION__( KFItemModule::OnRemoveItemCallBack )
    {
        auto itemid = kfdata->Get( kfdata->_data_setting->_config_key_name );
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return;
        }

        auto count = kfdata->Get<uint32>( __STRING__( count ) );
        CallItemLuaFunction( player, KFItemEnum::RemoveFunction, kfsetting, __MAX__( count, 1 ) );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::tuple<KFData*, KFData*> KFItemModule::FindItem( KFEntity* player, uint64 itemuuid )
    {
        for ( auto& iter : KFItemBagConfig::Instance()->_settings._objects )
        {
            auto kfitemrecord = player->Find( iter.first );
            auto kfitem = kfitemrecord->Find( itemuuid );
            if ( kfitem != nullptr )
            {
                return std::make_tuple( kfitemrecord, kfitem );
            }
        }

        return std::make_tuple( nullptr, nullptr );
    }

    void KFItemModule::AddItemResult( KFElementResult* kfresult, uint32 itemid, KFData* kfitem )
    {
        kfresult->AddResult( itemid, kfitem );

        // 判断是否需要显示
        auto kfsetting = KFItemBagConfig::Instance()->FindSetting( kfitem->_data_setting->_name );
        if ( kfsetting != nullptr && !kfsetting->_is_add_show )
        {
            kfresult->_is_need_show = false;
        }
    }

    void KFItemModule::AddItemResult( KFElementResult* kfresult, const std::string& itemname, uint32 itemid, uint32 count )
    {
        kfresult->AddResult( itemid, itemname, __STRING__( count ), count );

        // 判断是否需要显示
        auto kfsetting = KFItemBagConfig::Instance()->FindSetting( itemname );
        if ( kfsetting != nullptr && !kfsetting->_is_add_show )
        {
            kfresult->_is_need_show = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleSellItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSellItemReq );

        auto kfitemrecord = player->Find( kfmsg.name() );
        auto kfitem = kfitemrecord->Find( kfmsg.uuid() );
        if ( kfitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        auto itemid = kfitem->Get<uint32>( kfitemrecord->_data_setting->_config_key_name );
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist );
        }

        if ( kfsetting->_sell_elements.IsEmpty() )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotSell );
        }

        auto count = kfitem->Get<uint32>( kfitemrecord->_data_setting->_value_key_name );
        if ( kfmsg.count() == 0u || count < kfmsg.count() )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemSellCountError );
        }

        // 更新道具数量
        player->UpdateData( kfitem, kfitemrecord->_data_setting->_value_key_name, KFEnum::Dec, kfmsg.count() );

        // 添加道具
        player->AddElement( &kfsetting->_sell_elements, kfmsg.count(), __STRING__( sell ), itemid, __FUNC_LINE__ );

        KFMsg::MsgSellItemAck ack;
        ack.set_itemid( itemid );
        ack.set_count( kfmsg.count() );
        _kf_player->SendToClient( player, KFMsg::MSG_SELL_ITEM_ACK, &ack );
    }
}