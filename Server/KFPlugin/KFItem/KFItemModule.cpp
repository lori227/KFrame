#include "KFItemModule.h"
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
        _kf_component->RegisterUpdateDataFunction( __KF_STRING__( item ), __KF_STRING__( count ), this, &KFItemModule::OnItemCountUpdateCallBack );
        //////////////////////////////////////////////////////////////////////////////////////////////////

        _kf_component->RegisterAddElementFunction( __KF_STRING__( item ), this, &KFItemModule::AddItemElement );
        _kf_component->RegisterCheckElementFunction( __KF_STRING__( item ), this, &KFItemModule::CheckItemElement );
        _kf_component->RegisterRemoveElementFunction( __KF_STRING__( item ), this, &KFItemModule::RemoveItemElement );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_USE_ITEM_REQ, &KFItemModule::HandleUseItemReq );
    }

    void KFItemModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__( _kf_item_config );

        _kf_component->UnRegisterAddDataFunction( this, __KF_STRING__( item ) );
        _kf_component->UnRegisterRemoveDataFunction( this, __KF_STRING__( item ) );
        _kf_component->UnRegisterUpdateDataFunction( this, __KF_STRING__( item ), __KF_STRING__( count ) );

        _kf_component->UnRegisterAddElementFunction( __KF_STRING__( item ) );
        _kf_component->UnRegisterCheckElementFunction( __KF_STRING__( item ) );
        _kf_component->UnRegisterRemoveElementFunction( __KF_STRING__( item ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_USE_ITEM_REQ );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ADD_ELEMENT_FUNCTION__( KFItemModule::AddItemElement )
    {
        if ( !kfelement->IsObject() )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_parent->_data );
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id!", kfelement->_parent->_data );
        }

        auto kfsetting = _kf_item_config->FindItemSetting( kfelementobject->_config_id );
        if ( kfsetting == nullptr )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] itemsetting = null!", kfelement->_parent->_data );
        }

        // 只有真正的实体道具才会添加到背包中
        if ( kfsetting->IsRealItem() )
        {
            // 计算物品数量
            auto itemcount = kfelementobject->CalcValue( __KF_STRING__( count ) );
            if ( itemcount == _invalid_int )
            {
                return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] itemcount = null!", kfelement->_parent->_data );
            }

            do
            {
                itemcount = AddOverlayCountItemData( player, kfdata, kfelementobject, kfsetting, itemcount );
            } while ( itemcount > _invalid_int );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CallItemLuaFunction( player, kfsetting, KFItemEnum::AddFunction );
    }

    uint64 KFItemModule::AddOverlayCountItemData( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint64 count )
    {
        auto datasetting = kfparent->GetDataSetting();

        // 叠加数量 > 1
        if ( kfsetting->_overlay_count > 1u )
        {
            std::list< KFData* > finditem;
            kfparent->FindData( datasetting->_config_key_name, kfsetting->_id, finditem );

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
                player->UpdateData( kfitem, __KF_STRING__( count ), KFOperateEnum::Add, addcount );

                count -= addcount;
                if ( count == _invalid_int )
                {
                    return _invalid_int;
                }
            }
        }

        // 添加新的物品
        return AddNewItemData( player, kfparent, kfelementobject, kfsetting, count );
    }

    uint64 KFItemModule::AddNewItemData( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint64 count )
    {
        auto datasetting = kfparent->GetDataSetting();
        uint64 addcount = __MIN__( count, kfsetting->_overlay_count );

        auto kfitem = _kf_kernel->CreateObject( datasetting );
        kfitem->SetValue( datasetting->_config_key_name, kfsetting->_id );
        kfitem->SetValue( __KF_STRING__( count ), addcount );

        // 设置属性
        for ( auto& iter : kfelementobject->_values._objects )
        {
            auto& name = iter.first;
            if ( name == __KF_STRING__( count ) )
            {
                continue;
            }

            auto value = iter.second->GetValue();
            kfitem->SetValue( name, value );
        }

        // guid
        auto itemguid = KFGlobal::Instance()->Make64Guid();
        kfitem->SetValue( datasetting->_key_name, itemguid );

        // 添加物品
        player->AddData( kfparent, itemguid, kfitem );
        return count - addcount;
    }

    KFData* KFItemModule::MoveItem( KFEntity* player, KFData* kfsource, KFData* kftarget, uint64 itemcount  )
    {
        auto datasetting = kfsource->GetDataSetting();
        if ( kftarget == nullptr )
        {
            kftarget = _kf_kernel->CreateObject( datasetting );
        }

        kftarget->CopyFrom( kfsource );

        auto sourceguid = kfsource->GetKeyID();
        auto sourcecount = kfsource->GetValue( __KF_STRING__( count ) );

        // 如果堆叠物品取出来一部分, 判断数量
        if ( itemcount < sourcecount )
        {
            // 设置数量
            kftarget->SetValue( __KF_STRING__( count ), itemcount );

            // 设置新的guid
            kftarget->SetKeyID( KFGlobal::Instance()->Make64Guid() );
            player->UpdateData( kfsource, __KF_STRING__( count ), KFOperateEnum::Dec, itemcount );
        }
        else
        {
            player->RemoveData( kfsource->GetParent(), sourceguid );
        }

        return kftarget;
    }

    __KF_UPDATE_DATA_FUNCTION__( KFItemModule::OnItemCountUpdateCallBack )
    {
        // 调用脚本
        auto itemcount = kfdata->GetValue();
        if ( itemcount > _invalid_int )
        {
            return;
        }

        // 调用删除脚本
        auto kfitem = kfdata->GetParent();
        auto itemid = kfitem->GetValue( kfitem->GetDataSetting()->_config_key_name );
        auto kfsetting = _kf_item_config->FindItemSetting( itemid );
        if ( kfsetting != nullptr )
        {
            CallItemLuaFunction( player, kfsetting, KFItemEnum::RemoveFunction );
        }

        // 删除该物品
        player->RemoveData( __KF_STRING__( item ), key );
    }

    void KFItemModule::CallItemLuaFunction( KFEntity* player, const KFItemSetting* kfsetting, uint32 functiontype )
    {
        if ( kfsetting->_lua_file.empty() )
        {
            return;
        }

        auto& luafunction = kfsetting->GetFunction( functiontype );
        if ( luafunction.empty() )
        {
            return;
        }

        auto playerid = player->GetKeyID();
        _kf_lua->Call( kfsetting->_lua_file, luafunction, playerid, kfsetting->_id );
    }

    __KF_CHECK_ELEMENT_FUNCTION__( KFItemModule::CheckItemElement )
    {
        if ( !kfelement->IsObject() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_parent->_data );
            return false;
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id!", kfelement->_parent->_data );
            return false;
        }

        // 判断数量
        auto itemcount = kfelementobject->CalcValue( __KF_STRING__( count ) );
        if ( itemcount == _invalid_int )
        {
            __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no count!", kfelement->_parent->_data );
            return false;
        }

        // 获取item列表
        std::list< KFData* > finditem;
        kfdata->FindData( kfdata->GetDataSetting()->_config_key_name, kfelementobject->_config_id, finditem );
        if ( finditem.empty() )
        {
            return false;
        }

        uint64 totalcount = _invalid_int;
        for ( auto kfitem : finditem )
        {
            // 累计数量
            totalcount += kfitem->GetValue( __KF_STRING__( count ) );
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
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] not object!", kfelement->_parent->_data );
        }

        auto kfelementobject = reinterpret_cast< KFElementObject* >( kfelement );
        if ( kfelementobject->_config_id == _invalid_int )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no id!", kfelement->_parent->_data );
        }

        auto itemcount = kfelementobject->CalcValue( __KF_STRING__( count ) );
        if ( itemcount == _invalid_int )
        {
            return __LOG_ERROR_FUNCTION__( function, line, "element=[{}] no count!", kfelement->_parent->_data );
        }

        // 获取item列表
        std::list< KFData* > finditem;
        kfdata->FindData( kfdata->GetDataSetting()->_config_key_name, kfelementobject->_config_id, finditem );
        if ( finditem.empty() )
        {
            return;
        }

        uint64 totalcount = _invalid_int;
        for ( auto kfitem : finditem )
        {
            auto removecount = __MIN__( itemcount, kfitem->GetValue( __KF_STRING__( count ) ) );
            player->UpdateData( kfitem, __KF_STRING__( count ), KFOperateEnum::Dec, removecount );
            itemcount -= removecount;
            if ( itemcount == _invalid_int )
            {
                break;
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemModule::HandleUseItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUseItemReq );

        auto kfobject = player->GetData();
        auto kfitemrecord = kfobject->FindData( __KF_STRING__( item ) );

        // 判断是否有这个道具
        auto kfitem = kfitemrecord->FindData( kfmsg.itemguid() );
        if ( kfitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemNotExist );
        }

        auto itemid = kfitem->GetValue( __KF_STRING__( id ) );
        auto kfsetting = _kf_item_config->FindItemSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataError, itemid );
        }

        if ( !kfsetting->CheckCanUse() )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotUse );
        }

        // 删除道具
        player->UpdateData( kfitem, __KF_STRING__( count ), KFOperateEnum::Dec, 1u );

        // 添加奖励
        if ( kfsetting->_reward_type == KFItemEnum::ConfigReward )
        {
            player->AddElement( &kfsetting->_rewards, true, __FUNC_LINE__ );
        }

        // 调用lua脚本
        CallItemLuaFunction( player, kfsetting, KFItemEnum::UseFunction );
    }
}