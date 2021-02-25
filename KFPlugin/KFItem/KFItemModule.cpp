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
    void KFItemModule::BindItemInitFunction( uint32 itemtype, const std::string& module, KFItemInitFunction& function )
    {
        auto kffunctionmap = _item_init_function.Create( itemtype );
        auto kffunction = kffunctionmap->Create( module );
        kffunction->_function = function;
    }

    void KFItemModule::UnBindItemInitFunction( uint32 itemtype, const std::string& module )
    {
        auto kffunctionmap = _item_init_function.Find( itemtype );
        if ( kffunctionmap != nullptr )
        {
            kffunctionmap->Remove( module );
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleRemoveItemReq, KFMsg::MsgRemoveItemReq )
    {
        __LOG_INFO__( "player=[{}] remove item=[{}:{}]", entity->GetKeyID(), kfmsg->sourcename(), kfmsg->uuid() );

        if ( kfmsg->uuid() == 0u )
        {
            entity->ClearRecord( kfmsg->sourcename() );
        }
        else
        {
            entity->RemoveRecord( kfmsg->sourcename(), kfmsg->uuid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleRemoveItemCountReq, KFMsg::MsgRemoveItemCountReq )
    {
        __LOG_INFO__( "player=[{}] remove itemcount=[{}:{}]", entity->GetKeyID(), kfmsg->itemid(), kfmsg->count() );

        auto kfitemrecord = FindItemBag( entity, kfmsg->itemid() );
        RemoveItem( entity, kfitemrecord, kfmsg->itemid(), kfmsg->count() );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_GET_CONFIG_VALUE_FUNCTION__( KFItemModule::GetItemTotalCount )
    {
        auto kfitemrecord = FindItemBag( player, id );
        if ( kfitemrecord == nullptr )
        {
            return 0u;
        }

        return GetItemCount( player, kfitemrecord, id, maxvalue );
    }

    uint32 KFItemModule::GetItemCount( EntityPtr player, DataPtr kfitemrecord, uint32 itemid, uint32 maxcount /* = __MAX_UINT32__ */ )
    {
        auto totalcount = 0u;
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            if ( itemid != kfitem->Get<uint32>( __STRING__( id ) ) )
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


        auto kfitemrecord = FindItemBag( player, kfelementobject->_config_id );
        if ( kfitemrecord != nullptr )
        {
            kfparent = kfitemrecord;
        }

        auto setting = KFItemConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( setting == nullptr )
        {
            return false;
        }

        return !CheckItemBagFull( player, kfparent, setting, itemcount );
    }

    bool KFItemModule::CheckItemBagFull( EntityPtr player, DataPtr kfitemrecord, const KFItemSetting* setting, uint32 itemcount )
    {
        auto maxsize = GetItemBagMaxCount( player, kfitemrecord );
        auto maxoverlaycount = setting->_overlay_count;

        // 计算占用格子数
        auto cursize = kfitemrecord->Size();
        uint32 usesize = ( itemcount + maxoverlaycount - 1 ) / maxoverlaycount;
        auto leftsize = ( maxsize > cursize ) ? ( maxsize - cursize ) : 0u;
        if ( leftsize >= usesize )
        {
            return false;
        }

        if ( setting->_overlay_count > 1u )
        {
            // 剩余格子可叠加物品数
            auto canaddcount = leftsize * maxoverlaycount;
            for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
            {
                if ( setting->_id != kfitem->Get( __STRING__( id ) ) )
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

    uint32 KFItemModule::GetCanAddItemCount( EntityPtr player, uint32 itemid, uint32 itemcount )
    {
        auto setting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( setting == nullptr )
        {
            __LOG_ERROR__( "item=[{}] no setting", setting->_id );
            return 0u;
        }

        auto kfitemrecord = FindItemBag( player, itemid );
        if ( kfitemrecord == nullptr )
        {
            return 0u;
        }

        auto maxsize = GetItemBagMaxCount( player, kfitemrecord );
        auto maxoverlaycount = setting->_overlay_count;

        // 计算占用格子数
        auto cursize = kfitemrecord->Size();
        uint32 usesize = ( itemcount + maxoverlaycount - 1 ) / maxoverlaycount;
        auto leftsize = ( maxsize > cursize ) ? ( maxsize - cursize ) : 0u;
        if ( leftsize >= usesize )
        {
            return itemcount;
        }

        // 不能叠加
        if ( setting->_overlay_count == 1u )
        {
            return leftsize;
        }

        // 剩余格子可添加物品数
        auto canaddcount = leftsize * maxoverlaycount;
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            if ( setting->_id != kfitem->Get<uint32>( __STRING__( id ) ) )
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

    uint32 KFItemModule::GetItemBagMaxCount( EntityPtr player, DataPtr kfitemrecord )
    {
        auto kfitembagsetting = KFItemBagConfig::Instance()->FindSetting( kfitemrecord->_data_setting->_name );
        if ( kfitembagsetting == nullptr )
        {
            return kfitemrecord->_data_setting->_int_max_value;
        }

        return kfitembagsetting->_max_count;
    }

    uint32 KFItemModule::GetItemEmptyCount( EntityPtr player, DataPtr kfitemrecord )
    {
        auto maxcount = GetItemBagMaxCount( player, kfitemrecord );
        return maxcount - kfitemrecord->Size();
    }

    bool KFItemModule::CheckItemBagFull( EntityPtr player, DataPtr kfitemrecord )
    {
        auto maxcount = GetItemBagMaxCount( player, kfitemrecord );
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

        auto kfitemsetting = KFItemConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( kfitemsetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "id=[{}] itemsetting = null", kfelementobject->_config_id );
            _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist, kfelementobject->_config_id );
            return false;
        }

        auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfitemsetting->_type );
        if ( kftypesetting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "itemtype=[{}] typesetting = null", kfitemsetting->_type );
            _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist, kfelementobject->_config_id );
            return false;
        }

        // 查找背包数据
        kfparent = FindFinalItemBag( player, kfparent, kfitemsetting, kftypesetting, 0u );

        // 计算物品数量
        auto itemcount = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( count ), kfresult->_multiple );
        if ( itemcount == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] count = 0", kfelementobject->_config_id );
            return false;
        }

        // 判断堆叠类型
        if ( kfitemsetting->_overlay_type == KFItemEnum::OverlayByTime )
        {
            // 计算时间
            auto itemtime = kfelementobject->CalcValue( kfparent->_data_setting, __STRING__( time ), kfresult->_multiple );
            if ( itemtime == 0u )
            {
                __LOG_ERROR_FUNCTION__( function, line, "time item id=[{}] count = 0", kfelementobject->_config_id );
                return false;
            }

            AddOverlayTimeItem( player, kfparent, kfresult, kfitemsetting, kftypesetting, itemcount, itemtime );
        }
        else if ( kfitemsetting->_overlay_type == KFItemEnum::OverlayByCount )
        {
            // 按数量叠加道具
            if ( kfitemsetting->_overlay_count > 1u )
            {
                AddOverlayCountItem( player, kfparent, kfresult, kfitemsetting, kftypesetting, itemcount );
            }
            else
            {
                AddNotOverlayItem( player, kfparent, kfresult, kfitemsetting, kftypesetting, itemcount );
            }
        }

        return true;
    }

    void KFItemModule::AddOverlayTimeItem( EntityPtr player, DataPtr kfparent, KFElementResult* kfresult,
                                           const KFItemSetting* kfitemsetting, const KFItemTypeSetting* kftypesetting, uint32 count, uint32 time )
    {
        std::list<DataPtr> finditem;
        kfparent->Find( __STRING__( id ), kfitemsetting->_id, finditem, false );

        DataPtr kfitem = nullptr;
        if ( !finditem.empty() )
        {
            kfitem = finditem.front();
            player->UpdateObject( kfitem, __STRING__( time ), KFEnum::Add, count * time );
        }
        else
        {
            kfitem = AddNewItemData( player, kfparent, kfitemsetting, kftypesetting, count, time );
        }

        AddItemResult( kfresult, kfitemsetting->_id, kfitem );
    }

    void KFItemModule::AddOverlayCountItem( EntityPtr player, DataPtr kfparent, KFElementResult* kfresult,
                                            const KFItemSetting* kfitemsetting, const KFItemTypeSetting* kftypesetting, uint32 count )
    {
        auto maxoverlaycount = kfitemsetting->_overlay_count;

        // 查找相同道具的id
        auto totaladdcount = 0u;
        for ( auto kfitem = kfparent->First(); kfitem != nullptr; kfitem = kfparent->Next() )
        {
            if ( kfitemsetting->_id != kfitem->Get<uint32>( __STRING__( id ) ) )
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
            player->UpdateObject( kfitem, __STRING__( count ), KFEnum::Add, addcount );
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
            AddItemResult( kfresult, kfparent->_data_setting->_name, kfitemsetting->_id, totaladdcount );
        }

        // 添加完成, 直接返回
        if ( count == 0u )
        {
            return;
        }

        // 添加新物品
        do
        {
            auto kfitem = AddNewItemData( player, kfparent, kfitemsetting, kftypesetting, count, 0u );
            AddItemResult( kfresult, kfitem->_data_setting->_name, kfitemsetting->_id, kfitem->Get<uint32>( __STRING__( count ) ) );
        } while ( count > 0u );
    }

    void KFItemModule::AddNotOverlayItem( EntityPtr player, DataPtr kfparent, KFElementResult* kfresult,
                                          const KFItemSetting* kfitemsetting, const KFItemTypeSetting* kftypesetting, uint32 count )
    {
        // 添加新物品
        do
        {
            auto kfitem = AddNewItemData( player, kfparent, kfitemsetting, kftypesetting, count, 0u );
            AddItemResult( kfresult, kfitemsetting->_id, kfitem );
        } while ( count > 0u );
    }

    DataPtr KFItemModule::AddNewItemData( EntityPtr player, DataPtr kfparent,
                                          const KFItemSetting* kfitemsetting, const KFItemTypeSetting* kftypesetting, uint32& count, uint32 time )
    {
        auto maxoverlaycount = kfitemsetting->_overlay_count;
        auto addcount = __MIN__( count, maxoverlaycount );

        // 判断满包情况, 需要添加另外的背包
        kfparent = FindFinalItemBag( player, kfparent, kfitemsetting, kftypesetting, addcount );
        auto kfitem = player->CreateData( kfparent );

        // id
        kfitem->Set( __STRING__( id ), kfitemsetting->_id );

        // count
        kfitem->Set( __STRING__( count ), addcount );

        // time
        if ( time != 0u )
        {
            kfitem->Set( __STRING__( time ), KFGlobal::Instance()->_real_time + time * count );
        }

        // 初始化数据
        CallItemInitFunction(  player, kfitem, kfitemsetting );

        // uuid
        auto uuid = KFGlobal::Instance()->STMakeUuid( __STRING__( item ) );

        // 添加新的物品
        count -= addcount;
        player->AddRecord( kfparent, uuid, kfitem );
        return kfitem;
    }

    void KFItemModule::CallItemInitFunction( EntityPtr player, DataPtr kfitem, const KFItemSetting* kfitemsetting )
    {
        auto kffunctionmap = _item_init_function.Find( kfitemsetting->_type );
        if ( kffunctionmap == nullptr )
        {
            return;
        }

        for ( auto& iter : kffunctionmap->_objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player, kfitem, kfitemsetting );
        }
    }

    DataPtr KFItemModule::CreateItem( EntityPtr player, uint32 itemid, DataPtr kfitem, const char* function, uint32 line )
    {
        // 没有传入item, 创建一个新的item
        if ( kfitem == nullptr )
        {
            auto kfitembag = FindItemBag( player, itemid );
            if ( kfitembag == nullptr )
            {
                return nullptr;
            }

            kfitem = player->CreateData( kfitembag );
            if ( kfitem == nullptr )
            {
                return nullptr;
            }
        }

        auto setting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( setting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item=[{}] can't find setting", itemid );
            _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist, itemid );
            return nullptr;
        }

        // id
        kfitem->Set( __STRING__( id ), setting->_id );

        // count
        kfitem->Set( __STRING__( count ), 1u );

        // uuid
        kfitem->SetKeyID( KFGlobal::Instance()->STMakeUuid( __STRING__( item ) ) );

        // 初始化数据
        CallItemInitFunction( player, kfitem, setting );
        return kfitem;
    }

    DataPtr KFItemModule::FindItemBag( EntityPtr player, DataPtr kfitem )
    {
        auto itemid = kfitem->Get<uint32>( __STRING__( id ) );
        return FindItemBag( player, itemid );
    }

    DataPtr KFItemModule::FindItemBag( EntityPtr player, uint32 itemid )
    {
        auto kfitemsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfitemsetting == nullptr )
        {
            __LOG_ERROR__( "item=[{}] can't find item setting", itemid );
            return nullptr;
        }

        auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( kfitemsetting->_type );
        if ( kftypesetting == nullptr )
        {
            __LOG_ERROR__( "item=[{}] can't find type=[{}] setting", itemid, kfitemsetting->_type );
            return nullptr;
        }

        auto kffindrecord = player->Find( kftypesetting->_obtain_bag );
        if ( kffindrecord == nullptr )
        {
            __LOG_ERROR__( "bag=[{}] can't itembag record", kftypesetting->_obtain_bag );
            return nullptr;
        }

        return FindFinalItemBag( player, kffindrecord, kfitemsetting, kftypesetting, 0u );
    }


    DataPtr KFItemModule::FindFinalItemBag( EntityPtr player, DataPtr kfitemrecord, const KFItemSetting* kfitemsetting, const KFItemTypeSetting* kftypesetting, uint32 itemcount )
    {
        {
            auto kffindrecord = player->Find( kftypesetting->_obtain_bag );
            if ( kffindrecord != nullptr )
            {
                kfitemrecord = kffindrecord;
            }
            else
            {
                __LOG_ERROR__( "item=[{}] type=[{}] can't find itembag", kfitemsetting->_id, kfitemsetting->_type );
            }
        }

        if ( itemcount > 0u )
        {
            // 判断包裹是否满了
            auto kfbagsetting = KFItemBagConfig::Instance()->FindSetting( kfitemrecord->_data_setting->_name );
            if ( kfbagsetting != nullptr && !kfbagsetting->_extend_bag.empty() )
            {
                auto isfull = CheckItemBagFull( player, kfitemrecord, kfitemsetting, itemcount );
                if ( isfull )
                {
                    auto kffindrecord = player->Find( kfbagsetting->_extend_bag );
                    if ( kffindrecord != nullptr )
                    {
                        kfitemrecord = kffindrecord;
                    }
                }
            }
        }

        return kfitemrecord;
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
        player->RemoveRecord( kfdata->GetParent()->GetParent(), key );
    }

    const std::list<DataPtr>& KFItemModule::FindRemoveItemList( EntityPtr player, DataPtr kfparent, const KFItemSetting* setting, uint32 itemcount )
    {
        std::list<DataPtr> itemrecordlist;
        itemrecordlist.push_back( kfparent );

        auto kfbagsetting = KFItemBagConfig::Instance()->FindSetting( kfparent->_data_setting->_name );
        if ( kfbagsetting != nullptr && !kfbagsetting->_use_child_bag.empty() )
        {
            for ( auto& childbag : kfbagsetting->_use_child_bag )
            {
                auto kfitemrecord = player->Find( childbag );
                if ( kfitemrecord != nullptr )
                {
                    itemrecordlist.push_back( kfitemrecord );
                }
            }
        }

        static std::list<DataPtr> _result_list;
        _result_list.clear();

        auto totalcount = 0u;
        for ( auto kfitemrecord : itemrecordlist )
        {
            for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
            {
                if ( setting->_id != kfitem->Get( __STRING__( id  ) ) )
                {
                    continue;
                }

                _result_list.push_back( kfitem );
                totalcount += kfitem->Get<uint32>( __STRING__( count ) );
                if ( totalcount >= itemcount )
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

        auto setting = KFItemConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( setting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] setting = nullptr", kfelementobject->_config_id );
            _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist, kfelementobject->_config_id );
            return false;
        }

        auto& itemlist = FindRemoveItemList( player, kfparent, setting, itemcount );
        return !itemlist.empty();
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

        auto setting = KFItemConfig::Instance()->FindSetting( kfelementobject->_config_id );
        if ( setting == nullptr )
        {
            __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] can't find setting", kfelementobject->_config_id );
            return false;
        }

        // 删除道具数量
        auto& itemlist = FindRemoveItemList( player, kfparent, setting, itemcount );
        for ( auto kfitem : itemlist )
        {
            auto removecount = RemoveItemCount( player, kfitem, itemcount );
            itemcount -= removecount;
            kfresult->AddResult( setting->_id, kfitem->_data_setting->_name, __STRING__( count ), removecount );
            if ( itemcount == _invalid_int )
            {
                break;
            }
        }

        return true;
    }

    uint32 KFItemModule::RemoveItemCount( EntityPtr player, DataPtr kfitem, uint32 itemcount )
    {
        auto oldcount = kfitem->Get<uint32>( __STRING__( count ) );
        if ( itemcount >= oldcount )
        {
            itemcount = oldcount;
            player->RemoveRecord( kfitem->GetParent(), kfitem->GetKeyID() );
        }
        else
        {
            player->UpdateObject( kfitem, __STRING__( count ), KFEnum::Dec, itemcount );
        }

        return itemcount;
    }

    void KFItemModule::RemoveItem( EntityPtr player, DataPtr kfitemrecord, uint32 itemid, uint32 itemcount )
    {
        if ( itemcount == 0u )
        {
            return;
        }

        auto findcount = 0u;
        std::list<DataPtr> finditem;
        for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
        {
            if ( itemid != kfitem->Get<uint32>( __STRING__( id ) ) )
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

    __KF_ADD_DATA_FUNCTION__( KFItemModule::OnAddItemCallBack )
    {
        auto itemtime = kfdata->Get<uint64>();
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

    __KF_PLAYER_ENTER_FUNCTION__( KFItemModule::OnEnterItemModule )
    {
        for ( auto& iter : KFItemBagConfig::Instance()->_setting_list._objects )
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

    __KF_PLAYER_LEAVE_FUNCTION__( KFItemModule::OnLeaveItemModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    void KFItemModule::StartItemCheckTimer( EntityPtr player, DataPtr kfitem )
    {
        auto intervaltime = 1000u;
        auto now_time = KFGlobal::Instance()->_real_time;
        auto itemtime = kfitem->Get<uint64>( __STRING__( time ) );
        if ( itemtime > now_time )
        {
            intervaltime = ( itemtime - now_time + 1 ) * 1000;
        }

        __COUNT_TIMER_2__( player->GetKeyID(), kfitem->GetKeyID(), intervaltime, 1, &KFItemModule::OnTimerRemoveTimeItem );
    }

    __KF_TIMER_FUNCTION__( KFItemModule::OnTimerRemoveTimeItem )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        auto tupledata = FindItem( player, subid );
        auto kfitembag = std::get<0>( tupledata );
        auto kfitem = std::get<1>( tupledata );
        if ( kfitembag != nullptr && kfitem != nullptr )
        {
            player->RemoveRecord( kfitembag, subid );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::tuple<DataPtr, DataPtr> KFItemModule::FindItem( EntityPtr player, uint64 itemuuid )
    {
        for ( auto& iter : KFItemBagConfig::Instance()->_setting_list._objects )
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

    void KFItemModule::AddItemResult( KFElementResult* kfresult, uint32 itemid, DataPtr kfitem )
    {
        kfresult->AddResult( itemid, kfitem );

        // 判断是否需要显示
        auto setting = KFItemBagConfig::Instance()->FindSetting( kfitem->_data_setting->_name );
        if ( setting != nullptr && !setting->_show_when_add )
        {
            kfresult->_is_need_show = false;
        }
    }

    void KFItemModule::AddItemResult( KFElementResult* kfresult, const std::string& itemname, uint32 itemid, uint32 count )
    {
        kfresult->AddResult( itemid, itemname, __STRING__( count ), count );

        // 判断是否需要显示
        auto setting = KFItemBagConfig::Instance()->FindSetting( itemname );
        if ( setting != nullptr && !setting->_show_when_add )
        {
            kfresult->_is_need_show = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleSellItemReq, KFMsg::MsgSellItemReq )
    {
        auto kfitembag = entity->Find( kfmsg->name() );
        auto kfitem = kfitembag->Find( kfmsg->uuid() );
        if ( kfitem == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::ItemDataNotExist );
        }

        auto itemid = kfitem->Get<uint32>( __STRING__( id ) );
        auto setting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::ItemSettingNotExist );
        }

        if ( setting->_sell.IsEmpty() )
        {
            return _kf_display->SendToClient( entity, KFMsg::ItemCanNotSell );
        }

        auto count = kfitem->Get<uint32>( __STRING__( count ) );
        if ( kfmsg->count() == 0u || count < kfmsg->count() )
        {
            return _kf_display->SendToClient( entity, KFMsg::ItemSellCountError );
        }

        // 更新道具数量
        entity->UpdateObject( kfitem, __STRING__( count ), KFEnum::Dec, kfmsg->count() );

        // 添加道具
        entity->AddElement( &setting->_sell, kfmsg->count(), __STRING__( sell ), itemid, __FUNC_LINE__ );

        KFMsg::MsgSellItemAck ack;
        ack.set_itemid( itemid );
        ack.set_count( kfmsg->count() );
        _kf_player->SendToClient( entity, KFMsg::MSG_SELL_ITEM_ACK, &ack );
    }
}