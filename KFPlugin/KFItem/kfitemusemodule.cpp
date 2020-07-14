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
        if ( kfsetting->_use_limit == KFItemEnum::UseInAll )
        {
            return true;
        }

        auto ok = false;
        auto status = player->GetStatus();
        switch ( status )
        {
        case KFMsg::ExploreStatus:
            ok = KFUtility::HaveBitMask<uint32>( kfsetting->_use_limit, KFItemEnum::UseInExplore );
            break;
        case KFMsg::PVEStatus:
            ok = KFUtility::HaveBitMask<uint32>( kfsetting->_use_limit, KFItemEnum::UseInFight );
            break;
        default:
            ok = KFUtility::HaveBitMask<uint32>( kfsetting->_use_limit, KFItemEnum::UseInCity );
            break;
        }

        return ok;
    }

    __KF_MESSAGE_FUNCTION__( KFItemUseModule::HandleUseItemReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUseItemReq );

        // 判断是否有这个道具
        auto kfitem = player->Find( kfmsg.name(), kfmsg.uuid() );
        if ( kfitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name  );
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

        UseCoseItem( player, kfitem, kfsetting );
        _kf_display->SendToClient( player, KFMsg::ItemUseOk, kfsetting->_id );
    }

    bool KFItemUseModule::UseItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting )
    {
        switch ( kfsetting->_type )
        {
        case KFItemEnum::Gift:		// 礼包
            return UseGiftItem( player, kfitem, kfsetting );
            break;
        case KFItemEnum::Drug:		// 药品道具
            return UseDrugItem( player, kfitem, kfsetting );
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
            _kf_drop->Drop( player, kfsetting->_drop_id, __STRING__( useitem ), kfsetting->_id, __FUNC_LINE__ );
        }

        if ( !kfsetting->_reward.IsEmpty() )
        {
            player->AddElement( &kfsetting->_reward, _default_multiple, __STRING__( useitem ), kfsetting->_id, __FUNC_LINE__ );
        }

        return true;
    }

    bool KFItemUseModule::UseDrugItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting )
    {
        if ( kfsetting->_use_target > 0u )
        {
            return false;
        }

        auto& executelist = kfsetting->_execute_list;
        for ( auto iter : executelist )
        {
            _kf_execute->Execute( player, iter, __STRING__( useitem ), kfsetting->_id, __FUNC_LINE__ );
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
        auto kfitem = player->Find( kfmsg.name(), kfmsg.itemuuid() );
        if ( kfitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        // 判断英雄是否存在
        auto kfhero = player->Find( __STRING__( hero ), kfmsg.herouuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto hp = kfhero->Get( __STRING__( fighter ), __STRING__( hp ) );
        if ( hp == 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroIsDead );
        }

        auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
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

        // 该药品不能对英雄使用
        if ( kfsetting->_use_target == 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotUseToHero );
        }

        if ( !CheckCanUseItem( player, kfsetting ) )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemCanNotUseStatus );
        }

        // 对英雄使用
        auto& executelist = kfsetting->_execute_list;
        for ( auto iter : executelist )
        {
            iter->_calc_value = kfmsg.herouuid();
            _kf_execute->Execute( player, iter, __STRING__( useitem ), itemid, __FUNC_LINE__ );
        }

        UseCoseItem( player, kfitem, kfsetting );
        _kf_display->SendToClient( player, KFMsg::ItemUseToHeroOk, kfsetting->_id, kfmsg.herouuid() );
    }

    void KFItemUseModule::UseCoseItem( KFEntity* player, KFData* kfitem, const KFItemSetting* kfsetting )
    {
        // 扣除数量
        auto usecount = kfitem->Get<uint32>( __STRING__( usecount ) );
        if ( usecount + 1u >= kfsetting->_use_count )
        {
            _kf_item->RemoveItemCount( player, kfitem, 1u );
        }
        else
        {
            player->UpdateData( kfitem, __STRING__( usecount ), KFEnum::Add, 1u );
        }
    }
}