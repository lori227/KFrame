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
        _kf_component->RegisterUpdateDataFunction( __KF_STRING__( item ), __KF_STRING__( count ), this, &KFItemModule::OnItemCountUpdateCallBack );
        //////////////////////////////////////////////////////////////////////////////////////////////////

        _kf_component->RegisterAddElementFunction( __KF_STRING__( item ), this, &KFItemModule::AddItemElement );
        _kf_component->RegisterCheckRemoveElementFunction( __KF_STRING__( item ), this, &KFItemModule::CheckItemElement );
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
        _kf_component->UnRegisterCheckRemoveElementFunction( __KF_STRING__( item ) );
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

        // 只有真正的实体道具才会添加到背包中
        if ( kfsetting->IsRealItem() )
        {
            // 计算物品数量
            auto itemcount = kfelementobject->GetValue( kfparent->_class_setting, __KF_STRING__( count ), multiple );
            if ( itemcount == _invalid_int )
            {
                __LOG_ERROR_FUNCTION__( function, line, "item id=[{}] count = 0!", kfelementobject->_config_id );
                return std::make_tuple( KFDataDefine::Show_None, nullptr );
            }

            do
            {
                itemcount = AddOverlayCountItemData( player, kfparent, kfelementobject, kfsetting, itemcount );
            } while ( itemcount > _invalid_int );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CallItemLuaFunction( player, kfsetting, KFItemEnum::AddFunction );
        return std::make_tuple( KFDataDefine::Show_Element, kfparent );
    }

    uint32 KFItemModule::AddOverlayCountItemData( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint32 count )
    {
        auto datasetting = kfparent->_data_setting;

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
                player->UpdateData( kfitem, __KF_STRING__( count ), KFEnum::Add, addcount );

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

    uint32 KFItemModule::AddNewItemData( KFEntity* player, KFData* kfparent, KFElementObject* kfelementobject, const KFItemSetting* kfsetting, uint32 count )
    {
        auto datasetting = kfparent->_data_setting;
        auto addcount = __MIN__( count, kfsetting->_overlay_count );

        auto kfitem = _kf_kernel->CreateObject( datasetting );

        // 设置属性
        player->SetElementToData( kfelementobject, kfitem, 1.0f );

        // id count
        kfitem->SetValue( __KF_STRING__( count ), addcount );
        kfitem->SetValue( datasetting->_config_key_name, kfsetting->_id );

        // guid
        auto itemguid = KFGlobal::Instance()->MakeUUID( KFMsg::UUidItem );
        kfitem->SetValue( datasetting->_key_name, itemguid );

        // 添加物品
        player->AddData( kfparent, itemguid, kfitem );
        return count - addcount;
    }

    KFData* KFItemModule::MoveItem( KFEntity* player, KFData* kfsource, KFData* kftarget, uint64 itemcount  )
    {
        auto datasetting = kfsource->_data_setting;
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
            kftarget->SetKeyID( KFGlobal::Instance()->MakeUUID( KFMsg::UUidItem ) );
            player->UpdateData( kfsource, __KF_STRING__( count ), KFEnum::Dec, itemcount );
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
        auto itemid = kfitem->GetValue( kfitem->_data_setting->_config_key_name );
        auto kfsetting = _kf_item_config->FindSetting( itemid );
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

    __KF_CHECK_REMOVE_ELEMENT_FUNCTION__( KFItemModule::CheckItemElement )
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
        kfparent->FindData( kfparent->_data_setting->_config_key_name, kfelementobject->_config_id, finditem );
        if ( finditem.empty() )
        {
            return false;
        }

        uint32 totalcount = _invalid_int;
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
        kfparent->FindData( kfparent->_data_setting->_config_key_name, kfelementobject->_config_id, finditem );
        if ( finditem.empty() )
        {
            return;
        }

        uint64 totalcount = _invalid_int;
        for ( auto kfitem : finditem )
        {
            auto removecount = __MIN__( itemcount, kfitem->GetValue( __KF_STRING__( count ) ) );
            player->UpdateData( kfitem, __KF_STRING__( count ), KFEnum::Dec, removecount );
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
        auto kfitem = kfitemrecord->FindData( kfmsg.uuid() );
        if ( kfitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotFind );
        }

        auto itemid = kfitem->GetValue( __KF_STRING__( id ) );
        auto kfsetting = _kf_item_config->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotFindData, itemid );
        }

        if ( !kfsetting->CheckCanUse() )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotUse );
        }

        // 删除道具
        player->UpdateData( kfitem, __KF_STRING__( count ), KFEnum::Dec, 1u );

        // 添加奖励
        if ( kfsetting->_reward_type == KFItemEnum::ConfigReward )
        {
            player->AddElement( &kfsetting->_rewards, true, __FUNC_LINE__ );
        }

        // 调用lua脚本
        CallItemLuaFunction( player, kfsetting, KFItemEnum::UseFunction );
    }
}