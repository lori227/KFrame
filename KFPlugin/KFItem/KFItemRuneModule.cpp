#include "KFItemRuneModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFItemRuneModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_ENTER_PLAYER__( &KFItemRuneModule::OnEnterRuneModule );

        __REGISTER_ADD_DATA_1__( __STRING__( item ), &KFItemRuneModule::OnAddRuneItemCallBack );
        ///////////////////////////////////////////////////////////////////////////////////////////////////

        __REGISTER_MESSAGE__( KFMsg::MSG_MOVE_RUNE_REQ, &KFItemRuneModule::HandleMoveRuneReq );
    }

    void KFItemRuneModule::BeforeShut()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_TIMER_0__();
        __UN_ENTER_PLAYER__();

        __UN_ADD_DATA_1__( __STRING__( item ) );
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_MOVE_RUNE_REQ );
    }

    __KF_ENTER_PLAYER_FUNCTION__( KFItemRuneModule::OnEnterRuneModule )
    {
        auto runecompound = player->Get<uint32>( __STRING__( runecompound ) );
        if ( runecompound == 0u )
        {
            return;
        }

        auto iter = _add_rune_data.find( player->GetKeyID() );
        if ( iter == _add_rune_data.end() )
        {
            auto kfitemrecord = player->Find( __STRING__( rune ) );
            if ( kfitemrecord == nullptr )
            {
                return;
            }

            UInt32Set runeset;
            for ( auto kfitem = kfitemrecord->First(); kfitem != nullptr; kfitem = kfitemrecord->Next() )
            {
                auto itemid = kfitem->Get( __STRING__( id ) );
                auto compoundid = KFItemConfig::Instance()->GetRuneCompoundId( itemid );
                if ( compoundid == _invalid_int )
                {
                    continue;
                }

                runeset.insert( itemid );
            }

            OnDelayCompoundRune( player, runeset, false );
        }
        else
        {
            OnDelayPutOnAndCompoundRune( player, false );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFItemRuneModule::HandleMoveRuneReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgMoveRuneReq );

        if ( ( kfmsg.sourcename() != __STRING__( rune ) && kfmsg.sourcename() != __STRING__( runeslot ) ) ||
                ( kfmsg.targetname() != __STRING__( rune ) && kfmsg.targetname() != __STRING__( runeslot ) ) )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        auto kfsourcerecord = player->Find( kfmsg.sourcename() );
        auto kftargetrecord = player->Find( kfmsg.targetname() );
        if ( kfsourcerecord == nullptr || kftargetrecord == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemBagNameError );
        }

        auto kfsourceitem = kfsourcerecord->Find( kfmsg.sourceuuid() );
        if ( kfsourceitem == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ItemDataNotExist );
        }

        auto operateresult = _invalid_int;

        if ( kfmsg.sourcename() == __STRING__( rune ) && kfmsg.targetname() == __STRING__( runeslot ) )
        {
            // 穿上符石需要判断类型限制
            auto result = CheckSameRuneType( player, kfsourceitem, kftargetrecord, kfmsg.targetindex() );
            if ( result != KFMsg::Ok )
            {
                return _kf_display->SendToClient( player, result );
            }

            operateresult = KFMsg::RunePutOnSuc;
        }
        else if ( kfmsg.sourcename() == __STRING__( runeslot ) && kfmsg.targetname() == __STRING__( rune ) )
        {
            auto sourceindex = kfsourceitem->Get<uint32>( __STRING__( index ) );
            auto kftargetitem = _kf_item_move->FindIndexItem( player, kftargetrecord, kfmsg.targetindex() );
            if ( kftargetitem != nullptr )
            {
                auto result = CheckSameRuneType( player, kftargetitem, kfsourcerecord, sourceindex );
                if ( result != KFMsg::Ok )
                {
                    return _kf_display->SendToClient( player, result );
                }

                operateresult = KFMsg::RunePutOnSuc;
            }

            operateresult = KFMsg::RuneTakeOffSuc;
        }

        auto result = _kf_item_move->MoveItem( player, kfsourcerecord, kfsourceitem, kftargetrecord, kfmsg.targetindex() );
        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, result );
        }
        else if ( operateresult != _invalid_int )
        {
            return _kf_display->DelayToClient( player, operateresult );
        }
    }

    __KF_ADD_DATA_FUNCTION__( KFItemRuneModule::OnAddRuneItemCallBack )
    {
        auto itemid = kfdata->Get<uint32>( kfdata->_data_setting->_config_key_name );
        auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
        if ( kfsetting == nullptr )
        {
            return;
        }

        if ( kfsetting->_type != KFItemEnum::Rune )
        {
            return;
        }

        player->UpdateData( __STRING__( runecompound ), KFEnum::Set, 1u );

        // 保存添加的道具信息
        auto itemuuid = kfdata->Get( __STRING__( uuid ) );
        auto iter = _add_rune_data.find( player->GetKeyID() );
        if ( iter == _add_rune_data.end() )
        {
            __DELAY_TIMER_1__( player->GetKeyID(), 10, &KFItemRuneModule::OnDelayTimerOperate );
        }

        _add_rune_data[player->GetKeyID()].insert( itemuuid );
    }

    __KF_TIMER_FUNCTION__( KFItemRuneModule::OnDelayTimerOperate )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            _add_rune_data.erase( objectid );
            return;
        }

        OnDelayPutOnAndCompoundRune( player );
    }

    uint32 KFItemRuneModule::CheckSameRuneType( KFEntity* player, KFData* kfitem, KFData* kftargetrecord, uint32 excludeindex )
    {
        auto sourceitemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
        auto kfsourcesetting = KFItemConfig::Instance()->FindSetting( sourceitemid );
        if ( kfsourcesetting == nullptr )
        {
            return KFMsg::ItemSettingNotExist;
        }

        auto runetype = kfsourcesetting->_rune_type;

        for ( auto iter = kftargetrecord->First(); iter != nullptr; iter = kftargetrecord->Next() )
        {
            auto itemindex = iter->Get<uint32>( __STRING__( index ) );
            if ( itemindex == excludeindex )
            {
                continue;
            }

            auto itemid = iter->Get<uint32>( __STRING__( id ) );
            auto kfitemsetting = KFItemConfig::Instance()->FindSetting( itemid );
            if ( kfitemsetting != nullptr && runetype == kfitemsetting->_rune_type )
            {
                return KFMsg::RuneSameTypeLimit;
            }
        }

        return KFMsg::Ok;
    }

    void KFItemRuneModule::PutOnRune( KFEntity* player, const UInt64Set& uuidlist )
    {
        static auto _option = KFGlobal::Instance()->FindConstant( "autoequiprune" );
        if ( _option->_uint32_value == 0u )
        {
            // 不需要自动装备
            return;
        }

        auto kfrunerecord = player->Find( __STRING__( rune ) );
        auto kfruneslotrecord = player->Find( __STRING__( runeslot ) );
        if ( kfrunerecord == nullptr || kfruneslotrecord == nullptr )
        {
            return;
        }

        for ( auto itemuuid : uuidlist )
        {
            if ( _kf_item->IsItemRecordFull( player, kfruneslotrecord ) )
            {
                return;
            }

            auto kfitem = kfrunerecord->Find( itemuuid );
            if ( kfitem == nullptr )
            {
                continue;
            }

            auto itemid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
            auto kfsetting = KFItemConfig::Instance()->FindSetting( itemid );
            if ( kfsetting == nullptr )
            {
                continue;
            }

            auto result = CheckSameRuneType( player, kfitem, kfruneslotrecord );
            if ( result != KFMsg::Ok )
            {
                continue;
            }

            _kf_item_move->MoveItem( player, kfrunerecord, kfitem, kfruneslotrecord, 0u );
        }
    }

    void KFItemRuneModule::CompoundRune( KFEntity* player, KFCompoundData& compounddata )
    {
        auto runeid = compounddata.runeid;
        auto compoundid = KFItemConfig::Instance()->GetRuneCompoundId( runeid );
        if ( compoundid == _invalid_int )
        {
            if ( compounddata.compoundnum > 0u )
            {
                compounddata.addlist[runeid] = compounddata.compoundnum;
            }
            return;
        }

        // 获得物品id数量，优先消耗符石槽
        auto slotuuid = _invalid_int;
        auto slotnum = _invalid_int;
        if ( runeid == compounddata.replaceid )
        {
            slotuuid = compounddata.replaceuuid;
            slotnum++;
        }
        else
        {
            auto kfruneslotrecord = player->Find( __STRING__( runeslot ) );
            for ( auto iter = kfruneslotrecord->First(); iter != nullptr; iter = kfruneslotrecord->Next() )
            {
                if ( runeid == iter->Get<uint32>( kfruneslotrecord->_data_setting->_config_key_name ) )
                {
                    slotuuid = iter->Get( __STRING__( uuid ) );
                    slotnum++;

                    break;
                }
            }
        }

        UInt64Vector baglist;
        auto kfrunerecord = player->Find( __STRING__( rune ) );
        if ( kfrunerecord == nullptr )
        {
            return;
        }

        for ( auto kfitem = kfrunerecord->First(); kfitem != nullptr; kfitem = kfrunerecord->Next() )
        {
            if ( runeid == kfitem->Get<uint32>( kfrunerecord->_data_setting->_config_key_name ) )
            {
                baglist.push_back( kfitem->Get( __STRING__( uuid ) ) );
            }
        }

        // 总符石数
        auto totalnum = slotnum + compounddata.compoundnum + baglist.size();
        static auto _option = KFGlobal::Instance()->FindConstant( "runecompoundnum" );
        if ( totalnum < _option->_uint32_value )
        {
            // 合成符石数量不足
            if ( compounddata.compoundnum > 0u )
            {
                compounddata.addlist[runeid] = compounddata.compoundnum;
            }

            return;
        }

        compounddata.compoundid = compoundid;

        // 合成消耗符石数
        auto compoundnum = totalnum / _option->_uint32_value;
        auto consumenum = compoundnum * _option->_uint32_value;

        if ( slotnum > 0u )
        {
            // 更新符石槽符石
            compounddata.replaceuuid = slotuuid;
            compounddata.replaceid = compoundid;
        }

        // 添加新的符石数量
        auto addnum = 0u;
        if ( compounddata.compoundnum > consumenum - slotnum )
        {
            addnum = compounddata.compoundnum - ( consumenum - slotnum );
        }

        // 消耗背包数量
        auto bagnum = consumenum - slotnum - ( compounddata.compoundnum - addnum );

        compounddata.runeid = compoundid;
        compounddata.compoundnum = compoundnum - slotnum;

        if ( addnum > 0u )
        {
            compounddata.addlist[runeid] = addnum;
        }

        if ( bagnum > 0u )
        {
            compounddata.removelist.insert( compounddata.removelist.end(), baglist.begin(), baglist.begin() + bagnum );
        }

        CompoundRune( player, compounddata );
    }

    void KFItemRuneModule::OnDelayPutOnAndCompoundRune( KFEntity* player, bool is_send )
    {
        auto iterator = _add_rune_data.find( player->GetKeyID() );
        if ( iterator == _add_rune_data.end() )
        {
            return;
        }

        auto kfrunerecord = player->Find( __STRING__( rune ) );
        if ( kfrunerecord == nullptr )
        {
            _add_rune_data.erase( player->GetKeyID() );
            return;
        }

        UInt32Set runeset;
        UInt32Set newruneset;
        for ( auto iter : iterator->second )
        {
            auto kfitem = kfrunerecord->Find( iter );
            if ( kfitem == nullptr )
            {
                continue;
            }

            auto runeid = kfitem->Get<uint32>( kfitem->_data_setting->_config_key_name );
            runeset.insert( runeid );

            if ( AddRealmRuneData( player, runeid ) )
            {
                newruneset.insert( runeid );
            }
        }

        // 发送获得新符石消息
        if ( is_send && newruneset.size() > 0u )
        {
            KFMsg::MsgGetNewRuneAck ack;

            auto& runelist = *ack.mutable_runelist();
            for ( auto iter : newruneset )
            {
                runelist[iter] = 0u;
            }

            _kf_player->SendToClient( player, KFMsg::MSG_GET_NEW_RUNE_ACK, &ack );
        }

        // 先穿戴符石
        PutOnRune( player, iterator->second );

        // 再合成符石
        player->SyncDataToClient();
        OnDelayCompoundRune( player, runeset, is_send );
    }

    void KFItemRuneModule::OnDelayCompoundRune( KFEntity* player, const UInt32Set& runeset, bool is_send )
    {
        player->UpdateData( __STRING__( runecompound ), KFEnum::Set, 0u );

        // 获取所有的符石道具列表
        auto kfrunerecord = player->Find( __STRING__( rune ) );
        auto kfruneslotrecord = player->Find( __STRING__( runeslot ) );
        if ( kfrunerecord == nullptr || kfruneslotrecord == nullptr )
        {
            _add_rune_data.erase( player->GetKeyID() );
            return;
        }

        // 清空玩家的数据
        _add_rune_data.erase( player->GetKeyID() );

        // 符石合成表<符石类型 合成数据>
        std::map<uint32, KFCompoundData> compoundmap;
        for ( auto iter : runeset )
        {
            // 合成符石
            KFCompoundData compounddata;
            compounddata.runeid = iter;

            CompoundRune( player, compounddata );

            // 删除符石道具
            for ( auto it : compounddata.removelist )
            {
                player->RemoveData( kfrunerecord, it );
            }

            // 添加符石道具
            for ( auto it : compounddata.addlist )
            {
                _kf_item->AddItem( player, kfrunerecord, it.first, it.second );

                AddRealmRuneData( player, it.first );
            }

            // 替换符石槽符石
            if ( compounddata.replaceuuid > 0u && compounddata.replaceid > 0u )
            {
                auto kfruneslot = kfruneslotrecord->Find( compounddata.replaceuuid );
                if ( kfruneslot != nullptr )
                {
                    auto itemindex = kfruneslot->Get<uint32>( __STRING__( index ) );
                    player->RemoveData( kfruneslotrecord, compounddata.replaceuuid );
                    _kf_item->AddItem( player, kfruneslotrecord, compounddata.replaceid, 1u, itemindex );

                    AddRealmRuneData( player, compounddata.replaceid );
                }

                if ( compounddata.compoundid == compounddata.replaceid )
                {
                    compounddata.compoundnum += 1u;
                }
            }

            // 保存不同类型合成信息
            if ( is_send && compounddata.compoundid != _invalid_int )
            {
                auto kfitemsetting = KFItemConfig::Instance()->FindSetting( compounddata.compoundid );
                if ( kfitemsetting != nullptr )
                {
                    compoundmap[kfitemsetting->_rune_type] = compounddata;
                }
            }
        }

        // 发送合成不同类型等级最高的符石
        if ( is_send && compoundmap.size() > 0u )
        {
            KFMsg::MsgRuneCompoundAck ack;

            auto& compoundlist = *ack.mutable_compoundlist();
            for ( auto iter : compoundmap )
            {
                auto& compounddata = iter.second;
                compoundlist[compounddata.compoundid] = compounddata.compoundnum;
            }

            _kf_player->SendToClient( player, KFMsg::MSG_RUNE_COMPOUND_ACK, &ack );
        }
    }

    bool KFItemRuneModule::AddRealmRuneData( KFEntity* player, uint32 runeid )
    {
        auto realmdata = _kf_realm->GetRealmData( player );
        if ( realmdata == nullptr )
        {
            return false;
        }

        auto& runedata = *realmdata->_data.mutable_runedata();
        if ( runedata.find( runeid ) == runedata.end() )
        {
            // 新的符石
            runedata[runeid] = 0u;

            return true;
        }

        return false;
    }
}