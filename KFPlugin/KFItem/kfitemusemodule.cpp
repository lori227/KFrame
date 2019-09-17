#include "KFItemUseModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFItemUseModule::BeforeRun()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_USE_ITEM_REQ, &KFItemUseModule::HandleUseItemReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_USE_ITEM_TO_HERO_REQ, &KFItemUseModule::HandleUseItemToHeroReq );
    }

    void KFItemUseModule::BeforeShut()
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_USE_ITEM_REQ );
        __UN_MESSAGE__( KFMsg::MSG_USE_ITEM_TO_HERO_REQ );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFItemUseModule::CheckCanUseItem( KFEntity* player, const KFItemSetting* kfsetting )
    {
        return true;
        //if ( kfsetting->_use_limit == KFItemEnum::UseInAll )
        //{
        //    return true;
        //}

        //auto ok = false;
        //return ok;
    }

    __KF_MESSAGE_FUNCTION__( KFItemUseModule::HandleUseItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUseItemReq );

        // 判断是否有这个道具
        auto kfitem = player->GetData()->FindData( kfmsg.name(), kfmsg.uuid() );
        if ( kfitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        auto itemid = kfitem->GetValue<uint32>( kfitem->_data_setting->_config_key_name  );
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist, itemid );
        }

        if ( !kfsetting->CheckCanUse() )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotUse );
        }

        if ( !CheckCanUseItem( player, kfsetting ) )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotUseStatus );
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

        _kf_display->SendToClient( player, KFMsg::ItemUseOk, kfsetting->_id );
    }

    bool KFItemUseModule::UseItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting )
    {
        switch ( kfsetting->_type )
        {
        case KFItemEnum::Gift:	// 礼包
            return UseGiftItem( player, kfitem, kfsetting );
            break;
        case KFItemEnum::Script:	// 脚本
            return UseScriptItem( player, kfitem, kfsetting );
            break;
        default:
            break;
        }

        return false;
    }

    bool KFItemUseModule::UseGiftItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting )
    {
        // 判断各类资源是否满了
        if ( kfsetting->_drop_id != 0u )
        {
            _kf_drop->Drop( player, kfsetting->_drop_id, 1u, true, __FUNC_LINE__ );
        }

        if ( !kfsetting->_reward.IsEmpty() )
        {
            player->AddElement( &kfsetting->_reward, true, __FUNC_LINE__ );
        }

        return true;
    }

    bool KFItemUseModule::UseScriptItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting )
    {
        if ( kfsetting->_lua_file.empty() )
        {
            return false;
        }

        auto& luafunction = kfsetting->GetFunction( KFItemEnum::UseFunction );
        if ( luafunction.empty() )
        {
            return false;
        }

        auto playerid = player->GetKeyID();
        auto result = _kf_lua->Call( kfsetting->_lua_file, luafunction, playerid, kfsetting->_id, 1u );
        return result != 0u;
    }

    __KF_MESSAGE_FUNCTION__( KFItemUseModule::HandleUseItemToHeroReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUseItemToHeroReq );

        // 判断是否有这个道具
        auto kfitem = player->GetData()->FindData( kfmsg.name(), kfmsg.itemuuid() );
        if ( kfitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        // 判断英雄是否存在
        auto kfhero = player->GetData()->FindData( __KF_STRING__( hero ), kfmsg.herouuid() );
        if ( kfhero == nullptr )
        {
            //return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto itemid = kfitem->GetValue<uint32>( kfitem->_data_setting->_config_key_name );
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemSettingNotExist, itemid );
        }

        // 不是药品不能使用
        if ( kfsetting->_type != KFItemEnum::Drug )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotUse );
        }

        if ( !CheckCanUseItem( player, kfsetting ) )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotUseStatus );
        }

        // 对英雄使用
        for ( auto& iter : kfsetting->_drug_values )
        {
            //_kf_hero->AddHeroData( player, kfhero, iter.first, iter.second );
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

        _kf_display->SendToClient( player, KFMsg::ItemUseToHeroOk, kfsetting->_id, kfmsg.herouuid() );
    }
}