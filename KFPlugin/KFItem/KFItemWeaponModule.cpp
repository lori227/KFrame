#include "KFItemWeaponModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFItemWeaponModule::BeforeRun()
    {
        __REGISTER_INIT_ITEM__( KFItemEnum::Weapon, &KFItemWeaponModule::InitWeaponData );

        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_UPDATE_DATA_2__( __STRING__( weapon ), __STRING__( durability ), &KFItemWeaponModule::OnWeaponDurabilityUpdate );
        __REGISTER_UPDATE_DATA_2__( __STRING__( hero ), __STRING__( weapontype ), &KFItemWeaponModule::OnWeaponTypeUpdate );
        __REGISTER_REMOVE_DATA_1__( __STRING__( hero ), &KFItemWeaponModule::OnRemoveHero );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_HERO_WEAPON_REQ, &KFItemWeaponModule::HandleHeroWeaponReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_HERO_UNWEAPON_REQ, &KFItemWeaponModule::HandleHeroUnWeaponReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_DURABILITY_REQ, &KFItemWeaponModule::HandleUpdateDurabilityReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_HERO_WEAPON_ANOTHER_REQ, &KFItemWeaponModule::HandleHeroWeaponAnotherReq );
    }

    void KFItemWeaponModule::BeforeShut()
    {
        __UN_INIT_ITEM__( KFItemEnum::Weapon );

        __UN_UPDATE_DATA_2__( __STRING__( weapon ), __STRING__( durability ) );
        __UN_UPDATE_DATA_2__( __STRING__( hero ), __STRING__( weapontype ) );
        __UN_REMOVE_DATA_1__( __STRING__( hero ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_HERO_WEAPON_REQ );
        __UN_MESSAGE__( KFMsg::MSG_HERO_UNWEAPON_REQ );
        __UN_MESSAGE__( KFMsg::MSG_UPDATE_DURABILITY_REQ );
        __UN_MESSAGE__( KFMsg::MSG_HERO_WEAPON_ANOTHER_REQ );
    }

    KFData* KFItemWeaponModule::FindWeaponRecord( KFEntity* player )
    {
        auto status = player->GetStatus();
        auto kftypesetting = KFItemTypeConfig::Instance()->FindSetting( KFItemEnum::Weapon );
        return player->Find( kftypesetting->GetBagName( status ) );
    }

    __KF_INIT_ITEM_FUNCTION__( KFItemWeaponModule::InitWeaponData )
    {
        // 耐久
        kfitem->Set( __STRING__( durability ), kfsetting->_durability );

        // 词缀
        auto affixcount = kfsetting->_affix_pool_list.size();
        if ( affixcount > 0u )
        {
            auto kfarray = kfitem->Find( __STRING__( affix ) );
            for ( auto poolid : kfsetting->_affix_pool_list )
            {
                // 随机词缀
                auto weightdata = KFWeightConfig::Instance()->RandWeight( poolid );
                if ( weightdata == nullptr )
                {
                    __LOG_ERROR__( "weightpool=[{}] can't rand weight", poolid );
                    continue;
                }

                if ( weightdata->_id == 0 )
                {
                    continue;
                }

                player->AddArray( kfarray, weightdata->_id );
            }
        }
    }

    uint32 KFItemWeaponModule::CheckHeroWeapon( KFEntity* player, KFData* kfhero, KFData* kfweapon )
    {
        auto itemid = kfweapon->Get<uint32>( kfweapon->_data_setting->_config_key_name );
        auto kfsettting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsettting == nullptr )
        {
            return KFMsg::ItemSettingNotExist;
        }

        if ( kfsettting->_type != KFItemEnum::Weapon )
        {
            return KFMsg::ItemWeaponOnly;
        }

        // 种族限制
        if ( !kfsettting->_race_limit_list.empty() )
        {
            auto race = kfhero->Get<uint32>( __STRING__( race ) );
            if ( kfsettting->_race_limit_list.find( race ) == kfsettting->_race_limit_list.end() )
            {
                return KFMsg::ItemWeaponRaceLimit;
            }
        }

        // 英雄武器限制
        auto weapontype = kfhero->Get<uint32>( __STRING__( weapontype ) );
        if ( kfsettting->_weapon_type != weapontype )
        {
            return KFMsg::ItemWeaponHeroLimit;
        }

        // 等级限制
        if ( kfsettting->_level_limit != 0u )
        {
            auto level = kfhero->Get<uint32>( __STRING__( level ) );
            if ( level < kfsettting->_level_limit )
            {
                return KFMsg::ItemWeaponLevelLimit;
            }
        }

        return KFMsg::Ok;
    }

    __KF_MESSAGE_FUNCTION__( KFItemWeaponModule::HandleHeroWeaponReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgHeroWeaponReq );

        auto kfhero = player->Find( __STRING__( hero ), kfmsg.herouuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        KFData* kfweapon = nullptr;
        KFData* kfitemrecord = nullptr;
        std::tie( kfitemrecord, kfweapon ) = _kf_item->FindItem( player, kfmsg.itemuuid() );
        if ( kfweapon == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        // 判断是否能装备
        auto checkresult = CheckHeroWeapon( player, kfhero, kfweapon );
        if ( checkresult != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, checkresult );
        }

        // 更换同步顺序, 武器是先移除, 然后会update到英雄身上
        player->SyncDataToClient( KFEnum::Dec, KFEnum::Set, KFEnum::Add );

        // 先卸下英雄身上的装备
        auto kfheroweapon = kfhero->Move( __STRING__( weapon ), true );

        // 装备武器
        player->MoveData( kfitemrecord, kfmsg.itemuuid(), kfhero, __STRING__( weapon ) );

        // 如果原来有武器, 把他放回背包
        if ( kfheroweapon != nullptr )
        {
            player->AddData( kfitemrecord, kfheroweapon->GetKeyID(), kfheroweapon, false );
        }

        // 通知装备成功
        _kf_display->DelayToClient( player, KFMsg::ItemWeaponOk );
    }

    __KF_MESSAGE_FUNCTION__( KFItemWeaponModule::HandleHeroWeaponAnotherReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgHeroWeaponAnotherReq );

        auto kfherorecord = player->Find( __STRING__( hero ) );
        auto kfsourcehero = kfherorecord->Find( kfmsg.sourceherouuid() );
        auto kftargethero = kfherorecord->Find( kfmsg.targetherouuid() );
        if ( kfsourcehero == nullptr || kftargethero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto kfsourceweapon = kfsourcehero->Find( __STRING__( weapon ) );
        if ( kfsourceweapon == nullptr || kfsourceweapon->GetKeyID() == 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemWeaponNotExist );
        }

        // 判断源英雄武器是否能装备到目标英雄
        auto checkresult = CheckHeroWeapon( player, kftargethero, kfsourceweapon );
        if ( checkresult != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, checkresult );
        }

        // 更换同步顺序, 武器是先移除, 然后会update到英雄身上
        player->SyncDataToClient( KFEnum::Dec, KFEnum::Set, KFEnum::Add );

        // 判断包裹是否满子
        auto kfitemrecord = FindWeaponRecord( player );
        auto kftargetweapon = kftargethero->Find( __STRING__( weapon ) );
        if ( kftargetweapon != nullptr && kftargetweapon->GetKeyID() != 0u )
        {
            // 目标英雄有装备武器需要判断背包是否已满
            if ( _kf_item->IsItemRecordFull( player, kfitemrecord ) )
            {
                return _kf_display->SendToClient( player, KFMsg::ItemBagFull );
            }

            // 先卸下目标英雄身上的武器
            player->MoveData( kftargethero, __STRING__( weapon ), kfitemrecord );
        }

        // 装备武器
        kfsourceweapon = player->MoveData( kfsourcehero, __STRING__( weapon ), kftargethero, __STRING__( weapon ) );
        if ( kfsourceweapon == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemWeaponFailed );
        }

        // 通知装备成
        _kf_display->DelayToClient( player, KFMsg::ItemWeaponOk );
    }

    __KF_MESSAGE_FUNCTION__( KFItemWeaponModule::HandleHeroUnWeaponReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgHeroUnWeaponReq );

        // 判断武器背包是否满了
        auto kfitemrecord = FindWeaponRecord( player );
        if ( _kf_item->IsItemRecordFull( player, kfitemrecord ) )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagFull );
        }

        auto kfhero = player->Find( __STRING__( hero ), kfmsg.herouuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        // 卸下武器
        auto kfweapon = player->MoveData( kfhero, __STRING__( weapon ), kfitemrecord );
        if ( kfweapon == nullptr || kfweapon->GetKeyID() == 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemWeaponNotExist );
        }

        _kf_display->DelayToClient( player, KFMsg::ItemUnWeaponOk );
    }

    __KF_MESSAGE_FUNCTION__( KFItemWeaponModule::HandleUpdateDurabilityReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateDurabilityReq );

        if ( kfmsg.operate() != KFEnum::Add && kfmsg.operate() != KFEnum::Dec )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemUpdateDurabilityError );
        }

        auto kfhero = player->Find( __STRING__( hero ), kfmsg.herouuid() );
        if ( kfhero == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::HeroNotExist );
        }

        auto kfweapon = kfhero->Find( __STRING__( weapon ) );
        if ( kfweapon == nullptr || kfweapon->GetKeyID() == 0u )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemWeaponNotExist );
        }

        player->UpdateData( kfweapon, __STRING__( durability ), kfmsg.operate(), kfmsg.durability() );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFItemWeaponModule::OnWeaponDurabilityUpdate )
    {
        if ( newvalue == 0u )
        {
            // 删除武器
            auto kfhero = kfdata->GetParent()->GetParent();
            player->RemoveData( kfhero, __STRING__( weapon ) );
        }
    }

    __KF_UPDATE_DATA_FUNCTION__( KFItemWeaponModule::OnWeaponTypeUpdate )
    {
        // 判断是否可以装备
        auto kfhero = kfdata->GetParent();
        auto kfweapon = kfhero->Find( __STRING__( weapon ) );
        if ( kfweapon == nullptr || kfweapon->GetKeyID() == 0u )
        {
            return;
        }

        // 判断是否能装备
        auto checkresult = CheckHeroWeapon( player, kfhero, kfweapon );
        if ( checkresult == KFMsg::Ok )
        {
            return;
        }

        // 不能装备卸下来
        auto kfitemrecord = FindWeaponRecord( player );
        player->MoveData( kfhero, __STRING__( weapon ), kfitemrecord );
    }

    __KF_REMOVE_DATA_FUNCTION__( KFItemWeaponModule::OnRemoveHero )
    {
        auto kfweapon = kfdata->Move( __STRING__( weapon ), true );
        if ( kfweapon == nullptr || kfweapon->GetKeyID() == 0u )
        {
            return;
        }

        auto kfitemrecord = FindWeaponRecord( player );
        player->AddData( kfitemrecord, kfweapon->GetKeyID(), kfweapon, false );
    }
}